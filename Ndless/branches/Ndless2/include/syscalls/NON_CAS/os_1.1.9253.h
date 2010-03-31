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

#define OS_VERSION      1.1.9253

.set OS_FUNC( TCT_Local_Control_Interrupts_ ),  0x1014B308
.set OS_FUNC( memset_ ),                        0x101860C4
.set OS_FUNC( memcpy_ ),                        0x10186048
.set OS_FUNC( memcmp_ ),                        0x10186008
.set OS_FUNC( unlink_ ),                        0x1018A5E0
.set OS_FUNC( rename_ ),                        0x10189F14
.set OS_FUNC( fopen_ ),                         0x101850B8
.set OS_FUNC( fread_ ),                         0x101851F4
.set OS_FUNC( fwrite_ ),                        0x101855A8
.set OS_FUNC( fclose_ ),                        0x10184AB0
.set OS_FUNC( fgets_ ),                         0x10184E10
.set OS_FUNC( fgetc_ ),                         0x10184D6C
.set OS_FUNC( malloc_ ),                        0x10181AD8
.set OS_FUNC( free_ ),                          0x10181ACC
.set OS_FUNC( stat_ ),                          0x1018A304
.set OS_FUNC( create_path_filename_ ),          0x10004BD8
.set OS_FUNC( sprintf_ ),                       0x10182D20
.set OS_FUNC( mkdir_ ),                         0x101888F4
.set OS_FUNC( rmdir_ ),                         0x1018A168
.set OS_FUNC( set_current_path_ ),              0x10187A2C
.set OS_FUNC( purge_directory_ ),               0x1004E678
.set OS_FUNC( power_off_ ),                     0x1004DA1C
.set OS_FUNC( ascii2utf16_ ),                   0x100D09AC
.set OS_FUNC( show_dialog_box_ ),               0x10011CE0
.set OS_FUNC( show_dialog_box2_ ),              0x10011B20
.set OS_FUNC( NU_Remove_Dir_ ),                 0x1018B76C
.set OS_FUNC( log_rs232_ ),                     0x1014C500
.set OS_FUNC( log_rs232_param2_ ),              0x1054EA50
.set OS_FUNC( printf_rs232_ ),                  0x10182C60
