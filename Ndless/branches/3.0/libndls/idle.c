/****************************************************************************
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
 * Contributor(s): Goplat
 ****************************************************************************/

#include <os.h>

void idle(void) {
	int irq_mask = *(volatile int *)0xDC000008;
	*(volatile int *)0xDC00000C = ~(1 << 19); // Disable all IRQs except timer
  __asm volatile("mcr p15, 0, %0, c7, c0, 4" : : "r"(0) ); // Wait for an interrupt to occur
	*(volatile int *)0x900A0020 = 1; // Acknowledge timer interrupt at source
	*(volatile int *)0xDC000028; // Make interrupt controller stop asserting nIRQ if there aren't any active IRQs left
	*(volatile int *)0xDC000008 = irq_mask; // Re-enable disabled IRQs
}
