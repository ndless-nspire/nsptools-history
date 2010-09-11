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

void hpg_draw_line_on(hpg_t *g, int x1, int y1, int x2, int y2)
{
    hpg_init();

    if (y1 == y2)
    {
        int sx, len;
        if (x1 > x2)
        {
            sx = x2;
            len = x1 - x2 + 1;
        }
        else
        {
            sx = x1;
            len = x2 - x1 + 1;
        }

        _hpg_draw(g, sx, y1, len, -1, -1);
    }
    else
    {
        int dx, dy;
        int numpixels;
        int dinc1, dinc2, xinc1, xinc2, yinc1, yinc2;

        int x, y, d;
        int px, py, sx;

        int i;

        dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);
        dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);

        if (dx > dy)
        {
            numpixels = dx + 1;
            d = (2 * dy) - dx;
            dinc1 = 2 * dy;
            dinc2 = (dy - dx) * 2;
            xinc1 = 1;
            xinc2 = 1;
            yinc1 = 0;
            yinc2 = 1;
        }
        else
        {
            numpixels = dy + 1;
            d = (2 * dx) - dy;
            dinc1 = 2 * dx;
            dinc2 = (dx - dy) * 2;
            xinc1 = 0;
            xinc2 = 1;
            yinc1 = 1;
            yinc2 = 1;
        }

        if (x1 > x2)
        {
            xinc1 = -xinc1;
            xinc2 = -xinc2;
        }

        if (y1 > y2)
        {
            yinc1 = -yinc1;
            yinc2 = -yinc2;
        }

        x = x1;
        y = y1;

        sx = x;

        for (i = 0; i < numpixels; i++)
        {
            px = x;
            py = y;

            if (d < 0)
            {
                d += dinc1;
                x += xinc1;
                y += yinc1;
            }
            else
            {
                d += dinc2;
                x += xinc2;
                y += yinc2;
            }

            if (y != py)
            {
                if (xinc2 < 0) _hpg_draw(g, px, py, sx - px + 1, -1, -1);
                else _hpg_draw(g, sx, py, px - sx + 1, -1, -1);
                sx = x;
            }
        }

        if (x != sx)
        {
            if (xinc2 < 0) _hpg_draw(g, x + 1, y, sx - x, -1, -1);
            else _hpg_draw(g, sx, y, x - sx, -1, -1);
        }
    }
}
