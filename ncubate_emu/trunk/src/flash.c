#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emu.h"

struct flash {
	u32 operation;
	union {
		u32 full_address;
		struct {
			u8 column_address;
			u8 page_address_lo;
			u8 page_address_mid;
			u8 page_address_hi;
		};
	};
	u32 op_size;
	u32 ram_address;
	u16 area;             /* 0x000=A, 0x100=B, 0x200=C */
} flash;

/* 64k pages of 512+16 bytes each (32 MB total) */
#define NUM_BLOCKS 0x800
#define PAGES_PER_BLOCK 0x20
#define PAGE_SIZE 0x210

#define NUM_PAGES (NUM_BLOCKS * PAGES_PER_BLOCK)
#define BLOCK_SIZE (PAGES_PER_BLOCK * PAGE_SIZE)

u8 flash_data[NUM_PAGES][PAGE_SIZE];
u8 flash_block_modified[NUM_BLOCKS];

static void *ram_ptr_nonull(u32 addr, u32 size) {
	void *p = phys_mem_ptr(addr, size);
	if (!p)
		error("NAND flash: address %x is not in RAM\n", addr);
	return p;
}

static void flash_read_page() {
	u32 page = flash.full_address >> 8;
	if (page >= NUM_PAGES)
		error("NAND flash: read nonexistent page %x", page);

	if (flash.ram_address != 0) {
		u8 *pagedata = flash_data[page];
		u32 offset = flash.area + flash.column_address;
		if (offset + flash.op_size > PAGE_SIZE)
			error("NAND flash: read too long (%d bytes from %x)", flash.op_size, offset);

		memcpy(ram_ptr_nonull(flash.ram_address, flash.op_size), &pagedata[offset], flash.op_size);
	}
}

static void flash_erase_block() {
	u32 page = flash.full_address;
	if (page & (-NUM_PAGES | (PAGES_PER_BLOCK - 1)))
		error("NAND flash: erase nonexistent block %x", page);
	memset(&flash_data[page][0], 0xFF, BLOCK_SIZE);
	flash_block_modified[page / PAGES_PER_BLOCK] = true;
}

static void flash_program_page() {
	u32 page = flash.full_address >> 8;
	if (page >= NUM_PAGES)
		error("NAND flash: program nonexistent page %x", page);

	u32 offset = /*flash.area + */flash.column_address;
	u32 i;
	if (offset + flash.op_size > PAGE_SIZE)
		error("NAND flash: program too long (%d bytes from %x)", flash.op_size, offset);

	u8 *pagedata = &flash_data[page][offset];
	u8 *progdata = ram_ptr_nonull(flash.ram_address + offset, flash.op_size);
	for (i = 0; i < flash.op_size; i++)
		pagedata[i] &= progdata[i];
	flash_block_modified[page / PAGES_PER_BLOCK] = true;
}

static void flash_begin_op() {
	logprintf(LOG_FLASH, "NAND flash: op=%06x addr=%08x size=%08x raddr=%08x\n",
		flash.operation, flash.full_address, flash.op_size, flash.ram_address);

	switch (flash.operation & 0xFF) {
		case 0x00: /* Read Area A */
			flash.area = 0;
			flash_read_page();
			break;
		case 0x50: /* Read Area C */
			flash.area = 0x200;
			flash_read_page();
			break;
		case 0x60: /* Block Erase */
			flash_erase_block();
			break;
		case 0x70: /* Read Status Register */
			break;
		case 0x80: /* Page Program */
			flash_program_page();
			break;
		case 0x90: /* Read Electronic Signature */
			/* ST Micro NAND256R3A */
			memcpy(ram_ptr_nonull(flash.ram_address, 2), "\x20\x35", 2);
			break;
		case 0xFF: /* Reset */
			break;
		default:
			error("NAND flash: Unknown operation type");
	}
}

u32 nand_flash_read_word(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x00: return 0; /* ??? */
		case 0x08: return 0; /* "Operation in progress" register */
		case 0x34: return 0x40; /* Status register (bit 0 = error, bit 6 = ready, bit 7 = writeprot */
		case 0x40: return 1; /* ??? */
		case 0x44: return 0xFFFFFF; /* Calculate page ECC */
	}
	return bad_read_word(addr);
}

void nand_flash_write_word(u32 addr, u32 value) {
	switch (addr & 0x3FFFFFF) {
		case 0x00: return;
		case 0x04: return;
		case 0x08:
			if (value != 1)
				error("NAND flash: wrote something other than 1 to reg 8");
			flash_begin_op();
			return;
		case 0x0C: flash.operation = value;        return;
		case 0x10: flash.column_address = value;   return;
		case 0x14: flash.page_address_lo = value;  return;
		case 0x18: flash.page_address_mid = value; return;
		case 0x1C: flash.page_address_hi = value;  return;
		case 0x24: flash.op_size = value;          return;
		case 0x28: flash.ram_address = value;      return;
		case 0x2C: return; /* AHB speed / 2500000 */
		case 0x30: return; /* ASB speed / 250000  */
		case 0x40: return;
		case 0x44: return;
		case 0x48: return;
		case 0x4C: return;
		case 0x50: return;
		case 0x54: return;
	}
	bad_write_word(addr, value);
}

char flash_filename[MAX_PATH];

void flash_save_changes() {
	FILE *f = fopen(flash_filename, "r+b");
	if (!f) {
		printf("NAND flash: could not open ");
		perror(flash_filename);
		exit(1);
	}
	u32 block, count = 0;
	for (block = 0; block < NUM_BLOCKS; block++) {
		if (flash_block_modified[block]) {
			fseek(f, block * BLOCK_SIZE, SEEK_SET);
			fwrite(flash_data[block * PAGES_PER_BLOCK], BLOCK_SIZE, 1, f);
			flash_block_modified[block] = false;
			count++;
		}
	}
	fclose(f);
	printf("NAND flash: saved %d modified blocks to file\n", count);
}

int flash_save_as(void) {
	printf("Saving flash image %s...\n", flash_filename);
	FILE *f = fopen(flash_filename, "wb");
	if (!f) {
		printf("NAND flash: could not open ");
		perror(flash_filename);
		exit(1);
	}
	if (!fwrite(flash_data, sizeof flash_data, 1, f) || fflush(f)) {
		fclose(f);
		remove(flash_filename);
		printf("NAND flash: could not write to ");
		perror(flash_filename);
		exit(1);
	}
	fclose(f);
	memset(flash_block_modified, 0, sizeof(flash_block_modified));
	printf("Flash image saved.\n");
	return 0;
}

static int preload(int page, char *name, char *filename) {
	FILE *f = fopen(filename, "rb");
	if (!f) {
		perror(filename);
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	u32 size = ftell(f);
	fseek(f, 0, SEEK_SET);

	int offset = 0x20;

	strcpy((char *)&flash_data[page][0], name);
	*(u32 *)&flash_data[page][20] = BSWAP32(0x55F00155);
	*(u32 *)&flash_data[page][24] = 0;
	*(u32 *)&flash_data[page][28] = BSWAP32(size);

	while (fread(&flash_data[page][offset], 1, 0x200 - offset, f)) {
		page++;
		if (page == NUM_PAGES) {
			printf("Preload image(s) too large\n");
			exit(1);
		}
		offset = 0;
	}

	// Round to next block
	return (page + 0x1F) & -0x20;
}

void flash_load(const char *filename) {
	strncpy(flash_filename, filename, sizeof(flash_filename));
	FILE *f = fopen(filename, "r+b");
	if (!f) {
		perror(filename);
		exit(1);
	}
	if (!fread(flash_data, sizeof flash_data, 1, f)) {
		printf("Could not read flash image from %s\n", flash_filename);
		exit(1);
	}
	fclose(f);
	memset(flash_block_modified, 0, sizeof(flash_block_modified));
}

void flash_reload(void) {
	flash_load(flash_filename);
}

void flash_initialize(char *preload_boot2, char *preload_diags, char *preload_os) {
	memset(flash_data, 0xFF, sizeof flash_data);

	/* After creating the filesystem, the first 16kB block of flash is
	 * copied to a file called /phoenix/manuf.dat. To pass OS validation,
	 * that file must contain the following values. */
	*(u32 *)&flash_data[0][0] = 0x796EB03C;
	/* First two hex digits of product ID */
	*(u16 *)&flash_data[4][4] = emulate_cas ? 0x0C : 0x0E;
	/* Third hex digit of product ID. Exact value doesn't seem to matter,
	 * but must be < 0x10. */
	*(u16 *)&flash_data[4][6] = 0;

	int page = 0x1000;
	if (preload_boot2) page = preload(page, "***PRELOAD_BOOT2***", preload_boot2);
	if (preload_diags) page = preload(page, "***PRELOAD_DIAGS***", preload_diags);
	if (preload_os)    page = preload(page, "***PRELOAD_IMAGE***", preload_os);
}

struct flash_saved_state {
	struct flash flash;
};

void *flash_save_state(size_t *size) {
	*size = sizeof(struct flash_saved_state);
	struct flash_saved_state *state = malloc(*size);
	memcpy(&state->flash, &flash, sizeof(flash));
	return state;
}

void flash_reload_state(void *state) {
	struct flash_saved_state *_state = (struct flash_saved_state *)state;
	memcpy(&flash, &_state->flash, sizeof(flash));
}
