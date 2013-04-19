#include <os.h>
#include <usbdi.h>
#include <usb.h>

//#define DEBUG

#ifdef DEBUG
#include <nspireio2.h>
nio_console csl;
#endif

// 0xFB00: single click
// 0xAC00: drag
static const unsigned send_click_event_addrs[] = {0x1005E350, 0, 0, 0x1005D9B0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define send_click_event SYSCALL_CUSTOM(send_click_event_addrs, void, struct s_ns_event* /* eventbuf */, unsigned short /* keycode_asciicode */, BOOL /* is_key_up */, BOOL /* unknown */)

static const unsigned send_pad_event_addrs[] = {0x1005E410, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define send_pad_event SYSCALL_CUSTOM(send_pad_event_addrs, void, struct s_ns_event* /* eventbuf */, unsigned short /* keycode_asciicode */, BOOL /* is_key_up */, BOOL /* unknown */)

static int match(device_t self) {
#ifdef DEBUG
	lcd_ingray();
	// 53 columns, 15 rows. 0/110px offset for x/y. Background color 15 (white), foreground color 0 (black)
	nio_InitConsole(&csl,53,15,0,110,15,0);
	nio_DrawConsole(&csl);
#endif
	struct usb_attach_arg *uaa = device_get_ivars(self);
  if (!uaa->iface)
  	return UMATCH_NONE;
  usb_interface_descriptor_t *id = usbd_get_interface_descriptor(uaa->iface);
  if (!id)
  	return UMATCH_NONE;
  if (id->bInterfaceClass == UICLASS_HID &&
	    id->bInterfaceSubClass == UISUBCLASS_BOOT &&
	    id->bInterfaceProtocol == UIPROTO_MOUSE)
		return UMATCH_IFACECLASS;
	else
		return UMATCH_NONE;
}

struct ums_softc {
	device_t sc_dev;
	usbd_interface_handle sc_iface;
	usbd_pipe_handle sc_intrpipe;
	int sc_ep_addr;
	struct s_usb_pipe_buf sc_ibuf;
	int sc_isize;
	int sc_enabled;
	char sc_prev_button;
	BOOL sc_dragging;
	int sc_xcoord;
	int sc_ycoord;
	int sc_click_xcoord;
	int sc_click_ycoord;
};

struct s_boot_ums_report {
	char button;        
	signed char xdispl;
	signed char ydispl;
	char bytes3to7[5] ; //optional bytes
};

static void ums_intr(usbd_xfer_handle __attribute__((unused)) xfer, usbd_private_handle addr, usbd_status __attribute__((unused)) status) {
	struct ums_softc *sc = addr;
	struct s_boot_ums_report *ibuf = (struct s_boot_ums_report *)sc->sc_ibuf.buf;
	struct s_ns_event ns_ev;
	
	if (status == USBD_CANCELLED)
		return;
	
	if ((ibuf->xdispl || ibuf->ydispl)) {
		sc->sc_xcoord += (int)(ibuf->xdispl / 2);
		sc->sc_ycoord += (int)(ibuf->ydispl / 2);
		if (sc->sc_xcoord < 0) sc->sc_xcoord = 0;
		if (sc->sc_ycoord < 0) sc->sc_ycoord = 0;
		if (sc->sc_xcoord >= SCREEN_WIDTH) sc->sc_xcoord = SCREEN_WIDTH - 1;
		if (sc->sc_ycoord >= SCREEN_HEIGHT) sc->sc_ycoord = SCREEN_HEIGHT - 1;
		if (ibuf->button && !sc->sc_dragging) {
			sc->sc_dragging = TRUE;
			memset(&ns_ev, 0, sizeof(struct s_ns_event));
			ns_ev.cursor_x = sc->sc_click_xcoord;
			ns_ev.cursor_y = sc->sc_click_ycoord;
			ns_ev.click = 0x8;
			send_click_event(&ns_ev, 0xAC00, FALSE, TRUE); // dragging
		}
		
		memset(&ns_ev, 0, sizeof(struct s_ns_event));
		ns_ev.cursor_x = sc->sc_xcoord;
		ns_ev.cursor_y = sc->sc_ycoord;
		if (ibuf->button)
			ns_ev.click = 0x8;
		send_pad_event(&ns_ev, 0x7F00, FALSE, FALSE); // move
		send_pad_event(&ns_ev, 0x7F00, TRUE, TRUE);
	}
	// button pressed
	if (!(sc->sc_prev_button) && (ibuf->button)) {
		sc->sc_click_xcoord = sc->sc_xcoord;
		sc->sc_click_ycoord = sc->sc_ycoord;
	}
	// button released
	else if ((sc->sc_prev_button) && !(ibuf->button)) {
		if (!sc->sc_dragging) {
			memset(&ns_ev, 0, sizeof(struct s_ns_event));
			ns_ev.click = 0x8;
			ns_ev.cursor_x = sc->sc_xcoord;
			ns_ev.cursor_y = sc->sc_ycoord;
			send_click_event(&ns_ev, 0xFB00, FALSE, TRUE); // click
		}
		memset(&ns_ev, 0, sizeof(struct s_ns_event));
		ns_ev.click = 0x8;
		ns_ev.cursor_x = sc->sc_xcoord;
		ns_ev.cursor_y = sc->sc_ycoord;
		send_click_event(&ns_ev, sc->sc_dragging ? 0xAC00 : 0xFB00, TRUE, TRUE); // up
		if (sc->sc_dragging) {
			memset(&ns_ev, 0, sizeof(struct s_ns_event));
			ns_ev.click = 0x8;
			ns_ev.cursor_x = sc->sc_xcoord;
			ns_ev.cursor_y = sc->sc_ycoord;
			send_click_event(&ns_ev, 0xAC00, FALSE, TRUE); // dragging again... strangely required to stop dragging.
			sc->sc_dragging = FALSE;
		}
	}
	sc->sc_prev_button = ibuf->button;
}

static int attach(device_t self) {
	struct ums_softc *sc = device_get_softc(self);
	struct usb_attach_arg *uaa = device_get_ivars(self);
	usbd_status err;
	if (sc == NULL)
		return (ENXIO);
	sc->sc_iface = uaa->iface;
	sc->sc_dev = self;
	usb_endpoint_descriptor_t *ed = usbd_interface2endpoint_descriptor(sc->sc_iface, 0);
	sc->sc_ep_addr = ed->bEndpointAddress;
	sc->sc_isize = 16;
	usbd_set_protocol(sc->sc_iface, 0);
	usbd_set_idle(sc->sc_iface, 0, 0);
	sc->sc_ibuf.dummy1 = 0;
	sc->sc_ibuf.dummy2 = 0;
	sc->sc_ibuf.buf = malloc(sc->sc_isize);
	if (!sc->sc_ibuf.buf)
		return (ENXIO);
	sc->sc_enabled = 1;
	err = usbd_open_pipe_intr(sc->sc_iface, sc->sc_ep_addr,
	                          USBD_SHORT_XFER_OK, &sc->sc_intrpipe, sc,
	                          &sc->sc_ibuf, sc->sc_isize, ums_intr,
	                          USBD_DEFAULT_INTERVAL);
	if (err) {
		free(sc->sc_ibuf.buf);
		sc->sc_enabled = 0;
		return (ENXIO);
	}
	return 0;
}

static int detach(device_t self) {
	struct ums_softc *sc = device_get_softc(self);
	if (sc->sc_enabled) {
		usbd_abort_pipe(sc->sc_intrpipe);
		usbd_close_pipe(sc->sc_intrpipe);
		free(sc->sc_ibuf.buf);
		sc->sc_enabled = 0;
	}
	return 0;
}

static int (*methods[])(device_t) = {match, attach, detach, NULL};

int main(void) {
	nl_relocdata((unsigned*)methods, sizeof(methods)/sizeof(methods[0]) - 1);
	usb_register_driver(2, methods, "ums", 0, sizeof(struct ums_softc));
	nl_set_resident();
	return 0;
}
