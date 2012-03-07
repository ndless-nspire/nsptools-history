#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emu.h"

struct nand_metrics {
	u8 chip_manuf, chip_model;
	u16 page_size;
	u8 log2_pages_per_block;
	u32 num_pages;
};

struct nand_metrics nand_metrics;
u8 *nand_data;
u8 *nand_block_modified;
bool nand_writable;
int nand_state = 0xFF;
u32 nand_row;
u32 nand_column;
u8 nand_buffer[0x840];
int nand_buffer_pos;

static const struct nand_metrics chips[] = {
	{ 0x20, 0x35, 0x210, 5, 0x10000 }, // ST Micro NAND256R3A
	{ 0xEC, 0xA1, 0x840, 6, 0x10000 }, // Samsung 1 GBit
};

void nand_initialize(bool large) {
	memcpy(&nand_metrics, &chips[large], sizeof(nand_metrics));
	nand_data = malloc(nand_metrics.page_size * nand_metrics.num_pages);
	nand_block_modified = calloc(nand_metrics.num_pages >> nand_metrics.log2_pages_per_block, 1);
}

void nand_write_command(u8 command, u32 address, int addr_bytes) {
	//printf("\nCommand %02X Addr %08X(%d) PC %08X\n", command, address, addr_bytes, arm.reg[15]);
	switch (command) {
		case 0x01: case 0x50:
			if (nand_metrics.page_size >= 0x800)
				goto unknown;
			// Fallthrough
		case 0x00:
			if (addr_bytes) {
				if (nand_metrics.page_size < 0x800) {
					nand_column = (command == 0x50 ? 2 : command) << 8 | (address & 0xFF);
					nand_row = address >> 8;
				} else {
					nand_column = address & 0xFFFF;
					nand_row = address >> 16;
				}
				if (nand_row >= nand_metrics.num_pages) {
					warn("bad row");
					nand_row &= nand_metrics.num_pages - 1;
				}
			}
			nand_state = command & ~1;
			break;
		case 0x10:
			if (nand_state == 0x80) {
				if (!nand_writable)
					error("erase with write protect on");
				u8 *pagedata = &nand_data[nand_row * nand_metrics.page_size + nand_column];
				int i;
				for (i = 0; i < nand_buffer_pos; i++)
					pagedata[i] &= nand_buffer[i];
				nand_block_modified[nand_row >> nand_metrics.log2_pages_per_block] = true;

				nand_state = 0xFF;
			}
			break;
		case 0x30: // ???
			break;
		case 0x60: {
			u32 valid_bits = nand_metrics.num_pages - (1 << nand_metrics.log2_pages_per_block);
			if (addr_bytes < 2)
				error("ERASE op missing address");
			nand_row = address;
			if (nand_row & ~valid_bits) {
				warn("NAND flash: erase nonexistent block %x", nand_row);
				nand_row &= valid_bits; // Assume extra bits ignored like read
			}
			nand_state = command;
			break;
		}
		case 0x80:
			if (nand_metrics.page_size < 0x800) {
				if (addr_bytes < 3)
					error("PROGRAM op missing address");
				//nand_column = (nand_column & ~0xFF) | (address & 0xFF);
				nand_column = address & 0xFF;
				nand_row = address >> 8;
			} else {
				if (addr_bytes < 4)
					error("PROGRAM op missing address");
				nand_column = address & 0xFFFF;
				nand_row = address >> 16;
			}
			if (nand_row >= nand_metrics.num_pages) {
				warn("bad row");
				nand_row &= nand_metrics.num_pages - 1;
			}
			nand_buffer_pos = 0;
			nand_state = command;
			break;
		case 0xD0:
			if (nand_state == 0x60) {
				if (!nand_writable)
					error("erase with write protect on");
				memset(&nand_data[nand_row * nand_metrics.page_size], 0xFF,
				       nand_metrics.page_size << nand_metrics.log2_pages_per_block);
				nand_block_modified[nand_row >> nand_metrics.log2_pages_per_block] = true;
				nand_state = 0xFF;
			}
			break;
		case 0xFF:
			nand_row = 0;
			nand_column = 0;
			// fallthrough
		case 0x70:
		case 0x90:
			nand_state = command;
			break;
		default:
		unknown:
			warn("Unknown NAND command %02X", command);
			//debugger();
	}
	//printf("State %02X row %08X column %04X\n", nand_state, nand_row, nand_column);
}

u8 nand_read_data_byte() {
	//putchar('r');
	switch (nand_state) {
		case 0x00:
		case 0x50:
			if (nand_column >= nand_metrics.page_size) {
				//warn("read past end of page");
				return 0;
			}
			return nand_data[nand_row * nand_metrics.page_size + nand_column++];
		case 0x70: return 0x40 | (nand_writable << 7); // Status register
		case 0x90: nand_state++; return nand_metrics.chip_manuf;
		case 0x90+1: nand_state = 0xFF; return nand_metrics.chip_model;
		default:
			//printf("NAND read byte in state %02X\n", nand_state);
			//debugger();
			return 0;
	}
}
u32 nand_read_data_word() {
	//putchar('R');
	switch (nand_state) {
		case 0x00:
		case 0x50:
			if (nand_column + 4 > nand_metrics.page_size) {
				//warn("read past end of page\n");
				return 0;
			}
			return *(u32 *)&nand_data[nand_row * nand_metrics.page_size + (nand_column += 4) - 4];
		case 0x70: return 0x40 | (nand_writable << 7); // Status register
		case 0x90: nand_state = 0xFF; return nand_metrics.chip_model << 8 | nand_metrics.chip_manuf;
		default:
			//printf("NAND read word in state %02X\n", nand_state);
			//debugger();
			return 0;
	}
}
void nand_write_data_byte(u8 value) {
	//putchar('w');
	switch (nand_state) {
		case 0x80:
			if (nand_buffer_pos + nand_column >= nand_metrics.page_size)
				warn("program past end of page");
			else
				nand_buffer[nand_buffer_pos++] = value;
			return;
		default:
			printf("NAND write in state %02X\n", nand_state);
			debugger();
			return;
	}
}
void nand_write_data_word(u32 value) {
	//putchar('W');
	switch (nand_state) {
		case 0x80:
			if (nand_buffer_pos + nand_column + 4 > nand_metrics.page_size)
				error("program past end of page");
			*(u32 *)&nand_buffer[nand_buffer_pos] = value;
			nand_buffer_pos += 4;
			break;
		default:
			printf("NAND write in state %02X\n", nand_state);
			debugger();
			return;
	}
}


static u32 parity(u32 word) {
	word ^= word >> 16;
	word ^= word >> 8;
	word ^= word >> 4;
	return 0x6996 >> (word & 15) & 1;
}
static u32 ecc_calculate(u8 page[512]) {
	u32 ecc = 0;
	u32 *in = (u32 *)page;
	u32 temp[64];
	int i, j;
	u32 words;

	for (j = 64; j != 0; j >>= 1) {
		words = 0;
		for (i = 0; i < j; i++) {
			words ^= in[i];
			temp[i] = in[i] ^ in[i + j];
		}
		ecc = ecc << 2 | parity(words);
		in = temp;
	}

	words = temp[0];
	ecc = ecc << 2 | parity(words & 0x0000FFFF);
	ecc = ecc << 2 | parity(words & 0x00FF00FF);
	ecc = ecc << 2 | parity(words & 0x0F0F0F0F);
	ecc = ecc << 2 | parity(words & 0x33333333);
	ecc = ecc << 2 | parity(words & 0x55555555);
	return (ecc | ecc << 1) ^ (parity(words) ? 0x555555 : 0xFFFFFF);
}

struct {
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
	u32 ecc;
} nand_phx;
u32 nand_phx_read_word(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x00: return 0; /* ??? */
		case 0x08: return 0; /* "Operation in progress" register */
		case 0x34: return 0x40; /* Status register (bit 0 = error, bit 6 = ready, bit 7 = writeprot */
		case 0x40: return 1; /* ??? */
		case 0x44: return nand_phx.ecc; /* Calculate page ECC */
	}
	return bad_read_word(addr);
}
void nand_phx_write_word(u32 addr, u32 value) {
	switch (addr & 0x3FFFFFF) {
		case 0x00: return;
		case 0x04: nand_writable = value; return;
		case 0x08: // Begin operation
			if (value != 1)
				error("NAND controller: wrote something other than 1 to reg 8");

			logprintf(LOG_FLASH, "NAND controller: op=%06x addr=%08x size=%08x raddr=%08x\n",
				nand_phx.operation, nand_phx.full_address, nand_phx.op_size, nand_phx.ram_address);

			nand_write_command(nand_phx.operation, nand_phx.full_address, nand_phx.operation >> 8 & 7);

			if (nand_phx.operation & 0x400800) {
				u8 *ptr = phys_mem_ptr(nand_phx.ram_address, nand_phx.op_size);
				if (!ptr)
					error("NAND controller: address %x is not in RAM\n", addr);

				if (nand_phx.operation & 0x000800) {
					u32 i;
					for (i = 0; i < nand_phx.op_size; i++)
						nand_write_data_byte(ptr[i]);
				} else {
					u32 i;
					for (i = 0; i < nand_phx.op_size; i++)
						ptr[i] = nand_read_data_byte();
				}

				if (nand_phx.op_size >= 0x200) { // XXX: what really triggers ECC?
					// Set ECC register
					if (!memcmp(&nand_data[0x206], "\xFF\xFF\xFF", 3))
						nand_phx.ecc = 0xFFFFFF; // flash image created by old version of nspire_emu
					else
						nand_phx.ecc = ecc_calculate(ptr);
				}
			}

			if (nand_phx.operation & 0x100000) // Confirm code
				nand_write_command(nand_phx.operation >> 12, 0, 0);
			return;
		case 0x0C: nand_phx.operation = value;        return;
		case 0x10: nand_phx.column_address = value;   return;
		case 0x14: nand_phx.page_address_lo = value;  return;
		case 0x18: nand_phx.page_address_mid = value; return;
		case 0x1C: nand_phx.page_address_hi = value;  return;
		case 0x20: return; /* ??? */
		case 0x24: nand_phx.op_size = value;          return;
		case 0x28: nand_phx.ram_address = value;      return;
		case 0x2C: return; /* AHB speed / 2500000 */
		case 0x30: return; /* APB speed / 250000  */
		case 0x40: return;
		case 0x44: return;
		case 0x48: return;
		case 0x4C: return;
		case 0x50: return;
		case 0x54: return;
	}
	bad_write_word(addr, value);
}

u8 nand_cx_read_byte(u32 addr) {
	if ((addr & 0xFF180000) == 0x81080000)
		return nand_read_data_byte();
	return bad_read_byte(addr);
}
u32 nand_cx_read_word(u32 addr) {
	if ((addr & 0xFF180000) == 0x81080000)
		return nand_read_data_word();
	return bad_read_word(addr);
}
void nand_cx_write_byte(u32 addr, u8 value) {
	if ((addr & 0xFF080000) == 0x81080000) {
		nand_write_data_byte(value);

		if (addr & 0x100000)
			nand_write_command(addr >> 11, 0, 0);
		return;
	}
	bad_write_byte(addr, value);
}
void nand_cx_write_word(u32 addr, u32 value) {
	if (addr >= 0x81000000 && addr < 0x82000000) {
		if (addr & 0x080000)
			nand_write_data_word(value);
		else
			nand_write_command(addr >> 3, value, addr >> 21 & 7);

		if (addr & 0x100000)
			nand_write_command(addr >> 11, 0, 0);
		return;
	}
	bad_write_word(addr, value);
}

FILE *flash_file;

void flash_open(char *filename) {
	bool large = false;
	flash_file = fopen(filename, "r+b");
	if (!flash_file) {
		perror(filename);
		exit(1);
	}
	fseek(flash_file, 0, SEEK_END);
	u32 size = ftell(flash_file);
	fseek(flash_file, 0, SEEK_SET);
	if (size == 33*1024*1024)
		large = false;
	else if (size == 132*1024*1024)
		large = true;
	else {
		printf("%s not a flash image (wrong size)\n", filename);
		exit(1);
	}
	nand_initialize(large);
	if (!fread(nand_data, nand_metrics.page_size * nand_metrics.num_pages, 1, flash_file)) {
		printf("Could not read flash image from %s\n", filename);
		exit(1);
	}
}

void flash_save_changes() {
	if (flash_file == NULL) {
		printf("NAND flash: no file\n");
		return;
	}
	u32 block, count = 0;
	u32 block_size = nand_metrics.page_size << nand_metrics.log2_pages_per_block;
	for (block = 0; block < nand_metrics.num_pages; block += 1 << nand_metrics.log2_pages_per_block) {
		if (nand_block_modified[block >> nand_metrics.log2_pages_per_block]) {
			fseek(flash_file, block * nand_metrics.page_size, SEEK_SET);
			fwrite(&nand_data[block * nand_metrics.page_size], block_size, 1, flash_file);
			nand_block_modified[block >> nand_metrics.log2_pages_per_block] = false;
			count++;
		}
	}
	fflush(flash_file);
	printf("NAND flash: saved %d modified blocks to file\n", count);
}

int flash_save_as(char *filename) {
	FILE *f = fopen(filename, "wb");
	if (!f) {
		printf("NAND flash: could not open ");
		perror(filename);
		return 1;
	}
	printf("Saving flash image %s...", filename);
	if (!fwrite(nand_data, nand_metrics.page_size * nand_metrics.num_pages, 1, f) || fflush(f)) {
		fclose(f);
		remove(filename);
		printf("\nNAND flash: could not write to ");
		perror(filename);
		return 1;
	}
	memset(nand_block_modified, 0, nand_metrics.num_pages >> nand_metrics.log2_pages_per_block);
	if (flash_file)
		fclose(flash_file);
	flash_file = f;
	printf("done\n");
	return 0;
}

static void ecc_fix(int page) {
	u8 *data = &nand_data[page * nand_metrics.page_size];
	if (nand_metrics.page_size < 0x800) {
		u32 ecc = ecc_calculate(data);
		data[0x206] = ecc >> 6;
		data[0x207] = ecc >> 14;
		data[0x208] = ecc >> 22 | ecc << 2;
	} else {
		int i;
		for (i = 0; i < 4; i++) {
			u32 ecc = ecc_calculate(&data[i * 0x200]);
			data[0x808 + i * 0x10] = ecc >> 6;
			data[0x809 + i * 0x10] = ecc >> 14;
			data[0x80A + i * 0x10] = ecc >> 22 | ecc << 2;
		}
	}
}

static u32 load(u32 page, u32 offset, char *filename, u32 *psize) {
	FILE *f = fopen(filename, "rb");
	if (!f) {
		perror(filename);
		exit(1);
	}

	u32 size = 0;
	u32 page_data_size = (nand_metrics.page_size & ~0x7F);
	int read;
	while ((read = fread(&nand_data[page * nand_metrics.page_size + offset],
	                     1, page_data_size - offset, f)) > 0)
	{
		size += read;
		ecc_fix(page);
		page++;
		if (page == nand_metrics.num_pages) {
			printf("Preload image(s) too large\n");
			exit(1);
		}
		offset = 0;
	}

	fclose(f);

	if (psize) *psize = size;

	// Round to next block
	u32 mask = -1 << nand_metrics.log2_pages_per_block;
	return (page + ~mask) & mask;
}

static u32 preload(u32 page, char *name, char *filename) {
	u32 size;
	u32 next = load(page, 32, filename, &size);

	u8 *pagep = &nand_data[page * nand_metrics.page_size];
	sprintf((char *)&pagep[0], "***PRELOAD_%s***", name);
	*(u32 *)&pagep[20] = BSWAP32(0x55F00155);
	*(u32 *)&pagep[24] = 0;
	*(u32 *)&pagep[28] = BSWAP32(size);
	ecc_fix(page);

	return next;
}

struct manuf_data_804 {
	u16 product;
	u16 revision;
	char locale[8];
	char _unknown_810[8];
	struct manuf_data_ext {
		u32 signature;
		u32 features;
		u32 default_keypad;
		u16 lcd_width;
		u16 lcd_height;
		u16 lcd_bpp;
		u16 lcd_color;
		u32 offset_diags;
		u32 offset_boot2;
		u32 offset_bootdata;
		u32 offset_filesys;
		u32 config_clocks;
		u32 config_sdram;
		u32 lcd_spi_count;
		u32 lcd_spi_data[8][2];
		u16 lcd_light_min;
		u16 lcd_light_max;
		u16 lcd_light_default;
		u16 lcd_light_incr;
	} ext;
	u8 bootgfx_count;
	u8 bootgfx_iscompressed;
	u16 bootgfx_unknown;
	struct {
		u16 pos_y;
		u16 pos_x;
		u16 width;
		u16 height;
		u32 offset;
	} bootgfx_images[12];
	u32 bootgfx_compsize;
	u32 bootgfx_rawsize;
	u32 bootgfx_certsize;
};


void flash_create_new(char **preload_file, int product, bool large_sdram) {
	memset(nand_data, 0xFF, nand_metrics.page_size * nand_metrics.num_pages);

	struct manuf_data_804 *manuf = (struct manuf_data_804 *)&nand_data[0x844];

	if (preload_file[0]) {
		load(0, 0, preload_file[0], NULL);
	} else {
		*(u32 *)&nand_data[0] = 0x796EB03C;
		ecc_fix(0);

		manuf->product = product;
		manuf->revision = (product == 0x0C) ? 1 : 0;
		if (manuf->product >= 0x0F) {
			manuf->ext.signature = 0x4C9E5F91;
			manuf->ext.features = 5;
			manuf->ext.default_keypad = 76; // Touchpad
			manuf->ext.lcd_width = 320;
			manuf->ext.lcd_height = 240;
			manuf->ext.lcd_bpp = 16;
			manuf->ext.lcd_color = 1;
			if (nand_metrics.page_size < 0x800) {
				manuf->ext.offset_diags    = 0x160000;
				manuf->ext.offset_boot2    = 0x004000;
				manuf->ext.offset_bootdata = 0x150000;
				manuf->ext.offset_filesys  = 0x200000;
			} else {
				manuf->ext.offset_diags    = 0x320000;
				manuf->ext.offset_boot2    = 0x020000;
				manuf->ext.offset_bootdata = 0x2C0000;
				manuf->ext.offset_filesys  = 0x400000;
			}
			manuf->ext.config_clocks = 0x561002; // 132 MHz
			manuf->ext.config_sdram = large_sdram ? 0xFC018012 : 0xFE018011;
			manuf->ext.lcd_spi_count = 0;
			manuf->ext.lcd_light_min = 0x11A;
			manuf->ext.lcd_light_max = 0x1CE;
			manuf->ext.lcd_light_default = 0x16A;
			manuf->ext.lcd_light_incr = 0x14;
			manuf->bootgfx_count = 0;
		}
		ecc_fix(nand_metrics.page_size < 0x800 ? 4 : 1);
	}

	int page = nand_metrics.page_size < 0x800 ? 0x1000 : 0x800;
	if (preload_file[1]) page = preload(page, "BOOT2", preload_file[1]);
	//if (preload_boot2) load(nand_metrics.page_size < 0x800 ? 32 : 64, 0, preload[1], NULL);
	if (preload_file[2]) page = preload(page, "DIAGS", preload_file[2]);
	if (preload_file[3]) page = preload(page, "IMAGE", preload_file[3]);
}

void flash_read_settings(u32 *sdram_size) {
	struct manuf_data_804 *manuf = (struct manuf_data_804 *)&nand_data[0x844];

	// 0C-0E (CAS, lab cradle, plain Nspire) use old ASIC
	// 0F-12 (CX CAS, CX, CM CAS, CM) use new ASIC
	product = manuf->product;
	emulate_cx = manuf->product >= 0x0F;

	static const unsigned char flags[] = { 1, 0, 0, 1, 0, 3, 2 };
	if (manuf->product >= 0x0C && manuf->product <= 0x12)
		asic_user_flags = flags[manuf->product - 0x0C];
	else
		asic_user_flags = 0;

	if (emulate_cx && manuf->ext.signature == 0x4C9E5F91) {
		u32 cfg = manuf->ext.config_sdram;
		int logsize = (cfg & 7) + (cfg >> 3 & 7);
		if (logsize > 4) {
			fprintf(stderr, "invalid SDRAM size\n");
			exit(1);
		}
		*sdram_size = (4 * 1024 * 1024) << logsize;
	} else {
		*sdram_size = 32 * 1024 * 1024;
	}
}
