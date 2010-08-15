#include "emu.h"
#include <string.h>

/* Copy of translation table in memory (hack to approximate effect of having a TLB) */
static u32 mmu_translation_table[0x1000];

/* Translate a virtual address to a physical address */
u32 mmu_translate(u32 addr, fault_proc *fault) {
	u32 page_size;
	if (!(arm.control & 1))
		return addr;

	u32 *table = mmu_translation_table;
	u32 entry = table[addr >> 20];
	u32 domain = entry >> 5 & 0x0F;
	u32 domain_status = arm.domain_access_control >> (domain << 1) & 3;

	u32 status = domain << 4;

	switch (entry & 3) {
		default: /* Invalid */
			if (fault) fault(addr, status + 0x5); /* Section translation fault */
			return 0xFFFFFFFF;
		case 1: /* Course page table (one entry per 4kB) */
			table = phys_mem_ptr(entry & 0xFFFFFC00, 0x400);
			if (!table) {
				if (fault) error("Bad page table pointer");
				return 0xFFFFFFFF;
			}
			entry = table[addr >> 12 & 0xFF];
			break;
		case 2: /* Section (1MB) */
			page_size = 0x100000;
			goto section;
		case 3: /* Fine page table (one entry per 1kB) */
			table = phys_mem_ptr(entry & 0xFFFFF000, 0x1000);
			if (!table) {
				if (fault) error("Bad page table pointer");
				return 0xFFFFFFFF;
			}
			entry = table[addr >> 10 & 0x3FF];
			break;
	}

	status += 2;
	switch (entry & 3) {
		default: /* Invalid */
			if (fault) fault(addr, status + 0x5); /* Page translation fault */
			return 0xFFFFFFFF;
		case 1: /* Large page (64kB) */
			page_size = 0x10000;
			break;
		case 2: /* Small page (4kB) */
			page_size = 0x1000;
			break;
		case 3: /* Tiny page (1kB) */
			page_size = 0x400;
			break;
	}
section:

	if (!(domain_status & 1)) {
		if (fault) fault(addr, status + 0x9); /* Domain fault */
		return 0xFFFFFFFF;
	}

	/* TODO: Permission bit checking */

	return (entry & -page_size) | (addr & (page_size - 1));
}

u8 *addr_cache_delta[4194304];

#define AC_VALID_MAX 256
static u32 ac_valid_index;
static u32 ac_valid_list[AC_VALID_MAX];

void addr_cache_init() {
	u32 i;
	for (i = 0; i < 4194304; i++)
		addr_cache_delta[i] = (u8 *)AC_INVALID_PTR - (i << 10);
}

void *addr_cache_miss(u32 virt, u32 align_bits, fault_proc *fault) {
	if (virt & align_bits)
		return 0;
	u32 phys = mmu_translate(virt, fault);
	u8 *ptr = phys_mem_ptr(phys, 1);
	if (ptr) {
		addr_cache_delta[ac_valid_list[ac_valid_index]] = (u8 *)AC_INVALID_PTR - (ac_valid_list[ac_valid_index] << 10);
		addr_cache_delta[virt >> 10] = ptr - virt;
		ac_valid_list[ac_valid_index] = virt >> 10;
		ac_valid_index = (ac_valid_index + 1) % AC_VALID_MAX;
	}
	return ptr;
}

void addr_cache_flush() {
	u32 i;

	if (arm.control & 1) {
		u32 *table = phys_mem_ptr(arm.translation_table_base, 0x4000);
		if (!table)
			error("Bad translation table base register: %x", arm.translation_table_base);
		memcpy(mmu_translation_table, table, 0x4000);
	}

	for (i = 0; i < AC_VALID_MAX; i++)
		addr_cache_delta[ac_valid_list[i]] = (u8 *)AC_INVALID_PTR - (ac_valid_list[i] << 10);
}


void *mmu_save_state(size_t *size) {
	*size = 0;
	return NULL;
}

void mmu_reload_state(void *state __attribute__((unused))) {
	addr_cache_flush();
}
