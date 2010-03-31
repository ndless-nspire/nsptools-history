/****************************************************************************
 * @(#) Ndless - OS Table 1.7.2741
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

#define OS_VERSION      1.7.2741

.set OS_FUNC( TCT_Local_Control_Interrupts_ ),  0x10211C08
.set OS_FUNC( memset_ ),                        0x102A62D4
.set OS_FUNC( memcpy_ ),                        0x102A611C
.set OS_FUNC( memcmp_ ),                        0x102A60DC
.set OS_FUNC( unlink_ ),                        0x102AA5A8
.set OS_FUNC( rename_ ),                        0x102A9EDC
.set OS_FUNC( fopen_ ),                         0x102A4CBC
.set OS_FUNC( fread_ ),                         0x102A4EB0
.set OS_FUNC( fwrite_ ),                        0x102A5264
.set OS_FUNC( fclose_ ),                        0x102A46B4
.set OS_FUNC( fgets_ ),                         0x102A4A14
.set OS_FUNC( fgetc_ ),                         0x102A4970
.set OS_FUNC( malloc_ ),                        0x102A11C8
.set OS_FUNC( free_ ),                          0x102A11BC
.set OS_FUNC( stat_ ),                          0x102AA2CC
.set OS_FUNC( create_path_filename_ ),          0x00000000
.set OS_FUNC( sprintf_ ),                       0x102A280C
.set OS_FUNC( mkdir_ ),                         0x102A8864
.set OS_FUNC( rmdir_ ),                         0x00000000
.set OS_FUNC( set_current_path_ ),              0x102A799C
.set OS_FUNC( purge_directory_ ),               0x1007A380
.set OS_FUNC( power_off_ ),                     0x00000000
.set OS_FUNC( ascii2utf16_ ),                   0x1017209C
.set OS_FUNC( show_dialog_box_ ),               0x1001D8A4
.set OS_FUNC( show_dialog_box2_ ),              0x1001D528
.set OS_FUNC( NU_Remove_Dir_ ),                 0x102AB764
.set OS_FUNC( log_rs232_ ),                     0x00000000
.set OS_FUNC( log_rs232_param2_ ),              0x00000000
.set OS_FUNC( printf_rs232_ ),                  0x00000000
