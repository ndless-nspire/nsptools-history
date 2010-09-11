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

void hpg_draw_text_on(hpg_t *g, char *s, int x, int y)
{
    hpg_init();

    if (g->font == NULL) g->font = hpg_get_minifont();

    struct hpg_pattern charpat = {
        NULL, g->font->height, 0
    };

    struct hpg_pattern *oldpat = g->pattern;
    g->pattern = &charpat;

    int startx = x;
    while (*s != '\0')
    {
        if (*s == '\n')
        {
            x = startx;
            y += g->font->height;
        }
        else
        {
            if (*s < g->font->count)
            {
                charpat.buffer = g->font->buffer + (g->font->height * (*s));
                hpg_fill_rect_on(g, x, y,
                    x + g->font->advance - 1, y + g->font->height - 1);

                x += g->font->advance;
            }
        }

        s++;
    }

    g->pattern = oldpat;
}
