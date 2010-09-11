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

#include <stddef.h>
#include "hpgraphics.h"
#include "hpgpriv.h"

/*
 * Common drawing function.  This function provides clipping and culling
 * of draws to avoid duplication between the drawing modes.  It then
 * delegates to mode-specific code for the actual drawing.
 */
void _hpg_draw(hpg_t *g, int x, int y, int len, int patx, int paty)
{
    /*
     * Clip to clipping region.  Any drawing that lies entirely off the
     * screen, in either the x or y dimensions, is culled here.
     */
    if ((y < g->clipy1) || (y >= g->clipy2)) return;
    if (x < g->clipx1)
    {
        len += x - g->clipx1;
        x = g->clipx1;
    }
    if (x + len >= g->clipx2)
    {
        len = g->clipx2 - x;
    }

    if (len <= 0) return;

    /*
     * Delegate to mode-specific code for the remainder of the drawing.
     */
    (*g->drawfn)(g, x, y, len, patx, paty);
}
