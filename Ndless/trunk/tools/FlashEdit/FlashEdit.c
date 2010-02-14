#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOADER_RES_FILE_BYTES_SIZE      5192
#define ORIGINAL_RES_FILE_BYTES_SIZE    10855
#define FLASH_PAGE_BYTES_SIZE           512
#define HEADER_ORIG_RESOURCE            0x6801

void printUsage() {
  puts(
    "Usage: FlashEdit -b <strbackup.tns> <flash_rom.bin> <resource_offset>\n"
    "       FlashEdit <loader.tns> <flash_rom.bin> <resource_offset>\n"
    "       \n"
    "   -b                backup the original strings.res\n"
    "   <strbackup.tns>   backup file to create\n"
    "   <loader.tns>      Ndless loader to install\n"
    "   <flash_rom.bin>   nspire_emu ROM image\n"
    "   <resource_offset> offset of the resource file\n"
    "                     'phoenix/syst/locales/en/strings.res' in the ROM image"
  );
}

void printErrorAndExit(const char* msg, int code) {
  printf("Error: %s\n", msg);
  exit(code);
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

void backup(FILE* fOutResource, FILE* fFlash, unsigned long resource_offset) {
  char* flash_page = (char*)malloc(FLASH_PAGE_BYTES_SIZE);
  int total_size = 0, page_size;
    
  fseek(fFlash, resource_offset, SEEK_SET);
  do {
    page_size = fread(flash_page, 1, FLASH_PAGE_BYTES_SIZE, fFlash);
    total_size += page_size;
    fwrite(flash_page, 1,
      total_size > ORIGINAL_RES_FILE_BYTES_SIZE ? page_size - (total_size - ORIGINAL_RES_FILE_BYTES_SIZE): page_size,
      fOutResource);
    fseek(fFlash, 16, SEEK_CUR);
  } while (total_size < ORIGINAL_RES_FILE_BYTES_SIZE);
  
  free(flash_page);
  
}

void edit(FILE* fResource, FILE* fFlash, unsigned long resource_offset) {
  char* flash_page = (char*)malloc(FLASH_PAGE_BYTES_SIZE);
  int n;
    
  fseek(fFlash, resource_offset, SEEK_SET);
  while (!feof(fResource)) {
    n = fread(flash_page, 1, FLASH_PAGE_BYTES_SIZE, fResource);
    fwrite(flash_page, 1, n, fFlash);
    fseek(fFlash, 16, SEEK_CUR);
  }
  
  free(flash_page);
}

int main(int argc, char* argv[]) {
  FILE* fileNewResource, *fileFlash, *fileOrigResource;
  char *pathNewResource = NULL, *pathFlash = NULL, *pathOrigResource = NULL, *strOffset = NULL;
  int fileNewResourceSize;
  char headerOrigResource[2];
  unsigned long resource_offset;
  
  if (argc == 5 && !strcmp(argv[1], "-b")) {
    pathOrigResource = argv[2];
    pathFlash = argv[3];
    strOffset = argv[4];
  } else if (argc == 4) {
    pathNewResource = argv[1];
    pathFlash = argv[2];
    strOffset = argv[3];
  } else {
    printUsage();
    return -1; 
  }
  
  resource_offset = strtoul(strOffset, 0, 16);
  fileFlash = fopen(pathFlash, "r+b");
  if (fileFlash == NULL) {
    fclose(fileNewResource);
    printErrorAndExit("Could not open <flash_rom.bin>.", 2);  
  }
  
  // install
  if (pathNewResource) {
    fileNewResource = fopen(pathNewResource, "rb");
    if (fileNewResource == NULL) {
      printErrorAndExit("Could not open <infile.tns>.", 1); 
    }
    
    fileNewResourceSize = getFileSize(fileNewResource);
    if (fileNewResourceSize != LOADER_RES_FILE_BYTES_SIZE) {
      printf("Error: <infile.tns> size must be of %d bytes\n", LOADER_RES_FILE_BYTES_SIZE);
      fclose(fileNewResource);
      exit(3);
    }
    
    edit(fileNewResource, fileFlash, resource_offset);
    fclose(fileNewResource);
    printf("File '%s' successfully modified!\n", pathFlash);

  }
  
  // backup
  if (pathOrigResource) {
    fileOrigResource = fopen(pathOrigResource, "w+b");
      if (fileOrigResource == NULL) {
        printErrorAndExit("Could not create <strbackup.tns>.", 3);  
    }
    backup(fileOrigResource, fileFlash, resource_offset);
    fseek (fileOrigResource, 0, SEEK_SET);
    if (fread(headerOrigResource, 1, sizeof(headerOrigResource), fileOrigResource) !=  sizeof(headerOrigResource)
      || (headerOrigResource[0] << 8) + headerOrigResource[1] != HEADER_ORIG_RESOURCE) {
        fclose(fileOrigResource);
        unlink(pathOrigResource);
        printErrorAndExit("Resource not found in ROM image (the image must be an original one)", 4);
    }
    fclose(fileOrigResource);
    printf("File '%s' successfully created!\n", pathOrigResource);
  }
  
  fclose(fileFlash);
  
  return EXIT_SUCCESS;  
}
