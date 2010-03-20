/**
 * Fork an address of the OS
 *
 * Input: r2 = Your address that replace the OS call.
 * Destroy: r0, r1
 *
 * Note: This method erases the instruction that follows the OS call.
 * Please, remember to restore this instruction and the OS call to continue on a valid execution.
 */
.macro fork_address os_address
    ldr     r0, =\os_address
    ldr     r1, =0xE51FF004     @ ldr pc, [pc, #-4]
    str     r1, [r0]
    str     r2, [r0, #4]
.endm
