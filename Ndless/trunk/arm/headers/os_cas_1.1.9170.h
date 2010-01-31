  .equ TCT_Local_Control_Interrupts,  0x1014B338
  .equ memset,                        0x10185DCC
	.equ memcpy,                        0x10185D50
  .equ memcmp,                        0x10185D10
	.equ unlink,                        0x1018A2E8
	.equ rename,                        0x10189C1C
	.equ fopen,                         0x10184DC0
	.equ fread,                         0x10184EFC
	.equ fwrite,                        0x101852B0
	.equ fclose,                        0x101847B8
  .equ fgets,                         0x10184B18
  .equ fgetc,                         0x10184A74
	.equ malloc,                        0x101817DC
	.equ free,                          0x101817D0
	.equ stat,                          0x1018A00C
  .equ create_path_filename,          0x10004BC0
  .equ sprintf,                       0x10182A28
  .equ mkdir,                         0x10184B18
  .equ set_current_dir,               0x10187734
  .equ power_off,                     0x1004D9D0
  .equ ascii2utf16,                   0x100D09DC
  .equ show_dialog_box,               0x10011CB8
  .equ show_dialog_box2,              0x10011AF8
  .equ NU_Remove_Dir,                 0x1118B474

	.macro oscall address
		 	mov	lr,pc
 			ldr pc,=\address
	.endm
