/****************************************************************************
 * Escapes the installer binary file forbidden half-words and generates an
 * escape table.
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
 * Contributor(s): 
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MIN(a,b) ({typeof(a) __a = (a); typeof(b) __b = (b); (__a < __b) ? __a : __b;})

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

int main(int argc, const char* argv[]) {
	if (argc != 2) {
		puts("Usage: MakeEscapeTable <installer.bin>\n"
			 "Escape forbidden half-words (0000 and 0009).\n"
		     "stage0 will unescape them. \n"
		     "The address of the escape table is written to the DEADBEEF variable in stage0.");
		return 0;
	}
	FILE *finst = fopen(argv[1], "r+b");
	if (!finst) error("can't open input file");
	unsigned inst_size = file_size(finst);
	char *inbuf = malloc(inst_size);
	if (fread(inbuf, inst_size, 1, finst) != 1) error("can't read installer file");
	
	// search and update the escape table offset in stage0
	uint8_t max_esc_table_offset = MIN(200UL, inst_size);
	unsigned char *p = (unsigned char*)inbuf;
	uint8_t table_offset_found = 0;
	while (p < (unsigned char*)inbuf + max_esc_table_offset) {
		if (*(p+0) == 0xEF && *(p+1) == 0xBE && *(p+2) == 0xAD && *(p+3) == 0xDE) { // marker
			fseek(finst, p - (unsigned char*)inbuf, SEEK_SET);
			if (   fputc(inst_size & 0x00FF, finst) == EOF
			    || fputc((inst_size & 0xFF00) >> 8, finst) == EOF)
				error("can't update stage0's escape table offset");
			table_offset_found = 1;
			break;
		}
		p += 2;
	}
	if (!table_offset_found)
		error("can't find stage0's escape table offset to update");
	
	// build the escape table at the end of the installer
	// format: 
	// SSSS: number of entries
	// OOOO*: offset from the beginning of the installer
	//        FFFF if 0000 is escaped.
	//        EEEE if 0009 is escaped.
	fseek(finst, 0, SEEK_END);
	if (   fputc(0, finst) == EOF
	    || fputc(0, finst) == EOF) // the table size will be written here afterwards
		error("can't write nul escape table size to installer file");
	uint16_t *p16 = (uint16_t*)inbuf;
	uint16_t escape_table_size = 0;
	while (p16 < (uint16_t*)((char*)inbuf + inst_size)) {
		uint16_t escaped;
		if (*p16 == 0x0000)
			escaped = 0xFFFF;
		else if (*p16 == 0x0009)
			escaped = 0xEEEE;
		else {
			p16++;
			continue;
		}
		// write the escaped half-word
		fseek(finst, (char*)p16 - (char*)inbuf, SEEK_SET);
		if (   fputc(escaped & 0x00FF, finst) == EOF
			|| fputc((escaped & 0xFF00) >> 8, finst) == EOF)
			error("can't write escaped half-word");
		// write the offset
		fseek(finst, 0, SEEK_END);
		if (   fputc(((char*)p16 - (char*)inbuf) & 0x00FF, finst) == EOF
			|| fputc((((char*)p16 - (char*)inbuf) & 0xFF00) >> 8, finst) == EOF)
			error("can't write to escape table to installer file");
		escape_table_size++;
		p16++;
	}
	// write the table size
	fseek(finst, inst_size, SEEK_SET);
	if (   fputc(escape_table_size & 0x00FF, finst) == EOF
	    || fputc((escape_table_size & 0xFF00) >> 8, finst) == EOF)
		error("can't write escape table size to installer file");
	free(inbuf);
	fclose(finst);
	return 0;
}
