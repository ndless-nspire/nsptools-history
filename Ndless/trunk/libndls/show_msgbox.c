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

void * alloca (size_t size);

void show_msgbox(const char *title, const char *msg) {
	/* required since OS 2.1 for OS key scan */
	int orig_mask = TCT_Local_Control_Interrupts(0);
	char title16[(strlen(title) + 1) * 2];
	char msg16[(strlen(msg) + 1) * 2];
	ascii2utf16(title16, title, sizeof(title16));
	ascii2utf16(msg16, msg, sizeof(msg16));
	show_dialog_box2(0, title16, msg16);
	TCT_Local_Control_Interrupts(orig_mask);
}
