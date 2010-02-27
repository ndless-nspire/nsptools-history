/****************************************************************************
 * @(#) Ndless - OS Table (CAS 1.1.9170)
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

#define TCT_Local_Control_Interrupts_  0x1014B338
#define memset_                        0x10185DCC
#define memcpy_                        0x10185D50
#define memcmp_                        0x10185D10
#define unlink_                        0x1018A2E8
#define rename_                        0x10189C1C
#define fopen_                         0x10184DC0
#define fread_                         0x10184EFC
#define fwrite_                        0x101852B0
#define fclose_                        0x101847B8
#define fgets_                         0x10184B18
#define fgetc_                         0x10184A74
#define malloc_                        0x101817DC
#define free_                          0x101817D0
#define stat_                          0x1018A00C
#define create_path_filename_          0x10004BC0
#define sprintf_                       0x10182A28
#define mkdir_                         0x101885FC
#define rmdir_                         0x10189E70
#define set_current_path_              0x10187734
#define purge_directory_               0x1004E62C
#define power_off_                     0x1004D9D0
#define ascii2utf16_                   0x100D09DC
#define show_dialog_box_               0x10011CB8
#define show_dialog_box2_              0x10011AF8
#define NU_Remove_Dir_                 0x1118B474
#define log_rs232_                     0x1014C530
#define log_rs232_param2_              0x10522A50
#define printf_rs232_                  0x10182968

#endif
