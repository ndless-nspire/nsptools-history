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

// initially from hpsys.h
#define MODE_MONO 0
#define MODE_4GRAY 1
#define MODE_16GRAY 2

#define DEFAULT_PLANE ((char *) __display_buf)

#define SCR_WIDTH 131
#define SCR_HEIGHT 80

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif /* min */
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif /* max */

struct hpg_graphics
{
    char *org_fb1,*org_fb2;
    volatile char *fb;
    volatile char *drawfb;
    void (*drawfn)(hpg_t *g, int x, int y, int len, int patx, int paty);
    unsigned char (*getfn)(hpg_t *g, int x, int y);
    unsigned int width, height;
    unsigned int fbwidth;
    unsigned int clipx1, clipy1, clipx2, clipy2;
    unsigned char mode;
    unsigned char color;
    hpg_pattern_t *pattern;
    hpg_font_t *font;
};

struct hpg_font
{
    char *buffer;
    unsigned int count;
    unsigned char height;
    unsigned char advance;
};

struct hpg_pattern
{
    char *buffer;
    unsigned char height;
    unsigned char fixed;
};

void _hpg_set_mode(int mode,char *visfb, char *drawfb,
    void (*drawfn)(hpg_t *, int, int, int, int, int),
    unsigned char (*getfn)(hpg_t *, int, int));

void _hpg_mono_draw(hpg_t *g, int x, int y, int len, int patx, int paty);
void _hpg_gray4_draw(hpg_t *g, int x, int y, int len, int patx, int paty);
void _hpg_gray16_draw(hpg_t *g, int x, int y, int len, int patx, int paty);
unsigned char _hpg_mono_get(hpg_t *g, int x, int y);
unsigned char _hpg_gray4_get(hpg_t *g, int x, int y);
unsigned char _hpg_gray16_get(hpg_t *g, int x, int y);
void _hpg_draw(hpg_t *g, int x, int y, int len, int patx, int paty);
void _hpg_poly(hpg_t *g, int vx[], int vy[], int len, int fill);

struct hpg_xpm_info
{
    int width, height;
    int ncolors;
    int cpp;
};

struct hpg_xpm_info _hpg_get_xpm_info(char *xpm[]);
void _hpg_load_xpm(struct hpg_xpm_info info, hpg_t *img, char *xpm[]);
