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

    hpg_draw_text("0", 0, 0);
    hpg_draw_text("1", 8, 0);
    hpg_draw_text("2", 16, 0);
    hpg_draw_text("3", 24, 0);
    hpg_draw_text("4", 32, 0);
    hpg_draw_text("5", 40, 0);
    hpg_draw_text("6", 48, 0);
    hpg_draw_text("7", 56, 0);
    hpg_draw_text("8", 64, 0);
    hpg_draw_text("9", 72, 0);
    hpg_draw_text("10", 80, 0);
    hpg_draw_text("11", 88, 0);
    hpg_draw_text("12", 96, 0);
    hpg_draw_text("13", 104, 0);
    hpg_draw_text("14", 112, 0);
    hpg_draw_text("15", 120, 0);

    keyb_waitKeyPressed();

    return 0;
}
