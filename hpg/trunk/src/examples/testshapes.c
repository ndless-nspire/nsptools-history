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


    hpg_clear();

    hpg_draw_pixel(80, 10);
    hpg_draw_pixel(80, 20);
    hpg_draw_pixel(80, 30);

    hpg_draw_rect(0, 0, 9, 9);
    hpg_draw_rect(9, 9, 19, 19);
    hpg_fill_rect(0, 9, 9, 19);
    hpg_fill_rect(9, 0, 19, 9);

    hpg_draw_line(20, 9, 39, 9);
    hpg_draw_line(20, 5, 39, 14);
    hpg_draw_line(20, 0, 39, 19);
    hpg_draw_line(20, 19, 39, 0);
    hpg_draw_line(29, 0, 29, 19);
    hpg_draw_line(34, 0, 24, 19);
    hpg_draw_line(20, 14, 39, 5);
    hpg_draw_line(25, 0, 34, 19);

    hpg_draw_circle(45, 5, 5);
    hpg_fill_circle(45, 15, 5);

    int x1[] = { 10, 20, 30 };
    int y1[] = { 30, 50, 25 };
    int x2[] = { 50, 60, 70 };
    int y2[] = { 30, 50, 25 };

    hpg_fill_polygon(x1, y1, 3);
    hpg_draw_polygon(x2, y2, 3);

    keyb_waitKeyPressed();

    return 0;
}
