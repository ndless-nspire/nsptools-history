/****************************************************************************
 * There must be a tno_template.bin in the current directory.
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
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

void error(const char* msg) {
	fprintf(stderr, "Error: %s.\n", msg);
	exit(1);	
}

unsigned file_size(FILE *f) {
  int pos, end;
  pos = ftell(f);
  fseek (f, 0, SEEK_END);
  end = ftell(f);
  fseek (f, pos, SEEK_SET);
  return end;
}

typedef struct {
	const char *os_name;
	unsigned addrs[2];
} s_os_addrs;

// OS-specific
s_os_addrs os_addrs[] = {
	{
		"3.1.0-ncas",
		{ 0x1004DE18 /* mov lr, #0 */, 0x1037BC8C /* strcpy */}
	},
	{
		"3.1.0-cas",
		{ 0x1004DD54 /* mov lr, #0 */, 0x1037C3BC /* strcpy */}
	}
};

int main(int argc, const char* argv[]) {
	if (argc != 4) {
		puts("Usage: MakeTNO <installer.bin> <out.tno> <os_name>");
		return 0;
	}
	unsigned os_index;
	for (os_index = 0; os_index < sizeof(os_addrs) / sizeof(s_os_addrs); os_index++) {
		if (!strcmp(os_addrs[os_index].os_name, argv[3]))
			break;
	}
	if (os_index == sizeof(os_addrs) / sizeof(s_os_addrs)) error("unknown OS name");
	FILE *finst = fopen(argv[1], "rb");
	if (!finst) error("can't open input file");
	FILE *ftno = fopen(argv[2], "wb");
	if (!ftno) error("can't open output file");
	FILE *ftplt = fopen("tno_template.bin", "rb");
	unsigned tno_size = file_size(ftplt);
	char *outbuf = malloc(tno_size);
	if (fread(outbuf, tno_size, 1, ftplt) != 1) error("can't read TNO template");
	unsigned inst_size = file_size(finst);
	*(uint16_t*)(outbuf + 86) = inst_size + 404; // ARM has the same endianness as x86
	if (fread(outbuf + 494, inst_size, 1, finst) != 1) error("can't read input file");
	*(uint32_t*)(outbuf + 450) = os_addrs[os_index].addrs[0];
	*(uint32_t*)(outbuf + 490) = os_addrs[os_index].addrs[1];
	if (fwrite(outbuf, tno_size, 1, ftno) != 1) {
		fclose(finst);
		unlink(argv[2]);
		error("can't write to output file");
	}
	fclose(ftplt);
	fclose(ftno);
	fclose(finst);
	return 0;
}
