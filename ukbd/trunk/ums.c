#include <os.h>
#include <usbdi.h>
#include <usb.h>
#include <nspireio2.h>

nio_console csl;

static void init_console(void) {
	// 53 columns, 15 rows. 0/110px offset for x/y. Background color 15 (white), foreground color 0 (black)
	nio_InitConsole(&csl,53,15,0,110,15,0);
	nio_DrawConsole(&csl);
}

static int match(device_t self) {
	init_console();
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
};

struct s_boot_ums_report {
	char button;        
	signed char xdispl;
	signed char ydispl;
	char bytes3to7[5] ; //optional bytes
};

static int ums_xcoord = 0;
static int ums_ycoord = 0;

static void ums_intr(usbd_xfer_handle __attribute__((unused)) xfer, usbd_private_handle addr, usbd_status __attribute__((unused)) status) {
	struct ums_softc *sc = addr;
	struct s_boot_ums_report *ibuf = (struct s_boot_ums_report *)sc->sc_ibuf.buf;
	ums_xcoord += (int)(ibuf->xdispl / 2);
	ums_ycoord += (int)(ibuf->ydispl / 2);
	if (ums_xcoord < 0) ums_xcoord = 0;
	if (ums_ycoord < 0) ums_ycoord = 0;
	if (ums_xcoord >= SCREEN_WIDTH) ums_xcoord = SCREEN_WIDTH - 1;
	if (ums_ycoord >= SCREEN_HEIGHT) ums_ycoord = SCREEN_HEIGHT - 1;
	setPixel(ums_xcoord, ums_ycoord, BLACK);
	if(ibuf->button)
		putChar(ums_xcoord, ums_ycoord, 'i', BLACK, WHITE);
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
		//usbd_abort_pipe(sc->sc_intrpipe); // strangely crashes
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
