  .equ TCT_Local_Control_Interrupts,  0x1014B308
  .equ memset,                        0x101860C4
	.equ memcpy,                        0x10186048
  .equ memcmp,                        0x10186008
	.equ unlink,                        0x1018A5E0
	.equ rename,                        0x10189F14
	.equ fopen,                         0x101850B8
	.equ fread,                         0x101851F4
	.equ fwrite,                        0x101855A8
	.equ fclose,                        0x10184AB0
  .equ fgets,                         0x10184E10
  .equ fgetc,                         0x10184D6C
	.equ malloc,                        0x10181AD8
	.equ free,                          0x10181ACC
	.equ stat,                          0x1018A304
  .equ create_path_filename,          0x10004BD8
  .equ sprintf,                       0x10182D20
  .equ mkdir,                         0x101888F4
  .equ set_current_dir,               0x10187A2C
  .equ power_off,                     0x1004DA1C
  .equ ascii2utf16,                   0x00000000
  .equ show_dialog_box,               0x00000000
  .equ show_dialog_box2,              0x00000000
  .equ NU_Remove_Dir,                 0x1018B76C

	.macro oscall address
		 	mov	lr,pc
 			ldr pc,=\address
	.endm
