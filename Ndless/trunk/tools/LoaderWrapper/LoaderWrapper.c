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
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_RESOURCE_BYTES_SIZE			5180										// size of the first string resource: just enough to overflow the buffer
#define PAYLOAD_BASE_ADDRESS					0x1800E15C							// stack address where is located the resource buffer
#define MAX_PAYLOAD_BYTES_SIZE				FIRST_RESOURCE_BYTES_SIZE - 4 // 4: size of the function return address overwritten

void printUsage() {
	puts(
		"Usage: LoaderWrapper <infile.bin> <outfile.tns>\n"
		"<infile> contains raw ARM code"
	); 	
}

void printErrorAndExit(const char* msg, int code) {
	printf("Error: %s\n", msg);
	exit(code);
}

void writeLittleEndian(FILE* fOut, unsigned long i) {
	/*unsigned long iEndianness = ((i & 0xff) << 24) + ((i & 0xff00) << 8)
														+ ((i & 0xff0000) >> 8) + ((i >> 24) & 0xff);*/
														
	unsigned long iEndianness = i;	// Thanks of x86 architecture
														
	fwrite(&iEndianness, sizeof(unsigned long), 1, fOut);
}

int getFileSize(FILE *f) {
	int pos;
	int end;

	pos = ftell(f);
	fseek (f, 0, SEEK_END);
	end = ftell(f);
	fseek (f, pos, SEEK_SET);

	return end;
}

void wrap(FILE* fIn, FILE* fOut, int payload_size) {
	char* buffer;
	
	if (payload_size > MAX_PAYLOAD_BYTES_SIZE) {
		return;	
	}
	
	// Build a strings.res file (header)
	writeLittleEndian(fOut, 1); // number of string resources in the file
	writeLittleEndian(fOut, 0xFFFF); // a size used for overflow checking, not sure what it is, make it high
	writeLittleEndian(fOut, FIRST_RESOURCE_BYTES_SIZE);
	
	buffer = (char*)malloc(MAX_PAYLOAD_BYTES_SIZE);
	memset(buffer, 0x00, MAX_PAYLOAD_BYTES_SIZE);
	fread(buffer, payload_size, 1, fIn);
	fwrite(buffer, MAX_PAYLOAD_BYTES_SIZE, 1, fOut);
	free(buffer);
	
	writeLittleEndian(fOut, PAYLOAD_BASE_ADDRESS);
}

int main(int argc, char* argv[]) {
	FILE* fileInput, *fileOutput;
	int payload_size;
	
	if (argc != 3) {
		printUsage();
		return -1;	
	}
	
	fileInput = fopen(argv[1], "rb");
	if (fileInput == NULL) {
		printErrorAndExit("Could not open the input file.", 1);	
	}
	
	payload_size = getFileSize(fileInput);
	if (payload_size > MAX_PAYLOAD_BYTES_SIZE) {
		printf("Error: Input file cannot be bigger than %ul bytes", MAX_PAYLOAD_BYTES_SIZE);
		fclose(fileInput);
		exit(3);
	}
	
	fileOutput = fopen(argv[2], "wb");
	if (fileOutput == NULL) {
		fclose(fileInput);
		printErrorAndExit("Could not create the output file.", 2);	
	}
	
	wrap(fileInput, fileOutput, payload_size);
	
	fclose(fileInput);
	fclose(fileOutput);
	
	printf("File '%s' successfully created!\n", argv[2]);
	
	return EXIT_SUCCESS;	
}
