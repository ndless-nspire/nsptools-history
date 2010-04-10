/****************************************************************************
 * @(#) Ndless - OS Table 2.0.1188
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

#define OS_VERSION      2.0.1.60

.set OS_FUNC( TCT_Local_Control_Interrupts_ ),  0x102669B8
.set OS_FUNC( memset_ ),                        0x102F7C5C
.set OS_FUNC( memcpy_ ),                        0x102F7AA4
.set OS_FUNC( memcmp_ ),                        0x102F7A64
.set OS_FUNC( unlink_ ),                        0x00000000
.set OS_FUNC( rename_ ),                        0x102FC4E0
.set OS_FUNC( fopen_ ),                         0x102F65F0
.set OS_FUNC( fread_ ),                         0x102F67E4
.set OS_FUNC( fwrite_ ),                        0x00000000
.set OS_FUNC( fclose_ ),                        0x102F5EDC
.set OS_FUNC( fgets_ ),                         0x102F6348
.set OS_FUNC( fgetc_ ),                         0x102F62A4
.set OS_FUNC( malloc_ ),                        0x102F29D0
.set OS_FUNC( free_ ),                          0x102F2984
.set OS_FUNC( stat_ ),                          0x102FC8D0
.set OS_FUNC( create_path_filename_ ),          0x00000000
.set OS_FUNC( sprintf_ ),                       0x00000000
.set OS_FUNC( mkdir_ ),                         0x102FAE68
.set OS_FUNC( rmdir_ ),                         0x00000000
.set OS_FUNC( set_current_path_ ),              0x00000000
.set OS_FUNC( purge_directory_ ),               0x00000000
.set OS_FUNC( power_off_ ),                     0x00000000
.set OS_FUNC( ascii2utf16_ ),                   0x00000000
.set OS_FUNC( show_dialog_box_ ),               0x00000000
.set OS_FUNC( show_dialog_box2_ ),              0x00000000
.set OS_FUNC( NU_Remove_Dir_ ),                 0x102FDD68
.set OS_FUNC( log_rs232_ ),                     0x00000000
.set OS_FUNC( log_rs232_param2_ ),              0x00000000
.set OS_FUNC( printf_rs232_ ),                  0x00000000               
