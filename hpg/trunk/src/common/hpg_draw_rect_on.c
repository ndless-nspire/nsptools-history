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

void hpg_draw_rect_on(hpg_t *g, int x1, int y1, int x2, int y2)
{
    hpg_init();

    if (x2 < x1)
    {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (y2 < y1)
    {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    _hpg_draw(g, x1, y1, x2 - x1 + 1, -1, -1);
    if (y2 - y1 == 0) return;

    _hpg_draw(g, x1, y2, x2 - x1 + 1, -1, -1);
    if (y2 - y1 == 1) return;

    int y;
    for (y = y1 + 1; y < y2; y++)
    {
        _hpg_draw(g, x1, y, 1, -1, -1);
        if (x2 != x1) _hpg_draw(g, x2, y, 1, -1, -1);
    }
}
