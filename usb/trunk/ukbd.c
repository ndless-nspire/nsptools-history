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
	    id->bInterfaceProtocol == UIPROTO_BOOT_KEYBOARD)
		return UMATCH_IFACECLASS;
	else
		return UMATCH_NONE;
}

struct ukbd_softc {
 device_t sc_dev;
 usbd_interface_handle sc_iface;
 usbd_pipe_handle sc_intrpipe;
 int sc_ep_addr;
 struct s_usb_pipe_buf sc_ibuf;
 int sc_isize;
 int sc_enabled;
};

struct s_boot_kbd_report {
	struct {
		unsigned LCtrl:1;
		unsigned LShift:1;
		unsigned LAlt:1;
		unsigned LWin:1;
		unsigned RCtrl:1;
		unsigned RShift:1;
		unsigned RAlt:1;
		unsigned RWin:1;
	} mod;
	char reserved;
	char keycode[6];
};

// From https://github.com/felis/lightweight-usb-host/blob/e790c8c7e29f04fd938939f4dd5f88b52125b8af/cli.c
// TODO cleanup
/* function to convert HID keyboard code to ASCII */
char HIDtoa(struct s_boot_kbd_report *buf, unsigned index) {
	char HIDcode = buf->keycode[ index ];    
	/* symbols a-z,A-Z */
	if( HIDcode >= 0x04 && HIDcode <= 0x1d ) {  
			if( buf->mod.LShift || buf->mod.RShift ) {                          //uppercase 
					return( HIDcode + 0x3d );
			}
			if( buf->mod.LCtrl || buf->mod.RCtrl ) {                            //Ctrl
					return( HIDcode - 3 );
			}
			return( HIDcode + 0x5d );                             //lowercase
	}
	/* Numbers 1-9,0 */
	if(  HIDcode >= 0x1e && HIDcode <= 0x27 ) {
			if( buf->mod.LShift || buf->mod.RShift ) {                          //uppercase
					switch( HIDcode ) {
							case( 0x1f ):   //HID code for '2'
									return('@');
							case( 0x23 ):   //HID code for '6'
									return('^');
							case( 0x24 ):   //HID code for '7'
									return('&');    
							case( 0x25 ):   //HID code for '8'
									return('*');
							case( 0x26 ):   //HID code for '9'
									return('(');
							case( 0x27 ):   //HID code for '0'
									return(')');     
							default:        //1,3,4,5
									return( HIDcode + 3 );
					}     
			}
			return( HIDcode + 0x13 );
	}
	/* Misc. non-modifiable keys in no particular order */
	switch( HIDcode ) {
			case( 0x28 ):       //Enter
					return( 0x0d ); //CR
			case( 0x29 ):       //ESC
					return( 0x1b ); //ESC
			case( 0x2c ):       //spacebar
					return( 0x20 ); //
			case( 0x36 ):       //comma
					return(',');
			case( 0x37 ):       //dot        
					return('.');
	}
	return( 0x07 );         //Bell 
}

// from 'A' to 'Z'
unsigned char ascii_to_ns_key[] = {0x66, 0x46, 0x26, 0x85, 0x65, 0x45, 0x25, 0x84, 0x64, 0x44 /* J */, 0x24, 0x83, 0x63, 0x43, 0x23, 0x82, 0x62, 0x42, 0x22 /* S */, 0x81, 0x61, 0x41, 0x21, 0x80, 0x60, 0x40};

unsigned short ascii_to_ns_key_ascii(char ascii) {
	if (!ascii || ascii > 'z' || ascii < 'a')
		return 0;
	return ascii_to_ns_key[ascii - 'a'] << 8 | ascii;
}

static void ukbd_intr(usbd_xfer_handle __attribute__((unused)) xfer, usbd_private_handle addr, usbd_status  __attribute__((unused)) status) {
	struct ukbd_softc *sc = addr;
	struct s_boot_kbd_report *ibuf = (struct s_boot_kbd_report *)sc->sc_ibuf.buf;
	unsigned i;
		struct s_ns_event ns_ev;
	for (i = 0; i < 6; i++) {
		if (ibuf->keycode[i]) {
			unsigned short ns_key_ascii = ascii_to_ns_key_ascii(HIDtoa(ibuf, i));
			send_key_event(&ns_ev, ns_key_ascii, FALSE, TRUE);
			send_key_event(&ns_ev, ns_key_ascii, TRUE, TRUE);
		}
	}
}

static int attach(device_t self) {
	struct ukbd_softc *sc = device_get_softc(self);
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
	                          &sc->sc_ibuf, sc->sc_isize, ukbd_intr,
	                          USBD_DEFAULT_INTERVAL);
	if (err) {
		free(sc->sc_ibuf.buf);
		sc->sc_enabled = 0;
		return (ENXIO);
	}
	return 0;
}

static int detach(device_t self) {
	struct ukbd_softc *sc = device_get_softc(self);
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
	usb_register_driver(2, methods, "ukbd", 0, sizeof(struct ukbd_softc));
	nl_set_resident();
	return 0;
}
