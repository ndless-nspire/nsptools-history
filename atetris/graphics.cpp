//---------------------------------------------------------------//
//                                                               //
//                            GRAPHICS.CPP                       //
//                                                               //
//                              Source code by Marko             //
//                              http://www.gamedesign-online.com //
//---------------------------------------------------------------//

#include "graphics.h"


// GRAPHICS class constructor. Is copies the screen pointer defined 
// in the core class, so as to have direct access to that surface 
// and render images on it
graphics::graphics(SDL_Surface *_screen)
{
    screen = _screen;
    
    for (int i=0; i<IMAGE_COUNT; i++)
        surfaces[i] = NULL;
}

// GRAPHICS destructor : for each allocated image, we perform a 
// "clean" release :)
graphics::~graphics()
{
    for (int i=0; i<IMAGE_COUNT; i++)
	   if( surfaces[i] ) // Is it pointing to anything?
		   SDL_FreeSurface( surfaces[i] );
}

// A function used to draw a pixel on a surface. The surface is 
// automatically defined as the screen in this function. You can
// always change this if you need to write a pixel on another
// surface.
//
// The function handles all 4 pixel formats (8, 16, 24 and 32 bits)
// to cover all possible video modes. Feel free to clean this up
// and only keep the video mode you wish if you have a lot of
// pixels to render via this function... it will slightly improve
// your rendering speed
void graphics::DrawPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  Uint32 color = SDL_MapRGB(screen->format, R, G, B);
  switch (screen->format->BytesPerPixel)
  {
    case 1: // Assuming 8-bpp
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
        *bufp = color;
      }
      break;
    case 2: // Probably 15-bpp or 16-bpp
      {
        Uint16 *bufp;
        bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
        *bufp = color;
      }
      break;
    case 3: // Slow 24-bpp mode, usually not used
      {
        Uint8 *bufp;
        bufp = (Uint8 *)screen->pixels + y*screen->pitch + x * 3;
        if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
        {
          bufp[0] = color;
          bufp[1] = color >> 8;
          bufp[2] = color >> 16;
        } else {
          bufp[2] = color;
          bufp[1] = color >> 8;
          bufp[0] = color >> 16;
        }
      }
      break;
    case 4: // Probably 32-bpp
      {
        Uint32 *bufp;
        bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
        *bufp = color;
      }
      break;
  }
}

// A prototype line drawing function I found on Internet. Very handy,
// it has covered all of my needs in line rendering up until today
void graphics::DrawLine(int x1, int y1, int x2, int y2, Uint32 color)
{
  int d;
  int x;
  int y;
  int ax;
  int ay;
  int sx;
  int sy;
  int dx;
  int dy;

  Uint8 *lineAddr;
  Sint32 yOffset;

  dx = x2 - x1;  
  ax = abs(dx) << 1;  
  sx=0;
  if (dx<0)
     sx=-1;
  if (dx>0)
     sx=1;

  dy = y2 - y1;  
  ay = abs(dy) << 1;  
  sy=0;
  if (dy<0)
     sy=-1;
  if (dy>0)
     sy=1;

  yOffset = sy * screen->pitch;

  x = x1;
  y = y1;

  lineAddr = ((Uint8 *)(screen->pixels)) + (y * screen->pitch);
  if (ax>ay)
  {                      /* x dominant */
    d = ay - (ax >> 1);
    for (;;)
    {
      *((Uint32 *)(lineAddr + (x << 2))) = (Uint32)color;

      if (x == x2)
      {
        return;
      }
      if (d>=0)
      {
        y += sy;
        lineAddr += yOffset;
        d -= ax;
      }
      x += sx;
      d += ay;
    }
  }
  else
  {                      /* y dominant */
    d = ax - (ay >> 1);
    for (;;)
    {
      *((Uint32 *)(lineAddr + (x << 2))) = (Uint32)color;

      if (y == y2)
      {
        return;
      }
      if (d>=0) 
      {
        x += sx;
        d -= ay;
      }
      y += sy;
      lineAddr += yOffset;
      d += ax;
    }
  }
}


// This Drawline function calls the line rendering function above but takes
// R,G,B values instead of a Uint32 colour. It's more "user friendly" since
// it's exremely hard to give a specific colour as 1 32 bit integer
void graphics::DrawLine(int x1, int y1, int x2, int y2, int R, int G, int B)
{
     Uint32 colour = SDL_MapRGB(screen->format, R, G, B);
     DrawLine(x1, y1, x2, y2, colour);
}


// A "pixel-reading" function. You specify the X,Y coordinates of the
// pixel you're willing to read and it will fill the R,G,B pointers
// you passed in parameters
void graphics::getPixelRGB(int x, int y, Uint8 *r, Uint8 *g, Uint8 *b)
{
        Uint32 *bufp;
        bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
        SDL_GetRGB(*bufp, screen->format, r, g, b);
     
}

// Same "pixel-reading" function except it dosen't read a pixel from 
// the screen but from another SDL_surface
void graphics::getPixelRGB(int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, SDL_Surface* surf)
{
        Uint32 *bufp;
        bufp = (Uint32 *)surf->pixels + y*surf->pitch/4 + x;
        SDL_GetRGB(*bufp, surf->format, r, g, b);
     
}

// Image drawing function. This simply writes the SDL surface 
// specified in the ID at the X,Y coordinates
bool graphics::blitImage(int id, int pos_x, int pos_y)
{
     if (id <IMAGE_COUNT && id >-1)
     {
              SDL_Rect dest;
              dest.x = pos_x;
              dest.y = pos_y;
              SDL_BlitSurface(surfaces[id], NULL, screen, &dest);
              return true;
     }
     return false;
}


bool graphics::blitImage(SDL_Surface *surf, int pos_x, int pos_y)
{
     if ( surf )
     {
              SDL_Rect dest;
              dest.x = pos_x;
              dest.y = pos_y;
              SDL_BlitSurface(surf, NULL, screen, &dest);
              return true;
     }
     return false;
}

// Advanced version of the function above. It allows the user to
// write a "section" of a surface on screen. This is basically how
// the majority of 2D animations are handled... You have 1 surface
// containing all the animation frames and you render a section
// of that surface at each frame
bool graphics::blitClipImage(int id, int x, int y, int w, int h, int x2, int y2)
{
     if (id <IMAGE_COUNT && id >-1)
     { 
          // Source surface
          SDL_Rect dest2;
          dest2.x = x2;
          dest2.y = y2;
          dest2.w = w;
          dest2.h = h;
          
          // Destination surface
          SDL_Rect dest;
          dest.x = x;
          dest.y = y;
          dest.w = w;
          dest.h = h;
          
          // Let's hope I didn't screw this up or I'll be called a mytho again :)
          SDL_BlitSurface(surfaces[id], &dest2, screen, &dest);
          return true;
     }
     return false;
}

unsigned short graphics::image_cube_green[] = {
    0x2a01,0x000F,0x000F,0x0000,0x0000,0x0000,0x0000,0x0200,0x02a0,0x0260,
    0x0240,0x0240,0x0240,0x0240,0x0240,0x0220,0x02c0,0x0280,0x0140,0x0000,
    0x0000,0x0000,0x04a0,0x05a0,0x0540,0x0500,0x0500,0x0500,0x0500,0x0500,
    0x04e0,0x05a0,0x04e0,0x0280,0x0000,0x00a0,0x0200,0x5eab,0x676c,0x672c,
    0x66ec,0x66ec,0x66ec,0x66ec,0x66ec,0x66ec,0x2f05,0x05a0,0x02c0,0x0000,
    0x01c0,0x0580,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
    0xffff,0x76ee,0x04e0,0x0220,0x0000,0x01a0,0x0520,0xf7fe,0xffff,0xffff,
    0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0x6f0d,0x0500,0x0240,0x0000,
    0x0180,0x04c0,0xe7dc,0xf7fe,0xf7fe,0xf7de,0xf7de,0xf7de,0xefbd,0xf7de,
    0xf7de,0x66ec,0x0500,0x0240,0x0000,0x01a0,0x04e0,0xeffd,0xffff,0xffff,
    0xffff,0xffff,0xffff,0xf7de,0xffff,0xffff,0x66ec,0x0500,0x0240,0x0000,
    0x01a0,0x04e0,0xe7fc,0xf7fe,0xf7fe,0xf7fe,0xf7fe,0xf7fe,0xefdd,0xf7fe,
    0xf7fe,0x66ec,0x0500,0x0240,0x0000,0x01a0,0x0500,0xf7fe,0xffff,0xffff,
    0xffff,0xffff,0xffff,0xffdf,0xffff,0xffff,0x6eed,0x0500,0x0240,0x0000,
    0x01a0,0x04e0,0xcff9,0xe7fc,0xe7fc,0xdffb,0xdffb,0xdffb,0xdffb,0xe7fc,
    0xe7fc,0x5f0b,0x0500,0x0240,0x0000,0x0180,0x04c0,0x57ea,0x57ea,0x57ea,
    0x57ea,0x57ea,0x57ea,0x57ea,0x57ea,0x57ea,0x26e4,0x04e0,0x0220,0x0000,
    0x01a0,0x0520,0x07c0,0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x07c0,0x07e0,
    0x07e0,0x0700,0x0520,0x0260,0x0000,0x01c0,0x04c0,0x0520,0x04c0,0x04e0,
    0x0500,0x0500,0x0500,0x0500,0x04e0,0x04c0,0x05a0,0x04e0,0x0280,0x0000,
    0x00a0,0x01c0,0x01a0,0x0180,0x01a0,0x01a0,0x01a0,0x01a0,0x01a0,0x01a0,
    0x0180,0x0200,0x01c0,0x00e0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

unsigned short graphics::image_cube_green2[] = {
    0x2a01,0x000F,0x000F,0x0000,0x0000,0x41c8,0x9452,0x01a0,0x00e0,0x0120,
    0x01a0,0x01c0,0x01c0,0x01c0,0x01c0,0x01a0,0x0200,0x01c0,0x00e0,0x0000,
    0x6b2d,0xff9f,0x1402,0x02a0,0x0340,0x0380,0x0380,0x0380,0x0380,0x0380,
    0x0360,0x03e0,0x0360,0x01c0,0x0000,0x4aa9,0xb6d6,0x6e0d,0x6d4d,0x7e0f,
    0x14a2,0x0400,0x0420,0x0440,0x0440,0x0420,0x04a0,0x03e0,0x0200,0x0000,
    0x0080,0x02a0,0xffff,0xffff,0xd75a,0x2444,0x03a0,0x03c0,0x03e0,0x03e0,
    0x03e0,0x0420,0x0360,0x01a0,0x0000,0x00c0,0x0320,0xffff,0xe79c,0x4d49,
    0x03e0,0x0c01,0x0400,0x0400,0x0400,0x03e0,0x0440,0x0380,0x01c0,0x0000,
    0x0100,0x0b81,0xaed5,0x7dcf,0x03e0,0x03c0,0x0420,0x0400,0x0400,0x0400,
    0x03e0,0x0440,0x0380,0x01c0,0x0000,0x0140,0x0380,0x03a0,0x03a0,0x0400,
    0x0400,0x0400,0x0400,0x0400,0x0400,0x03e0,0x0440,0x0380,0x01c0,0x0000,
    0x0140,0x0380,0x03e0,0x03c0,0x0400,0x0400,0x0400,0x0400,0x0400,0x0400,
    0x03e0,0x0440,0x0380,0x01c0,0x0000,0x0140,0x0380,0x0420,0x0400,0x0400,
    0x0400,0x0400,0x0400,0x0400,0x0400,0x03e0,0x0440,0x0380,0x01c0,0x0000,
    0x0140,0x0380,0x0420,0x03e0,0x0400,0x0400,0x0400,0x0400,0x0400,0x0400,
    0x03e0,0x0440,0x0380,0x01c0,0x0000,0x0120,0x0360,0x0400,0x03e0,0x03e0,
    0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x0420,0x0360,0x01a0,0x0000,
    0x0140,0x03a0,0x0420,0x0400,0x0420,0x0420,0x0420,0x0420,0x0400,0x0420,
    0x0400,0x0460,0x03a0,0x01c0,0x0000,0x0140,0x0360,0x03a0,0x0360,0x0380,
    0x0380,0x0380,0x0380,0x0380,0x0380,0x0360,0x03e0,0x0360,0x01a0,0x0000,
    0x0060,0x0140,0x0140,0x0120,0x0140,0x0140,0x0140,0x0140,0x0140,0x0140,
    0x0120,0x0160,0x0140,0x00a0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

unsigned short graphics::image_cube_purple[] = {
    0x2a01,0x000F,0x000F,0x0000,0x0000,0x0000,0x0000,0x2805,0x3807,0x3006,
    0x3006,0x3006,0x3006,0x3006,0x3006,0x2805,0x4008,0x3807,0x2004,0x0000,
    0x0000,0x0000,0x680d,0x8010,0x780f,0x700e,0x780f,0x780f,0x700e,0x700e,
    0x700e,0x8010,0x780f,0x3807,0x0000,0x0801,0x2805,0xb2f6,0xcb39,0xc338,
    0xc338,0xc338,0xc338,0xbb17,0xc338,0xbb37,0xb156,0x8811,0x4008,0x0000,
    0x2004,0x8010,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
    0xffff,0xc398,0x700e,0x2805,0x0000,0x2004,0x780f,0xffbf,0xffff,0xffff,
    0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xc358,0x700e,0x3006,0x0000,
    0x2004,0x700e,0xf73e,0xffbf,0xffbf,0xffbf,0xffbf,0xffbf,0xf77e,0xffbf,
    0xffbf,0xbb17,0x780f,0x3006,0x0000,0x2004,0x700e,0xf75e,0xffff,0xffff,
    0xffdf,0xffdf,0xffdf,0xff9f,0xffff,0xffff,0xc338,0x780f,0x3006,0x0000,
    0x2004,0x700e,0xf73e,0xffbf,0xffbf,0xffbf,0xffbf,0xffbf,0xff7f,0xffbf,
    0xffbf,0xc338,0x780f,0x3006,0x0000,0x2004,0x700e,0xf7be,0xffff,0xffff,
    0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xc378,0x780f,0x3006,0x0000,
    0x2004,0x700e,0xfe7f,0xff1f,0xff1f,0xfeff,0xfeff,0xfeff,0xfedf,0xff1f,
    0xff1f,0xc2d8,0x700e,0x3006,0x0000,0x2004,0x700e,0xfa9f,0xfabf,0xfabf,
    0xfabf,0xfabf,0xfabf,0xfabf,0xfabf,0xfabf,0xc118,0x700e,0x2805,0x0000,
    0x2004,0x780f,0xf01e,0xf81f,0xf81f,0xf81f,0xf81f,0xf81f,0xf01e,0xf81f,
    0xf81f,0xc018,0x780f,0x3006,0x0000,0x2805,0x700e,0x780f,0x700e,0x700e,
    0x700e,0x700e,0x700e,0x700e,0x700e,0x700e,0x8010,0x700e,0x3807,0x0000,
    0x1002,0x2805,0x2024,0x2024,0x2024,0x2024,0x2024,0x2024,0x2024,0x2024,
    0x2024,0x2805,0x2805,0x1002,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

unsigned short graphics::image_cube_red[] = {
    0x2a01,0x000F,0x000F,0x0000,0x0000,0x3a08,0x84b2,0x3800,0x2000,0x2800,
    0x3000,0x3000,0x3000,0x3000,0x3000,0x2800,0x4000,0x3800,0x2000,0x0000,
    0x636d,0xf7df,0x8882,0x6000,0x7000,0x7800,0x7800,0x7800,0x7000,0x7000,
    0x7000,0x8000,0x7800,0x3800,0x0000,0x5269,0xdd96,0xcb4d,0xb34d,0xbbcf,
    0xc0a2,0xc000,0xc000,0xb800,0xc000,0xb800,0xb000,0x8800,0x4000,0x0000,
    0x1800,0x5800,0xffff,0xffff,0xfeba,0xf904,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xc000,0x7000,0x2800,0x0000,0x2000,0x7000,0xffff,0xff3c,0xfa69,
    0xf820,0xf841,0xf800,0xf800,0xf800,0xf800,0xc000,0x7000,0x3000,0x0000,
    0x2000,0x7841,0xf575,0xfbef,0xf800,0xf800,0xf820,0xf800,0xf000,0xf800,
    0xf800,0xb800,0x7800,0x3000,0x0000,0x2000,0x7000,0xf000,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xc000,0x7800,0x3000,0x0000,
    0x2000,0x7000,0xf000,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xc000,0x7800,0x3000,0x0000,0x2000,0x7000,0xf020,0xf820,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xc000,0x7800,0x3000,0x0000,
    0x2000,0x7000,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,
    0xf800,0xc000,0x7000,0x3000,0x0000,0x2000,0x7000,0xf800,0xf800,0xf800,
    0xf800,0xf800,0xf800,0xf800,0xf800,0xf800,0xc000,0x7000,0x2800,0x0000,
    0x2000,0x7800,0xf000,0xf800,0xf800,0xf800,0xf800,0xf800,0xf000,0xf800,
    0xf800,0xc000,0x7800,0x3000,0x0000,0x2800,0x7000,0x7800,0x7000,0x7000,
    0x7000,0x7000,0x7000,0x7000,0x7000,0x7000,0x8000,0x7000,0x3800,0x0000,
    0x1000,0x2800,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,0x2000,
    0x2000,0x2800,0x2800,0x1000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

unsigned short graphics::image_cube_yellow[] = {
    0x2a01,0x000F,0x000F,0x0000,0x0000,0x29a8,0x7c12,0x7a60,0x79e0,0x7a20,
    0x7a20,0x7a20,0x7a20,0x7a20,0x7a20,0x7a20,0x7aa0,0x7a60,0x3940,0x0000,
    0x52ed,0xef9f,0xf542,0xec40,0xf4c0,0xf4e0,0xf4e0,0xf4e0,0xf4e0,0xf4c0,
    0xeca0,0xf560,0xecc0,0x7a60,0x0000,0x62c9,0xff76,0xff2d,0xfe6d,0xfecf,
    0xfec2,0xfec0,0xfec0,0xfec0,0xfec0,0xfea0,0xfec0,0xfd80,0x8aa0,0x0000,
    0x5140,0xec40,0xf7ff,0xefff,0xf7fa,0xf7e4,0xf7e0,0xf7e0,0xf7e0,0xf7e0,
    0xefe0,0xf6c0,0xeca0,0x7a00,0x0000,0x5980,0xf4a0,0xffff,0xf7fc,0xffe9,
    0xffe0,0xffe1,0xffe0,0xffe0,0xffe0,0xf7e0,0xfee0,0xf4c0,0x7a20,0x0000,
    0x59a0,0xf4e1,0xffd5,0xf7cf,0xffc0,0xffc0,0xffc0,0xffc0,0xffa0,0xffc0,
    0xf7c0,0xfec0,0xf4c0,0x7a20,0x0000,0x5980,0xf4c0,0xffc0,0xf7e0,0xffe0,
    0xffe0,0xffe0,0xffe0,0xffc0,0xffe0,0xf7e0,0xfec0,0xf4c0,0x7a20,0x0000,
    0x5980,0xf4c0,0xffc0,0xf7e0,0xffe0,0xffe0,0xffe0,0xffe0,0xffc0,0xffe0,
    0xf7e0,0xfec0,0xf4c0,0x7a20,0x0000,0x5980,0xf4c0,0xffc0,0xf7e0,0xffe0,
    0xffe0,0xffe0,0xffe0,0xffc0,0xffe0,0xf7e0,0xfec0,0xf4e0,0x7a20,0x0000,
    0x5980,0xf4c0,0xffe0,0xf7e0,0xffe0,0xffe0,0xffe0,0xffe0,0xffe0,0xffe0,
    0xf7e0,0xfee0,0xf4c0,0x7a20,0x0000,0x5180,0xeca0,0xf7e0,0xefe0,0xf7e0,
    0xf7e0,0xf7e0,0xf7e0,0xf7e0,0xf7e0,0xefe0,0xf6c0,0xeca0,0x7a00,0x0000,
    0x59a0,0xf4e0,0xffc0,0xffe0,0xffe0,0xffc0,0xffc0,0xffc0,0xffc0,0xffe0,
    0xffe0,0xfee0,0xfd00,0x7a40,0x0000,0x51a0,0xe4a0,0xf4e0,0xeca0,0xf4c0,
    0xf4c0,0xf4c0,0xf4c0,0xf4c0,0xf4c0,0xeca0,0xf560,0xecc0,0x7260,0x0000,
    0x18a0,0x51a0,0x59a0,0x5180,0x5980,0x5980,0x5980,0x5980,0x5980,0x5980,
    0x5180,0x59e0,0x51c0,0x28e0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

unsigned short graphics::image_cube_blue[] = {
    0x2a01,0x000F,0x000F,0x0000,0x0000,0x41c8,0x9452,0x01a0,0x00e0,0x0120,
    0x01a0,0x01c0,0x01c0,0x01c0,0x01c0,0x01a0,0x0200,0x01c0,0x00e0,0x0000,
    0x6b2d,0xffff,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x01c0,0x0000,0x4aa9,0xffff,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x001f,0x0200,0x0000,
    0x0080,0x001f,0xffff,0xffff,0xffff,0x041f,0x041f,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x001f,0x01a0,0x0000,0x00c0,0x001f,0xffff,0xe79c,0x041f,
    0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x001f,0x01c0,0x0000,
    0x0100,0x001f,0xffff,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x001f,0x01c0,0x0000,0x0140,0x001f,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x001f,0x01c0,0x0000,
    0x0140,0x001f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x001f,0x01c0,0x0000,0x0140,0x001f,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x001f,0x01c0,0x0000,
    0x0140,0x001f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x001f,0x01c0,0x0000,0x0120,0x001f,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x001f,0x01a0,0x0000,
    0x0140,0x001f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,0x041f,
    0x041f,0x041f,0x001f,0x01c0,0x0000,0x0140,0x001f,0x001f,0x001f,0x001f,
    0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x001f,0x01a0,0x0000,
    0x0060,0x0140,0x0140,0x0120,0x0140,0x0140,0x0140,0x0140,0x0140,0x0140,
    0x0120,0x0160,0x0140,0x00a0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

void graphics::loadImage(unsigned nb, unsigned short  *img) {
	SDL_Surface* temp = nSDL_LoadImage(img);
	surfaces[nb] = SDL_DisplayFormat(temp);
	SDL_SetColorKey(surfaces[nb],SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(screen->format, 0, 0, 0));
	surfaces[nb] = SDL_DisplayFormatAlpha(surfaces[nb]);
	SDL_FreeSurface(temp);
}

void graphics::loadImageData()
{
	loadImage(0, image_cube_green);
	loadImage(1, image_cube_green2);
	loadImage(2, image_cube_purple);
	loadImage(3, image_cube_red);
	loadImage(4, image_cube_yellow);
	loadImage(5, image_cube_blue);
}

void graphics::drawBackground(void)
{
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
}

// Font rendering function. The font_id param is used to specify which font you want
// to render on the screen. The next params are the text you want to print out
// and its position on screen...
bool graphics::renderText(int font_id, const char* text, int R, int G, int B, int x, int y)
{
	nSDL_Font *font = nSDL_LoadFont(NSDL_FONT_TINYTYPE, R, G, B);
	int result = nSDL_DrawString(screen, font, x, y, text);
	nSDL_FreeFont(font);
	return result;
}
