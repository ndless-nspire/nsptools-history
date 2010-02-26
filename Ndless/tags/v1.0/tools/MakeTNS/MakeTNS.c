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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getFileSize(FILE *f) {
	int pos;
	int end;
  
	pos = ftell(f);
	fseek (f, 0, SEEK_END);
	end = ftell(f);
	fseek (f, pos, SEEK_SET);
  
	return end;
}

int main(int argc, const char* argv[]) {
  static const char* prg_signature = "PRG";
  
  FILE* fIn, *fOut;
  int size;
  unsigned long data;
  int n;
  void* buff;
  
  if (argc != 3) {
    puts("Usage: MakeTNS <bin> <tns>");
    return 0;
  }
  
  fIn = fopen(argv[1], "rb");
  fOut = fopen(argv[2], "wb");

  size = getFileSize(fIn);
  
  do {
    n = fread(&data, sizeof(unsigned long), 1, fIn);
    if (n == 0) {
      puts("Failed !");
      fclose(fIn);
      fclose(fOut);
      return 0;
    }
    size -= 4;
  } while(memcmp((char*)&data, prg_signature, sizeof(prg_signature)));
  
  buff = malloc(size + 4);
  memcpy(buff, prg_signature, sizeof(prg_signature));
  fread(buff + 4, size, 1, fIn);
  fwrite(buff, size + 4, 1, fOut);
  
  free(buff);
          
  fclose(fIn);
  fclose(fOut);
  
  printf("'%s' successfully created !\n", argv[2]);

  return 0;
}
