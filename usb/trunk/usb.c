#include <os.h>

static const unsigned device_get_ivars_addrs[] = {0x10354C20, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define device_get_ivars SYSCALL_CUSTOM(device_get_ivars_addrs, void *, device_t dev)

static const unsigned device_get_softc_addrs[] = {0x10354C60, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define device_get_softc SYSCALL_CUSTOM(device_get_softc_addrs, void *, device_t dev)

#include "debug.h"
#include "usbdi.h"
#include "usb.h"
#include "nspireio2.h"

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

struct ukbd_softc {
 device_t sc_dev;                /* base device */
 usbd_interface_handle sc_iface; /* interface */
 usbd_pipe_handle sc_intrpipe;   /* interrupt pipe */
 int sc_ep_addr;
 u_char *sc_ibuf;
 int sc_isize;
};

static void ukbd_intr(usbd_xfer_handle xfer, usbd_private_handle addr, usbd_status status) {
	struct ukbd_softc *sc = addr;
	u_char *ibuf = sc->sc_ibuf;
	nio_printf(&csl, "ukbd_intr\n");
}

static int attach(device_t self) {
	struct ukbd_softc *sc = device_get_softc(self);
	struct usb_attach_arg *uaa = device_get_ivars(self);
	//dbg_dump(&csl, uaa);
	usbd_status err;
	if (sc == NULL)
		return (ENXIO);
	sc->sc_iface = uaa->iface;
	nio_printf(&csl, "sc->sc_iface=%p\n", sc->sc_iface); 
	sc->sc_dev = self;
	nio_printf(&csl, "usbd_interface2endpoint_descriptor\n");
	usb_endpoint_descriptor_t *ed = usbd_interface2endpoint_descriptor(sc->sc_iface, 0);
	nio_printf(&csl, "ed=%p\n", ed);
	sc->sc_ep_addr = ed->bEndpointAddress;
	sc->sc_isize = 8;
	sc->sc_ibuf = malloc(sc->sc_isize);
	nio_printf(&csl, "ibuf=%p\n", sc->sc_ibuf);
	if (!sc->sc_ibuf)
		return (ENXIO);
	nio_printf(&csl, "usbd_set_protocol\n");
	usbd_set_protocol(sc->sc_iface, 0);
	nio_printf(&csl, "usbd_open_pipe_intr(%i\n", sc->sc_ep_addr);
	err = usbd_open_pipe_intr(sc->sc_iface, sc->sc_ep_addr,
	                          USBD_SHORT_XFER_OK, &sc->sc_intrpipe, sc,
	                          sc->sc_ibuf, sc->sc_isize, ukbd_intr,
	                          USBD_DEFAULT_INTERVAL);
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

	dbg_init();
	dbg_break();
	printf("hello\n");
	dbg_cleanup();
	
	//register_my_driver();
	
	//nl_set_resident();
	return 0;
}
