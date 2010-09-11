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

    hpg_set_mode_gray16(0);
    hpg_clear();

    hpg_set_color(hpg_stdscreen, HPG_COLOR_WHITE);
    hpg_fill_rect(0, 10, 7, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_1);
    hpg_fill_rect(8, 10, 15, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_2);
    hpg_fill_rect(16, 10, 23, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_3);
    hpg_fill_rect(24, 10, 31, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_4);
    hpg_fill_rect(32, 10, 39, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_5);
    hpg_fill_rect(40, 10, 47, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_6);
    hpg_fill_rect(48, 10, 55, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_7);
    hpg_fill_rect(56, 10, 63, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_8);
    hpg_fill_rect(64, 10, 71, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_9);
    hpg_fill_rect(72, 10, 79, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_10);
    hpg_fill_rect(80, 10, 87, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_11);
    hpg_fill_rect(88, 10, 95, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_12);
    hpg_fill_rect(96, 10, 103, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_13);
    hpg_fill_rect(104, 10, 111, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_14);
    hpg_fill_rect(112, 10, 119, 20);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_BLACK);
    hpg_fill_rect(120, 10, 127, 20);

    keyb_waitKeyPressed();


    return 0;
}
