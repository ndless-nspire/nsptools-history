#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE				1024
#define DEFAULT_N_BLOCKS	22000

#define xstr(s) str(s)
#define str(s) #s

void printUsage(void) {
	puts("Usage: MakeLoader <loader.bin> <output.xml> [<n blocks>]");	
}

void printErrorAndExit(const char* msg, int error_code) {
	fprintf(stderr, "Error: %s.\n", msg);
	exit(error_code);	
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

int createPaginedFile(FILE* fileInput, FILE* fileOutput, int page_size, int n_pages) {
  unsigned char* buffer = malloc(page_size);
  if (buffer == NULL) {
    return 0;
  }

  memset(buffer, 0x00, page_size);
  fread(buffer, 1, page_size, fileInput);
  while (n_pages--) {
    fwrite(buffer, 1, page_size, fileOutput);
  }
  free(buffer);

  return -1;
}

int main(int argc, const char* argv[]) {
	FILE* pFileLoader = NULL;
	FILE* pFileOutput = NULL;
	int nBlocks = DEFAULT_N_BLOCKS;
  int success = 0;
	
	if ((argc < 3) || (argc > 4)) {
    printErrorAndExit("Too few or too many arguments specified", 1);
		printUsage();
	}
	
	// Get parameters
  pFileLoader = fopen(argv[1], "rb");
	if (pFileLoader == NULL) {
    printErrorAndExit("Cannot open the loader", 2);
	}

  pFileOutput = fopen(argv[2], "wb");
  if (pFileOutput == NULL) {
    fclose(pFileLoader);
    printErrorAndExit("Cannot create the output file", 3);
  }
	
  if (argc == 4) {
    nBlocks = atoi(argv[3]);
  }

  if (getFileSize(pFileLoader) > BLOCK_SIZE) {
    fclose(pFileLoader);
    fclose(pFileOutput);
    printErrorAndExit("The loader size must be less than "xstr(BLOCK_SIZE)" bytes", 4);
  }

  // Create the output file
  success = createPaginedFile(pFileLoader, pFileOutput, BLOCK_SIZE, nBlocks);

  fclose(pFileLoader);
  fclose(pFileOutput);

  if (success == 0) {
    fprintf(stderr, "Error: An error occurred while writing the output file.\n");
  }
  else {
    printf("'%s' successfully created (%d bytes written)!\n", argv[1], nBlocks * BLOCK_SIZE);
  }

	return EXIT_SUCCESS;	
}
