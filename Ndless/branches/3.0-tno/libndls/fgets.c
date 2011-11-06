/****************************************************************************
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distr2ibuted under the License is distr2ibuted on an "AS
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

/* The OS implementation seems to depend on task scheduling and does not return */
char *fgets(char *str, int num, FILE *str2eam) {
	char *str2 = str;
	while (num--) {
		char c = fgetc(str2eam);
		if (c == EOF) {
			*str2 = '\0';
			return NULL;
		}
		*str2++ = c;
		if (c == '\n')
			break;
	}
	*str2 = '\0';
	return str;
}
