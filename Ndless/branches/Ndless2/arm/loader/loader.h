/****************************************************************************
 * @(#) Ndless - Loader header
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

#ifndef _LOADER_H_
#define _LOADER_H_

#define STACK_PTR_BASE_ADDRESS        0x1800F000
#define MEMSPACE_BASE_ADDRESS         0x11E11000
#define MEMSPACE_END_ADDRESS          0x11F01060
#define MEMSPACE_MAX_BYTES_SIZE       (MEMSPACE_END_ADDRESS - MEMSPACE_BASE_ADDRESS)

#define OS_FLAGS_COUNT                3
#define OS_FLAGS_TABLE_BYTES_SIZE     (OS_FLAGS_COUNT * 8)

#define __OS_FUNC(f, v)   f ## _ ## v
#define _OS_FUNC(f, v)    __OS_FUNC(f, v)
#define OS_FUNC(f)        _OS_FUNC(f, OS_VERSION)

#endif
