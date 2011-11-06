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

int main(void) {
	wait_no_key_pressed();
	while (1) {
		wait_key_pressed();
		if (isKeyPressed(KEY_NSPIRE_RET)) nputs("KEY_NSPIRE_RET");
		if (isKeyPressed(KEY_NSPIRE_ENTER)) nputs("KEY_NSPIRE_ENTER");
		if (isKeyPressed(KEY_NSPIRE_SPACE)) nputs("KEY_NSPIRE_SPACE");
		if (isKeyPressed(KEY_NSPIRE_NEGATIVE)) nputs("KEY_NSPIRE_NEGATIVE");
		if (isKeyPressed(KEY_NSPIRE_Z)) nputs("KEY_NSPIRE_Z");
		if (isKeyPressed(KEY_NSPIRE_PERIOD)) nputs("KEY_NSPIRE_PERIOD");
		if (isKeyPressed(KEY_NSPIRE_Y)) nputs("KEY_NSPIRE_Y");
		if (isKeyPressed(KEY_NSPIRE_0)) nputs("KEY_NSPIRE_0");
		if (isKeyPressed(KEY_NSPIRE_X)) nputs("KEY_NSPIRE_X");
		if (isKeyPressed(KEY_NSPIRE_THETA)) nputs("KEY_NSPIRE_THETA");
		if (isKeyPressed(KEY_NSPIRE_COMMA)) nputs("KEY_NSPIRE_COMMA");
		if (isKeyPressed(KEY_NSPIRE_PLUS)) nputs("KEY_NSPIRE_PLUS");
		if (isKeyPressed(KEY_NSPIRE_W)) nputs("KEY_NSPIRE_W");
		if (isKeyPressed(KEY_NSPIRE_3)) nputs("KEY_NSPIRE_3");
		if (isKeyPressed(KEY_NSPIRE_V)) nputs("KEY_NSPIRE_V");
		if (isKeyPressed(KEY_NSPIRE_2)) nputs("KEY_NSPIRE_2");
		if (isKeyPressed(KEY_NSPIRE_U)) nputs("KEY_NSPIRE_U");
		if (isKeyPressed(KEY_NSPIRE_1)) nputs("KEY_NSPIRE_1");
		if (isKeyPressed(KEY_NSPIRE_T)) nputs("KEY_NSPIRE_T");
		if (isKeyPressed(KEY_NSPIRE_eEXP)) nputs("KEY_NSPIRE_eEXP");
		if (isKeyPressed(KEY_NSPIRE_PI)) nputs("KEY_NSPIRE_PI");
		if (isKeyPressed(KEY_NSPIRE_QUES)) nputs("KEY_NSPIRE_QUES");
		if (isKeyPressed(KEY_NSPIRE_QUESEXCL)) nputs("KEY_NSPIRE_QUESEXCL");
		if (isKeyPressed(KEY_NSPIRE_MINUS)) nputs("KEY_NSPIRE_MINUS");
		if (isKeyPressed(KEY_NSPIRE_S)) nputs("KEY_NSPIRE_S");
		if (isKeyPressed(KEY_NSPIRE_6)) nputs("KEY_NSPIRE_6");
		if (isKeyPressed(KEY_NSPIRE_R)) nputs("KEY_NSPIRE_R");
		if (isKeyPressed(KEY_NSPIRE_5)) nputs("KEY_NSPIRE_5");
		if (isKeyPressed(KEY_NSPIRE_Q)) nputs("KEY_NSPIRE_Q");
		if (isKeyPressed(KEY_NSPIRE_4)) nputs("KEY_NSPIRE_4");
		if (isKeyPressed(KEY_NSPIRE_P)) nputs("KEY_NSPIRE_P");
		if (isKeyPressed(KEY_NSPIRE_TENX)) nputs("KEY_NSPIRE_TENX");
		if (isKeyPressed(KEY_NSPIRE_EE)) nputs("KEY_NSPIRE_EE");
		if (isKeyPressed(KEY_NSPIRE_COLON)) nputs("KEY_NSPIRE_COLON");
		if (isKeyPressed(KEY_NSPIRE_MULTIPLY)) nputs("KEY_NSPIRE_MULTIPLY");
		if (isKeyPressed(KEY_NSPIRE_O)) nputs("KEY_NSPIRE_O");
		if (isKeyPressed(KEY_NSPIRE_9)) nputs("KEY_NSPIRE_9");
		if (isKeyPressed(KEY_NSPIRE_N)) nputs("KEY_NSPIRE_N");
		if (isKeyPressed(KEY_NSPIRE_8)) nputs("KEY_NSPIRE_8");
		if (isKeyPressed(KEY_NSPIRE_M)) nputs("KEY_NSPIRE_M");
		if (isKeyPressed(KEY_NSPIRE_7)) nputs("KEY_NSPIRE_7");
		if (isKeyPressed(KEY_NSPIRE_L)) nputs("KEY_NSPIRE_L");
		if (isKeyPressed(KEY_NSPIRE_SQU)) nputs("KEY_NSPIRE_SQU");
		if (isKeyPressed(KEY_NSPIRE_II)) nputs("KEY_NSPIRE_II");
		if (isKeyPressed(KEY_NSPIRE_QUOTE)) nputs("KEY_NSPIRE_QUOTE");
		if (isKeyPressed(KEY_NSPIRE_DIVIDE)) nputs("KEY_NSPIRE_DIVIDE");
		if (isKeyPressed(KEY_NSPIRE_K)) nputs("KEY_NSPIRE_K");
		if (isKeyPressed(KEY_NSPIRE_TAN)) nputs("KEY_NSPIRE_TAN");
		if (isKeyPressed(KEY_NSPIRE_J)) nputs("KEY_NSPIRE_J");
		if (isKeyPressed(KEY_NSPIRE_COS)) nputs("KEY_NSPIRE_COS");
		if (isKeyPressed(KEY_NSPIRE_I)) nputs("KEY_NSPIRE_I");
		if (isKeyPressed(KEY_NSPIRE_SIN)) nputs("KEY_NSPIRE_SIN");
		if (isKeyPressed(KEY_NSPIRE_H)) nputs("KEY_NSPIRE_H");
		if (isKeyPressed(KEY_NSPIRE_EXP)) nputs("KEY_NSPIRE_EXP");
		if (isKeyPressed(KEY_NSPIRE_GTHAN)) nputs("KEY_NSPIRE_GTHAN");
		if (isKeyPressed(KEY_NSPIRE_APOSTROPHE)) nputs("KEY_NSPIRE_APOSTROPHE");
		if (isKeyPressed(KEY_NSPIRE_CAT)) nputs("KEY_NSPIRE_CAT");
		if (isKeyPressed(KEY_NSPIRE_FRAC)) nputs("KEY_NSPIRE_FRAC");
		if (isKeyPressed(KEY_NSPIRE_G)) nputs("KEY_NSPIRE_G");
		if (isKeyPressed(KEY_NSPIRE_RP)) nputs("KEY_NSPIRE_RP");
		if (isKeyPressed(KEY_NSPIRE_F)) nputs("KEY_NSPIRE_F");
		if (isKeyPressed(KEY_NSPIRE_LP)) nputs("KEY_NSPIRE_LP");
		if (isKeyPressed(KEY_NSPIRE_E)) nputs("KEY_NSPIRE_E");
		if (isKeyPressed(KEY_NSPIRE_VAR)) nputs("KEY_NSPIRE_VAR");
		if (isKeyPressed(KEY_NSPIRE_D)) nputs("KEY_NSPIRE_D");
		if (isKeyPressed(KEY_NSPIRE_CAPS)) nputs("KEY_NSPIRE_CAPS");
		if (isKeyPressed(KEY_NSPIRE_DEL)) nputs("KEY_NSPIRE_DEL");
		if (isKeyPressed(KEY_NSPIRE_LTHAN)) nputs("KEY_NSPIRE_LTHAN");
		if (isKeyPressed(KEY_NSPIRE_FLAG)) nputs("KEY_NSPIRE_FLAG");
		if (isKeyPressed(KEY_NSPIRE_CLICK)) nputs("KEY_NSPIRE_CLICK");
		if (isKeyPressed(KEY_NSPIRE_C)) nputs("KEY_NSPIRE_C");
		if (isKeyPressed(KEY_NSPIRE_HOME)) nputs("KEY_NSPIRE_HOME");
		if (isKeyPressed(KEY_NSPIRE_B)) nputs("KEY_NSPIRE_B");
		if (isKeyPressed(KEY_NSPIRE_MENU)) nputs("KEY_NSPIRE_MENU");
		if (isKeyPressed(KEY_NSPIRE_A)) nputs("KEY_NSPIRE_A");
		if (isKeyPressed(KEY_NSPIRE_ESC)) { nputs("KEY_NSPIRE_ESC"); return 0; }
		if (isKeyPressed(KEY_NSPIRE_BAR)) nputs("KEY_NSPIRE_BAR");
		if (isKeyPressed(KEY_NSPIRE_TAB)) nputs("KEY_NSPIRE_TAB");
		if (isKeyPressed(KEY_NSPIRE_EQU)) nputs("KEY_NSPIRE_EQU");
		if (isKeyPressed(KEY_NSPIRE_UP)) nputs("KEY_NSPIRE_UP");
		if (isKeyPressed(KEY_NSPIRE_UPRIGHT)) nputs("KEY_NSPIRE_UPRIGHT");
		if (isKeyPressed(KEY_NSPIRE_RIGHT)) nputs("KEY_NSPIRE_RIGHT");
		if (isKeyPressed(KEY_NSPIRE_RIGHTDOWN)) nputs("KEY_NSPIRE_RIGHTDOWN");
		if (isKeyPressed(KEY_NSPIRE_DOWN)) nputs("KEY_NSPIRE_DOWN");
		if (isKeyPressed(KEY_NSPIRE_DOWNLEFT)) nputs("KEY_NSPIRE_DOWNLEFT");
		if (isKeyPressed(KEY_NSPIRE_LEFT)) nputs("KEY_NSPIRE_LEFT");
		if (isKeyPressed(KEY_NSPIRE_LEFTUP)) nputs("KEY_NSPIRE_LEFTUP");
		if (isKeyPressed(KEY_NSPIRE_CLEAR)) nputs("KEY_NSPIRE_CLEAR");
		if (isKeyPressed(KEY_NSPIRE_SHIFT)) nputs("KEY_NSPIRE_SHIFT");
		if (isKeyPressed(KEY_NSPIRE_CTRL)) nputs("KEY_NSPIRE_CTRL");
		if (isKeyPressed(KEY_NSPIRE_DOC)) nputs("KEY_NSPIRE_DOC");
		if (isKeyPressed(KEY_NSPIRE_TRIG)) nputs("KEY_NSPIRE_TRIG");
		if (isKeyPressed(KEY_NSPIRE_SCRATCHPAD)) nputs("KEY_NSPIRE_SCRATCHPAD");
		if (on_key_pressed()) nputs("ON");
		wait_no_key_pressed();
	}
	return 0;
}
