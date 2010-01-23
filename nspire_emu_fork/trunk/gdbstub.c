/*
 * TODO:
 * - Trap exceptions (trap_low, handle_exception defined but not used, set_mem_fault_trap, trap...)
 * - Signals (hard_trap_info, sig...)
 * - Breakpoints ("Need to flush the instruction cache here")
 * - Sync up at startup ("This function will generate a breakpoint exception")
 * - alignment: not with tabs
 * - Support thumb (REGISTERS[PC] += 4)
 * 
 */

/*
 * Derived from GDB's sparc-stub.c.
 *
 * The following gdb commands are supported:
 *
 * command          function                               Return value
 *
 *    g             return the value of the CPU registers  hex data or ENN
 *    G             set the value of the CPU registers     OK or ENN
 *
 *    mAA..AA,LLLL  Read LLLL bytes at address AA..AA      hex data or ENN
 *    MAA..AA,LLLL: Write LLLL bytes at address AA.AA      OK or ENN
 *
 *    c             Resume at current address              SNN   ( signal NN)
 *    cAA..AA       Continue at address AA..AA             SNN
 *
 *    s             Step one instruction                   SNN
 *    sAA..AA       Step one instruction from AA..AA       SNN
 *
 *    k             kill
 *
 *    ?             What was the last sigval ?             SNN   (signal NN)
 *
 * All commands and responses are sent with a packet which includes a
 * checksum.  A packet consists of
 *
 * $<packet info>#<checksum>.
 *
 * where
 * <packet info> :: <characters representing the command or response>
 * <checksum>    :: < two hex digits computed as modulo 256 sum of <packetinfo>>
 *
 * When a packet is received, it is first acknowledged with either '+' or '-'.
 * '+' indicates a successful transfer.  '-' indicates a failed transfer.
 *
 * Example:
 *
 * Host:                  Reply:
 * $m0,10#2a               +$00010203040506070809101112131415#42
 *
 ****************************************************************************/

#include <stdio.h>
#include <string.h>

/************************************************************************/
/* BUFMAX defines the maximum number of characters in inbound/outbound buffers*/
/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 2048

static void set_mem_fault_trap();

static const char hexchars[]="0123456789abcdef";

#define NUMREGS 72

/* Number of bytes of registers.  */
#define NUMREGBYTES (NUMREGS * 4)
enum regnames {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, SP, LR, PC,
	F0, F1, F2, F3, F4, F5, F6, F7, FPS, CPSR};

void putDebugChar(char c) {
	printf("%c", c);
}

#include <conio.h>
int getDebugChar(void) {
	// TODO
	return _getch();
}

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
		while ((ch = getDebugChar()) != '$')
			;
		
retry:
		checksum = 0;
		xmitcsum = -1;
		count = 0;
		
		/* now, read until a # or end of buffer is found */
		while (count < BUFMAX - 1) {
			ch = getDebugChar();
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
			ch = getDebugChar();
			xmitcsum = hex(ch) << 4;
			ch = getDebugChar();
			xmitcsum += hex(ch);

			if (checksum != xmitcsum) {
				putDebugChar('-');	/* failed checksum */
			}	else {
				putDebugChar('+');	/* successful transfer */
	
				/* if a sequence char is present, reply the sequence ID */
				if(buffer[2] == ':') {
					putDebugChar(buffer[0]);
					putDebugChar(buffer[1]);
		
					return &buffer[3];
				}
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
			putDebugChar('$');
			checksum = 0;
			count = 0;

			while ((ch = buffer[count])) {
				putDebugChar(ch);
				checksum += ch;
				count += 1;
			}

			putDebugChar('#');
			putDebugChar(hexchars[checksum >> 4]);
			putDebugChar(hexchars[checksum & 0xf]);

		}
	while (getDebugChar() != '+');
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

/*
 * This function does all command processing for interfacing to gdb.  It
 * returns 1 if you should skip the instruction at the trap address, 0
 * otherwise.
 */
extern void breakinst();

void handle_exception(unsigned long *registers) {
	int tt;			/* Trap type */
	int sigval;
	int addr;
	int length;
	char *ptr;
	unsigned long *sp;

	// TODO define breakinst
//	if (registers[PC] == (unsigned long)breakinst) {
//		registers[PC] += 4;
//	}

	sp = (unsigned long *)registers[SP];

	// TODO
	//tt = (registers[TBR] >> 4) & 0xff;

	/* reply to host that an exception has occurred */
	sigval = 0; // TODO: computeSignal(tt);
	ptr = remcomOutBuffer;

	*ptr++ = 'T';
	*ptr++ = hexchars[sigval >> 4];
	*ptr++ = hexchars[sigval & 0xf];

	*ptr++ = hexchars[PC >> 4];
	*ptr++ = hexchars[PC & 0xf];
	*ptr++ = ':';
	ptr = mem2hex((char *)&registers[PC], ptr, sizeof(unsigned long), 0);
	*ptr++ = ';';

	// TODO define FP constant and find its value
	#define FP 0
	*ptr++ = hexchars[FP >> 4];
	*ptr++ = hexchars[FP & 0xf];
	*ptr++ = ':';
	// TODO
	ptr = mem2hex((char*)(sp + 8 + 6), ptr, sizeof(unsigned long), 0); /* FP */
	*ptr++ = ';';

	*ptr++ = hexchars[SP >> 4];
	*ptr++ = hexchars[SP & 0xf];
	*ptr++ = ':';
	ptr = mem2hex((char *)&sp, ptr, sizeof(unsigned long), 0);
	*ptr++ = ';';

	// TODO NPC? Are there others?

	*ptr++ = 0;

	putpacket(remcomOutBuffer);

	while (1)	{
		remcomOutBuffer[0] = 0;

		ptr = getpacket();
		switch (*ptr++) 	{
			case '?':
				remcomOutBuffer[0] = 'S';
				remcomOutBuffer[1] = hexchars[sigval >> 4];
				remcomOutBuffer[2] = hexchars[sigval & 0xf];
				remcomOutBuffer[3] = 0;
				break;

			case 'd':		/* toggle debug flag */
				break;
	
			// TODO check the format expected for ARM target. Change '18' constant elsewhere
			case 'g':		/* return the value of the CPU registers */
				ptr = remcomOutBuffer;
				ptr = mem2hex((char *)registers, ptr, 26 * sizeof(unsigned long), 0); 
				break;
		
			// TODO check the format expected for ARM target
			case 'G': {  /* set the value of the CPU registers - return OK */
				unsigned long *newsp;
		
				hex2mem(ptr, (char *)registers, 26 * sizeof(unsigned long), 0);

				// TODO
				/* See if the stack pointer has moved.  If so, then copy the saved
				   locals and ins to the new location.  This keeps the window
				   overflow and underflow routines happy.  */
				newsp = (unsigned long *)registers[SP];
				if (sp != newsp)
					sp = memcpy(newsp, sp, 26 * sizeof(unsigned long));
		
				strcpy(remcomOutBuffer,"OK");
				break;
			}
		
			case 'm':	  /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
				/* Try to read %x,%x.  */
		
				if (hexToInt(&ptr, &addr)
				    && *ptr++ == ','
				    && hexToInt(&ptr, &length)) {
					if (mem2hex((char *)addr, remcomOutBuffer, length, 1))
						break;
		
					strcpy(remcomOutBuffer, "E03");
				}	else
					strcpy(remcomOutBuffer,"E01");
				break;
		
			case 'M': /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
				/* Try to read '%x,%x:'.  */
		
				if (hexToInt(&ptr, &addr)
				    && *ptr++ == ','
				    && hexToInt(&ptr, &length)
				    && *ptr++ == ':')	{
					if (hex2mem(ptr, (char *)addr, length, 1))
						strcpy(remcomOutBuffer, "OK");
					else
						strcpy(remcomOutBuffer, "E03");
				}	else
					strcpy(remcomOutBuffer, "E02");
				break;
		
			case 'c':    /* cAA..AA    Continue at address AA..AA(optional) */
				/* try to read optional parameter, pc unchanged if no parm */
		
				if (hexToInt(&ptr, &addr)) {
					registers[PC] = addr;
				}
		
				return;
		
				/* kill the program */
			case 'k' :		/* do nothing */
				break;
#if 0
			case 't':		/* Test feature */
				break;
#endif
			case 'r':		/* Reset */
				// TODO
				break;
		}			/* switch */

		/* reply to the request */
		putpacket(remcomOutBuffer);
	}
}
