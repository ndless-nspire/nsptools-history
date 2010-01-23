#include "os.h"

#define BOOT2_SIZE 0x140000
#define SCR_ADDR (void*)0xA4000100
#define SCR_SIZE 38400

#define read_flash (_oscall(void, read_flash, void* dest, int size, int offset, int, int percent_max, void *progress_cb))
#define read_flash_addr 0x1015F3A0

void fillscr(char c) {
	memset(SCR_ADDR, c, SCR_SIZE);
}

void error(void) {
	fillscr(0b11110000);
}

void done(void) {
	fillscr(0);
}


int main(void) {
	FILE *ofile = fopen("/documents/ndless-installation/boot2nand.tns", "wb");
	if (!ofile) {
		error();
		return 1;
	}
	void *buf = malloc(BOOT2_SIZE);
	if (!buf) {
		fclose(ofile);
		error();
		return 1;
	}
	read_flash(buf, BOOT2_SIZE, 0x4000, 0, 0, NULL);
	if (fwrite(buf, 1, BOOT2_SIZE, ofile) != BOOT2_SIZE) {
		free(buf);
		fclose(ofile);
		error();
		return 1;
	}
	free(buf);
	fclose(ofile);
	done();
	return 0;
}
