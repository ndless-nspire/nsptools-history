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
 * The Initial Developer of the Original Code is Fabian Vogt.
 * Portions created by the Initial Developer are Copyright (C) 2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Olivier ARMAND <olivier.calc@gmail.com>
 ****************************************************************************/

#include <stdio.h>
#include <stdint.h>

static unsigned file_size(FILE *f) {
  int pos, end;
  pos = ftell(f);
  fseek (f, 0, SEEK_END);
  end = ftell(f);
  fseek (f, pos, SEEK_SET);
  return end;
}

/* Format of escaped installer:
 * 4-byte size
 * FF byte0 FF byte1 FF byte2 ...
 */
int main(int argc, char* argv[]) {
	if(argc != 3) {
		puts("Usage: EscapeInst <in_installer.bin> <out_installer.bin>\n"
			 "Escape forbidden half-words (0000 and 0009).\n"
		     "stage0 will unescape them.");
		return 1;
	}
	
	FILE *input = fopen(argv[1], "rb");
	if (!input) {
		printf("Open #1 failed.\n");
		return 1;
	}
	FILE *output = fopen(argv[2], "wb");
	if(!output)	{
		printf("Open #2 failed.\n");
		return 1;
	}
	
	uint32_t filesize = file_size(input);
	uint32_t escaped_size = (filesize >> 2) * 8;
	if (filesize % 4 > 0)
		escaped_size += 8;

	//escaped_size & 0xFFFF == 0x0009 can never happen
	if (escaped_size > 0xFFFFFF || (escaped_size & 0xFFFF) == 0x0000) {
		printf("Unsupported size.\n");
		return 1;
	}

	escaped_size = escaped_size | 0xFF000000;
	fseek(input, 0, SEEK_SET);
	
	fwrite(&escaped_size, 4, 1, output);
	
	uint32_t word;
	//If we read less than 4 it doesn't matter
	while (fread(&word, 1, 4, input) > 0) {
		uint8_t byte0 = word & 0xFF;
		word >>= 8;
		uint8_t byte1 = word & 0xFF;
		word >>= 8;
		uint8_t byte2 = word & 0xFF;
		word >>= 8;
		uint8_t byte3 = word & 0xFF;

		uint32_t newword_1 = 0xFF00FF00 | byte0 << 16 | byte1;
		uint32_t newword_2 = 0xFF00FF00 | byte2 << 16 | byte3;
		
		fwrite(&newword_1, 4, 1, output);
		fwrite(&newword_2, 4, 1, output);
	}
	
	fclose(input);
	fclose(output);
	return 0;
}
