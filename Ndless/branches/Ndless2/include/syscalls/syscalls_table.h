/****************************************************************************
 * @(#) Ndless - Syscalls table
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

#ifndef OS_FUNC
	#error "Expected preprocessor directive OS_FUNC."
#else
  // Basic syscalls table used by the loader
  #ifdef USE_BASIC_SYSCALLS_TABLE
	  .long OS_FUNC( TCT_Local_Control_Interrupts_ )
	  .long OS_FUNC( power_off_ )
	  .long OS_FUNC( ascii2utf16_ )
	  .long OS_FUNC( show_dialog_box_ )
	  .long OS_FUNC( show_dialog_box2_ )
	  .long OS_FUNC( fopen_ )
	  .long OS_FUNC( fclose_ )
	  .long OS_FUNC( fread_ )
	  .long OS_FUNC( memset_ )
	  .long OS_FUNC( memcpy_ )

  // Complete syscalls table
	#else
	  .long OS_FUNC( TCT_Local_Control_Interrupts_ )
	  .long OS_FUNC( power_off_ )
	  .long OS_FUNC( ascii2utf16_ )
	  .long OS_FUNC( show_dialog_box_ )
	  .long OS_FUNC( show_dialog_box2_ )
	  .long OS_FUNC( log_rs232_ )
	  .long OS_FUNC( log_rs232_param2_ )
	  .long OS_FUNC( printf_rs232_ )
	  .long OS_FUNC( fopen_ )
	  .long OS_FUNC( fclose_ )
	  .long OS_FUNC( fread_ )
	  .long OS_FUNC( fwrite_ )
	  .long OS_FUNC( fgetc_ )
	  .long OS_FUNC( fgets_ )
	  .long OS_FUNC( unlink_ )
	  .long OS_FUNC( rename_ )
	  .long OS_FUNC( stat_ )
	  .long OS_FUNC( mkdir_ )
	  .long OS_FUNC( rmdir_ )
	  .long OS_FUNC( set_current_path_ )
	  .long OS_FUNC( purge_directory_ )
	  .long OS_FUNC( create_path_filename_ )
	  .long OS_FUNC( malloc_ )
	  .long OS_FUNC( free_ )
	  .long OS_FUNC( memset_ )
	  .long OS_FUNC( memcpy_ )
	  .long OS_FUNC( memcmp_ )
	  .long OS_FUNC( sprintf_ )
	  .long OS_FUNC( NU_Remove_Dir_ )
	#endif  
#endif
