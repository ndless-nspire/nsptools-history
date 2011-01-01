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

#if 0
static void dump_packet(__attribute__((char * __attribute__((unused)) type, void * __attribute__((unused)) data, u32 __attribute__((unused)) size) {
#else
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

bool usblink_sending;
u32 usblink_read_thread;
u32 usblink_read_buffer;
u32 usblink_read_size;
u32 usblink_read_size_addr;
u32 *usblink_read_size_ptr;

enum { DISCONNECTED, CONNECTING, CONNECTED, DISCONNECTING } usblink_state;

struct os_version {
	u32  initialize;        // INT_Initialize (used for OS version identification)

	u32  set_events;        // EVC_Set_Events
	u32  schedule;          // TCT_Schedule
	u32  suspend_service;   // TCC_Suspend_Service
	u32  resume_service;    // TCC_Resume_Service
	u32 *current_thread;    // TCD_Current_Thread

	u32 *event_struct;      // Address of a pointer to a structure containing event group
	u16  event_offset;      // Offset of event group within structure
	u8   event_connect;     // Connect event
	u8   event_disconnect;  // Disconnect event

	u32  stream_names;      // { "nSpireDev0" }
	u32 *cn_funcs;          // Connector functions
};

#define OSV(i,se,s,ss,rs,ct,es,eo,ec,ed,sn,cn) { \
	0x##i, 0x##se, 0x##s, 0x##ss, 0x##rs, MEM_PTR(0x##ct), \
	MEM_PTR(0x##es), 0x##eo, 0x##ec, 0x##ed, 0x##sn, MEM_PTR(0x##cn) }
const struct os_version all_versions[] = {
/* BOOT2 1.1.8981  */ OSV(1181e610,1180788c,1181f194,1181dee0,1181daf4,119cf304,119d1a9c,194,20,40,11841694,119c1500),
/*   CAS 1.1.9170  */ OSV(1014a9f0,100f6618,1014b574,1014a2b8,10149ecc,10562344,10564d1c,194,20,40,10172bc8,1053bad0),
/*NonCAS 1.1.9253  */ OSV(1014a9c0,100f65e8,1014b544,1014a288,10149e9c,1058e344,10590d1c,194,20,40,10172ec4,10567ad0),
/*   CAS 1.2.2394  */ OSV(10193530,10137898,101940c4,10192df8,10192a0c,10642e94,105cb5a4,194,20,40,101bf584,1061c460),
/*NonCAS 1.2.2398  */ OSV(101919a0,10135d08,10192534,10191268,10190e7c,1066aea4,1058bb8c,194,20,40,101bd9f4,10644470),
/*   CAS 1.3.2437  */ OSV(101a2760,10146acc,101a32f4,101a202c,101a1c40,10657384,105e2860,194,20,40,101cf8dc,10630a30),
/*NonCAS 1.3.2438  */ OSV(101a0c30,10144f94,101a17c4,101a04f4,101a0108,10693394,105b27dc,194,20,40,101cddac,1066ca40),
/* BOOT2 1.4.1571  */ OSV(1181f220,11808490,1181fdb4,1181eae4,1181e6f8,119cd2c4,11987edc,194,20,40,11846538,119bf4b0),
/*   CAS 1.4.11643 */ OSV(10362cb0,102f5cd4,10363844,103622b4,10361ec8,107980e4,106e85ec,194,20,40,10392710,107713e0),
/*NonCAS 1.4.11653 */ OSV(103141a0,102a71c4,10314d34,103137a4,103133b8,107d40e4,106b5140,194,20,40,10343c00,107ad3e0),
/*   CAS 1.6.4295  */ OSV(10205e90,10198ea0,10206a44,10205480,10205094,107b0874,106f13e4,194,20,40,10286f88,10789eb0),
/*NonCAS 1.6.4379  */ OSV(10203ea0,10196eb8,10204a54,10203498,102030ac,107ec874,106bfae8,194,20,40,10284f98,107c5eb0),
/*   CAS 1.7.2741  */ OSV(102132a0,101a62b4,10213e54,10212894,102124a8,107e9574,1072751c,194,20,40,10292418,10778ce0),
/*NonCAS 1.7.2741  */ OSV(10211290,101a429c,10211e44,1021087c,10210490,10825584,106f4c14,194,20,40,10290408,107b4cf0),
/*   CAS 1.7.1.50  */ OSV(1021c4a0,101ae904,1021d054,1021ba94,1021b6a8,107db614,10748244,194,20,40,1029d174,107a4cf0),
/*NonCAS 1.7.1.50  */ OSV(1021a490,101ac8ec,1021b044,10219a7c,10219690,10817624,10715934,194,20,40,1029b164,107e0d00),
/*   CAS 1.7.2.59  */ OSV(1021c490,101ae8ec,1021d044,1021ba7c,1021b690,107db614,1074821c,194,20,40,1029d164,107a4cf0),
/*   CAS 2.0.0.1188*/ OSV(102507d0,101e2610,10251394,1024fdbc,1024f9d0,10822584,102c950c,028,01,01,102c92d8,107df5c0),
/*NonCAS 2.0.0.1188*/ OSV(1024ff00,101e1d40,10250ac4,1024f4ec,1024f100,1085e594,102c8c3c,028,01,01,102c8a08,1081b5d0),
/*   CAS 2.0.1.60  */ OSV(10266900,101f8740,102674c4,10265eec,10265b00,1082a584,102df63c,028,01,01,102df408,107e75c0),
/*NonCAS 2.0.1.60  */ OSV(10266030,101f7e78,10266bf4,10265624,10265238,10862594,102ded6c,028,01,01,102deb38,1081f5d0),
/*   CAS 2.1.0.631 */ OSV(1027a640,1020c480,1027b204,10279c2c,10279840,1093aca4,102e0314,008,01,01,102e0064,108f7ce0),
/*NonCAS 2.1.0.631 */ OSV(10279d70,1020bbb8,1027a934,10279364,10278f78,10976ca4,102dfa44,008,01,01,102df794,10933ce0),
};
#undef OSV

const struct os_version *usblink_addrs;
u32 usblink_hooks[USBLINK_NUM_HOOKS];

void usblink_complete_send() {
	usblink_read_thread = 0;
	usblink_sending = false;
	u32 size = 16 + usblink_send_buffer.data_size;
	if (size > usblink_read_size) {
		error("send_size = %x, read size = %x\n", size, usblink_read_size);
	}
	*usblink_read_size_ptr = size;
	memcpy(phys_mem_ptr(usblink_read_buffer, size), &usblink_send_buffer, size);
	usblink_sent_packet();
}

void usblink_start_send() {
	usblink_sending = true;
}

void usblink_reset() {
	int hook;
	if (usblink_hooks[0]) {
		for (hook = 0; hook < USBLINK_NUM_HOOKS; hook++) {
			usblink_hooks[hook] = 0;
			RAM_FLAGS(phys_mem_ptr(usblink_hooks[hook], 4)) &= ~RF_EXEC_HACK;
		}
	}
	usblink_state = DISCONNECTED;
}

int usblink_hook(int hook) {
	u32 *retsize;
	switch (hook) {
		case 0: // TCT_Schedule
			if (usblink_state == CONNECTING) {
				arm.reg[0] = *usblink_addrs->event_struct + usblink_addrs->event_offset;
				arm.reg[1] = usblink_addrs->event_connect;
				arm.reg[2] = 0;
				arm.reg[14] = arm.reg[15];
				arm.reg[15] = usblink_addrs->set_events;
				usblink_state = CONNECTED;
				printf("usblink connected\n");
				return 1;
			}
			if (usblink_state == DISCONNECTING) {
				if (usblink_read_thread) {
					arm.reg[0] = usblink_read_thread;
					arm.reg[14] = arm.reg[15];
					arm.reg[15] = usblink_addrs->resume_service;
					usblink_read_thread = 0;
					*usblink_read_size_ptr = 0;
					return 1;
				}
				arm.reg[0] = *usblink_addrs->event_struct + usblink_addrs->event_offset;
				arm.reg[1] = usblink_addrs->event_disconnect;
				arm.reg[2] = 0;
				arm.reg[14] = arm.reg[15];
				arm.reg[15] = usblink_addrs->set_events;
				usblink_state = DISCONNECTED;
				return 1;
			}
			if (usblink_sending && usblink_read_thread) {
				arm.reg[14] = arm.reg[15];
				arm.reg[0] = usblink_read_thread;
				arm.reg[15] = usblink_addrs->resume_service;
				usblink_complete_send();
				return 1;
			}
			break;
		case 1: // Connector: enumerate streams
			//printf("EnumerateStreams(%p)\n", arm.reg[0]);
			retsize = phys_mem_ptr(arm.reg[0], 4);
			if (!retsize)
				error("bad usblink hook call");
			*retsize = (usblink_state != DISCONNECTED);
			arm.reg[0] = usblink_addrs->stream_names;
			arm.reg[15] = arm.reg[14];
			return 1;
		case 2: // Connector: get stream type
			//printf("GetStreamType(%p)\n", arm.reg[0]);
			arm.reg[0] = 2; // host
			arm.reg[15] = arm.reg[14];
			return 1;
		case 3: // Connector: read
			//vprintf("Read(%p,%p,%p,%p)\n", (void *)&arm.reg[0]);
			if (usblink_state == DISCONNECTED) {
				arm.reg[0] = -4;
				arm.reg[15] = arm.reg[14];
				usblink_reset();
				printf("usblink disconnected\n");
				return 1;
			}
			usblink_read_thread = *usblink_addrs->current_thread;
			usblink_read_buffer = arm.reg[1];
			usblink_read_size = arm.reg[2];
			usblink_read_size_addr = arm.reg[3];
			usblink_read_size_ptr = phys_mem_ptr(usblink_read_size_addr, 4);
			if (!usblink_read_size_ptr)
				error("bad usblink hook call");
			if (usblink_sending) {
				usblink_complete_send();
				arm.reg[0] = 0;
				arm.reg[15] = arm.reg[14];
			} else {
				arm.reg[0] = usblink_read_thread;
				arm.reg[15] = usblink_addrs->suspend_service;
			}
			return 1;
		case 4: // Connector: write
			//vprintf("Write(%p,%p,%p,%p)\n", (void *)&arm.reg[0]);
			if (usblink_state == DISCONNECTED) {
				//debugger();
				arm.reg[0] = -4;
				arm.reg[15] = arm.reg[14];
				return 1;
			}
			retsize = phys_mem_ptr(arm.reg[3], 4);
			if (!retsize)
				error("bad usblink hook call");
			usblink_received_packet(phys_mem_ptr(arm.reg[1], arm.reg[2]), arm.reg[2]);
			*retsize = arm.reg[2];
			arm.reg[0] = 0;
			arm.reg[15] = arm.reg[14];
			return 1;
	}
	return 0;
}

void usblink_connect() {
	int i, hook;

	if (usblink_hooks[0])
		goto hooks_done;

	for (i = 0; i != sizeof(all_versions) / sizeof(*all_versions); i++) {
		if (*(u32 *)MEM_PTR(0xA4000020) == all_versions[i].initialize) {
			usblink_addrs = &all_versions[i];
			goto found;
		}
	}
	printf("Unknown OS version\n");
	return;

found:
	usblink_hooks[0] = usblink_addrs->schedule;
	memcpy(&usblink_hooks[1], &usblink_addrs->cn_funcs[1], 16);
#if USBLINK_NUM_HOOKS != 5
#error don't forget to update the .h
#endif
	for (hook = 0; hook < USBLINK_NUM_HOOKS; hook++) {
		u32 addr = usblink_hooks[hook];
		u32 *ptr = phys_mem_ptr(addr, 4);
		if (!ptr)
			error("usblink: bad address %08x for hook %d", addr, hook);
		RAM_FLAGS(ptr) |= RF_EXEC_HACK;
	}

	flush_translations();
hooks_done:
	usblink_state = CONNECTING;
	prev_seqno = 0xFF;
}

void usblink_disconnect() {
	usblink_sending = false;
	usblink_state = DISCONNECTING;
}

struct usblink_saved_state {
	u8 prev_seqno;
	u32 usblink_events;
	bool usblink_sending;
	u32 usblink_read_thread;
	u32 usblink_read_buffer;
	u32 usblink_read_size;
	u32 usblink_read_size_addr;
};

void *usblink_save_state(size_t *size) {
	*size = sizeof(struct usblink_saved_state);
	struct usblink_saved_state *state = malloc(*size);
	state->prev_seqno = prev_seqno;
	state->usblink_sending = usblink_sending;
	state->usblink_read_thread = usblink_read_thread;
	state->usblink_read_buffer = usblink_read_buffer;
	state->usblink_read_size = usblink_read_size;
	state->usblink_read_size_addr = usblink_read_size_addr;
	return state;
}

void usblink_reload_state(void *state) {
	struct usblink_saved_state *_state = (struct usblink_saved_state *)state;
	prev_seqno = _state->prev_seqno;
	usblink_sending = _state->usblink_sending;
	usblink_read_thread = _state->usblink_read_thread;
	usblink_read_buffer = _state->usblink_read_buffer;
	usblink_read_size = _state->usblink_read_size;
	usblink_read_size_addr = _state->usblink_read_size_addr;
	usblink_read_size_ptr = phys_mem_ptr(_state->usblink_read_size_addr, 4);
}
