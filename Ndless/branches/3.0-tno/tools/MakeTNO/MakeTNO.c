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

#define MAX(a,b) ({typeof(a) __a = (a); typeof(b) __b = (b); (__a > __b) ? __a : __b;})

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
	},
	{
		"3.1.0-ncascx",
		{ 0x1004D514 /* mov lr, #0 */, 0x1037954C /* strcpy */}
	},
	{
		"3.1.0-cascx",
		{ 0x1004D478 /* mov lr, #0 */, 0x10379CDC /* strcpy */}
	},
};

int main(int argc, const char* argv[]) {
	if (argc != 6) {
		puts("Usage: MakeTNO <installer.bin> <out.tno> <os_name> <template> <offset>"); // hex_offset: size of the header before zipped file name size
		return 0;
	}
	long offset = strtol(argv[5], NULL, 10);
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
	FILE *ftplt = fopen(argv[4], "rb");
	unsigned tno_size = file_size(ftplt);
	unsigned inst_size = file_size(finst);
	size_t outbuf_size = MAX(tno_size, 408 + inst_size);
	char *outbuf = malloc(outbuf_size);
	if (fread(outbuf, tno_size, 1, ftplt) != 1) error("can't read TNO template");
	*(uint16_t*)(outbuf + offset) = inst_size + 404; // 404=trailer size. ARM has the same endianness as x86
	if (fread(outbuf + 408 + offset, inst_size, 1, finst) != 1) error("can't read input file");
	*(uint32_t*)(outbuf + 364 + offset) = os_addrs[os_index].addrs[0];
	*(uint32_t*)(outbuf + 404 + offset) = os_addrs[os_index].addrs[1];
	if (fwrite(outbuf, outbuf_size, 1, ftno) != 1) {
		fclose(finst);
		unlink(argv[2]);
		error("can't write to output file");
	}
	fclose(ftplt);
	fclose(ftno);
	fclose(finst);
	return 0;
}
