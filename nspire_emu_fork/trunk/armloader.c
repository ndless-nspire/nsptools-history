/* Loads and run ARM code snippets on the target */

#include <string.h>
#include <stdio.h>
#include "emu.h"

struct arm_state armloader_orig_arm_state;

void armloader_restore_state(void) {
	memcpy(&arm, &armloader_orig_arm_state, sizeof(arm));
}

/* Load the snippet and jump to it. 
 * snippets are defined in armsnippets.S.
 * params may contain pointer to data which should be copied to device space.
 * Each param will be copied to the ARM stack, and its address written in rX, starting from r0.
 * params_num must be less or equal than 12.
 * returns 0 if success.
 * A 'next' breakpoint is set after the snippet exectution.
 * armloader_restore_state() should be called at that time.
 */
int armloader_load_snippet(enum SNIPPETS snippet, struct armloader_load_params params[],
	                         unsigned params_num) {
	unsigned int i, oscalls_tbl_num;
	int code_size;
	void *code_ptr;
	u32 *entry_points, *oscalls_ptr;
	u32 os_version;
	
	code_size = binary_snippets_bin_end - binary_snippets_bin_start;
	if (code_size % 4)
		code_size += 4 - (code_size % 4); // word-aligned
	
	memcpy(&armloader_orig_arm_state, &arm, sizeof(arm));
	
	if (!ram_ptr(arm.reg[13] /* sp */, 4)) {
		printf("sp points to an invalid address\n");
		armloader_restore_state();
		return -1;
	}
	arm.reg[13] -= code_size;
	code_ptr = ram_ptr(arm.reg[13], code_size);
	if (!code_ptr) {
		printf("not enough stack space to run snippet\n");
		armloader_restore_state();
		return -1;
	}
	memcpy(code_ptr, binary_snippets_bin_start, code_size);
	entry_points = code_ptr;
	
	/* get the oscall table ptr */
	oscalls_ptr = code_ptr + entry_points[SNIPPETS_EP_OSCALLS_TABLE];
	oscalls_tbl_num = *oscalls_ptr++;
	arm.reg[11] = 0;
	os_version = OS_VERSION;
	while(oscalls_tbl_num--) {
		if (os_version == *oscalls_ptr++) {
			arm.reg[11] = arm.reg[13] + *oscalls_ptr; // oscall table
			break;
		}
		oscalls_ptr++;
	}
	if (!arm.reg[11]) {
		printf("OS version not supported by ARM loader\n");
		armloader_restore_state();
		return -1;
	}
	
	/* set a 'next' breakpoint at the current PC, which is used as return address */
	debug_set_next_brkpt(arm.reg[15]);
	arm.reg[14] = arm.reg[15]; // return address
	arm.reg[15] = arm.reg[13] + entry_points[SNIPPETS_EP_LOAD];
	arm.reg[12] = snippet;
	
	for (i = 0; i < params_num; i++) {
		void *param_ptr;
		if (params[i].t == ARMLOADER_PARAM_VAL)
			arm.reg[i] = params[i].v;
		else {
			unsigned int size = params[i].p.size;
			if (size % 4)
				size += 4 - size % 4; // word-aligned
			arm.reg[13] -= size;
			arm.reg[i] = arm.reg[13];
			param_ptr = ram_ptr(arm.reg[13], 4);
			if (!param_ptr) {
				printf("not enough stack space for snippet parameters\n");
				armloader_restore_state();
				return -1;
			}
			memcpy(param_ptr, params[i].p.ptr, params[i].p.size);
		}
	}

// TODO for debugging
						u32 *flags = &RAM_FLAGS(ram_ptr(arm.reg[15], 4));
										if (*flags & RF_CODE_TRANSLATED) flush_translations();
									*flags |= RF_EXEC_BREAKPOINT;	
	
	return 0;
}
