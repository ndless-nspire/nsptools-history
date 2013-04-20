#include <os.h>
#include <usbdi.h>
#include <usb.h>
#include <nspireio2.h>
#include "hidn.h"

#ifdef DEBUG
nio_console csl;
#endif

int main(int __attribute__((unused)) argc, char **argv) {
	assert_ndless_rev(750); 
	ums_register();
	
	// if program name ends with -azerty, use azerty map
	unsigned azerty_suffix_len = strlen("-azerty.tns");
	unsigned prgm_path_len = strlen(argv[0]);
	if (prgm_path_len >= azerty_suffix_len && !strcmp(argv[0] + prgm_path_len - azerty_suffix_len, "-azerty.tns"))
			ukbd_register(TRUE);
	else 
			ukbd_register(FALSE);

	nl_set_resident();
	if (!nl_isstartup()) {
		nl_no_scr_redraw();
		nio_grid_puts(0, 0, 12, 1, "USB HID drivers installed!", is_cx ? NIO_COLOR_BLACK : NIO_COLOR_WHITE, is_cx ? NIO_COLOR_WHITE : NIO_COLOR_BLACK);
	}
	return 0;
}
