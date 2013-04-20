#include <os.h>
#include <usbdi.h>
#include <usb.h>

//#define DEBUG

#ifdef DEBUG
#include <nspireio2.h>
nio_console csl;
#endif

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
	    id->bInterfaceProtocol == UIPROTO_BOOT_KEYBOARD)
		return UMATCH_IFACECLASS;
	else
		return UMATCH_NONE;
}

#define NKEYCODE 6

struct s_boot_kbd_report {
	unsigned char  modifiers;
#define MOD_CONTROL_L   0x01
#define MOD_CONTROL_R   0x10
#define MOD_SHIFT_L     0x02
#define MOD_SHIFT_R     0x20
#define MOD_ALT_L       0x04
#define MOD_ALT_R       0x40
#define MOD_WIN_L       0x08
#define MOD_WIN_R       0x80
	unsigned char reserved;
	unsigned char keycode[NKEYCODE];
};

struct ukbd_softc {
	device_t sc_dev;
	usbd_interface_handle sc_iface;
	usbd_pipe_handle sc_intrpipe;
	int sc_ep_addr;
	struct s_usb_pipe_buf sc_ibuf;
	int sc_isize;
	int sc_enabled;
	struct s_boot_kbd_report sc_prev_report;
	unsigned int modifier; // active modifier TI-Nspire code
};

// USB usage ID to TI-Nspire scancode and ASCII code as expected by send_key_event()
// See http://www.usb.org/developers/devclass_docs/Hut1_11.pdf chap. 10.
static unsigned short trtab_key[256] = {
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x6600|'a', 0x4600|'b', 0x2600|'c', 0x8500|'d', /* 00 - 07 */
	0x6500|'e', 0x4500|'f', 0x2500|'g', 0x8400|'h', 0x6400|'i', 0x4400|'j', 0x2400|'k', 0x8300|'l', /* 08 - 0F */
	0x6300|'m', 0x4300|'n', 0x2300|'o', 0x8200|'p', 0x6200|'q', 0x4200|'r', 0x2200|'s', 0x8100|'t', /* 10 - 17 */
	0x6100|'u', 0x4100|'v', 0x2100|'w', 0x8000|'x', 0x6000|'y', 0x4000|'z', 0x0000    , 0x0000    , /* 18 - 1F */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 20 - 27 */
	0x1000|'\r',0x961b    , 0x1500|'\b',0x9500|'\t',0x2000|' ', 0x0000    , 0x0000    , 0x0000    , /* 28 - 2F TODO 2D to 49 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0xA000|',', 0x0000    , /* 30 - 37 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 38 - 3F */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 40 - 47 */
	0x0000    , 0x0000    , 0xFD00    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x2700    , /* 48 - 4F */
	0x0700    , 0x3700    , 0x1700    , 0x0000    , 0x1400|'/', 0x1300|'*', 0x1200|'-', 0x1100|'+', /* 50 - 57 */
	0x1000|'\r',0x7300|'1', 0x5300|'2', 0x3300|'3', 0x7200|'4', 0x5200|'5', 0x3200|'6', 0x7000|'7', /* 58 - 5F */
	0x5100|'8', 0x3100|'9', 0x5000|'0', 0x7000|'.', 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 60 - 67 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 68 - 6F */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 70 - 77 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 78 - 7F */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 80 - 87 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 88 - 8F */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 90 - 97 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* 98 - 9F */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* A0 - A7 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* A8 - AF */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* B0 - B7 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* B8 - BF */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* C0 - C7 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* C8 - CF */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* D0 - D7 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* D8 - DF */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* E0 - E7 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* E8 - EF */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* F0 - F7 */
	0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , 0x0000    , /* F8 - FF */   
};

// Language support - USB usage ID (QWERTY) to USB usage ID
static unsigned short map_lang_azerty[256] = {
	0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, /* 00 - 07 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 08 - 0F */   
	0x36, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, /* 10 - 17 */   
	0x00, 0x00, 0x1D, 0x00, 0x00, 0x1A, 0x00, 0x00, /* 18 - 1F */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 20 - 27 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 28 - 2F */   
	0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x33, 0x00, /* 30 - 37 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 38 - 3F */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 40 - 47 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 48 - 4F */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 50 - 57 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 58 - 5F */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 60 - 67 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 68 - 6F */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 70 - 77 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 78 - 7F */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 80 - 87 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 88 - 8F */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 90 - 97 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 98 - 9F */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* A0 - A7 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* A8 - AF */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* B0 - B7 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* B8 - BF */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* C0 - C7 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* C8 - CF */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* D0 - D7 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* D8 - DF */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* E0 - E7 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* E8 - EF */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* F0 - F7 */   
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* F8 - FF */   
};

static unsigned short *use_lang_map = NULL;

#define NMOD 8
// USB modifiers to TI-Nspire scancode/ASCII code and modifier as expected by send_key_event()
static struct {
	unsigned short mask, key, modifier;
} mods[NMOD] = {
	{ MOD_CONTROL_L, 0xAA00, 4},
	{ MOD_CONTROL_R, 0xAA00, 4},
	{ MOD_SHIFT_L,   0xAB00, 3},
	{ MOD_SHIFT_R,   0xAB00, 3},
	{ MOD_ALT_L,     0x0000, 0},
	{ MOD_ALT_R,     0x0000, 0},
	{ MOD_WIN_L,     0x0000, 0},
	{ MOD_WIN_R,     0x0000, 0},
};

static unsigned char map_key_with_lang(unsigned char key) {
	if (!use_lang_map || !use_lang_map[key]) return key;
	else return use_lang_map[key];
}

// Apply TI-Nspire modifier to TI-Nspire key. Quick workaround to handle capital letters.
static unsigned short nsmod(unsigned int ns_modifier, unsigned short ns_key) {
	if (ns_modifier == 3) { // shift
		if ((ns_key & 0xFF) >= 'a' && (ns_key & 0xFF) <= 'z')
			return (ns_key & 0xFF00) | ((ns_key & 0xFF) - 'a' + 'A');
	}
	return ns_key;
}

// TODO:
// fix random crash when replugged in
// uninstallation
// Ctrl and/or CAPS strangely always displayed
// broken modifiers, although the send_key_event is executed
// key repeat, not handled by the OS except for some keys such as the arrow keys
// handle special keys such as : ', ", : , ...
// special case for ErrorRollOver?
// LEDs
static void ukbd_intr(usbd_xfer_handle __attribute__((unused)) xfer, usbd_private_handle addr, usbd_status  __attribute__((unused)) status) {
	struct ukbd_softc *sc = addr;
	struct s_boot_kbd_report *ibuf = (struct s_boot_kbd_report *)sc->sc_ibuf.buf;
	unsigned i, j;
	unsigned char key;
	struct s_ns_event ns_ev;
	
	if (status != USBD_NORMAL_COMPLETION)
		return;
	
	memset(&ns_ev, 0, sizeof(struct s_ns_event));
	ns_ev.modifiers = sc->modifier;
	/* Check for modifiers */
	unsigned char mod = ibuf->modifiers;
	unsigned char omod = sc->sc_prev_report.modifiers;
	if (mod != omod) {
		for (i = 0; i < NMOD; i++) {
			if (   (mod & mods[i].mask) != (omod & mods[i].mask)
				&& (mods[i].key)) {
				sc->modifier = (mod & mods[i].mask) ? mods[i].modifier : 0;
				ns_ev.modifiers = sc->modifier;
				send_key_event(&ns_ev, mods[i].key, !!!(mod & mods[i].mask), TRUE);
			}
		}
	}

	/* Check for released keys. */
	for (i = 0; i < NKEYCODE; i++) {
		key = sc->sc_prev_report.keycode[i];
		if (key == 0)
			break;
		for (j = 0; j < NKEYCODE; j++) {
			if (ibuf->keycode[j] == 0)
				break;
			if (key == ibuf->keycode[j])
				goto rfound;
		}
		send_key_event(&ns_ev, nsmod(sc->modifier, trtab_key[map_key_with_lang(key)]), TRUE, TRUE); // released
		rfound:
			;
	}
					
	/* Check for pressed keys. */
	for (i = 0; i < NKEYCODE; i++) {
		key = ibuf->keycode[i];
		if (key == 0)
			break;
		for (j = 0; j < NKEYCODE; j++) {
			if (sc->sc_prev_report.keycode[j] == 0)
				break;
			if (key == sc->sc_prev_report.keycode[j])
				goto pfound;
		}
		send_key_event(&ns_ev, nsmod(sc->modifier, trtab_key[map_key_with_lang(key)]), FALSE, TRUE); // pressednsmod(
		pfound:
			;
	}
	
	memcpy(&(sc->sc_prev_report), ibuf, sizeof(struct s_boot_kbd_report));
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
	sc->sc_isize = sizeof(struct s_boot_kbd_report);
	usbd_set_protocol(sc->sc_iface, 0);
	usbd_set_idle(sc->sc_iface, 0, 0);
	sc->sc_ibuf.dummy1 = 0;
	sc->sc_ibuf.dummy2 = 0;
	sc->sc_ibuf.buf = malloc(sc->sc_isize);
	if (!sc->sc_ibuf.buf)
		return (ENXIO);
	sc->sc_enabled = 1;
	memset(&(sc->sc_prev_report), 0, sizeof(struct s_boot_kbd_report));
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
		usbd_abort_pipe(sc->sc_intrpipe);
		usbd_close_pipe(sc->sc_intrpipe);
		free(sc->sc_ibuf.buf);
		sc->sc_enabled = 0;
	}
	return 0;
}

static int (*methods[])(device_t) = {match, attach, detach, NULL};

int main(int __attribute__((unused)) argc, char **argv) {
	// if program name ends with -azerty, use azerty map
	unsigned azerty_suffix_len = strlen("-azerty.tns");
	unsigned prgm_path_len = strlen(argv[0]);
	if (prgm_path_len >= azerty_suffix_len && !strcmp(argv[0] + prgm_path_len - azerty_suffix_len, "-azerty.tns"))
		use_lang_map = map_lang_azerty;
	else
		use_lang_map = NULL;
	
	nl_relocdata((unsigned*)methods, sizeof(methods)/sizeof(methods[0]) - 1);
	usb_register_driver(2, methods, "ukbd", 0, sizeof(struct ukbd_softc));
	nl_set_resident();
	return 0;
}
