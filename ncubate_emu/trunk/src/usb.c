#include <stdio.h>
#include "emu.h"

/* B0000000 (and B4000000?): USB */
u8 usb_read_byte(u32 addr) {
	//printf("[usb readb %08x]\n", addr);
	if ((addr & 0x1FF) == 0x100) return 0x40; // CAPLENGTH: operational registers start at +40
	return bad_read_byte(addr);
}
u16 usb_read_half(u32 addr) {
	//printf("[usb readh %08x]\n", addr);
	if ((addr & 0x1FF) == 0x102) return 0x0100; // HCIVERSION: EHCI 1.0
	return bad_read_half(addr);
}
u32 usb_read_word(u32 addr) {
	//printf("[usb read  %08x]\n", addr);
	switch (addr & 0x1FF) {
		/* Module identification registers */
		case 0x000: return 0x0042FA05; // ID: revision 0x42, ID 5
		case 0x004: return 0x000002C5; // HWGENERAL
		case 0x008: return 0x10020001; // HWHOST
		case 0x00C: return 0x00000009; // HWDEVICE
		case 0x010: return 0x80050708; // HWTXBUF
		case 0x014: return 0x00000508; // HWRXBUF

		/* Capability registers */
		case 0x100: return 0x01000040; // CAPLENGTH and HCIVERSION
		case 0x104: return 0x00010011; // HCSPARAMS: Port indicator control, port power control, 1 port
		case 0x108: return 0x00000006; // HCCPARAMS: Asynchronous schedule park, programmable frame list
		case 0x10C: return 0x00000000; // HCSP-PORTROUTE

		case 0x120: return 0x00000001; // DCIVERSION: 0.1
		case 0x124: return 0x00000184; // DCCPARAMS: Host capable, device capable, 4 endpoints

		/* Operational registers */
		case 0x140: return 0;
		case 0x164: return 0;
		case 0x180: return 0;
		case 0x184: return 0;
		case 0x188: return 0;
		// returning 0 causes OS 1.1 to detect a "probe" (?)
		// and hang trying to communicate with it
		case 0x1A4: return -1;
		case 0x1B0: return 0;
		case 0x1B4: return 0;
		case 0x1C0: return 0;
	}
	return bad_read_word(addr);
}
void usb_write_word(u32 addr, u32 value) {
	//printf("[usb write %08x %08x]\n", addr, value);
	switch (addr & 0x1FF) {
		/* Device/host timer registers */
		case 0x080: return; // used by diags
		case 0x084: return; // used by diags

		/* Operational registers */
		case 0x140: return;
		case 0x144: return;
		case 0x148: return;
		case 0x150: return;
		case 0x158: return;
		case 0x180: return;
		case 0x184: return;
		case 0x188: return;
		case 0x1A4: return;
		case 0x1A8: return;
		case 0x1AC: return;
		case 0x1B4: return;
		case 0x1C0: return;
	}
	bad_write_word(addr, value);
}
