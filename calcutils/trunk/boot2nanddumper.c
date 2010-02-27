#include <os.h>

#define BOOT2_SIZE 0x140000

#ifdef CAS
#define read_nand_ 0x1015F3D0
#else
#define read_nand_ 0x1015F3A0
#endif
#define read_nand (_oscall(void, read_nand_, void* dest, int size, int offset, int, int percent_max, void *progress_cb))

asm(".string \"PRG\"\n");
int main(void) {
	TCT_Local_Control_Interrupts(0);
	FILE *ofile = fopen("/documents/ndless/boot2dump.tns", "wb");
	if (!ofile) {
		log_rs232("can't open output file");
		return 1;
	}
	void *buf = malloc(BOOT2_SIZE);
	if (!buf) {
		fclose(ofile);
		log_rs232("can't malloc");
		return 1;
	}
	read_nand(buf, BOOT2_SIZE, 0x4000, 0, 0, NULL);
	if (fwrite(buf, 1, BOOT2_SIZE, ofile) != BOOT2_SIZE) {
		free(buf);
		fclose(ofile);
		log_rs232("can't write output file");
		return 1;
	}
	free(buf);
	fclose(ofile);
	log_rs232("boot2 dumped!");
	return 0;
}
