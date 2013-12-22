/****************************************************************************
 * Produce the Lua installer file.
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
 * Portions created by the Initial Developer are Copyright (C) 2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Excale
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

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

int sfprintf(FILE *stream, const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	int ret = vfprintf(stream, format, argptr);
	va_end(argptr);
	if (ret <= 0)
		error("can't write to output file");
	return ret;
}

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		puts("Usage: MakeLuaInst <escaped_installer.bin> <output.lua>");
		return 0;
	}
	FILE *finst = fopen(argv[1], "rb");
	if (!finst) error("can't open input file");
	FILE *flua = fopen(argv[2], "wt");
	if (!flua) error("can't open output file");
	unsigned inst_size = file_size(finst);
	unsigned char *inbuf = malloc(inst_size);
	if (fread(inbuf, inst_size, 1, finst) != 1) error("can't read installer file");
	
	// header
	sfprintf(flua, "local uchar = string.uchar\n");
	sfprintf(flua, "local s = \"\"");
	
	// code
	unsigned char *p = inbuf;
	unsigned line_size = 0;
	while (p < inbuf + inst_size) {
		if (line_size >= 50) // avoid "chunk has too many syntax levels" Lua error
			line_size = 0;
		if (line_size) {
			sfprintf(flua, " .. ");
		}
		else {
			sfprintf(flua, "\ns = s .. ");
		}
		sfprintf(flua, "uchar(0x%02hx%02hx)", (unsigned short)(*(p+1)), (unsigned short)*p);
		line_size += 1;
		p += 2;
	}
	
    // address
	unsigned code_addr[]   = {0x0, 0x10E34D14, 0x0, 0x11112E5C};
	unsigned code_offset[] = {0x0, 0x10E48248, 0x0, 0x11126390};
    int size_to_pad;
    
	// padding
    sfprintf(flua, "\nif platform.isColorDisplay() then");
        size_to_pad = code_offset[3] - code_addr[3] - inst_size;
        if (size_to_pad < 0)
            error("installer is too long");
        sfprintf(flua, "\ns = s .. string.rep(uchar(0x0001), %i)", size_to_pad/2);
        sfprintf(flua, "\ns = s .. uchar(0x%02hx%02hx) .. uchar(0x%02hx%02hx)",
            (code_addr[3] & 0x0000FF00) >> 8, (code_addr[3] & 0x000000FF) >> 0, (code_addr[3] & 0xFF000000) >> 24, (code_addr[3] & 0x00FF0000) >> 16);
    sfprintf(flua, "\nelse");
        size_to_pad = code_offset[1] - code_addr[1] - inst_size;
        if (size_to_pad < 0)
            error("installer is too long");
        sfprintf(flua, "\ns = s .. string.rep(uchar(0x0001), %i)", size_to_pad/2);
        sfprintf(flua, "\ns = s .. uchar(0x%02hx%02hx) .. uchar(0x%02hx%02hx)",
            (code_addr[1] & 0x0000FF00) >> 8, (code_addr[1] & 0x000000FF) >> 0, (code_addr[1] & 0xFF000000) >> 24, (code_addr[1] & 0x00FF0000) >> 16);
    sfprintf(flua, "\nend");
	
	// footer
	sfprintf(flua, "\ntoolpalette.register{{s}}");
	
	free(inbuf);
	fclose(finst);
	fclose(flua);
	return 0;
}
