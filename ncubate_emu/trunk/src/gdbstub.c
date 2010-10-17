/*
 * TODO:
 * - Explicitely support the endianness (set/get_registers). Currently the host must be little-endian
 *   as ARM is.
 * - fix remote_read(), etc.
 */

/*
 * Some parts derive from GDB's sparc-stub.c.
 * Refer to Appendix D - GDB Remote Serial Protocol in GDB's documentation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#ifdef __MINGW32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "emu.h"

static void gdbstub_disconnect(void);

bool ndls_is_installed(void) {
	// The Ndless marker is 8 bytes before the SWI handler
	return *(unsigned*)(virt_mem_ptr(
		*(unsigned*)virt_mem_ptr(0x20 + 2 * 4 /* SWI handler address */, 4)
		, 4) - 8)
	== 0x4E455854 /* 'NEXT' */;
}

static int listen_socket_fd = 0;
static int socket_fd = 0;

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
#ifdef __MINGW32__
			if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
			if (errno == EAGAIN)
#endif
				continue; // not ready to send
			else {
				log_socket_error("Failed to send to GDB stub socket");
				break;
			}
		}
		p += n;
	}
	sockbufptr = sockbuf;
}

static void put_debug_char(char c) {
	if (log_enabled[LOG_GDB]) {
		logprintf(LOG_GDB, "%c", c);
		fflush(stdout);
		if (c == '+' || c == '-')
			logprintf(LOG_GDB, "\t");
	}
	if (sockbufptr == sockbuf + sizeof sockbuf)
		flush_out_buffer();
	*sockbufptr++ = c;
}

/* Returns -1 on disconnection */
static char get_debug_char(void) {
	char c;
	int r;
	while (1) {
		r = recv(socket_fd, &c, 1, 0);
		if (r == -1) {
#ifdef __MINGW32__
			if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
			if (errno == EAGAIN)
#endif
				continue; // data not yet available
			else {
				// only for debugging - log_socket_error("Failed to recv from GDB stub socket");
				return -1;
			}
		}
		if (r == 0)
			return -1; // disconnected
		break;
	}
	if (log_enabled[LOG_GDB]) {
		logprintf(LOG_GDB, "%c", c);
		fflush(stdout);
		if (c == '+' || c == '-')
			logprintf(LOG_GDB, "\n");
	}
	return c;
}

static void gdbstub_bind(int port) {
	struct sockaddr_in sockaddr;
	int r;
	
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
#ifdef __MINGW32__
	u_long mode = 1;
	ioctlsocket(listen_socket_fd, FIONBIO, &mode);
#else
  ret = fcntl(listen_socket_fd, F_GETFL, 0);
  fcntl(listen_socket_fd, F_SETFL, ret | O_NONBLOCK);
#endif

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
}

// program block pre-allocated by Ndless, used for vOffsets queries
static u32 ndls_debug_alloc_block = 0;

static void gdb_connect_ndls_cb(struct arm_state *state) {
	ndls_debug_alloc_block = state->reg[0]; // can be 0
}

/* BUFMAX defines the maximum number of characters in inbound/outbound buffers */
/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 2048

static const char hexchars[]="0123456789abcdef";

#define NUMREGS 26

/* Number of bytes of registers. */
#define NUMREGBYTES (NUMREGS * 4)
enum regnames {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC,
	F0, F1, F2, F3, F4, F5, F6, F7, FPS, CPSR};

// see GDB's include/gdb/signals.h
enum target_signal {SIGNAL_ILL_INSTR = 4, SIGNAL_TRAP = 5};

/* Convert ch from a hex digit to an int */
static int hex(char ch) {
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
static char databuffer[BUFMAX / 2];

/* scan for the sequence $<data>#<checksum>. # will be replaced with \0.
 * Returns NULL on disconnection. */
char *getpacket(void) {
	char *buffer = &remcomInBuffer[0];
	unsigned char checksum;
	unsigned char xmitcsum;
	int count;
	char ch;

	while (1) {
		/* wait around for the start character, ignore all other characters */
		do {
			ch = get_debug_char();
			if (ch == -1) // disconnected
				return NULL;
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
			buffer[count] = ch;
			if (ch == '#')
				break;
			count = count + 1;
			checksum = checksum + ch;
		}

		if (ch == '#') {
			buffer[count] = 0;
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
static void putpacket(char *buffer) {
	unsigned char checksum;
	int count;
	char ch;

	/*  $<packet info>#<checksum> */
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
			ch = get_debug_char();
	} while (ch != '+' && ch != -1);
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
static unsigned char *mem2hex(unsigned char *mem, unsigned char *buf, int count) {
	unsigned char ch;

	while (count-- > 0)	{
		ch = *mem++;
		if (mem_err)
			return 0;
		*buf++ = hexchars[ch >> 4];
		*buf++ = hexchars[ch & 0xf];
	}
	*buf = 0;
	return buf;
}

/* convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written.
 * If count is null stops at the first non hex digit */
static char *hex2mem(char *buf, unsigned char *mem, int count) {
	int i;
	char ch;

	for (i = 0; i < count || !count; i++) {
		ch = hex(*buf++);
		if (ch == -1)
			return mem;
		ch <<= 4;
		ch |= hex(*buf++);
		*mem++ = ch;
		if (mem_err)
			return 0;
	}
	return mem;
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

/* See Appendix D - GDB Remote Serial Protocol - Overview.
 * A null character is appended. */
static void binary_escape(char *in, int insize, char *out, int outsize) {
	while (insize-- > 0 && outsize > 1) {
		if (*in == '#' || *in == '$' || *in == '}' || *in == 0x2A) {
			if (outsize < 3)
				break;
			*out++ = '}';
			*out++ = (0x20 ^ *in++);
			outsize -= 2;
		}
		else {
			*out++ = *in++;
			outsize--;
		}
	}
	*out = '\0';
}

/* See Appendix D - GDB Remote Serial Protocol - Overview.
 * Returns the unescaped data size. */
static int binary_unescape(char *in, int insize, char *out, int outsize) {
	int unsize = 0;;
	while (insize-- > 0 && outsize > 0) {
		if (*in == '}') {
			in++;
			*out++ = (0x20 ^ *in++);
			insize -= 2;
		}
		else 
			*out++ = *in++;
		outsize--;
		unsize++;
	}
	return unsize;
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

/* GDB Host I/O */

static void remote_vfile_cb(int result) {
	char *ptr;
	/* response: F result [, errno] [; attachment] */
	ptr = remcomOutBuffer;
	sprintf(ptr, "F%x", result);
	if (result < 0)
		strcat(ptr, ",270F"); /* EUNKNOWN=9999 */
	else
		strcat(ptr, ",0"); /* success */
/*	if (data) {
		strcat(ptr, ";");
		ptr += strlen(ptr);
		binary_escape(data, datasize, ptr, remcomOutBuffer + sizeof(remcomOutBuffer) - ptr);
	}
	*/
	putpacket(remcomOutBuffer);
}

static void remote_open_cb(struct arm_state *after_exec_arm_state) {
	remote_vfile_cb(after_exec_arm_state->reg[0]);
}

/* returns the fd */
static int remote_open(char *pathname, int flags) {
	struct armloader_load_params params[2];
	params[0].t = ARMLOADER_PARAM_PTR;
	params[0].p.ptr = pathname;
	params[0].p.size = strlen(pathname) + 1;
	params[1].t = ARMLOADER_PARAM_VAL;
	params[1].v = flags;
	armloader_load_snippet(SNIPPET_file_open, params, 2, remote_open_cb);
	return 0;
}

/* returns 0 if successful */
static int remote_close(__attribute__((unused)) int fd) {
	return 0;
}

/* Writes the data read to databuffer.
 * returns the number of target bytes read */
static int remote_read(__attribute__((unused)) int fd, __attribute__((unused)) int count, __attribute__((unused)) int offset) {
	strcpy(databuffer, "a$a"); // test
	return 2;
}

/* returns the number of bytes written */
static int remote_write(__attribute__((unused)) int fd, __attribute__((unused)) int offset, char *data, int size) {
	printf("remote_write: ");
	int i;
	for (i = 0; i < size; i++)
		printf("%2x", (int)data[i]);
	return size;
}

/* returns 0 if successful */
static int remote_unlink(char *pathname) {
	puts(pathname);
	return 0;
}

#define append_hex_char(ptr,ch) do {*ptr++ = hexchars[(ch) >> 4]; *ptr++ = hexchars[(ch) & 0xf];} while (0)

void send_signal_reply(int signal) {
	char *ptr = remcomOutBuffer;
	*ptr++ = 'T';
	append_hex_char(ptr, signal);
	append_hex_char(ptr, 13);
	*ptr++ = ':';
	ptr = mem2hex((char *)&arm.reg[13], ptr, sizeof(u32));
	*ptr++ = ';';
	append_hex_char(ptr, 15);
	*ptr++ = ':';
	ptr = mem2hex((char *)&arm.reg[15], ptr, sizeof(u32));
	*ptr++ = ';';
	*ptr++ = 0;
	putpacket(remcomOutBuffer);
}

void gdbstub_loop(void) {
	int addr;
	int length;
	int ret;
	int datasize, size;
	char *ptr, *ptr1, *ptr2, *ptr3;
	int i, j;
	char *data;
	void *ramaddr;
	unsigned long regbuf[NUMREGS];
	bool reply, set;
	
	while (1)	{
		remcomOutBuffer[0] = 0;

		ptr = getpacket();
		if (!ptr) {
			gdbstub_disconnect();
			return;
		}
		reply = true;
		switch (*ptr++) 	{
			case '?':
				send_signal_reply(SIGNAL_TRAP);
				reply = false; // already done
				break;

			case 'g':  /* return the value of the CPU registers */
				get_registers(regbuf);
				ptr = remcomOutBuffer;
				ptr = mem2hex((char *)regbuf, ptr, NUMREGS * sizeof(unsigned long)); 
				break;
		
			case 'G':  /* set the value of the CPU registers - return OK */
				hex2mem(ptr, (char *)regbuf, NUMREGS * sizeof(unsigned long));
				set_registers(regbuf);
				strcpy(remcomOutBuffer,"OK");
				break;
				
			case 'p': /* pn Read the value of register n */
				if (hexToInt(&ptr, &addr) && (size_t)addr < sizeof(regbuf)) {
					mem2hex((char*)(get_registers(regbuf) + addr), remcomOutBuffer, sizeof(unsigned long));
				} else {
					strcpy(remcomOutBuffer,"E01");
				}
				break;
				
			case 'P': /* Pn=r Write register n with value r */
				ptr = strtok(ptr, "=");
				if (hexToInt(&ptr, &addr)
					  && (ptr=strtok(NULL, ""))
					  && (size_t)addr < sizeof(regbuf)
					  // TODO hex2mem doesn't check the format
					  && hex2mem((unsigned char*)ptr, (unsigned char*)&get_registers(regbuf)[addr], sizeof(u32))
					  ) {
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
					ramaddr = virt_mem_ptr(addr, length);
					if (!ramaddr || mem2hex((char *)ramaddr, remcomOutBuffer, length))
						break;
					strcpy(remcomOutBuffer, "E03");
				}	else
					strcpy(remcomOutBuffer,"E01");
				break;
		
			case 'M': /* MAA..AA,LLLL: Write LLLL bytes at address AA..AA  */
				/* Try to read '%x,%x:' */
				if (hexToInt(&ptr, &addr)
				    && *ptr++ == ','
				    && hexToInt(&ptr, &length)
				    && *ptr++ == ':')	{
				  ramaddr = virt_mem_ptr(addr, length);
				  if (!ramaddr) {
				  	strcpy(remcomOutBuffer, "E03");
				  	break;
				  }
				  if (range_translated((u32)ramaddr, (u32)((char *)ramaddr + length)))
				  	flush_translations();
					if (hex2mem(ptr, (char *)ramaddr, length))
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
			case 'q':
				if (!strcmp("Offsets", ptr)) {
					sprintf(remcomOutBuffer, "Text=%x;Data=%x;Bss=%x",
						ndls_debug_alloc_block, ndls_debug_alloc_block,	ndls_debug_alloc_block);
				}
				break;
			case 'v':
				ptr = strtok(ptr, ";:");
				if (!strcmp("File", ptr)) { /* vFile:operation:parameter... */
					ptr = strtok(NULL, ":"); /* operation */
					if (!ptr) {
						strcpy(remcomOutBuffer,"E01");
						break;
					}
					ptr1 = strtok(NULL, ","); /* arg1 */
					ptr2 = strtok(NULL, ","); /* arg1 */
					ptr3 = strtok(NULL, ","); /* arg3 */
					data = NULL;
					datasize = 0;
					ret = 0;
					if (!strcmp("open", ptr)) { /* pathname, flags, mode */
						if (!ptr1 || !ptr2 || !ptr3 || !hexToInt(&ptr2, &i))  {
							strcpy(remcomOutBuffer,"E02");
							break;
						}
						*hex2mem(ptr1, databuffer, 0) = '\0'; /* to string */
						ret = remote_open(databuffer, i);
						reply = false;
						return; // remote_vfile_cb will finish processing
					}
					else if (!strcmp("close", ptr)) { /* fd */
						if (!ptr1 || !hexToInt(&ptr1, &i))  {
							strcpy(remcomOutBuffer,"E02");
							break;
						}
						ret = remote_close(i);
						reply = false;
						return;
					}
					else if (!strcmp("pread", ptr)) { /* fd, count, offset */
						if (   !ptr1 || !ptr2 || !ptr3 || !hexToInt(&ptr1, &i)
							  || !hexToInt(&ptr2, &datasize) || !hexToInt(&ptr3, &j)) {
							strcpy(remcomOutBuffer,"E02");
							break;
						}
						if (datasize > (int)sizeof(databuffer)) {
							strcpy(remcomOutBuffer,"E03");
							break;
						}
						ret = remote_read(i, datasize, j);
						data = databuffer;
						datasize = ret;
						reply = false;
						return;
					}
					else if (!strcmp("pwrite", ptr)) { /* fd, offset, data */
						if (!ptr1 || !ptr2 || !ptr3 || !hexToInt(&ptr1, &i) || !hexToInt(&ptr2, &j))  {
							strcpy(remcomOutBuffer,"E02");
							break;
						}
						size = binary_unescape(ptr3, strchr(ptr3, '#') - ptr3, databuffer, sizeof(databuffer));
						ret = remote_write(i, j, databuffer, size);
						reply = false;
						return;
					}
					else if (!strcmp("unlink", ptr)) { /* pathname */
						if (!ptr1)  {
							strcpy(remcomOutBuffer,"E02");
							break;
						}
						*hex2mem(ptr1, databuffer, 0) = '\0'; /* to string */
						ret = remote_unlink(databuffer);
						reply = false;
						return;
					}
				}
				break;
			case 'Z': /* 0|1|2|3|4,addr,kind  */
				set = true;
				goto z;
			case 'z': /* 0|1|2|3|4,addr,kind  */
				set = false;
z:
				if (*ptr++ != '0')
					break;
				ptr = strtok(ptr, ","); 
				if (ptr && hexToInt(&ptr, &addr) && (ramaddr = virt_mem_ptr(addr, 4))) {
					if (set){
						if (RAM_FLAGS(ramaddr) & RF_CODE_TRANSLATED) flush_translations();
						RAM_FLAGS(ramaddr) |= RF_EXEC_BREAKPOINT;
					}	else
						RAM_FLAGS(ramaddr) &= ~RF_EXEC_BREAKPOINT;
					strcpy(remcomOutBuffer, "OK");
				}
				else
					strcpy(remcomOutBuffer, "E01");
				break;
		}			/* switch */

		/* reply to the request */
		if (reply)
			putpacket(remcomOutBuffer);
	}
}

void gdbstub_exception(int type) {
	int gdb_type;
	switch (type) {
		case EX_UNDEFINED: 
			gdb_type = SIGNAL_ILL_INSTR;
			break;
		default:
			gdb_type = SIGNAL_TRAP;
	}
	send_signal_reply(gdb_type);
	gdbstub_loop();
}

void gdbstub_init(int port) {
	gdbstub_bind(port);
}

static void gdbstub_disconnect(void) {
	puts("GDB disconnected.");
	closesocket(socket_fd);
	socket_fd = 0;
	if (ndls_is_installed())
		armloader_load_snippet(SNIPPET_ndls_debug_free, NULL, 0, NULL);
}

/* Non-blocking poll. Enter the debugger loop if a message is received. */
void gdbstub_recv(void) {
	int ret, on;
	if (!socket_fd) {
		ret = accept(listen_socket_fd, NULL, NULL);
		if (ret == -1)
			return;
		socket_fd = ret;
		/* Disable Nagle for low latency */
		on = 1;
#ifdef __MINGW32__
		ret = setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
#else
		ret = setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
#endif
		if (ret == -1)
			log_socket_error("setsockopt(TCP_NODELAY) failed for GDB stub socket");
		
		/* Interface with Ndless */
		if (ndls_is_installed())
			armloader_load_snippet(SNIPPET_ndls_debug_alloc, NULL, 0, gdb_connect_ndls_cb);
		puts("GDB connected.");
		return;
	}
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET((unsigned)socket_fd, &rfds);
	ret = select(socket_fd + 1, &rfds, NULL, NULL, &(struct timeval) {0, 0});
	if (ret == -1 && errno == EBADF) {
		gdbstub_disconnect();
	}
	else if (ret)
		gdbstub_debugger();
}


void gdbstub_debugger(void) {
	cpu_events &= ~EVENT_DEBUG_STEP;
	send_signal_reply(SIGNAL_TRAP);
	gdbstub_loop();
}
