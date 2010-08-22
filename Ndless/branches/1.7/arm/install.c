/****************************************************************************
 * Ndless installation
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Olivier ARMAND
 * <olivier.calc@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include <os.h>

/* Relocate the program by adapting our own Global Offset Table (GOT).
   This is required because we are using several global variables. */
static void relocate(void) {
	unsigned *gotp;
	unsigned prgmbase;
	// Trick to determine the address of the first instruction of the program
	asm volatile(
		" sub r0, pc, #4 \n" // Absolute address of  label '0'
		"0: ldr %0, =0b \n" // Offset from the program base to label '0'
		" sub %0, r0, %0 \n"
		: "=r" (prgmbase) :: "r0");
	// Get the absolute address of the got. See http://www.google.com/codesearch/p?hl=en#FiIujMxKUHU/sites/sources.redhat.com/pub/glibc/snapshots/glibc-ports-latest.tar.bz2%7CDNu48aiJSpY/glibc-ports-20090518/sysdeps/arm/dl-machine.h&q=%22.word%20_GLOBAL_OFFSET_TABLE_%22
  asm volatile(
  	"ldr %0, 2f \n"
  	"1: add %0, pc, %0 \n"
  	"b 3f \n"
  	"2: .word _GLOBAL_OFFSET_TABLE_ - (1b+8) \n"
  	"3:" : "=r" (gotp));
	while (*gotp) {
		*gotp += prgmbase;
		gotp++;
	}
}

void main(void) {
	relocate();
	ints_setup_handlers();
	ut_read_os_version_index();
	sc_setup();	
	puts("Ndless installed!");
	ut_os_reboot();
}
