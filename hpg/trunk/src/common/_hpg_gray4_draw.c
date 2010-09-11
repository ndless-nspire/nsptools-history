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
 * - No pattern
 * - Pattern becomes the general case
 */

static void draw_nonpat(hpg_t *g, int x, int y, int len)
{
    unsigned char color = g->color >> (6);
    unsigned char colormask =
        color | (color << 2) | (color << 4) | (color << 6);

    volatile char *fb = g->drawfb + g->fbwidth * y + (x >> 2);

    if (((x + len - 1) ^ x) < 4)
    {
        /*
         * Single-byte section.  Handle specially.
         */
        unsigned char mask = ((1 << (len << 1)) - 1) << ((x & 3) << 1);
        if (g->mode == HPG_MODE_PAINT) *fb &= ~(mask);
        *fb ^= mask & colormask;
    }
    else
    {
        volatile char *fbend = g->drawfb + g->fbwidth * y
            + ((x + len - 1) >> 2);

        if (x & 7)
        {
            int beginlen = 8 - (x & 3);
            unsigned char mask = ((1 << (beginlen << 1)) - 1) << ((x & 3) << 1);
            if (g->mode == HPG_MODE_PAINT) *fb &= ~mask;
            *(fb++) ^= mask & colormask;
        }

        while (fb < fbend)
        {
            if (g->mode == HPG_MODE_PAINT) *fb = colormask;
            else *fb ^= colormask;
            fb++;
        }

        int endlen = ((len + (x & 3) - 1) & 3) + 1;
        unsigned char mask = (1 << (endlen << 1)) - 1;
        if (g->mode == HPG_MODE_PAINT) *fb &= ~mask;
        *fb ^= mask & colormask;
    }
}

static void draw_pattern(hpg_t *g, int x, int y, int len, int patx, int paty)
{
    if (patx == -1) draw_nonpat(g, x, y, len);

    unsigned char color = g->color >> (6);
    unsigned char colormask =
        color | (color << 2) | (color << 4) | (color << 6);

    /*
     * Read the line from the pattern;
     */
    struct hpg_pattern *pat = g->pattern;

    if (pat->fixed)
    {
        patx = 0;
        paty = 0;
    }

    int idx;
    unsigned char patline = 0;

    idx = y - paty;
    while (idx < 0) idx += pat->height;
    while (idx >= pat->height) idx -= pat->height;
    patline = pat->buffer[idx];

    /*
     * Build the master pattern.  This is yet another bitmask applied to all
     * drawing operations, and it allows the user to draw a pattern onto the
     * screen.
     *
     * This is also a candidate to be cached in the graphics context.
     */
    unsigned short masterpattern = 0x00;
    unsigned char srcmask = 0x01;
    unsigned short dstmask = 0x03;

    while (dstmask != 0)
    {
        if ((patline & srcmask) > 0)
        {
            masterpattern |= dstmask;
        }

        dstmask <<= 2;
        srcmask <<= 1;

        if (srcmask == 0) srcmask = 1;
    }

    idx = ((x & ~3) - patx) & 7;
    masterpattern = (masterpattern >> (idx << 1))
        | (masterpattern << (16 - (idx << 1)));

    volatile char *fb = g->drawfb + g->fbwidth * y + (x >> 2);

    if (((x + len - 1) ^ x) < 4)
    {
        /*
         * Single-byte section.  Handle specially.
         */
        unsigned char mask = ((1 << (len << 1)) - 1) << ((x & 3) << 1);
        mask &= (unsigned char) masterpattern;
        if (g->mode == HPG_MODE_PAINT) *fb &= ~(mask);
        *fb ^= mask & colormask;
    }
    else
    {
        volatile char *fbend = g->drawfb + g->fbwidth * y
            + ((x + len - 1) >> 2);

        if (x & 3)
        {
            int beginlen = 4 - (x & 3);
            unsigned char mask = ((1 << (beginlen << 1)) - 1) << ((x & 3) << 1);
            mask &= (unsigned char) masterpattern;
            if (g->mode == HPG_MODE_PAINT) *fb &= ~mask;
            *(fb++) ^= mask & colormask;

            masterpattern = (masterpattern >> 8) | (masterpattern << 8);
        }

        while (fb < fbend)
        {
            unsigned char mask = (unsigned char) masterpattern;
            if (g->mode == HPG_MODE_PAINT) *fb &= ~mask;
            *fb ^= mask & colormask;
            fb++;

            masterpattern = (masterpattern >> 8) | (masterpattern << 8);
        }

        int endlen = ((len + (x & 3) - 1) & 3) + 1;
        unsigned char mask = ((1 << (endlen << 1)) - 1);
        mask &= (unsigned char) masterpattern;
        if (g->mode == HPG_MODE_PAINT) *fb &= ~mask;
        *fb ^= mask & colormask;
    }
}

/*
 * Select a drawing routine.  Many cases use special routines to
 * optimize the drawing process.  In the future, once accessors and
 * mutators are implemented for graphics contexts, this series of
 * conditionals should be changed to replacement of the drawfn function
 * pointer to add speed.
 */
void _hpg_gray4_draw(hpg_t *g, int x, int y, int len, int patx, int paty)
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
