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

extern unsigned int __display_buf; // default monochrome display buffer

// CHANGED BY CLAUDIO - Apr 2005
/* THE LCD CONTROLLER SEEMS TO HAVE PROBLEMS
   WHEN ADDRESSES END WITH 0xC AND 0x8
   REASON IS UNKNOWN BUT THE WORKAROUND IS
   TO GUARANTEE THAT FRAME BUFFER IS ALWAYS
   ALIGNED IN 0x0 (MULTIPLE OF 4 WORDS)
*/

static char *align_fb(char *mem)
{
    int fbval = (int) mem;
    fbval = (fbval + 15) & 0xFFFFFFF0;
    return (char *) fbval;
}

void _hpg_set_mode(int mode, char *visfb, char *drawfb, 
    void (*drawfn)(hpg_t *, int, int, int, int, int),
    unsigned char (*getfn)(hpg_t *, int, int))
{

    char *orgvisfb=visfb,*orgdrawfb=drawfb;
    char *finalbuffer;
    int linewidth;

    if (visfb != DEFAULT_PLANE)
    {
        finalbuffer = visfb = (char *) sys_map_v2p((unsigned int) align_fb(visfb));
    }
    else finalbuffer= (char *)((__display_buf&0xfffff)|0x40000000); 


    drawfb = (char *) sys_map_v2p((unsigned int) align_fb(drawfb));


    sys_LCDSynch();


    // HARDWARE CODE REMOVED BY CLAUDIO

    linewidth=sys_lcdsetmode(mode,(int *)finalbuffer);


     /*
     * Update graphics context state so the library knows how to
     * deal with the current graphics mode.
     */

    if (hpg_stdscreen->fb != DEFAULT_PLANE)
    {
        free((char *) hpg_stdscreen->org_fb1);
    }

    if ((hpg_stdscreen->drawfb != DEFAULT_PLANE)
        && (hpg_stdscreen->drawfb != hpg_stdscreen->fb))
    {
        free((char *) hpg_stdscreen->org_fb2);
    }

    hpg_stdscreen->org_fb1=orgvisfb;
    hpg_stdscreen->org_fb2=orgdrawfb;
    hpg_stdscreen->fb = visfb;
    hpg_stdscreen->drawfb = drawfb;
    hpg_stdscreen->fbwidth = linewidth;
    hpg_stdscreen->drawfn = drawfn;
    hpg_stdscreen->getfn = getfn;
}
