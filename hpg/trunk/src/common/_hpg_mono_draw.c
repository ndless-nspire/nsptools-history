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

/* The following cases are considered separately:
 *
 * - Drawing without a pattern
 * - Drawing with a pattern
 */

static void draw_nonpat(hpg_t *g, int x, int y, int len)
{
    unsigned char colormask = (g->color >> 7) ? 0xFF : 0x00;
    unsigned char xormask = (g->mode == HPG_MODE_XOR) ? 0xFF : 0x00;
    volatile char *fb = g->drawfb + g->fbwidth * y + (x >> 3);

    if (((x + len - 1) ^ x) < 8)
    {
        /*
         * Single-byte section.  Handle specially.
         */
        unsigned char mask = ((1 << len) - 1) << (x & 7);
        *fb = (*fb & (~mask | xormask)) ^ (mask & colormask);
    }
    else
    {
        volatile char *fbend = g->drawfb + g->fbwidth * y
            + ((x + len - 1) >> 3);

        if (x & 7)
        {
            int beginlen = 8 - (x & 7);
            unsigned char mask = ((1 << beginlen) - 1) << (x & 7);
            *fb = (*fb & (~mask | xormask)) ^ (mask & colormask);
            fb++;
        }

        while (fb < fbend)
        {
            *(fb) = (*fb & xormask) ^ colormask;
            fb++;
        }

        int endlen = ((len + (x & 7) - 1) & 7) + 1;
        unsigned char mask = (1 << endlen) - 1;
        *fb = (*fb & (~mask | xormask)) ^ (mask & colormask);
    }
}

static void draw_pattern(hpg_t *g, int x, int y, int len, int patx, int paty)
{
    if (patx == -1) draw_nonpat(g, x, y, len);

    unsigned char color = (g->color & 0x80) ? 0xFF : 0x00;

    /*
     * Build the master pattern.  This is yet another bitmask applied to all
     * drawing operations, and it allows the user to draw a pattern onto the
     * screen.
     *
     * This is also a candidate to be cached in the graphics context.
     */
    struct hpg_pattern *pat = g->pattern;
    if (pat->fixed) patx = paty = 0;

    int idx;

    idx = y - paty;
    while (idx < 0) idx += pat->height;
    while (idx >= pat->height) idx -= pat->height;
    int line = pat->buffer[idx];

    idx = (x - patx + (x & 7)) & 7;
    line = (line << (idx)) | (line >> (8 - idx));

    volatile char *fb = g->drawfb + g->fbwidth * y + (x >> 3);

    if (((x + len - 1) ^ x) < 8)
    {
        /*
         * Single-byte section.  Handle specially.
         */
        unsigned char mask = (((1 << len) - 1) << (x & 7)) & line;

        if (g->mode == HPG_MODE_PAINT) *fb &= ~mask;
        *fb ^= mask & color;
    }
    else
    {
        volatile char *fbend = g->drawfb + g->fbwidth * y
            + ((x + len - 1) >> 3);

        // first
        if ((x & 7) != 0)
        {
            int firstlen = 8 - (x & 7);
            unsigned char mask = (((1 << firstlen) - 1) << (x & 7)) & line;

            if (g->mode == HPG_MODE_PAINT) *fb &= ~mask;
            *fb ^= mask & color;
            fb++;
        }

        // middle
        while (fb < fbend)
        {
            if (g->mode == HPG_MODE_PAINT) *fb &= ~line;
            *fb ^= line & color;
            fb++;
        }

        // end
        int endlen = ((len + (x & 7) - 1) & 7) + 1;
        unsigned char mask = ((1 << endlen) - 1) & line;

        if (g->mode == HPG_MODE_PAINT) *fb &= ~mask;
        *fb ^= mask & color;
    }
}

/*
 * Select a drawing routine.  Many cases use special routines to
 * optimize the drawing process.  In the future, once accessors and
 * mutators are implemented for graphics contexts, this series of
 * conditionals should be changed to replacement of the drawfn function
 * pointer to add speed.
 */
void _hpg_mono_draw(hpg_t *g, int x, int y, int len, int patx, int paty)
{
    if (g->pattern == NULL)
    {
        draw_nonpat(g, x, y, len);
    }
    else
    {
        draw_pattern(g, x, y, len, patx, paty);
    }
}
