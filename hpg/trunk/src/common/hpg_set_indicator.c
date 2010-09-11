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

void hpg_set_indicator(unsigned char indicator, unsigned char color)
{
    hpg_init();

    /*
     * A special graphics device to be used for turning the screen indicators
     * on or off.  This device is identical to the main graphics device except
     * for the modified clipping rectangle, which allows writing to the
     * off-screen segments corresponding to the indicators.
     *
     * The locations of the indicator pixels are:
     *
     * (131, 0) - Remote
     * (131, 1) - Left Shift
     * (131, 2) - Right Shift
     * (131, 3) - Alpha
     * (131, 4) - Low Battery
     * (131, 5) - Wait
     */
    static hpg_t indicator_hpg;
    indicator_hpg = *hpg_stdscreen;
    indicator_hpg.clipx1 = 0;
    indicator_hpg.clipx2 = 160;
    indicator_hpg.clipy1 = 0;
    indicator_hpg.clipy2 = 80;

    indicator_hpg.color = color;
    hpg_draw_pixel_on(&indicator_hpg, 131, indicator);
}
