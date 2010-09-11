//& ****************************************************************************
//&
//& Copyright (C) 2004 HP-GCC Team
//&
//& ****************************************************************************
//&
//& This file is part of HP-GCC.
//&
//& HP-GCC is free software; you can redistribute it and/or modify
//& it under the terms of the GNU General Public License as published by
//& the Free Software Foundation; either version 2, or (at your option)
//& any later version.
//& 
//& HP-GCC is distributed in the hope that it will be useful,
//& but WITHOUT ANY WARRANTY; without even the implied warranty of
//& MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//& GNU General Public License for more details.
//& 
//& You should have received a copy of the GNU General Public License
//& along with HP-GCC; see the file COPYING. 
//& 
//& As a special exception, you may use this file as part of a free software
//& library without restriction.  Specifically, if other files instantiate
//& templates or use macros or inline functions from this file, or you compile
//& this file and link it with other files to produce an executable, this
//& file does not by itself cause the resulting executable to be covered by
//& the GNU General Public License.  This exception does not however
//& invalidate any other reasons why the executable file might be covered by
//& the GNU General Public License.
//&
//& ****************************************************************************

#include "hpgraphics.h"
#include "hpgpriv.h"

static char minifont_data[] = {
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x4, 0x6, 0x7, 0x6, 0x4, 0x0,
    0x7, 0x7, 0x7, 0x7, 0x7, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x5, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x5, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x2, 0x2, 0x2, 0x0, 0x2, 0x0,
    0x5, 0x5, 0x0, 0x0, 0x0, 0x0,
    0x2, 0x7, 0x2, 0x7, 0x2, 0x0,
    0x6, 0x3, 0x2, 0x6, 0x3, 0x0,
    0x5, 0x4, 0x2, 0x1, 0x5, 0x0,
    0x2, 0x5, 0xA, 0x5, 0xB, 0x0,
    0x2, 0x2, 0x0, 0x0, 0x0, 0x0,
    0x4, 0x2, 0x2, 0x2, 0x4, 0x0,
    0x1, 0x2, 0x2, 0x2, 0x1, 0x0,
    0x0, 0x5, 0x2, 0x5, 0x0, 0x0,
    0x0, 0x2, 0x7, 0x2, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x2, 0x2, 0x1,
    0x0, 0x0, 0x7, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x2, 0x0,
    0x0, 0x4, 0x2, 0x1, 0x0, 0x0,
    0x6, 0x5, 0x5, 0x5, 0x3, 0x0,
    0x2, 0x3, 0x2, 0x2, 0x7, 0x0,
    0x7, 0x4, 0x6, 0x1, 0x7, 0x0,
    0x7, 0x4, 0x2, 0x4, 0x3, 0x0,
    0x5, 0x5, 0x7, 0x4, 0x4, 0x0,
    0x7, 0x1, 0x7, 0x4, 0x3, 0x0,
    0x6, 0x1, 0x3, 0x5, 0x6, 0x0,
    0x7, 0x4, 0x2, 0x1, 0x1, 0x0,
    0x6, 0x5, 0x2, 0x5, 0x3, 0x0,
    0x3, 0x5, 0x6, 0x4, 0x3, 0x0,
    0x0, 0x2, 0x0, 0x2, 0x0, 0x0,
    0x0, 0x2, 0x0, 0x2, 0x2, 0x1,
    0x4, 0x2, 0x1, 0x2, 0x4, 0x0,
    0x0, 0x7, 0x0, 0x7, 0x0, 0x0,
    0x1, 0x2, 0x4, 0x2, 0x1, 0x0,
    0x3, 0x4, 0x2, 0x0, 0x2, 0x0,
    0x2, 0x7, 0x3, 0x1, 0x6, 0x0,
    0x2, 0x5, 0x7, 0x5, 0x5, 0x0,
    0x3, 0x5, 0x3, 0x5, 0x3, 0x0,
    0x6, 0x1, 0x1, 0x1, 0x6, 0x0,
    0x3, 0x5, 0x5, 0x5, 0x3, 0x0,
    0x7, 0x1, 0x3, 0x1, 0x7, 0x0,
    0x7, 0x1, 0x3, 0x1, 0x1, 0x0,
    0x6, 0x1, 0x5, 0x5, 0x6, 0x0,
    0x5, 0x5, 0x7, 0x5, 0x5, 0x0,
    0x7, 0x2, 0x2, 0x2, 0x7, 0x0,
    0x4, 0x4, 0x4, 0x5, 0x6, 0x0,
    0x5, 0x5, 0x3, 0x5, 0x5, 0x0,
    0x1, 0x1, 0x1, 0x1, 0x7, 0x0,
    0x5, 0x7, 0x7, 0x5, 0x5, 0x0,
    0x3, 0x5, 0x5, 0x5, 0x5, 0x0,
    0x2, 0x5, 0x5, 0x5, 0x2, 0x0,
    0x3, 0x5, 0x3, 0x1, 0x1, 0x0,
    0x2, 0x5, 0x5, 0x3, 0x6, 0x0,
    0x3, 0x5, 0x3, 0x5, 0x5, 0x0,
    0x6, 0x1, 0x2, 0x4, 0x3, 0x0,
    0x7, 0x2, 0x2, 0x2, 0x2, 0x0,
    0x5, 0x5, 0x5, 0x5, 0x7, 0x0,
    0x5, 0x5, 0x5, 0x2, 0x2, 0x0,
    0x5, 0x5, 0x7, 0x7, 0x5, 0x0,
    0x5, 0x5, 0x2, 0x5, 0x5, 0x0,
    0x5, 0x5, 0x2, 0x2, 0x2, 0x0,
    0x7, 0x4, 0x2, 0x1, 0x7, 0x0,
    0x6, 0x2, 0x2, 0x2, 0x6, 0x0,
    0x0, 0x1, 0x2, 0x4, 0x0, 0x0,
    0x3, 0x2, 0x2, 0x2, 0x3, 0x0,
    0x2, 0x5, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x7, 0x0,
    0x2, 0x4, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x6, 0x5, 0x5, 0x6, 0x0,
    0x1, 0x3, 0x5, 0x5, 0x3, 0x0,
    0x0, 0x6, 0x1, 0x1, 0x6, 0x0,
    0x4, 0x6, 0x5, 0x5, 0x6, 0x0,
    0x0, 0x6, 0x5, 0x3, 0x6, 0x0,
    0x6, 0x1, 0x3, 0x1, 0x1, 0x0,
    0x0, 0x6, 0x5, 0x6, 0x4, 0x3,
    0x1, 0x3, 0x5, 0x5, 0x5, 0x0,
    0x2, 0x0, 0x3, 0x2, 0x7, 0x0,
    0x4, 0x0, 0x4, 0x4, 0x5, 0x2,
    0x1, 0x5, 0x3, 0x5, 0x5, 0x0,
    0x2, 0x2, 0x2, 0x2, 0x4, 0x0,
    0x0, 0x5, 0x7, 0x5, 0x5, 0x0,
    0x0, 0x3, 0x5, 0x5, 0x5, 0x0,
    0x0, 0x2, 0x5, 0x5, 0x2, 0x0,
    0x0, 0x3, 0x5, 0x5, 0x3, 0x1,
    0x0, 0x6, 0x5, 0x5, 0x6, 0x4,
    0x0, 0x6, 0x1, 0x1, 0x1, 0x0,
    0x0, 0x6, 0x3, 0x4, 0x3, 0x0,
    0x2, 0x7, 0x2, 0x2, 0x4, 0x0,
    0x0, 0x5, 0x5, 0x5, 0x6, 0x0,
    0x0, 0x5, 0x5, 0x5, 0x2, 0x0,
    0x0, 0x5, 0x5, 0x7, 0x5, 0x0,
    0x0, 0x5, 0x2, 0x2, 0x5, 0x0,
    0x0, 0x5, 0x5, 0x6, 0x4, 0x3,
    0x0, 0x7, 0x4, 0x2, 0x7, 0x0,
    0x6, 0x2, 0x1, 0x2, 0x6, 0x0,
    0x2, 0x2, 0x2, 0x2, 0x2, 0x0,
    0x3, 0x2, 0x4, 0x2, 0x3, 0x0,
    0x0, 0x0, 0xA, 0x5, 0x0, 0x0,
    0x5, 0xA, 0x5, 0xA, 0x5, 0x0,
    0x0, 0x4, 0x2, 0x5, 0x7, 0x0,
    0x7, 0x0, 0x5, 0x2, 0x5, 0x0,
    0x0, 0x7, 0x5, 0x2, 0x0, 0x0,
    0x6, 0x2, 0x2, 0x3, 0x2, 0x0,
    0x4, 0x2, 0x2, 0x2, 0x1, 0x0,
    0x7, 0x1, 0x2, 0x1, 0x7, 0x0,
    0x1, 0x3, 0x7, 0x3, 0x1, 0x0,
    0x0, 0x7, 0x4, 0x5, 0x5, 0x0,
    0x2, 0x4, 0x6, 0x5, 0x2, 0x0,
    0x4, 0x2, 0x7, 0x0, 0x7, 0x0,
    0x1, 0x2, 0x7, 0x0, 0x7, 0x0,
    0x4, 0x7, 0x2, 0x7, 0x1, 0x0,
    0x0, 0xA, 0x5, 0x5, 0xA, 0x0,
    0x0, 0x4, 0xF, 0x4, 0x0, 0x0,
    0x0, 0x2, 0xF, 0x2, 0x0, 0x0,
    0x0, 0x2, 0x2, 0x7, 0x2, 0x0,
    0x2, 0x7, 0x2, 0x2, 0x0, 0x0,
    0x0, 0x4, 0x5, 0x6, 0x2, 0x2,
    0x2, 0x1, 0x2, 0x5, 0x2, 0x0,
    0x6, 0x1, 0x7, 0x1, 0x6, 0x0,
    0x0, 0x5, 0xA, 0xA, 0x8, 0x8,
    0x2, 0x5, 0x7, 0x5, 0x2, 0x0,
    0x0, 0x1, 0x2, 0x5, 0x5, 0x0,
    0x2, 0x5, 0x5, 0x3, 0x1, 0x0,
    0x0, 0xE, 0x5, 0x5, 0x2, 0x0,
    0x0, 0x7, 0x2, 0x2, 0x4, 0x0,
    0x0, 0x9, 0xB, 0xF, 0x6, 0x0,
    0x0, 0x2, 0x5, 0x7, 0x0, 0x0,
    0xF, 0x4, 0x5, 0x5, 0x5, 0x0,
    0x2, 0x5, 0x5, 0x2, 0x5, 0x0,
    0x0, 0x7, 0x7, 0x7, 0x0, 0x0,
    0x0, 0x5, 0x9, 0xA, 0x0, 0x0,
    0x6, 0x1, 0x3, 0x1, 0x6, 0x0,
    0x2, 0x0, 0x2, 0x2, 0x2, 0x0,
    0x2, 0x7, 0x1, 0x7, 0x2, 0x0,
    0x6, 0x2, 0x7, 0x2, 0x7, 0x0,
    0x0, 0x9, 0x6, 0x6, 0x9, 0x0,
    0x5, 0x5, 0x2, 0x7, 0x2, 0x0,
    0x2, 0x2, 0x0, 0x2, 0x2, 0x0,
    0x6, 0x1, 0x7, 0x6, 0x3, 0x0,
    0x5, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x6, 0x2, 0x6, 0x0, 0x0,
    0x6, 0x5, 0x6, 0xF, 0x0, 0x0,
    0x0, 0xA, 0x5, 0xA, 0x0, 0x0,
    0x0, 0x0, 0x7, 0x4, 0x0, 0x0,
    0x0, 0x0, 0x7, 0x0, 0x0, 0x0,
    0x3, 0x5, 0x3, 0x5, 0x0, 0x0,
    0x7, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x7, 0x5, 0x7, 0x0, 0x0, 0x0,
    0x0, 0x2, 0x7, 0x2, 0x7, 0x0,
    0x1, 0x2, 0x1, 0x3, 0x0, 0x0,
    0x3, 0x2, 0x3, 0x2, 0x3, 0x0,
    0x2, 0x1, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x5, 0x5, 0x7, 0x9, 0x0,
    0x7, 0xC, 0xD, 0x5, 0x5, 0x0,
    0x0, 0x0, 0x2, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x2, 0x4, 0x3,
    0x3, 0x2, 0x7, 0x0, 0x0, 0x0,
    0x2, 0x5, 0x2, 0x0, 0x0, 0x0,
    0x0, 0x5, 0xA, 0x5, 0x0, 0x0,
    0x1, 0x9, 0xD, 0xE, 0x8, 0x0,
    0x1, 0x5, 0x9, 0x4, 0xE, 0x0,
    0x3, 0xA, 0xD, 0xE, 0xB, 0x0,
    0x2, 0x0, 0x2, 0x1, 0x6, 0x0,
    0x1, 0x2, 0x5, 0x7, 0x5, 0x0,
    0x4, 0x2, 0x5, 0x7, 0x5, 0x0,
    0x2, 0x7, 0x5, 0x7, 0x5, 0x0,
    0xA, 0x7, 0x5, 0x7, 0x5, 0x0,
    0x5, 0x2, 0x5, 0x7, 0x5, 0x0,
    0x2, 0x2, 0x5, 0x7, 0x5, 0x0,
    0xE, 0x5, 0xF, 0x5, 0xD, 0x0,
    0x6, 0x1, 0x1, 0x1, 0x6, 0x3,
    0x1, 0x7, 0x3, 0x1, 0x7, 0x0,
    0x4, 0x7, 0x3, 0x1, 0x7, 0x0,
    0x2, 0x7, 0x3, 0x1, 0x7, 0x0,
    0x5, 0x7, 0x3, 0x1, 0x7, 0x0,
    0x1, 0x7, 0x2, 0x2, 0x7, 0x0,
    0x4, 0x7, 0x2, 0x2, 0x7, 0x0,
    0x2, 0x7, 0x2, 0x2, 0x7, 0x0,
    0x5, 0x7, 0x2, 0x2, 0x7, 0x0,
    0x6, 0xA, 0xB, 0xA, 0x6, 0x0,
    0x6, 0x3, 0x5, 0x5, 0x5, 0x0,
    0x1, 0x2, 0x5, 0x5, 0x2, 0x0,
    0x4, 0x2, 0x5, 0x5, 0x2, 0x0,
    0x2, 0x7, 0x5, 0x5, 0x2, 0x0,
    0xA, 0x7, 0x5, 0x5, 0x2, 0x0,
    0x5, 0x2, 0x5, 0x5, 0x2, 0x0,
    0x0, 0x5, 0x2, 0x5, 0x0, 0x0,
    0x4, 0x7, 0x5, 0x7, 0x1, 0x0,
    0x1, 0x2, 0x5, 0x5, 0x7, 0x0,
    0x4, 0x2, 0x5, 0x5, 0x7, 0x0,
    0x2, 0x5, 0x0, 0x5, 0x7, 0x0,
    0x5, 0x0, 0x5, 0x5, 0x7, 0x0,
    0x4, 0x2, 0x5, 0x7, 0x2, 0x0,
    0x1, 0x3, 0x5, 0x3, 0x1, 0x0,
    0x2, 0x5, 0x3, 0x5, 0x3, 0x1,
    0x1, 0x2, 0x6, 0x5, 0x6, 0x0,
    0x2, 0x1, 0x6, 0x5, 0x6, 0x0,
    0x2, 0x5, 0x6, 0x5, 0x6, 0x0,
    0xA, 0x5, 0x6, 0x5, 0x6, 0x0,
    0x5, 0x0, 0x6, 0x5, 0x6, 0x0,
    0x2, 0x0, 0x6, 0x5, 0x6, 0x0,
    0x0, 0xE, 0xD, 0x5, 0xE, 0x0,
    0x0, 0x6, 0x1, 0x1, 0x6, 0x3,
    0x1, 0x6, 0x5, 0x3, 0x6, 0x0,
    0x4, 0x6, 0x5, 0x3, 0x6, 0x0,
    0x2, 0x7, 0x5, 0x3, 0x6, 0x0,
    0x5, 0x6, 0x5, 0x3, 0x6, 0x0,
    0x2, 0x4, 0x3, 0x2, 0x7, 0x0,
    0x4, 0x2, 0x3, 0x2, 0x7, 0x0,
    0x2, 0x5, 0x3, 0x2, 0x7, 0x0,
    0x5, 0x0, 0x3, 0x2, 0x7, 0x0,
    0x1, 0x2, 0x2, 0x5, 0x2, 0x0,
    0xA, 0x5, 0x3, 0x5, 0x5, 0x0,
    0x2, 0x4, 0x2, 0x5, 0x2, 0x0,
    0x2, 0x1, 0x2, 0x5, 0x2, 0x0,
    0x2, 0x5, 0x2, 0x5, 0x2, 0x0,
    0xA, 0x5, 0x2, 0x5, 0x2, 0x0,
    0x5, 0x0, 0x2, 0x5, 0x2, 0x0,
    0x2, 0x0, 0x7, 0x0, 0x2, 0x0,
    0x8, 0x6, 0x5, 0x7, 0x1, 0x0,
    0x1, 0x2, 0x0, 0x5, 0x6, 0x0,
    0x4, 0x2, 0x0, 0x5, 0x6, 0x0,
    0x2, 0x5, 0x0, 0x5, 0x6, 0x0,
    0x5, 0x0, 0x5, 0x5, 0x6, 0x0,
    0x4, 0x2, 0x5, 0x6, 0x4, 0x3,
    0x1, 0x7, 0x5, 0x7, 0x1, 0x0,
    0x5, 0x0, 0x5, 0x6, 0x4, 0x3
};

static struct hpg_font minifont;

hpg_font_t *hpg_get_minifont()
{
    hpg_init();

    minifont.buffer = minifont_data;
    minifont.count = 256;
    minifont.height = 6;
    minifont.advance = 4;

    return &minifont;
}
