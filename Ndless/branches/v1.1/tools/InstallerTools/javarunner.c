/*
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
 */

/*
 * This programs runs the java.exe executable from NavNet's shared JRE.
 * It avoids copying the whole JRE: only this wrapper is expected by NavNet as "java.exe"
 * to run its RMI server.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int  __attribute__ ((unused)) argc, const char* argv[]) {
	char nnjavaexe[260];
	char *commonpf = getenv("CommonProgramFiles");
	if (!commonpf)
		return 1;
	strncpy(nnjavaexe, commonpf, sizeof(nnjavaexe));
	strncat(nnjavaexe, "/TI Shared/CommLib/1/jre/bin/java.exe", sizeof(nnjavaexe));
	execv(nnjavaexe, argv);
	return 0;
}
