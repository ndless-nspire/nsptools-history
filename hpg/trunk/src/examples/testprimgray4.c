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

/*
 * Exercise all test cases for the gray4 drawing primitives, with various
 * horizontal lines.
 */
int main(void)
{


    hpg_set_mode_gray4(0);
    hpg_clear();

    hpg_set_indicator(HPG_INDICATOR_LSHIFT, HPG_COLOR_BLACK);
    hpg_set_indicator(HPG_INDICATOR_ALPHA, HPG_COLOR_GRAY_8);
    hpg_set_indicator(HPG_INDICATOR_WAIT, HPG_COLOR_BLACK);
    keyb_waitKeyPressed();

    hpg_set_pattern(hpg_stdscreen, NULL);

    /*
     * Test black paint mode with no pattern.
     */
    hpg_set_color(hpg_stdscreen, HPG_COLOR_BLACK);
    hpg_set_mode(hpg_stdscreen, HPG_MODE_PAINT);
    hpg_fill_rect(0, 0, 7,  0);
    hpg_fill_rect(0, 1, 3,  1);
    hpg_fill_rect(3, 2, 7,  2);
    hpg_fill_rect(3, 3, 5,  3);
    hpg_fill_rect(3, 4, 5,  4);
    hpg_fill_rect(0, 5, 15, 5);
    hpg_fill_rect(3, 6, 15, 6);
    hpg_fill_rect(0, 7, 12, 7);
    hpg_fill_rect(3, 8, 12, 8);
    hpg_fill_rect(3, 9, 18, 9);
    keyb_waitKeyPressed();

    /*
     * Test white paint mode with no pattern.
     */
    hpg_fill_rect(0, 10, 23, 19);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_WHITE);
    hpg_set_mode(hpg_stdscreen, HPG_MODE_PAINT);
    hpg_fill_rect(0, 10, 7,  10);
    hpg_fill_rect(0, 11, 3,  11);
    hpg_fill_rect(3, 12, 7,  12);
    hpg_fill_rect(3, 13, 5,  13);
    hpg_fill_rect(3, 14, 5,  14);
    hpg_fill_rect(0, 15, 15, 15);
    hpg_fill_rect(3, 16, 15, 16);
    hpg_fill_rect(0, 17, 12, 17);
    hpg_fill_rect(3, 18, 12, 18);
    hpg_fill_rect(3, 19, 18, 19);
    keyb_waitKeyPressed();

    /*
     * Test black xor mode with no pattern.
     */
    hpg_set_color(hpg_stdscreen, HPG_COLOR_BLACK);
    hpg_fill_rect(0, 30, 23, 39);
    hpg_set_mode(hpg_stdscreen, HPG_MODE_XOR);
    hpg_fill_rect(0, 20, 7,  20);
    hpg_fill_rect(0, 21, 3,  21);
    hpg_fill_rect(3, 22, 7,  22);
    hpg_fill_rect(3, 23, 5,  23);
    hpg_fill_rect(3, 24, 5,  24);
    hpg_fill_rect(0, 25, 15, 25);
    hpg_fill_rect(3, 26, 15, 26);
    hpg_fill_rect(0, 27, 12, 27);
    hpg_fill_rect(3, 28, 12, 28);
    hpg_fill_rect(3, 29, 18, 29);
    hpg_fill_rect(0, 30, 7,  30);
    hpg_fill_rect(0, 31, 3,  31);
    hpg_fill_rect(3, 32, 7,  32);
    hpg_fill_rect(3, 33, 5,  33);
    hpg_fill_rect(3, 34, 5,  34);
    hpg_fill_rect(0, 35, 15, 35);
    hpg_fill_rect(3, 36, 15, 36);
    hpg_fill_rect(0, 37, 12, 37);
    hpg_fill_rect(3, 38, 12, 38);
    hpg_fill_rect(3, 39, 18, 39);
    keyb_waitKeyPressed();

    /*
     * Set a simple pattern.
     */
    char patdata = 0xAA;
    hpg_pattern_t *pat = hpg_alloc_pattern(&patdata, 1, 1);
    hpg_set_pattern(hpg_stdscreen, pat);

    /*
     * Test black paint mode with pattern.
     */
    hpg_set_color(hpg_stdscreen, HPG_COLOR_BLACK);
    hpg_set_mode(hpg_stdscreen, HPG_MODE_PAINT);
    hpg_fill_rect(0, 40, 7,  40);
    hpg_fill_rect(0, 41, 3,  41);
    hpg_fill_rect(3, 42, 7,  42);
    hpg_fill_rect(3, 43, 5,  43);
    hpg_fill_rect(3, 44, 5,  44);
    hpg_fill_rect(0, 45, 15, 45);
    hpg_fill_rect(3, 46, 15, 46);
    hpg_fill_rect(0, 47, 12, 47);
    hpg_fill_rect(3, 48, 12, 48);
    hpg_fill_rect(3, 49, 18, 49);
    keyb_waitKeyPressed();

    /*
     * Test white paint mode with pattern.
     */
    hpg_fill_rect(0, 50, 23, 59);
    hpg_set_color(hpg_stdscreen, HPG_COLOR_WHITE);
    hpg_set_mode(hpg_stdscreen, HPG_MODE_PAINT);
    hpg_fill_rect(0, 50, 7,  50);
    hpg_fill_rect(0, 51, 3,  51);
    hpg_fill_rect(3, 52, 7,  52);
    hpg_fill_rect(3, 53, 5,  53);
    hpg_fill_rect(3, 54, 5,  54);
    hpg_fill_rect(0, 55, 15, 55);
    hpg_fill_rect(3, 56, 15, 56);
    hpg_fill_rect(0, 57, 12, 57);
    hpg_fill_rect(3, 58, 12, 58);
    hpg_fill_rect(3, 59, 18, 59);
    keyb_waitKeyPressed();

    /*
     * Test black xor mode with pattern.
     */
    hpg_set_color(hpg_stdscreen, HPG_COLOR_BLACK);
    hpg_fill_rect(0, 70, 23, 79);
    hpg_set_mode(hpg_stdscreen, HPG_MODE_XOR);
    hpg_fill_rect(0, 60, 7,  60);
    hpg_fill_rect(0, 61, 3,  61);
    hpg_fill_rect(3, 62, 7,  62);
    hpg_fill_rect(3, 63, 5,  63);
    hpg_fill_rect(3, 64, 5,  64);
    hpg_fill_rect(0, 65, 15, 65);
    hpg_fill_rect(3, 66, 15, 66);
    hpg_fill_rect(0, 67, 12, 67);
    hpg_fill_rect(3, 68, 12, 68);
    hpg_fill_rect(3, 69, 18, 69);
    hpg_fill_rect(0, 70, 7,  70);
    hpg_fill_rect(0, 71, 3,  71);
    hpg_fill_rect(3, 72, 7,  72);
    hpg_fill_rect(3, 73, 5,  73);
    hpg_fill_rect(3, 74, 5,  74);
    hpg_fill_rect(0, 75, 15, 75);
    hpg_fill_rect(3, 76, 15, 76);
    hpg_fill_rect(0, 77, 12, 77);
    hpg_fill_rect(3, 78, 12, 78);
    hpg_fill_rect(3, 79, 18, 79);

    keyb_waitKeyPressed();

    hpg_free_pattern(pat);



    return 0;
}
