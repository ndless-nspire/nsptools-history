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

int main(void)
{

    hpg_t *image1 = hpg_alloc_mono_image(40, 40);
    hpg_t *image2 = hpg_alloc_gray4_image(40, 40);
    hpg_t *image3 = hpg_alloc_gray16_image(40, 40);

    hpg_clear();
    hpg_clear_on(image1);
    hpg_clear_on(image2);
    hpg_clear_on(image3);

    hpg_draw_circle_on(image1, 20, 20, 10);
    hpg_draw_circle_on(image2, 20, 20, 10);
    hpg_draw_circle_on(image3, 20, 20, 10);

    hpg_blit(image1, 0, 0, 40, 40, hpg_stdscreen, 0, 0);
    hpg_blit(image2, 0, 0, 40, 40, hpg_stdscreen, 40, 0);
    hpg_blit(image3, 0, 0, 40, 40, hpg_stdscreen, 0, 40);

    keyb_waitKeyPressed();


    hpg_free_image(image1);
    hpg_free_image(image2);
    hpg_free_image(image3);

    return 0;
}
