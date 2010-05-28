#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emu.h"

struct packet {
	u16 constant;
	struct { u16 addr, service; } src;
	struct { u16 addr, service; } dst;
	u16 data_check;
	u8 data_size;
	u8 ack;
	u8 seqno;
	u8 hdr_check;
	u8 data[255];
};

#define CONSTANT  BSWAP16(0x54FD)
#define SRC_ADDR  BSWAP16(0x6400)
#define DST_ADDR  BSWAP16(0x6401)

u16 usblink_data_checksum(struct packet *packet) {
	u16 check = 0;
	int i, size = packet->data_size;
	for (i = 0; i < size; i++) {
		u16 tmp = check << 12 ^ check << 8;
		check = (packet->data[i] << 8 | check >> 8)
		      ^ tmp ^ tmp >> 5 ^ tmp >> 12;
	}
	return BSWAP16(check);
}

u8 usblink_header_checksum(struct packet *packet) {
	u8 check = 0;
	int i;
	for (i = 0; i < 15; i++) check += ((u8 *)packet)[i];
	return check;
}

static void dump_packet(char *type, void *data, u32 size) {
	if (log_enabled[LOG_USB]) {
		u32 i;
		logprintf(LOG_USB, "%s", type);
		for (i = 0; i < size && i < 24; i++)
			logprintf(LOG_USB, " %02x", ((u8 *)data)[i]);
		if (size > 24)
			logprintf(LOG_USB, "...");
		logprintf(LOG_USB, "\n");
	}
}

struct packet usblink_send_buffer;
void usblink_send_packet() {
	extern void usblink_start_send();
	usblink_send_buffer.constant   = CONSTANT;
	usblink_send_buffer.src.addr   = SRC_ADDR;
	usblink_send_buffer.dst.addr   = DST_ADDR;
	usblink_send_buffer.data_check = usblink_data_checksum(&usblink_send_buffer);
	usblink_send_buffer.hdr_check  = usblink_header_checksum(&usblink_send_buffer);
	dump_packet("send", &usblink_send_buffer, 16 + usblink_send_buffer.data_size);
	usblink_start_send();
}

u8 prev_seqno;
u8 next_seqno() {
	prev_seqno = (prev_seqno == 0xFF) ? 0x01 : prev_seqno + 1;
	return prev_seqno;
}

FILE *put_file;
u32 put_file_size;
int put_file_state;
void put_file_next(struct packet *in) {
	struct packet *out = &usblink_send_buffer;
	switch (put_file_state) {
		case 1: /* Got ACK for 03 */
			put_file_state = 2;
			break;
		case 2: /* Got 04 */
			if (!in || in->data_size != 1 || in->data[0] != 0x04) {
				printf("File send error: Didn't get 04\n");
				put_file_state = 0;
				break;
			}
			put_file_state = 3;
			break;
		case 3: /* Sent ACK for 04, or got ACK for 05 */
			printf("Sending file: %u bytes left\n", put_file_size);
			if (put_file_size > 0) {
				/* Send data */
				u32 len = put_file_size;
				if (len > 253)
					len = 253;
				put_file_size -= len;
				out->src.service = BSWAP16(0x8001);
				out->dst.service = BSWAP16(0x4060);
				out->data_size = 1 + len;
				out->ack = 0;
				out->seqno = next_seqno();
				out->data[0] = 0x05;
				fread(&out->data[1], 1, len, put_file);
				usblink_send_packet();
				break;
			}
			put_file_state = 4;
			break;
		case 4:
			printf("Send done\n");
			put_file_state = 0;
			fclose(put_file);
			put_file = NULL;
			break;
	}
}

void usblink_sent_packet() {
	if (usblink_send_buffer.ack) {
		/* Received packet has been acked */
		if (usblink_send_buffer.dst.service == BSWAP16(0x4060))
			put_file_next(NULL);
	}
}

void usblink_received_packet(u8 *data, u32 size) {
	dump_packet("recv", data, size);
	struct packet *in = (struct packet *)data;
	struct packet *out = &usblink_send_buffer;

	if (in->dst.service == BSWAP16(0x8001))
		put_file_next(in);

	if (in->src.service == BSWAP16(0x4003)) { /* Address request */
		out->src.service = BSWAP16(0x4003);
		out->dst.service = BSWAP16(0x4003);
		out->data_size = 4;
		out->ack = 0;
		out->seqno = 1;
		*(u16 *)&out->data[0] = DST_ADDR;
		*(u16 *)&out->data[2] = BSWAP16(0xFF00);
		usblink_send_packet();
	} else if (!in->ack) {
		/* Send an ACK */
		out->src.service = BSWAP16(0x00FF);
		out->dst.service = in->src.service;
		out->data_size = 2;
		out->ack = 0x0A;
		out->seqno = in->seqno;
		*(u16 *)&out->data[0] = in->dst.service;
		usblink_send_packet();
	}
}

void usblink_put_file(char *filepath) {
	char *filename = filepath;
	char *p;
	for (p = filepath; *p; p++)
		if (*p == ':' || *p == '/' || *p == '\\')
			filename = p + 1;

	FILE *f = fopen(filepath, "rb");
	if (!f)
		return;
	if (put_file)
		fclose(put_file);
	put_file = f;
	fseek(f, 0, SEEK_END);
	put_file_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	put_file_state = 1;

	/* Send the first packet */
	struct packet *out = &usblink_send_buffer;
	out->src.service = BSWAP16(0x8001);
	out->dst.service = BSWAP16(0x4060);
	out->ack = 0;
	out->seqno = next_seqno();
	u8 *data = out->data;
	*data++ = 3;
	*data++ = 1;
	memcpy(data, "/Examples/", 10); data += 10;
	data += strlen(strcpy((char *)data, filename)) + 1;
	*(u32 *)data = BSWAP32(put_file_size); data += 4;
	out->data_size = data - out->data;
	usblink_send_packet();
}

u32 usblink_events;
bool usblink_sending;
bool usblink_reading;
u32 usblink_read_buffer;
u32 usblink_read_size;

u32 usblink_addr_set_event;
u32 usblink_addr_schedule;
u32 usblink_addr_submit_read_buffer;
u32 usblink_addr_submit_write_buffer;
u32 usblink_addr_nav_user;

void usblink_complete_send() {
	usblink_events |= 0x00400000; // read completion event
	usblink_reading = false;
	usblink_sending = false;
	u32 size = 16 + usblink_send_buffer.data_size;
	if (size > usblink_read_size) {
		error("send_size = %x, read size = %x\n", size, usblink_read_size);
	}
	*(u32 *)ram_ptr(usblink_addr_nav_user + 0x1C0, 4) = size;
	memcpy(ram_ptr(usblink_read_buffer, size), &usblink_send_buffer, size);
	usblink_sent_packet();
}

void usblink_start_send() {
	if (usblink_reading) {
		usblink_complete_send();
		if (usblink_events & 0x00800000)
			return; // submit_write_buffer will set the events
		RAM_FLAGS(ram_ptr(usblink_addr_schedule, 4)) |= RF_EXEC_HACK;
	} else {
		usblink_sending = true;
	}
}

void usblink_hook_schedule() {
	//printf("TCT_Schedule()\n");
	arm.reg[0] = usblink_addr_nav_user + 0x194;
	arm.reg[1] = usblink_events;
	arm.reg[2] = 0;
	arm.reg[14] = arm.reg[15];
	arm.reg[15] = usblink_addr_set_event;
	usblink_events = 0;
	RAM_FLAGS(ram_ptr(usblink_addr_schedule, 4)) &= ~RF_EXEC_HACK;
}

void usblink_hook_submit_read_buffer() {
	if (arm.reg[0] != usblink_addr_nav_user)
		error("r0 = %x, nav user = %x\n", arm.reg[0], usblink_addr_nav_user);
	//vprintf("NU_USBF_USER_NAV_SUBMIT_READ_BUFFER(%x,%x,%x)\n", (va_list)arm.reg);
	usblink_read_buffer = arm.reg[1];
	usblink_read_size   = arm.reg[2];
	if (usblink_sending) {
		usblink_complete_send();
		arm.reg[0]  = usblink_addr_nav_user + 0x194;
		arm.reg[1]  = usblink_events;
		arm.reg[2]  = 0;
		arm.reg[15] = usblink_addr_set_event;
		usblink_events = 0;
	} else {
		usblink_reading = true;
		arm.reg[0] = 0;
		arm.reg[15] = arm.reg[14];
	}
}

void usblink_hook_submit_write_buffer() {
	if (arm.reg[0] != usblink_addr_nav_user)
		error("r0 = %x, nav user = %x\n", arm.reg[0], usblink_addr_nav_user);
	//vprintf("NU_USBF_USER_NAV_SUBMIT_WRITE_BUFFER(%x,%x,%x)\n", (va_list)arm.reg);
	usblink_events |= 0x00800000; // write completion event
	usblink_received_packet(ram_ptr(arm.reg[1], arm.reg[2]), arm.reg[2]);
	arm.reg[0] = usblink_addr_nav_user + 0x194;
	arm.reg[1] = usblink_events;
	arm.reg[2] = 0;
	arm.reg[15] = usblink_addr_set_event;
	usblink_events = 0;
}

void usblink_connect() {
	switch (OS_VERSION) {
		case OS_VERSION_1_4_BOOT2:
			usblink_addr_set_event           = 0x11808490;
			usblink_addr_schedule            = 0x1181FDB4;
			usblink_addr_submit_read_buffer  = 0x11842AC8;
			usblink_addr_submit_write_buffer = 0x11842B74;
			usblink_addr_nav_user            = *(u32 *)RAM_PTR(0x11987EDC);
			break;
		case OS_VERSION_1_7_CAS:
			usblink_addr_set_event           = 0x101A62B4;
			usblink_addr_schedule            = 0x10213E54;
			usblink_addr_submit_read_buffer  = 0x1028E98C;
			usblink_addr_submit_write_buffer = 0x1028EA38;
			usblink_addr_nav_user            = *(u32 *)RAM_PTR(0x1072751C);
			break;
		case OS_VERSION_1_7_NON_CAS:
			usblink_addr_set_event           = 0x101A429C;
			usblink_addr_schedule            = 0x10211E44;
			usblink_addr_submit_read_buffer  = 0x1028C97C;
			usblink_addr_submit_write_buffer = 0x1028CA28;
			usblink_addr_nav_user            = *(u32 *)RAM_PTR(0x106F4C14);
			break;
		default:
			printf("Unknown OS version\n");
			return;
	}
	usblink_events = 0x20; // connection event
	RAM_FLAGS(RAM_PTR(usblink_addr_schedule))            |= RF_EXEC_HACK | RF_CODE_NO_TRANSLATE;
	RAM_FLAGS(RAM_PTR(usblink_addr_submit_read_buffer))  |= RF_EXEC_HACK;
	RAM_FLAGS(RAM_PTR(usblink_addr_submit_write_buffer)) |= RF_EXEC_HACK;
	flush_translations();
	prev_seqno = 0xFF;
}

void usblink_disconnect() {
	usblink_events = 0x40; // disconnection event
	usblink_sending = false;
	usblink_reading = false;
	RAM_FLAGS(RAM_PTR(usblink_addr_schedule))            |= RF_EXEC_HACK;
	RAM_FLAGS(RAM_PTR(usblink_addr_schedule))            &= ~RF_CODE_NO_TRANSLATE;
	RAM_FLAGS(RAM_PTR(usblink_addr_submit_read_buffer))  &= ~RF_EXEC_HACK;
	RAM_FLAGS(RAM_PTR(usblink_addr_submit_write_buffer)) &= ~RF_EXEC_HACK;
}
