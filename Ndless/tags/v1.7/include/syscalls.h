/****************************************************************************
 * Ndless - Syscalls enumeration
 ****************************************************************************/

#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

/* OS syscalls.
 * The syscall's name must be prefixed with e_.
 * NEVER change the value of these constants for backward compatibility.
 * Append new syscalls at the end of the list, and increment the enumeration.
 * Don't try to sort the symbols.
 * If a syscall becomes deprecated and is deleted from the list, its value
 * cannot be reused.
 */

// START_OF_LIST (always keep this line before the fist constant, used by mksyscalls.sh)
#define e_fopen 0
#define e_fread 1
#define e_fwrite 2
#define e_fclose 3
#define e_fgets 4
#define e_malloc 5
#define e_free 6
#define e_memset 7
#define e_memcpy 8
#define e_memcmp 9
#define e_printf 10
#define e_sprintf 11
#define e_fprintf 12
#define e_ascii2utf16 13
#define e_TCT_Local_Control_Interrupts 14
#define e_mkdir 15
#define e_rmdir 16
#define e_chdir 17
#define e_stat 18
#define e_unlink 19
#define e_rename 20
#define e_power_off 21
#define e_puts 22
#define e_NU_Get_First 23
#define e_NU_Get_Next 24
#define e_NU_Done 25
#define e_strcmp 26
#define e_strcpy 27
#define e_strncat 28
#define e_strlen 29
#define e_show_dialog_box2 30
#define e_strrchr 31
#define e_vsprintf 32
#define e_fseek 33
// END_OF_LIST (always keep this line after the last constant, used by mksyscalls.sh)

// Must be kept up-to-date with the value of the last syscall
#define __SYSCALLS_LAST 33

// Flag: 3 higher bits of the 3-bytes comment field of the swi instruction
#define __SYSCALLS_ISEXT 0x200000
#define __SYSCALLS_ISEMU 0x400000

/* Ndless extensions.
 * Not automatically parsed. Starts from 0. The recommandations for the standard syscalls enumeration apply.
 * The order is the same as in arm/syscalls.c/sc_ext_table[]
 * Must always be or-ed with __SYSCALLS_ISEXT
 * The extensions cannot be called in thumb state (the swi number is too high for the swi thumb instruction */
#define e_nl_osvalue (__SYSCALLS_ISEXT | 0)
#define e_nl_relocdatab (__SYSCALLS_ISEXT | 1)

// Must be kept up-to-date with the value of the last syscalls extension without __SYSCALLS_ISEXT
#define __SYSCALLS_LASTEXT 1

/* Ndless integration with emulators. Grouped to make the integration easier for the emulators (they require
 * only these constants).
 * Not automatically parsed. Starts from 0. The recommandations for the standard syscalls enumeration apply.
 * The order is the same as in arm/emu.c/emu_sysc_table[]
 * Must always be or-ed with __SYSCALLS_ISEMU */
#define NDLSEMU_DEBUG_ALLOC (__SYSCALLS_ISEMU | 0)
#define NDLSEMU_DEBUG_FREE (__SYSCALLS_ISEMU | 1)

// Must be kept up-to-date with the value of the last syscalls extension without __SYSCALLS_ISEXT
#define __SYSCALLS_LASTEMU 1

#endif
