/****************************************************************************
 * @(#) Ndless - Syscalls table implementation
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
 
// UNSUPPORTED (Check function get_os_syscalls_index) 
  /************
  * 1.1.9170 *
  ************/
  /*
    #undef OS_VERSION
    #include "CAS/os_1.1.9170.h"
    #include "syscalls_table.h"
  */

  /************  
  * 1.1.9253 *
  ************/
  /*
    #undef OS_VERSION
    #include "NON_CAS/os_1.1.9253.h"
    #include "syscalls_table.h"
  */

// SUPPORTED
/************  
 * 1.7.2741 *
 ************/
  #undef OS_VERSION
  #include "Both/os_1.7.2741.h"
  #include "syscalls_table.h"

/************  
 * 2.0.1188 *
 ************/
  #undef OS_VERSION
  #include "Both/os_2.0.1188.h"
  #include "syscalls_table.h"

/************
 * 2.0.1.60 *
 ************/
  #undef OS_VERSION
  #include "Both/os_2.0.1.60.h"
  #include "syscalls_table.h"
