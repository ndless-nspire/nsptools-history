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
	unsigned int i;
	int code_size = binary_snippets_bin_end - binary_snippets_bin_start;
	void *code_ptr;
	
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
/* set a 'next' breakpoint at the current PC, which is used as return address */
	debug_set_next_brkpt(arm.reg[15]);
	arm.reg[14] = arm.reg[15]; // return address
	arm.reg[15] = arm.reg[13]; // load_snippet
	arm.reg[12] = snippet;
	
	for (i = 0; i < params_num; i++) {
		void *param_ptr;
		if (params[i].t == ARMLOADER_PARAM_VAL)
			arm.reg[i] = params[i].v;
		else {
			arm.reg[13] -= params[i].p.size;
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
	
	return 0;
}
