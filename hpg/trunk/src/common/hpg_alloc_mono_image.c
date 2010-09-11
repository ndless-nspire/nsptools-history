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

hpg_t *hpg_alloc_mono_image(int width, int height)
{
    hpg_init();

    int fbwidth = (width + 7) >> 3; // width of a line in bytes
    char *fb = (char *) malloc(fbwidth * height);
    if (fb == NULL) return NULL;
    hpg_t *img = (hpg_t *) malloc(sizeof(hpg_t));
    if (img == NULL)
    {
        free(fb);
        return NULL;
    }

    img->fb = img->drawfb = fb;
    img->drawfn = _hpg_mono_draw;
    img->getfn = _hpg_mono_get;
    img->width = width;
    img->height = height;
    img->fbwidth = fbwidth;
    img->clipx1 = img->clipy1 = 0;
    img->clipx2 = width;
    img->clipy2 = height;
    img->mode = HPG_MODE_PAINT;
    img->color = HPG_COLOR_BLACK;
    img->pattern = NULL;
    img->font = NULL;

    return img;
}
