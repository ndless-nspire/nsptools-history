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

extern void *alloca(int size);

/*
 * Convert an RGB color value to grayscale.  The constants here are taken from
 * an approximation of ITU Recommendation 709.  They represent the degree of
 * sensors in our eyes for each color, so there is no purely mathematical
 * "correct" way to do this.
 */
static unsigned char rgb_to_gray(
    unsigned char r, unsigned char g, unsigned char b)
{
    return (unsigned char) ((227 * r + 724 * g + 73 * b) >> 10);
}

/*
 * Returns a color value if possible, or -1 if not.
 */
static int get_color(char *s)
{
    if (*s == '#')
    {
        /* RGB color */
        int r = 0, g = 0, b = 0;

        s++;
        if (*s >= 'a') r = (16 * r) + (*s - 'a' + 10);
        else if (*s >= 'A') r = (16 * r) + (*s - 'A' + 10);
        else if (*s >= '0') r = (16 * r) + (*s - '0');
        s++;
        if (*s >= 'a') r = (16 * r) + (*s - 'a' + 10);
        else if (*s >= 'A') r = (16 * r) + (*s - 'A' + 10);
        else if (*s >= '0') r = (16 * r) + (*s - '0');
        s++;
        if (*s >= 'a') g = (16 * g) + (*s - 'a' + 10);
        else if (*s >= 'A') g = (16 * g) + (*s - 'A' + 10);
        else if (*s >= '0') g = (16 * g) + (*s - '0');
        s++;
        if (*s >= 'a') g = (16 * g) + (*s - 'a' + 10);
        else if (*s >= 'A') g = (16 * g) + (*s - 'A' + 10);
        else if (*s >= '0') g = (16 * g) + (*s - '0');
        s++;
        if (*s >= 'a') b = (16 * b) + (*s - 'a' + 10);
        else if (*s >= 'A') b = (16 * b) + (*s - 'A' + 10);
        else if (*s >= '0') b = (16 * b) + (*s - '0');
        s++;
        if (*s >= 'a') b = (16 * b) + (*s - 'a' + 10);
        else if (*s >= 'A') b = (16 * b) + (*s - 'A' + 10);
        else if (*s >= '0') b = (16 * b) + (*s - '0');

        /*
         * Invert the color, since 0xFF is black on the HP calculator screen
         * and this is backwards from common image formats.
         */
        return 0xFF - rgb_to_gray(r, g, b);
    }
    else
    {
        /* No known form of color (HSV is currently unimplemented). */
        return -1;
    }
}

struct hpg_xpm_info _hpg_get_xpm_info(char *xpm[])
{
    struct hpg_xpm_info info;

    /*
     * Read size and format information from the header.
     */
    char *s = xpm[0];

    while (*s == ' ') s++;

    info.width = 0;
    while ((*s != ' ') && (*s != '\0'))
    {
        int digit = *(s++) - '0';
        info.width = (10 * info.width) + digit;
    }

    while (*s == ' ') s++;

    info.height = 0;
    while ((*s != ' ') && (*s != '\0'))
    {
        int digit = *(s++) - '0';
        info.height = (10 * info.height) + digit;
    }

    while (*s == ' ') s++;

    info.ncolors = 0;
    while ((*s != ' ') && (*s != '\0'))
    {
        int digit = *(s++) - '0';
        info.ncolors = (10 * info.ncolors) + digit;
    }

    while (*s == ' ') s++;

    info.cpp = 0;
    while ((*s != ' ') && (*s != '\0'))
    {
        int digit = *(s++) - '0';
        info.cpp = (10 * info.cpp) + digit;
    }

    return info;
}

void _hpg_load_xpm(struct hpg_xpm_info info, hpg_t *img, char *xpm[])
{
    /*
     * The color translation table is allocated on the stack, and used to
     * translate the character patterns from within the XPM file into color
     * data.
     */
    struct color
    {
        char *code;
        unsigned char color;
    };

    struct color *ctable = (struct color *)
        alloca(info.ncolors * sizeof(struct color));

    int i;
    for (i = 0; i < info.ncolors; i++)
    {
        char *s = xpm[1 + i];
        ctable[i].code = alloca((info.cpp + 1) * sizeof(char));
        strncpy(ctable[i].code, s, info.cpp);
        ctable[i].code[info.cpp] = '\0';

        s += info.cpp;
        while (*s == ' ') s++;

        /*
         * Pixmaps may contain color data in several different schemes.
         * We prefer them in the following order, from least to most
         * acceptable:
         *
         * none - If no acceptable color is found, the pixel is white
         * c    - Color data (converted to grayscale on the fly)
         * g    - Grayscale at greater than 4 levels
         * g4   - 4-level grayscale data
         *
         * The 'colorlevel' flag below tracks the best we have so far.
         * Future colors are only used if they are better than what's already
         * found.  Color keys not in the list above are useless, and are not
         * parsed.
         */
        unsigned char color = 0;
        char colorlevel = 0;

        while (*s != '\0')
        {
            /* Read a color key */
            char ckey[3];
            int j = 0;
            while ((*s != ' ') && (*s != '\0')) ckey[j++] = *(s++);
            ckey[j] = '\0';

            char newlevel = -1;
            if (strcmp(ckey, "c") == 0) newlevel = 1;
            else if (strcmp(ckey, "g") == 0) newlevel = 2;
            else if (strcmp(ckey, "g4") == 0) newlevel = 3;

            if (newlevel > colorlevel)
            {
                /* Read a color */
                while (*s == ' ') s++;
                int newcolor = get_color(s);
                if (newcolor != -1)
                {
                    color = (unsigned char) newcolor;
                    colorlevel = newlevel;
                }
            }

            /* Skip the color name */
            while ((*s != ' ') && (*s != '\0')) s++;

            while (*s == ' ') s++;
        }

        ctable[i].color = color;
    }

    i++; // i is now an index to the current line of the pixmap source
    int y;
    for (y = 0; y < info.height; y++, i++)
    {
        int x;
        for (x = 0; x < info.width; x++)
        {
            /* Find the appropriate color for this pixel */
            int c = 0;
            while (c < info.ncolors)
            {
                if (strncmp(xpm[i] + (x * info.cpp),
                    ctable[c].code, info.cpp) == 0)
                {
                    hpg_set_color(img, ctable[c].color);
                    hpg_draw_pixel_on(img, x, y);

                    /*
                     * The following serves to move commonly used colors to
                     * earlier spots in the color table, hence making it
                     * faster to load high color-depth images that reuse the
                     * same color frequently.
                     */
                    if (c > 0)
                    {
                        int nc = c >> 1;
                        struct color temp;

                        temp = ctable[nc];
                        ctable[nc] = ctable[c];
                        ctable[c] = temp;
                    }

                    break;
                }

                c++;
            }
        }
    }
}
