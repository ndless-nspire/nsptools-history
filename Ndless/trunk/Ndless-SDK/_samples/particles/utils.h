/****************************************************************************
 * @(#) Ndless - Utils
 *
 * Copyright (C) 2010 by ANNEHEIM Geoffrey and ARMAND Olivier
 * Contact: geoffrey.anneheim@gmail.com / olivier.calc@gmail.com
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * RCSID $Id$
 ****************************************************************************/

#ifndef _UTILS_H_
#define _UTILS_H_

typedef struct {
  int x, y, w, h;
} t_rect;

typedef struct {
  float x, y;
} t_point;

typedef enum {false, true} bool;

extern inline bool rect_intersect(const t_rect* r1, const t_rect* r2);
extern inline void clearScreen();
extern inline int getPixel(int x, int y);
extern inline void setPixel(int x, int y, int color);
extern float sqrtf(float num);
extern void fade(void *scrbuf, int n);

#endif
