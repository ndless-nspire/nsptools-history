#include <os.h>

static const unsigned device_get_ivars_addrs[] = {0x10354C20, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define device_get_ivars SYSCALL_CUSTOM(device_get_ivars_addrs, void *, device_t dev)

static const unsigned device_get_softc_addrs[] = {0x10354C60, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define device_get_softc SYSCALL_CUSTOM(device_get_softc_addrs, void *, device_t dev)

#include <ocd.h>
#include "usbdi.h"
#include "usb.h"
#include <nspireio2.h>

static const unsigned register_driver_addrs[] = {0x10354E44, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define register_driver SYSCALL_CUSTOM(register_driver_addrs, int, int a, int (*methods[])(device_t), const char *driver_name, int b, unsigned softc_size)

static const unsigned init_usb_stack_addrs[] = {0x1035F334, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define init_usb_stack SYSCALL_CUSTOM(init_usb_stack_addrs, void)

static const unsigned unregister_drivers_addrs[] = {0x10352F88, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define unregister_drivers SYSCALL_CUSTOM(unregister_drivers_addrs, void)

static const unsigned register_drivers_addrs[] = {0x10352F88, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define register_drivers SYSCALL_CUSTOM(register_drivers_addrs, void)

static const unsigned usbd_do_request_addrs[] = {0x103F4270, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define usbd_do_request SYSCALL_CUSTOM(usbd_do_request_addrs, usbd_status, usbd_device_handle dev, usb_device_request_t * req, void *data)

static const unsigned usbd_get_device_descriptor_addrs[] = {0x103F32E8, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define usbd_get_device_descriptor SYSCALL_CUSTOM(usbd_get_device_descriptor_addrs, usb_device_descriptor_t *, usbd_device_handle dev)

static const unsigned usbd_devinfo_addrs[] = {0x103F2E58, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define usbd_devinfo SYSCALL_CUSTOM(usbd_devinfo_addrs, void, usbd_device_handle dev, int showclass, char *cp)

static const unsigned usbd_interface2endpoint_descriptor_addrs[] = {0x103F32F0, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define usbd_interface2endpoint_descriptor SYSCALL_CUSTOM(usbd_interface2endpoint_descriptor_addrs, usb_endpoint_descriptor_t *, usbd_interface_handle iface, u_int8_t index)

static const unsigned usbd_get_interface_descriptor_addrs[] = {0x103F32E0, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define usbd_get_interface_descriptor SYSCALL_CUSTOM(usbd_get_interface_descriptor_addrs, usb_interface_descriptor_t *, usbd_interface_handle iface)

static const unsigned usbd_interface2device_handle_addrs[] = {0x103F3388, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define usbd_interface2device_handle SYSCALL_CUSTOM(usbd_interface2device_handle_addrs, void, usbd_interface_handle iface, usbd_device_handle *dev)

static const unsigned usbd_open_pipe_intr_addrs[] = {0x103F4548, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define usbd_open_pipe_intr SYSCALL_CUSTOM(usbd_open_pipe_intr_addrs, usbd_status, usbd_interface_handle iface, u_int8_t address, u_int8_t flags, usbd_pipe_handle *pipe, usbd_private_handle priv, void *buffer, u_int32_t len, usbd_callback cb, int ival)

static const unsigned bsd_malloc_addrs[] = {0x1035F998, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define bsd_malloc SYSCALL_CUSTOM(bsd_malloc_addrs, void*, size_t size, BOOL init)

// + cf debug.c

/* usbhid.h */ 

#define UR_SET_PROTOCOL		0x0b

#if 0
static void dump_info(usbd_device_handle dev) { // uaa->device
	char devinfo[1024];
	usbd_devinfo(dev, 0, devinfo);
	putStr(0, 0, devinfo, 15, 0);
}
#endif

nio_console csl;

static void init_console(void) {
	// 53 columns, 15 rows. 0/110px offset for x/y. Background color 15 (white), foreground color 0 (black)
	nio_InitConsole(&csl,53,15,0,110,15,0);
	nio_DrawConsole(&csl);
}

/** TODO
- free du malloc dans softc
- crash si shutdown. Manque methode non implémentée ?
*/

static int match(device_t self) {
	init_console();
	struct usb_attach_arg *uaa = device_get_ivars(self);
	//usb_device_descriptor_t *dd = usbd_get_device_descriptor(uaa->device);
  if (!uaa->iface) {
  	nio_printf(&csl, "nomatch\n");
  	return (UMATCH_NONE);
  }
  nio_printf(&csl, "match\n");
	return UMATCH_DEVCLASS_DEVSUBCLASS;
}

usbd_status usbd_set_protocol(usbd_interface_handle iface, int report) {
	usb_interface_descriptor_t *id = usbd_get_interface_descriptor(iface);
	nio_printf(&csl, "usbd_get_interface_descriptor: %p\n", id);
	usbd_device_handle dev;
	usb_device_request_t req;
	usbd_interface2device_handle(iface, &dev);
	nio_printf(&csl, "usbd_interface2device_handle: %p\n", dev);
	req.bmRequestType = UT_WRITE_CLASS_INTERFACE;
	req.bRequest = UR_SET_PROTOCOL;
	USETW(req.wValue, report);
	USETW(req.wIndex, id->bInterfaceNumber);
	USETW(req.wLength, 0);
	return (usbd_do_request(dev, &req, 0));
}

struct s_ibuf { // strangely needed by TI's usbd_open_pipe_intr
 	u_char *buf;
 	int dummy1;
 	int dummy2;
 };

struct ukbd_softc {
 device_t sc_dev;                /* base device */
 usbd_interface_handle sc_iface; /* interface */
 usbd_pipe_handle sc_intrpipe;   /* interrupt pipe */
 int sc_ep_addr;
 struct s_ibuf sc_ibuf;
 int sc_isize;
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

static void ukbd_intr(usbd_xfer_handle xfer, usbd_private_handle addr, usbd_status status) {
	nio_printf(&csl, "i");
	struct ukbd_softc *sc = addr;
	struct s_boot_kbd_report *ibuf = (struct s_boot_kbd_report *)sc->sc_ibuf.buf;
	unsigned i;
	for (i = 0; i < 6; i++) {
		if (ibuf->keycode[i])
			nio_printf(&csl, "%c", HIDtoa(ibuf, i));
	}
}

static int attach(device_t self) {
	struct ukbd_softc *sc = device_get_softc(self);
	struct usb_attach_arg *uaa = device_get_ivars(self);
	//ocd_dump(&csl, uaa);
	usbd_status err;
	if (sc == NULL)
		return (ENXIO);
	sc->sc_iface = uaa->iface; // or usbd_device2interface_handle
	nio_printf(&csl, "sc->sc_iface=%p\n", sc->sc_iface); 
	sc->sc_dev = self;
	nio_printf(&csl, "usbd_interface2endpoint_descriptor\n");
	usb_endpoint_descriptor_t *ed = usbd_interface2endpoint_descriptor(sc->sc_iface, 0);
	nio_printf(&csl, "ed=%p\n", ed);
	sc->sc_ep_addr = ed->bEndpointAddress;
	//sc->sc_isize = 8;
	sc->sc_isize = 16;
	sc->sc_ibuf.buf = bsd_malloc(sc->sc_isize, TRUE);
	nio_printf(&csl, "ibuf=%p\n", sc->sc_ibuf.buf);
	if (!sc->sc_ibuf.buf)
		return (ENXIO);
	nio_printf(&csl, "usbd_set_protocol\n");
	usbd_set_protocol(sc->sc_iface, 0);
	nio_printf(&csl, "usbd_open_pipe_intr(%i)\n", sc->sc_ep_addr);
	//ocd_set_breakpoint(0X103F7224);
	sc->sc_ibuf.dummy1 = 0;
	sc->sc_ibuf.dummy2 = 0;
	err = usbd_open_pipe_intr(sc->sc_iface, sc->sc_ep_addr,
	                          USBD_SHORT_XFER_OK, &sc->sc_intrpipe, sc,
	                          &sc->sc_ibuf, sc->sc_isize, ukbd_intr,
	                          0xC);//USBD_DEFAULT_INTERVAL);
	nio_printf(&csl, "err=%i\n", err);
	if (err)
		return (ENXIO);
	return 0;
}

static int detach(device_t self) {
	//putStr(0, 0, "                                                  ", 15, 0);
	return 0;
}

static int (*methods[])(device_t) = {match, attach, detach, NULL};

int register_my_driver(void) {
	// softc_size: must not be 0
	return register_driver(2, methods, "mydriver", 0, sizeof(struct ukbd_softc));
}

int main(void) {
	//TCT_Local_Control_Interrupts(0); if enabled and wait_key_pressed(), the hook won't work!
	nl_relocdata((unsigned*)methods, sizeof(methods)/sizeof(methods[0]) - 1);

	ocd_init();

	register_my_driver();
	
	nl_set_resident();

	return 0;
}
