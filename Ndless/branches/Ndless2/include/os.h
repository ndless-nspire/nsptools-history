/****************************************************************************
 * @(#) Ndless - OS Calls
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

#ifndef _OS_H_
#define _OS_H_

#ifndef OS_FUNC
  #define __OS_FUNC(f, v)   f ## _ ## v
  #define _OS_FUNC(f, v)    __OS_FUNC(f, v)
  #define OS_FUNC(f)        _OS_FUNC(f, OS_VERSION)
#endif

#endif
