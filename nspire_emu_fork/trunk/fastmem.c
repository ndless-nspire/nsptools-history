#include "emu.h"

/* Fast path for valid, non-breakpoint-hitting accesses to the SDRAM:
 * 32MB at 10000000
 *  1MB at 18000000 (alias of 11E00000, used by the OS as stack)
 *
 * Compile this file with -fomit-frame-pointer -O3. */

u32 __attribute__((fastcall)) read_byte(u32 addr) {
	u32 rel_addr = (addr - 0x10000000);
	if (__builtin_expect(!(rel_addr & 0xFE000000), 1)) {
		u32 flags = RAM_FLAGS(&ram_10[rel_addr & ~3]);
		if (__builtin_expect(!(flags & RF_READ_BREAKPOINT), 1))
			return ram_10[addr - 0x10000000];
	}
	rel_addr = (addr - 0x18000000);
	if (__builtin_expect(!(rel_addr & 0xFFF00000), 1)) {
		u32 flags = RAM_FLAGS(&ram_18[rel_addr & ~3]);
		if (__builtin_expect(!(flags & RF_READ_BREAKPOINT), 1))
			return ram_18[addr - 0x18000000];
	}
	return slow_read_byte(addr);
}

u32 __attribute__((fastcall)) read_half(u32 addr) {
	u32 rel_addr = (addr - 0x10000000);
	if (__builtin_expect(!(rel_addr & 0xFE000001), 1)) {
		u32 flags = RAM_FLAGS(&ram_10[rel_addr & ~3]);
		if (__builtin_expect(!(flags & RF_READ_BREAKPOINT), 1))
			return *(u16 *)&ram_10[addr - 0x10000000];
	}
	rel_addr = (addr - 0x18000000);
	if (__builtin_expect(!(rel_addr & 0xFFF00001), 1)) {
		u32 flags = RAM_FLAGS(&ram_18[rel_addr & ~3]);
		if (__builtin_expect(!(flags & RF_READ_BREAKPOINT), 1))
			return *(u16 *)&ram_18[addr - 0x18000000];
	}
	return slow_read_half(addr);
}

u32 __attribute__((fastcall)) read_word(u32 addr) {
	u32 rel_addr = (addr - 0x10000000);
	if (__builtin_expect(!(rel_addr & 0xFE000003), 1)) {
		u32 flags = RAM_FLAGS(&ram_10[rel_addr]);
		if (__builtin_expect(!(flags & RF_READ_BREAKPOINT), 1))
			return *(u32 *)&ram_10[addr - 0x10000000];
	}
	rel_addr = (addr - 0x18000000);
	if (__builtin_expect(!(rel_addr & 0xFFF00003), 1)) {
		u32 flags = RAM_FLAGS(&ram_18[rel_addr]);
		if (__builtin_expect(!(flags & RF_READ_BREAKPOINT), 1))
			return *(u32 *)&ram_18[addr - 0x18000000];
	}
	return slow_read_word(addr);
}

#define WRITE_SPECIAL_FLAGS (RF_WRITE_BREAKPOINT | RF_CODE_TRANSLATED | RF_CODE_NO_TRANSLATE)

void __attribute__((fastcall)) write_byte(u32 addr, u32 value) {
	u32 rel_addr = (addr - 0x10000000);
	if (__builtin_expect(!(rel_addr & 0xFE000000), 1)) {
		u32 flags = RAM_FLAGS(&ram_10[rel_addr & ~3]);
		if (__builtin_expect(!(flags & WRITE_SPECIAL_FLAGS), 1)) {
			ram_10[addr - 0x10000000] = value;
			return;
		}
	}
	rel_addr = (addr - 0x18000000);
	if (__builtin_expect(!(rel_addr & 0xFFF00000), 1)) {
		u32 flags = RAM_FLAGS(&ram_18[rel_addr & ~3]);
		if (__builtin_expect(!(flags & WRITE_SPECIAL_FLAGS), 1)) {
			ram_18[addr - 0x18000000] = value;
			return;
		}
	}
	slow_write_byte(addr, value);
}

void __attribute__((fastcall)) write_half(u32 addr, u32 value) {
	u32 rel_addr = (addr - 0x10000000);
	if (__builtin_expect(!(rel_addr & 0xFE000001), 1)) {
		u32 flags = RAM_FLAGS(&ram_10[rel_addr & ~3]);
		if (__builtin_expect(!(flags & WRITE_SPECIAL_FLAGS), 1)) {
			*(u16 *)&ram_10[addr - 0x10000000] = value;
			return;
		}
	}
	rel_addr = (addr - 0x18000000);
	if (__builtin_expect(!(rel_addr & 0xFFF00001), 1)) {
		u32 flags = RAM_FLAGS(&ram_18[rel_addr & ~3]);
		if (__builtin_expect(!(flags & WRITE_SPECIAL_FLAGS), 1)) {
			*(u16 *)&ram_18[addr - 0x18000000] = value;
			return;
		}
	}
	slow_write_half(addr, value);
}

void __attribute__((fastcall)) write_word(u32 addr, u32 value) {
	u32 rel_addr = (addr - 0x10000000);
	if (__builtin_expect(!(rel_addr & 0xFE000003), 1)) {
		u32 flags = RAM_FLAGS(&ram_10[rel_addr]);
		if (__builtin_expect(!(flags & WRITE_SPECIAL_FLAGS), 1)) {
			*(u32 *)&ram_10[addr - 0x10000000] = value;
			return;
		}
	}
	rel_addr = (addr - 0x18000000);
	if (__builtin_expect(!(rel_addr & 0xFFF00003), 1)) {
		u32 flags = RAM_FLAGS(&ram_18[rel_addr]);
		if (__builtin_expect(!(flags & WRITE_SPECIAL_FLAGS), 1)) {
			*(u32 *)&ram_18[addr - 0x18000000] = value;
			return;
		}
	}
	slow_write_word(addr, value);
}
