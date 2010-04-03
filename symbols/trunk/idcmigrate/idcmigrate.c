#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define PHX_BASE_ADDRESS 0x10000000

void abort_with_err(char *errformat, ...) {
  #define STRBUF_SIZE 300
  char strbuf[300];
  va_list vl;
  va_start(vl, errformat);
  vsnprintf(strbuf, STRBUF_SIZE, errformat, vl);
  perror(strbuf);
  va_end(vl);
  exit(1);
}

int main(int argc, char* argv[]) {
  char *phoenix_from, *idc, *phoenix_to, *syms_out, *missing_syms_out = NULL, *phoenix_from_ptr, *phoenix_to_ptr;
  FILE *fphoenix_from, *fidc, *fphoenix_to, *fsyms_out, *fmissing_syms_out = NULL;
  long fphoenix_from_size, fphoenix_to_size;

  if (argc < 5 || argc > 6) {
    puts("Usage: idcmigrate <phoenix_from.raw> <syms.idc> <phoenix_to.raw> <syms_out.idc> <missing_syms_out.txt>\n" \
         "Ports symbols to a new phoenix.raw.");
    return 0;
  }
  phoenix_from = argv[1];
  idc = argv[2];
  phoenix_to = argv[3];
  syms_out = argv[4];
  if (argc == 6)
    missing_syms_out = argv[5];
  
  if (!(fphoenix_from = fopen(phoenix_from, "rb")))
    abort_with_err("Could not open %s", phoenix_from);
  if (!(fidc = fopen(idc, "rb")))
    abort_with_err("Could not open %s", idc);
  if (!(fphoenix_to = fopen(phoenix_to, "rb")))
    abort_with_err("Could not open %s", phoenix_to);
  if (!(fsyms_out = fopen(syms_out, "wb")))
    abort_with_err("Could not create %s", syms_out);
  if (missing_syms_out) {
    if (!(fmissing_syms_out = fopen(missing_syms_out, "wb")))
      abort_with_err("Could not create %s", missing_syms_out);
  }
  
  fseek(fphoenix_from, 0, SEEK_END);
  fphoenix_from_size = ftell(fphoenix_from);
  rewind(fphoenix_from);
  phoenix_from_ptr = malloc(fphoenix_from_size);
  if (fread(phoenix_from_ptr, 1, fphoenix_from_size, fphoenix_from) != (size_t)fphoenix_from_size)
    abort_with_err("Could not read %s", phoenix_from);
    
  fseek(fphoenix_to, 0, SEEK_END);
  fphoenix_to_size = ftell(fphoenix_to);
  rewind(fphoenix_to);
  phoenix_to_ptr = malloc(fphoenix_to_size);
  if (fread(phoenix_to_ptr, 1, fphoenix_to_size, fphoenix_to) != (size_t)fphoenix_to_size)
    abort_with_err("Could not read %s", phoenix_to);
  
  unsigned int linenum = 0;
  #define LINE_SIZE 200
  char line[LINE_SIZE];
  char *s = line;
  
  do {
    #define TOKEN_SIZE 150
    #define TOKEN_SIZE_S "150"
    char name[TOKEN_SIZE];
    char *ptr;
    unsigned int address;
    
    fflush(stdout);
    linenum++;
    if (!(s = fgets(line, LINE_SIZE, fidc)))
      break;
    if (sscanf(line, " MakeName (%x, \"%" TOKEN_SIZE_S "s", &address, name) != 2)
      continue;
    if (address < PHX_BASE_ADDRESS)
      continue;
    address -= PHX_BASE_ADDRESS;
    if (!(s = strstr(name, "\");")))
      continue;
    *s = '\0'; // remove trailing characters
    
    if (*name == 'a' && *(name + 1) != '\0' && (isupper(*(name + 1)) || *(name + 1) == '_' || isdigit(*(name + 1)))) // it's a string symbol, skip it
      continue;
    if (   !strncmp("nullsub", name, sizeof("nullsub") - 1)
        || !strncmp("j_", name, sizeof("j_") - 1))
      continue;
    
    printf("\nLooking for %s...", name);
    #define CMP_SIZE 24
    #define MAX_SHIFT_SIZE 4
    int shift, found = 0;
    for (shift = 0; shift < MAX_SHIFT_SIZE * 4; shift += 4) { 
      if (phoenix_from_ptr + address + shift + CMP_SIZE >= phoenix_from_ptr + fphoenix_from_size)
        break;
      ptr = phoenix_to_ptr + shift;
      while (ptr + CMP_SIZE < phoenix_to_ptr + fphoenix_to_size) {
        if (!memcmp(phoenix_from_ptr + address + shift, ptr, CMP_SIZE)) {
          found = 1;
          break;
        }
        ptr += 4;
      }
      if (found)
        break;
    }
    if (!found) {
      if (fmissing_syms_out) {
        fprintf(fmissing_syms_out, "%s\n", name);
        fflush(fmissing_syms_out);
      }
      continue;
    }
    unsigned int new_address = PHX_BASE_ADDRESS + ptr - phoenix_to_ptr - shift;
    printf("\tFound at %#x", new_address);
    fprintf(fsyms_out, " 	MakeName	(%#X,	 \"%s\");\n", new_address, name);
    fflush(fsyms_out);
  } while(1);
  
  fclose(fsyms_out);
  if (fmissing_syms_out) fclose(fmissing_syms_out);
  puts("\nDone.");
  return 0;
}
