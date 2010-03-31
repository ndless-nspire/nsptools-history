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

#define OS_VERSION      1.1.9170

.set OS_FUNC( TCT_Local_Control_Interrupts_ ),  0x1014B338
.set OS_FUNC( memset_ ),                        0x10185DCC
.set OS_FUNC( memcpy_ ),                        0x10185D50
.set OS_FUNC( memcmp_ ),                        0x10185D10
.set OS_FUNC( unlink_ ),                        0x1018A2E8
.set OS_FUNC( rename_ ),                        0x10189C1C
.set OS_FUNC( fopen_ ),                         0x10184DC0
.set OS_FUNC( fread_ ),                         0x10184EFC
.set OS_FUNC( fwrite_ ),                        0x101852B0
.set OS_FUNC( fclose_ ),                        0x101847B8
.set OS_FUNC( fgets_ ),                         0x10184B18
.set OS_FUNC( fgetc_ ),                         0x10184A74
.set OS_FUNC( malloc_ ),                        0x101817DC
.set OS_FUNC( free_ ),                          0x101817D0
.set OS_FUNC( stat_ ),                          0x1018A00C
.set OS_FUNC( create_path_filename_ ),          0x10004BC0
.set OS_FUNC( sprintf_ ),                       0x10182A28
.set OS_FUNC( mkdir_ ),                         0x101885FC
.set OS_FUNC( rmdir_ ),                         0x10189E70
.set OS_FUNC( set_current_path_ ),              0x10187734
.set OS_FUNC( purge_directory_ ),               0x1004E62C
.set OS_FUNC( power_off_ ),                     0x1004D9D0
.set OS_FUNC( ascii2utf16_ ),                   0x100D09DC
.set OS_FUNC( show_dialog_box_ ),               0x10011CB8
.set OS_FUNC( show_dialog_box2_ ),              0x10011AF8
.set OS_FUNC( NU_Remove_Dir_ ),                 0x1118B474
.set OS_FUNC( log_rs232_ ),                     0x1014C530
.set OS_FUNC( log_rs232_param2_ ),              0x10522A50
.set OS_FUNC( printf_rs232_ ),                  0x10182968
