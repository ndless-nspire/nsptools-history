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

void hpg_fill_circle_on(hpg_t *g, int cx, int cy, int r)
{
    hpg_init();

    int x, y, d;

    d = 3 - (2 * r);
    x = 0;
    y = r;

    while (x <= y)
    {
        int px, py;
        px = x;
        py = y;

        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }

        x++;

        if (y != py)
        {
            _hpg_draw(g, cx - px, cy + py, 2 * px + 1, cx - r, cy - r);
            _hpg_draw(g, cx - px, cy - py, 2 * px + 1, cx - r, cy - r);
        }

        if (px != py)
        {
            _hpg_draw(g, cx - py, cy + px, 2 * py + 1, cx - r, cy - r);
            if (px != 0)
            {
                _hpg_draw(g, cx - py, cy - px, 2 * py + 1, cx - r, cy - r);
            }
        }
    }
}
