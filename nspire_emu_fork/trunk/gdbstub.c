/*
 * TODO:
 * - Fix cpsr not displayed by GDB/IDA
 * - 'Fail to bind' when restarting CPU from the menu
 * - Trap exceptions (trap_low, handle_exception defined but not used, set_mem_fault_trap, trap...)
 * - Signals (hard_trap_info, sig...)
 * - Breakpoints ("Need to flush the instruction cache here")
 * - Sync up at startup ("This function will generate a breakpoint exception")
 * 
 */

/*
 * Some parts derived from GDB's sparc-stub.c.
 * Refer to Appendix D - GDB Remote Serial Protocol in GDB's documentation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __MINGW32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "emu.h"

// #define TRACE_PACKETS 1

static int socket_fd;

static void log_socket_error(const char *msg) {
#ifdef __MINGW32__
	printf("%s: %i\n", msg, WSAGetLastError());
#else
	perror(msg);
#endif
}

static char sockbuf[4096];
static char *sockbufptr = sockbuf;

static void flush_out_buffer(void) {
	char *p = sockbuf;
	while (p != sockbufptr) {
		int n = send(socket_fd, p, sockbufptr-p, 0);
		if (n == -1) {
			log_socket_error("Failed to send to GDB stub socket");
			break;
		}
		p += n;
	}
	sockbufptr = sockbuf;
}

static void put_debug_char(char c) {
#if TRACE_PACKETS
	printf("%c", c);
#endif
	if (sockbufptr == sockbuf + sizeof sockbuf)
		flush_out_buffer();
	*sockbufptr++ = c;
}

static char get_debug_char(void) {
	char c;
	int r;
#if TRACE_PACKETS
	printf("%c", c);
#endif
	r = recv(socket_fd, &c, 1, 0);
	if (r == -1) {
		log_socket_error("Failed to recv from GDB stub socket");
		// TODO disconnect
	} else if(r == 0) {
		// TODO disconnect
		puts("GDB disconnected.");
		return -1;
	}
	return c;
}

static void wait_gdb_connection(int port) {
	int listen_socket_fd;
	struct sockaddr_in sockaddr;
	int r, on;
	
#ifdef __MINGW32__
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData)) {
		log_socket_error("WSAStartup failed");
		exit(1);
	}
#endif

	listen_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_socket_fd == -1) {
		log_socket_error("Failed to create GDB stub socket");
		exit(1);
	}
	memset (&sockaddr, '\000', sizeof sockaddr);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	r = bind(listen_socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	if (r == -1) {
		log_socket_error("Failed to bind GDB stub socket");
		exit(1);
	}
	r = listen(listen_socket_fd, 0);
	if (r == -1) {
		log_socket_error("Failed to listen on GDB stub socket");
	}
	puts("Waiting for GDB to connect...");
	socket_fd = accept(listen_socket_fd, NULL, NULL);
	if (socket_fd == -1) {
		log_socket_error("Failed to accept on GDB stub socket");
	}
	close(listen_socket_fd);
	/* Disable Nagle for low latency */
	on = 1;
#ifdef __MINGW32__
	r = setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
#else
	r = setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
#endif
	if (r == -1)
		log_socket_error("setsockopt(TCP_NODELAY) failed for GDB stub socket");
	puts("GDB connected.");
}

/* BUFMAX defines the maximum number of characters in inbound/outbound buffers */
/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 2048

static void set_mem_fault_trap();

static const char hexchars[]="0123456789abcdef";

#define NUMREGS 26

/* Number of bytes of registers. */
#define NUMREGBYTES (NUMREGS * 4)
enum regnames {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC,
	F0, F1, F2, F3, F4, F5, F6, F7, FPS, CPSR};

// see GDB's gdb/signal.h
enum target_signal {SIGNAL_TRAP = 5};

/* Convert ch from a hex digit to an int */
static int hex(unsigned char ch) {
	if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	if (ch >= '0' && ch <= '9')
		return ch-'0';
	if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	return -1;
}

static char remcomInBuffer[BUFMAX];
static char remcomOutBuffer[BUFMAX];

/* scan for the sequence $<data>#<checksum> */
unsigned char *getpacket(void) {
	unsigned char *buffer = &remcomInBuffer[0];
	unsigned char checksum;
	unsigned char xmitcsum;
	int count;
	char ch;

	while (1) {
		/* wait around for the start character, ignore all other characters */
		do {
			ch = get_debug_char();
			// TODO handle disconnections
			if (ch == -1)
				exit(1);
		}	while (ch != '$');
		
retry:
		checksum = 0;
		xmitcsum = -1;
		count = 0;
		
		/* now, read until a # or end of buffer is found */
		while (count < BUFMAX - 1) {
			ch = get_debug_char();
			if (ch == '$')
				goto retry;
			if (ch == '#')
				break;
			checksum = checksum + ch;
			buffer[count] = ch;
			count = count + 1;
		}
		buffer[count] = 0;

		if (ch == '#') {
			ch = get_debug_char();
			xmitcsum = hex(ch) << 4;
			ch = get_debug_char();
			xmitcsum += hex(ch);

			if (checksum != xmitcsum) {
				put_debug_char('-');	/* failed checksum */
				flush_out_buffer();
			}	else {
				put_debug_char('+');	/* successful transfer */
				
				/* if a sequence char is present, reply the sequence ID */
				if(buffer[2] == ':') {
					put_debug_char(buffer[0]);
					put_debug_char(buffer[1]);
					flush_out_buffer();
					return &buffer[3];
				}
				flush_out_buffer();
				return &buffer[0];
			}
		}
	}
}

/* send the packet in buffer.  */
static void putpacket(unsigned char *buffer) {
	unsigned char checksum;
	int count;
	unsigned char ch;

	/*  $<packet info>#<checksum>. */
	do {
			put_debug_char('$');
			checksum = 0;
			count = 0;

			while ((ch = buffer[count])) {
				put_debug_char(ch);
				checksum += ch;
				count += 1;
			}

			put_debug_char('#');
			put_debug_char(hexchars[checksum >> 4]);
			put_debug_char(hexchars[checksum & 0xf]);
			flush_out_buffer();
		}
	while (get_debug_char() != '+');
}

/* Indicate to caller of mem2hex or hex2mem that there has been an
	 error.  */
static volatile int mem_err = 0;

/* Convert the memory pointed to by mem into hex, placing result in buf.
 * Return a pointer to the last char put in buf (null), in case of mem fault,
 * return 0.
 * If MAY_FAULT is non-zero, then we will handle memory faults by returning
 * a 0, else treat a fault like any other fault in the stub.
 */
static unsigned char * mem2hex(unsigned char *mem, unsigned char *buf, int count, int may_fault) {
	unsigned char ch;

	set_mem_fault_trap(may_fault);
	while (count-- > 0)	{
		ch = *mem++;
		if (mem_err)
			return 0;
		*buf++ = hexchars[ch >> 4];
		*buf++ = hexchars[ch & 0xf];
	}
	*buf = 0;
	set_mem_fault_trap(0);
	return buf;
}

/* convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written */
static char *hex2mem(unsigned char *buf, unsigned char *mem, int count, int may_fault) {
	int i;
	unsigned char ch;

	set_mem_fault_trap(may_fault);
	for (i=0; i<count; i++) {
		ch = hex(*buf++) << 4;
		ch |= hex(*buf++);
		*mem++ = ch;
		if (mem_err)
			return 0;
	}
	set_mem_fault_trap(0);
	return mem;
}

// TODO
static void set_mem_fault_trap(int enable) {
}

/*
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 */
static int hexToInt(char **ptr, int *intValue) {
	int numChars = 0;
	int hexValue;
	
	*intValue = 0;
	while (**ptr) {
		hexValue = hex(**ptr);
		if (hexValue < 0)
			break;
		*intValue = (*intValue << 4) | hexValue;
		numChars ++;
		(*ptr)++;
	}

	return (numChars);
}

/* From emu to GDB. Returns regbuf. */
static unsigned long *get_registers(unsigned long regbuf[NUMREGS]) {
	// GDB's format in arm-tdep.c/arm_register_names
	memset(regbuf, 0, sizeof(unsigned long) * NUMREGS);
	memcpy(regbuf, arm.reg, sizeof(unsigned long) * 16);
	regbuf[NUMREGS-1] = (unsigned long)get_cpsr();
	return regbuf;
}

/* From GDB to emu */
static void set_registers(const unsigned long regbuf[NUMREGS]) {
	memcpy(arm.reg, regbuf, sizeof(unsigned long) * 16);
	set_cpsr_full(regbuf[NUMREGS-1]);
}

#define append_hex_char(ptr,ch) do {*ptr++ = hexchars[(ch) >> 4]; *ptr++ = hexchars[(ch) & 0xf];} while (0)

void send_signal_reply(int signal) {
	char *ptr = remcomOutBuffer;
	*ptr++ = 'T';
	append_hex_char(ptr, signal);
	append_hex_char(ptr, 13);
	*ptr++ = ':';
	ptr = mem2hex((char *)&arm.reg[13], ptr, sizeof(u32), 0);
	*ptr++ = ';';
	append_hex_char(ptr, 15);
	*ptr++ = ':';
	ptr = mem2hex((char *)&arm.reg[15], ptr, sizeof(u32), 0);
	*ptr++ = ';';
	*ptr++ = 0;
	putpacket(remcomOutBuffer);
}

extern void breakinst();

void handle_exception(void) {
	send_signal_reply(SIGNAL_TRAP);
}

void gdbstub_loop(void) {
	int addr, val;
	int length;
	char *ptr;
	void *ramaddr;
	unsigned long regbuf[NUMREGS];
	static bool first_pkt_received = 0;
	
	if (first_pkt_received)
		send_signal_reply(SIGNAL_TRAP);
	cpu_events &= ~EVENT_DEBUG_STEP;
	
	while (1)	{
		remcomOutBuffer[0] = 0;

		ptr = getpacket();
		first_pkt_received = 1;
		switch (*ptr++) 	{
			case '?':
				send_signal_reply(SIGNAL_TRAP);
				break;

			case 'g':  /* return the value of the CPU registers */
				get_registers(regbuf);
				ptr = remcomOutBuffer;
				ptr = mem2hex((char *)regbuf, ptr, NUMREGS * sizeof(unsigned long), 0); 
				break;
		
			case 'G':  /* set the value of the CPU registers - return OK */
				hex2mem(ptr, (char *)regbuf, NUMREGS * sizeof(unsigned long), 0);
				set_registers(regbuf);
				strcpy(remcomOutBuffer,"OK");
				break;
				
			case 'p': /* pn Read the value of register n */
				if (hexToInt(&ptr, &addr) && (size_t)addr < sizeof(regbuf)) {
					mem2hex((char*)(get_registers(regbuf) + addr), remcomOutBuffer, sizeof(unsigned long), 0);
				} else {
					strcpy(remcomOutBuffer,"E01");
				}
				break;
				
			case 'P': /* Pn=r Write register n with value r */
				ptr = strtok(ptr, "=");
				if (hexToInt(&ptr, &addr)
					  && (ptr=strtok(NULL, ""))
					  && hexToInt(&ptr, &val)
					  && (size_t)addr < sizeof(regbuf)) {
					get_registers(regbuf)[addr] = val;
					set_registers(regbuf);
					strcpy(remcomOutBuffer, "OK");
				} else {
					strcpy(remcomOutBuffer,"E01");
				}
				break;
		
			case 'm':  /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
				/* Try to read %x,%x */
				if (hexToInt(&ptr, &addr)
				    && *ptr++ == ','
				    && hexToInt(&ptr, &length)) {
					ramaddr = ram_ptr(addr, length);
					if (!ramaddr || mem2hex((char *)ramaddr, remcomOutBuffer, length, 1))
						break;
					strcpy(remcomOutBuffer, "E03");
				}	else
					strcpy(remcomOutBuffer,"E01");
				break;
		
			case 'M': /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
				/* Try to read '%x,%x:' */
				if (hexToInt(&ptr, &addr)
				    && *ptr++ == ','
				    && hexToInt(&ptr, &length)
				    && *ptr++ == ':')	{
				  ramaddr = ram_ptr(addr, length);
				  if (!ramaddr) {
				  	strcpy(remcomOutBuffer, "E03");
				  	break;
				  }
				  if (range_translated((u32)ramaddr, (u32)((char *)ramaddr + length)))
				  	flush_translations();
					if (hex2mem(ptr, (char *)ramaddr, length, 1))
						strcpy(remcomOutBuffer, "OK");
					else
						strcpy(remcomOutBuffer, "E03");
				}	else
					strcpy(remcomOutBuffer, "E02");
				break;
		
			case 'S': /* Ssig[;AA..AA] Step with signal at address AA..AA(optional). Same as 's' for us. */
				ptr = strchr(ptr, ';'); /* skip the signal */
				if (ptr)
					ptr++;
			case 's': /* s[AA..AA]  Step at address AA..AA(optional) */
				cpu_events |= EVENT_DEBUG_STEP;
				goto parse_new_pc;
			case 'C': /* Csig[;AA..AA] Continue with signal at address AA..AA(optional). Same as 'c' for us. */
				ptr = strchr(ptr, ';'); /* skip the signal */
				if (ptr)
					ptr++;
			case 'c':    /* c[AA..AA]    Continue at address AA..AA(optional) */
parse_new_pc:
				if (ptr && hexToInt(&ptr, &addr)) {
					arm.reg[15] = addr;
				}
				return;
			
			case 'v':
				ptr = strtok(ptr, ";:");
				if (!strcmp("Cont?", ptr)) { /* supported actions query */
					strcpy(remcomOutBuffer, "vCont;");
					strcat(remcomOutBuffer, "cs"); /* supports these actions */
				}
				else if (!strcmp("Cont", ptr)) {
					ptr = strtok(NULL, ""); /* [action[:thread-id];]* */
					while (ptr && *ptr) {
						char action = *ptr++;
						ptr = strtok(ptr, ";");
						if (*ptr != ':') { /* thread-ids or not supported: perform action if not a thread-id */
							switch (action) {
								case 's':
									cpu_events |= EVENT_DEBUG_STEP;
									return;
								case 'c':
									return;
							}
						}
						ptr = strtok(NULL, ";"); 
					}
				}
				break;
		}			/* switch */

		/* reply to the request */
		putpacket(remcomOutBuffer);
	}
}

void gdbstub_init(int port) {
	wait_gdb_connection(port);
}
