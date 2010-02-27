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
 * The Initial Developer of the Original Code is Geoffrey ANNEHEIM
 * <geoffrey.anneheim@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Olivier ARMAND <olivier.calc@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t getFileSize(FILE *f) {
	int pos;
	int end;
	
	pos = ftell(f);
	fseek (f, 0, SEEK_END);
	end = ftell(f);
	fseek (f, pos, SEEK_SET);
	
	return end;
}

int main(int argc, const char* argv[]) {
	static const char prg_signature[4] = "PRG\0";
	
	FILE* fIn, *fOut;
	size_t size;
	unsigned long data;
  int n;
	void* buff;
	char *pOutTns;
	#define MAX_PATH_SIZE 256
	#define TNS_EXT ".tns"
	char outTns[MAX_PATH_SIZE + sizeof(TNS_EXT) + sizeof('\0')];
	
	if (argc < 2 || argc > 3) {
		puts("Usage: MakeTNS <in.bin> [<out.tns>]\n"
				 "Creates an executable file from a raw binary file.\n"
				 "The input file must have the signature \"PRG\\0\" before its entry point.");
		return 0;
	}
	if (argc == 3) {
		pOutTns = (char*)argv[2];
	}
	else {
		// determine the output file name
		if (strlen(argv[1]) >= MAX_PATH_SIZE) {
			printf("Error: invalid parameter '%s'\n", argv[1]);
			return 1;
		}
		strncpy(outTns, argv[1], sizeof(outTns));
		// search file extension
		pOutTns = strrchr(outTns, '/');
		pOutTns = pOutTns == NULL ? outTns : pOutTns + 1;
		char *pOutTns2 = strrchr(pOutTns, '\\');
		pOutTns = pOutTns2 == NULL ? pOutTns : pOutTns2 + 1;
		char *pExt = strrchr(pOutTns, '.');
		if (pExt != NULL)
			strcpy(pExt, TNS_EXT);
		else
			strcat(pOutTns,TNS_EXT);
		pOutTns = outTns;
	}

	fIn = fopen(argv[1], "rb");
	fOut = fopen(pOutTns, "wb");
	if (!fIn) {
		printf("Error: could not open '%s'", argv[1]);
		return 1;
	}
	if (!fOut) {
		printf("Error: could not open '%s'", pOutTns);
		return 1;
	}

	size = getFileSize(fIn);
	
	// search the signature
	do {
		n = fread(&data, sizeof(unsigned long), 1, fIn);
		if (n == 0) {
			puts("Error: could not find the signature.");
			fclose(fIn);
			fclose(fOut);
			return 1;
		}
		size -= 4;
	} while(memcmp((char*)&data, prg_signature, sizeof(prg_signature)));
	
	buff = malloc(size + 4);
	memcpy(buff, prg_signature, sizeof(prg_signature));
	if (fread(buff + 4, 1, size, fIn) != size) {
		perror("Could not read input file");
		fclose(fOut);
		unlink(outTns);
		return 1;
	}
	if (fwrite(buff, 1, size + 4, fOut) != size + 4) {
		perror("Could not create output file");
		fclose(fOut);
		unlink(outTns);
		return 1;
	}
	
	free(buff);
					
	fclose(fIn);
	fclose(fOut);
	
	printf("'%s' successfully created!\n", pOutTns);

	return 0;
}
