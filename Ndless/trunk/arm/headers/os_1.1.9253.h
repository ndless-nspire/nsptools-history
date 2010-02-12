/****************************************************************************
 * @(#) Ndless - OS Table (Non-CAS 1.1.9253)
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

#ifndef _OS_CALLS_H_
#define _OS_CALLS_H_

#define TCT_Local_Control_Interrupts_  0x1014B308
#define memset_                        0x101860C4
#define memcpy_                        0x10186048
#define memcmp_                        0x10186008
#define unlink_                        0x1018A5E0
#define rename_                        0x10189F14
#define fopen_                         0x101850B8
#define fread_                         0x101851F4
#define fwrite_                        0x101855A8
#define fclose_                        0x10184AB0
#define fgets_                         0x10184E10
#define fgetc_                         0x10184D6C
#define malloc_                        0x10181AD8
#define free_                          0x10181ACC
#define stat_                          0x1018A304
#define create_path_filename_          0x10004BD8
#define sprintf_                       0x10182D20
#define mkdir_                         0x101888F4
#define rmdir_                         0x102156E0
#define set_current_path_              0x10187A2C
#define purge_directory_               0x1004E678
#define power_off_                     0x1004DA1C
#define ascii2utf16_                   0x100D09AC
#define show_dialog_box_               0x10011CE0
#define show_dialog_box2_              0x10011B20
#define NU_Remove_Dir_                 0x1018B76C
#define log_rs232_                     0x1014C500
#define printf_rs232_                  0x10182C60

#endif
