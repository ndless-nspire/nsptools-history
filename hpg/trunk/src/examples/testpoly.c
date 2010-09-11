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

#include <hpkeyb49.h>
#include <hpsys.h>
#include "hpgraphics.h"

int errno;

int main(void)
{


    /*
     * Set mode to XOR to ensure that no pixel drawings are repeated.
     */
    hpg_set_mode(hpg_stdscreen, HPG_MODE_XOR);

    int x1[] = { 10, 20, 30 };
    int y1[] = { 30, 50, 25 };

    int x2[] = { 10, 30, 10, 20 };
    int y2[] = { 10, 20, 30, 20 };

    int x3[] = { 10, 20, 30, 20 };
    int y3[] = { 10, 20, 10, 30 };

    int x4[] = { 10, 30, 30, 10, 20, 20 };
    int y4[] = { 25, 10, 40, 25, 30, 20 };

    int x5[] = { 10, 20, 35, 20, 20, 40 };
    int y5[] = { 40, 10, 35, 30, 35, 35 };

    int x6[] = { 10, 30, 30, 10 };
    int y6[] = { 10, 30, 10, 30 };

    hpg_clear();
    hpg_fill_polygon(x1, y1, 3);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_draw_polygon(x1, y1, 3);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_fill_polygon(x2, y2, 4);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_draw_polygon(x2, y2, 4);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_fill_polygon(x3, y3, 4);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_draw_polygon(x3, y3, 4);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_fill_polygon(x4, y4, 6);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_draw_polygon(x4, y4, 6);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_fill_polygon(x5, y5, 6);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_draw_polygon(x5, y5, 6);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_fill_polygon(x6, y6, 4);
    keyb_waitKeyPressed();


    hpg_clear();
    hpg_draw_polygon(x6, y6, 4);

    keyb_waitKeyPressed();


    return 0;
}
