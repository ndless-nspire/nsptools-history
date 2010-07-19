#include <os.h>

#define NAND_PAGE_SIZE 512
#define DIAGS_SIZE (0x500 * NAND_PAGE_SIZE)
#define DIAGS_NAND_OFFSET (0xB00 * NAND_PAGE_SIZE)

#ifdef CAS
#define read_nand_ 0x1015F3D0
#else
#define read_nand_ 0x1015F3A0
#endif
#define read_nand (_oscall(void, read_nand_, void* dest, int size, int offset, int, int percent_max, void *progress_cb))

asm(".string \"PRG\"\n");
int main(void) {
	TCT_Local_Control_Interrupts(0);
	FILE *ofile = fopen("/documents/ndless/diagsdump.tns", "wb");
	if (!ofile) {
		log_rs232("can't open output file");
		return 1;
	}
	void *buf = malloc(DIAGS_SIZE);
	if (!buf) {
		fclose(ofile);
		log_rs232("can't malloc");
		return 1;
	}
	read_nand(buf, DIAGS_SIZE, DIAGS_NAND_OFFSET, 0, 0, NULL);
	if (fwrite(buf, 1, DIAGS_SIZE, ofile) != DIAGS_SIZE) {
		free(buf);
		fclose(ofile);
		log_rs232("can't write output file");
		return 1;
	}
	free(buf);
	fclose(ofile);
	log_rs232("diags dumped!");
	return 0;
}
