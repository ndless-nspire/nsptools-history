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

//#define __USE_GCC_ALLOCA_

#ifdef __USE_GCC_ALLOCA_
extern void *alloca(int size);
#endif

/*
 * This is the base algorithm for all polygon operations, based on a
 * scanline approach to drawing operations.
 *
 * The code listed here is a heavily modified version of Uche Akotaobi's
 * scanline poly fill algorithm, published on his web site at
 * http://www-scf.usc.edu/~akotaobi/gptut19.html.  The primary difference
 * is that Uche uses floating-point math to resolve a lot of problems,
 * while the line intersections here are determined using Bresenham line
 * drawing terms.
 *
 * This code has also been modified to draw polygon outlines without overlaps
 * if requested, based on the value of the fill parameter.  This has a couple
 * advantages over drawing polygons with stock line-drawing code -- mainly
 * that we needn't worry about duplicate pixels for XOR mode, and that the
 * result is guaranteed to be a pixel-perfect match with filling the same
 * polygon data.
 */
void _hpg_poly(hpg_t *g, int vx[], int vy[], int len, int fill)
{
    if (len == 0) return;
    if (len == 1) _hpg_draw(g, vx[0], vy[0], 1, vx[0], vy[0]);

    int i, j;

    struct edge {
        unsigned top, bot;
        int x, d, xinc, dincx, dincy;
        int dir;
    };

    #ifdef __USE_GCC_ALLOCA_
    struct edge *edges = (struct edge *)alloca(len * sizeof(struct edge));
    #else
    struct edge *edges = (struct edge *)malloc(len * sizeof(struct edge));
    #endif
    int edgect = 0;

    /*
     * Populate and initialize the edges of the polygon.  Each edge
     * contains its bresenham line-drawing state, so that it can be
     * accurately tracked as progress is made down the screen.
     */
    for (i = 0; i < len; i++)
    {
        int start = i, next = (i == len - 1 ? 0 : i + 1);
        if (fill && (vy[start] == vy[next])) continue;

        int top, bot;

        if (vy[start] < vy[next])
        {
            edges[edgect].top = top = start;
            edges[edgect].bot = bot = next;
            edges[edgect].dir = 1;
        }
        else
        {
            edges[edgect].top = top = next;
            edges[edgect].bot = bot = start;
            edges[edgect].dir = -1;
        }

        int dx = vx[bot] - vx[top];
        int dy = vy[bot] - vy[top];
        if (dx < 0)
        {
            edges[edgect].xinc = -1;
            dx = -dx;
        }
        else edges[edgect].xinc = 1;

        edges[edgect].x = vx[top];
        edges[edgect].dincx = -2 * dy;
        edges[edgect].dincy = 2 * dx;

        if (dx > dy) edges[edgect].d = dx;
        else edges[edgect].d = 2 * dx - dy;

        edgect++;
    }

    /*
     * Find the bounds of the polygon.
     */
    int minx = vx[0], miny = vy[0], maxy = vy[0];
    for (i = 1; i < len; i++)
    {
        minx = min(minx, vx[i]);
        miny = min(miny, vy[i]);
        maxy = max(maxy, vy[i]);
    }

    /*
     * Allocate space to hold the data per scanline.  This includes the
     * active edge table, and the minimum and maximum values of x for each
     * active edge in this iteration.
     */
 
    #ifdef __USE_GCC_ALLOCA_
    unsigned *active = (unsigned *) alloca(len * sizeof(unsigned));
    int *xmin = (int *) alloca(len * sizeof(int));
    int *xmax = (int *) alloca(len * sizeof(int));
    #else
    unsigned *active = (unsigned *) malloc(len * sizeof(unsigned));
    int *xmin = (int *) malloc(len * sizeof(int));
    int *xmax = (int *) malloc(len * sizeof(int));
    #endif

    int y;
    for (y = miny; y <= maxy; y++)
    {
        int activect = 0;
        for (i = 0; i < edgect; i++)
        {
            if ((vy[edges[i].top] <= y) && (vy[edges[i].bot] >= y))
            {
                active[activect++] = i;
            }
        }

        /*
         * Two things happen at once here.  The x and d values of each active
         * edge are updated to their values for the NEXT scanline, and the
         * opposite extreme for this scanline is noted in the process.
         */
        for (i = 0; i < activect; i++)
        {
            struct edge *edge = edges + active[i];
            xmin[i] = xmax[i] = edge->x;

            while ((edge->d >= -edge->dincx) && (edge->x != vx[edge->bot]))
            {
                edge->x += edge->xinc;
                edge->d += edge->dincx;

                if (edge->x > xmax[i]) xmax[i] = edge->x;
                if (edge->x < xmin[i]) xmin[i] = edge->x;
            }

            if (edge->d >= 0)
            {
                edge->x += edge->xinc;
                edge->d += edge->dincx;
            }

            edge->d += edge->dincy;
        }

        /*
         * Selection sort to order edges by their initial x values.
         */
        for (i = 0; i < activect; i++)
        {
            for (j = i + 1; j < activect; j++)
            {
                if (xmin[i] > xmin[j])
                {
                    unsigned utemp;
                    int stemp;

                    utemp = active[i];
                    active[i] = active[j];
                    active[j] = utemp;

                    stemp = xmin[i];
                    xmin[i] = xmin[j];
                    xmin[j] = stemp;

                    stemp = xmax[i];
                    xmax[i] = xmax[j];
                    xmax[j] = stemp;
                }
            }
        }

        /*
         * The task is now to cross the screen and draw the relevant
         * pixels.  A bit of state is necessary, so sx is the starting
         * point for the next drawing operation, and pen is a boolean flag
         * indicating whether to fill the coming pixels or not.
         */
        int sx = 0;
        int pen = 0;
        int x = 0;

        for (i = 0; i < activect; i++)
        {
            if (fill)
            {
                if (pen)
                {
                    /*
                     * Were drawing, and hit an edge.  Fill the line to this
                     * point.
                     */
                    x = max(x, xmax[i]);
                    if (sx <= x) _hpg_draw(g, sx, y, x - sx + 1, minx, miny);
                    sx = max(sx, x + 1);
                }
                else
                {
                    /*
                     * Not drawing and hit an edge; bump sx up to this
                     * point, and x to the end of this intersection.
                     */
                    sx = max(sx, xmin[i]);
                    x = max(x, xmax[i]);
                }

                if (y == vy[edges[active[i]].bot])
                {
                    int edge = active[i];

                    int oe = edge + edges[edge].dir;
                    if (oe == -1) oe = edgect - 1;
                    if (oe == edgect) oe = 0;

                    if (vy[edges[oe].bot] > y)
                    {
                        /*
                         * This vertex crosses the scan line, so one edge needs
                         * to be ignored.  Draw only the vertex pixels
                         * themselves.
                         */
                        x = max(x, xmax[i]);
                        if (x >= sx)
                        {
                            _hpg_draw(g, sx, y, x - sx + 1, minx, miny);
                        }
                        continue;
                    }
                }

                pen = !pen;
            }
            else
            {
                sx = max(sx, xmin[i]);
                x = max(x, xmax[i]);
                if (x >= sx) _hpg_draw(g, sx, y, x - sx + 1, -1, -1);
                sx = x + 1;
            }
        }
    }
    
    #ifndef __USE_GCC_ALLOCA_
    free(edges);
    free(active);
    free(xmin);
    free(xmax);
    #endif
}
