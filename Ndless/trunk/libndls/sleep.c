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
 * Contributor(s): 
 ****************************************************************************/

#include <os.h>

void sleep(unsigned millisec) {
	volatile unsigned *timer = (unsigned*)0x900D0000;
	volatile unsigned *divider = (unsigned*)0x900D0004;
	unsigned orig_divider = *divider;
	unsigned orig_timer = *timer;
	*divider = 31;
	*timer = millisec;
	while (*timer > 0)
  	idle();
	*divider = orig_divider;
	*timer = orig_timer;
}
