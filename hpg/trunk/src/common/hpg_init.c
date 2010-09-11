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

#include <os.h>
#include "hpgraphics.h"
#include "hpgpriv.h"

extern unsigned int __display_buf; //default monochrome display buffer


/*
 * The graphics context representing the current screen.
 */
static hpg_t main_hpg;
static int inited = 0;

/*
 * The main pointer to the screen, initialized to the data structure above.
 */
hpg_t *hpg_stdscreen = &main_hpg;

/*
 * Initialization code for the screen
 */
void hpg_init(void)
{
    if (inited) return;

    hpg_stdscreen = &main_hpg;

    main_hpg.fb = DEFAULT_PLANE;
    main_hpg.drawfb = DEFAULT_PLANE;
    main_hpg.width = SCR_WIDTH;

    // FIXED BY CLAUDIO - MULTIPLE CALC SUPPORT
    main_hpg.height = sys_lcdgetheight();
    main_hpg.clipx1 = 0;
    main_hpg.clipy1 = 0;
    main_hpg.clipx2 = main_hpg.width;
    main_hpg.clipy2 = main_hpg.height;
    main_hpg.mode = HPG_MODE_PAINT;
    main_hpg.color = HPG_COLOR_BLACK;
    main_hpg.pattern = NULL;
    main_hpg.fbwidth = 0x14;
    main_hpg.drawfn = _hpg_mono_draw;
    main_hpg.getfn = _hpg_mono_get;

    inited = 1;
    atexit(hpg_cleanup);
}

void hpg_cleanup(void)
{
    if (!inited) return;

    // LCD RESTORE REMOVED BY CLAUDIO
    // LCD IS RESTORED BY DEFAULT ON CRT0

    if (main_hpg.org_fb1 != DEFAULT_PLANE)
    {
        free(main_hpg.org_fb1);
    }

    if ((main_hpg.org_fb2 != DEFAULT_PLANE)
        && (main_hpg.org_fb2 != main_hpg.org_fb1))
    {
        free(main_hpg.org_fb2);
    }

    inited = 0;
}
