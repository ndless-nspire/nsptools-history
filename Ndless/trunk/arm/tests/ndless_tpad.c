/****************************************************************************
 * Key scan test
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
 ****************************************************************************/
 
#include <os.h>

static void setPixel(int x, int y, int color) {
  unsigned char* p = (unsigned char*)(SCREEN_BASE_ADDRESS + ((x >> 1) + (y << 7) + (y << 5)));
  *p = (x & 1) ? ((*p & 0xF0) | color) : ((*p & 0x0F) | (color << 4));
}

int main(void) {
	touchpad_report_t tpad_report;
	touchpad_info_t *tpad_info;
	uint16_t x, y;
	if (!is_touchpad)
		return 0;
	clrscr();
	tpad_info = touchpad_getinfo();
	if (!tpad_info)
		return 0;
	while (1) {
		idle();
		if (isKeyPressed(KEY_NSPIRE_ESC))
			return 0;
		touchpad_scan(&tpad_report);
		if (tpad_report.contact) {
			x = tpad_report.x * (SCREEN_WIDTH - 1) / tpad_info->width;
			y = (SCREEN_HEIGHT - 1) - (tpad_report.y * (SCREEN_HEIGHT - 1) / tpad_info->height);
			setPixel(x, y, 0);
		}
	}
	return 0;
}
