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
#if 0
	if (log_enabled[LOG_USB]) {
		u32 i;
		logprintf(LOG_USB, "%s", type);
		for (i = 0; i < size && i < 24; i++)
			logprintf(LOG_USB, " %02x", ((u8 *)data)[i]);
		if (size > 24)
			logprintf(LOG_USB, "...");
		logprintf(LOG_USB, "\n");
	}
#endif
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
u16 put_file_port;
enum {
	SENDING_03         = 1,
	RECVING_04         = 2,
	ACKING_04_or_FF_00 = 3,
	SENDING_05         = 4,
	RECVING_FF_00      = 5,
	DONE               = 6,
	EXPECT_FF_00       = 16, // Sent to us after first OS data packet
} put_file_state;
void put_file_next(struct packet *in) {
	struct packet *out = &usblink_send_buffer;
	switch (put_file_state & 15) {
		case SENDING_03:
			if (!in || in->ack != 0x0A) goto fail;
			put_file_state++;
			break;
		case RECVING_04:
			if (!in || in->data_size != 1 || in->data[0] != 0x04) {
				printf("File send error: Didn't get 04\n");
				goto fail;
			}
			put_file_state++;
			break;
		case ACKING_04_or_FF_00:
			if (in) goto fail;
			put_file_state++;
		send_data:
			if (prev_seqno == 1)
				printf("Sending file: %u bytes left\n", put_file_size);
			if (put_file_size > 0) {
				/* Send data (05) */
				u32 len = put_file_size;
				if (len > 253)
					len = 253;
				put_file_size -= len;
				out->src.service = BSWAP16(0x8001);
				out->dst.service = put_file_port;
				out->data_size = 1 + len;
				out->ack = 0;
				out->seqno = next_seqno();
				out->data[0] = 0x05;
				fread(&out->data[1], 1, len, put_file);
				usblink_send_packet();
				break;
			}
			printf("Send complete\n");
			put_file_state = DONE;
			break;
		case SENDING_05:
			if (!in || in->ack != 0x0A) goto fail;
			if (put_file_state & EXPECT_FF_00) {
				put_file_state++;
				break;
			}
			goto send_data;
		case RECVING_FF_00: /* Got FF 00: OS header is valid */
			if (!in || in->data_size != 2 || in->data[0] != 0xFF || in->data[1]) {
				printf("File send error: Didn't get FF 00\n");
				goto fail;
			}
			put_file_state = ACKING_04_or_FF_00;
			break;
		fail:
			printf("Send failed\n");
		case DONE:
			put_file_state = 0;
			fclose(put_file);
			put_file = NULL;
			break;
	}
}

void usblink_sent_packet() {
	if (usblink_send_buffer.ack) {
		/* Received packet has been acked */
		u16 service = usblink_send_buffer.dst.service;
		if (service == BSWAP16(0x4060) || service == BSWAP16(0x4080))
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

void usblink_put_file(char *filepath, char *folder) {
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
	out->dst.service = put_file_port = BSWAP16(0x4060);
	out->ack = 0;
	out->seqno = next_seqno();
	u8 *data = out->data;
	*data++ = 3;
	*data++ = 1;
	data += sprintf((char *)data, "/%s/%s", folder, filename) + 1;
	*(u32 *)data = BSWAP32(put_file_size); data += 4;
	out->data_size = data - out->data;
	usblink_send_packet();
}

void usblink_send_os(char *filepath) {
	FILE *f = fopen(filepath, "rb");
	if (!f)
		return;
	if (put_file)
		fclose(put_file);
	put_file = f;
	fseek(f, 0, SEEK_END);
	put_file_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	put_file_state = 1 | 16;

	/* Send the first packet */
	struct packet *out = &usblink_send_buffer;
	out->src.service = BSWAP16(0x8001);
	out->dst.service = put_file_port = BSWAP16(0x4080);
	out->ack = 0;
	out->seqno = next_seqno();
	u8 *data = out->data;
	*data++ = 3;
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
	*(u32 *)phys_mem_ptr(usblink_addr_nav_user + 0x1C0, 4) = size;
	memcpy(phys_mem_ptr(usblink_read_buffer, size), &usblink_send_buffer, size);
	usblink_sent_packet();
}

void usblink_start_send() {
	if (usblink_reading) {
		usblink_complete_send();
		if (usblink_events & 0x00800000)
			return; // submit_write_buffer will set the events
		RAM_FLAGS(phys_mem_ptr(usblink_addr_schedule, 4)) |= RF_EXEC_HACK;
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
	RAM_FLAGS(phys_mem_ptr(usblink_addr_schedule, 4)) &= ~RF_EXEC_HACK;
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
	usblink_received_packet(phys_mem_ptr(arm.reg[1], arm.reg[2]), arm.reg[2]);
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
  			usblink_addr_nav_user            = *(u32 *)MEM_PTR(0x11987EDC);
  			break;
		case OS_VERSION_1_1_CAS:
			usblink_addr_set_event           = 0x100F6618;
			usblink_addr_schedule            = 0x1014B574;
			usblink_addr_submit_read_buffer  = 0x1017098C;
			usblink_addr_submit_write_buffer = 0x10170A38;
			usblink_addr_nav_user            = *(u32 *)MEM_PTR(0x10564D1C);
			break;
		case OS_VERSION_1_1_NON_CAS:
			usblink_addr_set_event           = 0x100F65E8;
			usblink_addr_schedule            = 0x1014B544;
			usblink_addr_submit_read_buffer  = 0x10170C88;
			usblink_addr_submit_write_buffer = 0x10170D34;
			usblink_addr_nav_user            = *(u32 *)MEM_PTR(0x10590D1C);
			break;
		case OS_VERSION_1_7_CAS:
  			usblink_addr_set_event           = 0x101A62B4;
  			usblink_addr_schedule            = 0x10213E54;
  			usblink_addr_submit_read_buffer  = 0x1028E98C;
  			usblink_addr_submit_write_buffer = 0x1028EA38;
  			usblink_addr_nav_user            = *(u32 *)MEM_PTR(0x1072751C);
  			break;
		case OS_VERSION_1_7_NON_CAS:
  			usblink_addr_set_event           = 0x101A429C;
  			usblink_addr_schedule            = 0x10211E44;
  			usblink_addr_submit_read_buffer  = 0x1028C97C;
				break;
		default:
			printf("Unknown OS version\n");
			return;
	}
	usblink_events = 0x20; // connection event
	RAM_FLAGS(MEM_PTR(usblink_addr_schedule))            |= RF_EXEC_HACK | RF_CODE_NO_TRANSLATE;
	RAM_FLAGS(MEM_PTR(usblink_addr_submit_read_buffer))  |= RF_EXEC_HACK;
	RAM_FLAGS(MEM_PTR(usblink_addr_submit_write_buffer)) |= RF_EXEC_HACK;
	flush_translations();
	prev_seqno = 0xFF;
}

void usblink_disconnect() {
	usblink_events = 0x40; // disconnection event
	usblink_sending = false;
	usblink_reading = false;
	RAM_FLAGS(MEM_PTR(usblink_addr_schedule))            |= RF_EXEC_HACK;
	RAM_FLAGS(MEM_PTR(usblink_addr_schedule))            &= ~RF_CODE_NO_TRANSLATE;
	RAM_FLAGS(MEM_PTR(usblink_addr_submit_read_buffer))  &= ~RF_EXEC_HACK;
	RAM_FLAGS(MEM_PTR(usblink_addr_submit_write_buffer)) &= ~RF_EXEC_HACK;
}

struct usblink_saved_state {
	u8 prev_seqno;
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
};

void *usblink_save_state(size_t *size) {
	*size = sizeof(struct usblink_saved_state);
	struct usblink_saved_state *state = malloc(*size);
	state->prev_seqno = prev_seqno;
	state->usblink_events = usblink_events;
	state->usblink_sending = usblink_sending;
	state->usblink_reading = usblink_reading;
	state->usblink_read_buffer = usblink_read_buffer;
	state->usblink_read_size = usblink_read_size;
	state->usblink_addr_set_event = usblink_addr_set_event;
	state->usblink_addr_schedule = usblink_addr_schedule;
	state->usblink_addr_submit_read_buffer = usblink_addr_submit_read_buffer;
	state->usblink_addr_submit_write_buffer = usblink_addr_submit_write_buffer;
	state->usblink_addr_nav_user = usblink_addr_nav_user;
	return state;
}

void usblink_reload_state(void *state) {
	struct usblink_saved_state *_state = (struct usblink_saved_state *)state;
	prev_seqno = _state->prev_seqno;
	usblink_events = _state->usblink_events;
	usblink_sending = _state->usblink_sending;
	usblink_reading = _state->usblink_reading;
	usblink_read_buffer = _state->usblink_read_buffer;
	usblink_read_size = _state->usblink_read_size;
	usblink_addr_set_event = _state->usblink_addr_set_event;
	usblink_addr_schedule = _state->usblink_addr_schedule;
	usblink_addr_submit_read_buffer = _state->usblink_addr_submit_read_buffer;
	usblink_addr_submit_write_buffer = _state->usblink_addr_submit_write_buffer;
	usblink_addr_nav_user = _state->usblink_addr_nav_user;
}
