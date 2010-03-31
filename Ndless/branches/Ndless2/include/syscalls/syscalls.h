/****************************************************************************
 * @(#) Ndless - Syscalls enumeration
 *
 * Copyright (C) 2010 by ANNEHEIM Geoffrey and ARMAND Olivier
 * Contact: geoffrey.anneheim@gmail.com / olivier.calc@gmail.com
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * RCSID $Id$
 ****************************************************************************/

#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#define SYSCALLS_TABLE_BYTES_SIZE       (SYSCALLS_TABLE_SIZE * 4)

// Basic syscalls table used by the loader to install Ndless
#ifdef USE_BASIC_SYSCALLS_TABLE
	#define SYSCALLS_TABLE_SIZE						10

	#define TCT_Local_Control_Interrupts  0
	#define power_off                     1
	#define ascii2utf16                   2
	#define show_dialog_box               3
	#define show_dialog_box2              4
	#define fopen                         5
	#define fclose                        6
	#define fread                         7
	#define memset                        8
	#define memcpy                        9
	
// Complete syscalls table
#else
	#define SYSCALLS_TABLE_SIZE						29

	#define TCT_Local_Control_Interrupts  0
	#define power_off                     1
	#define ascii2utf16                   2
	#define show_dialog_box               3
	#define show_dialog_box2              4
	#define log_rs232                     5
	#define log_rs232_param2              6
	#define printf_rs232                  7
	#define fopen                         8
	#define fclose                        9
	#define fread                         10
	#define fwrite                        11
	#define fgetc                         12
	#define fgets                         13
	#define unlink                        14
	#define rename                        15
	#define stat                          16
	#define mkdir                         17
	#define rmdir                         18
	#define set_current_path              19
	#define purge_directory               20
	#define create_path_filename          21
	#define malloc                        22
	#define free                          23
	#define memset                        24
	#define memcpy                        25
	#define memcmp                        26
	#define sprintf                       27
	#define NU_Remove_Dir                 28
#endif

#endif
