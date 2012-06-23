/****************************************************************************
 * Ndless - Syscalls enumeration. Lighter version for Ndless loader.
 ****************************************************************************/

#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

/* The syscall's name must be prefixed with e_. */

// START_OF_LIST (always keep this line before the fist constant, used by mksyscalls.sh)
#define e_fopen 0
#define e_fread 1
#define e_fclose 2
#define e_puts 3
#define e_malloc 4
#define e_stat 5
#define e_strcmp 6
// END_OF_LIST (always keep this line after the last constant, used by mksyscalls.sh)

// Must be kept up-to-date with the value of the last syscall
#define SYSCALLS_NUM 7

#endif
