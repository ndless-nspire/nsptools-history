//---------------------------------------------------------------//
//                                                               //
//                            GRAPHICS.H                         //
//                                                               //
//                              Source code by Marko             //
//                              http://www.gamedesign-online.com //
//---------------------------------------------------------------//

#ifndef GRAPHICS_H
#define GRAPHICS_H

// The aim of this file is to provide an image framework, and 
// make the image handling much easier for the developer.

// Indeed, the graphics class lets you load up to IMAGE_COUNT
// (defined in config.h) images and display them using their 
// unique ID

#include "config.h"


class graphics
{
      public:
             graphics(SDL_Surface *_screen);
             ~graphics();
             
             void DrawPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
             void DrawLine(int x1, int y1, int x2, int y2, Uint32 color);
             void DrawLine(int x1, int y1, int x2, int y2, int R, int G, int B);
             
             void getPixelRGB(int x, int y, Uint8 *r, Uint8 *g, Uint8 *b);
             void getPixelRGB(int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, SDL_Surface* surf);
             
             void drawBackground(void);
	  
             bool blitImage(int id, int pos_x, int pos_y);
             bool blitImage(SDL_Surface *surf, int pos_x, int pos_y);
             bool blitClipImage(int id, int x, int y, int w, int h, int x2, int y2);
             
             void clearScreen() {SDL_FillRect(screen,NULL,0x000000);}   // Clear the entire screen with black
             void clearScreen(Uint32 colour) {SDL_FillRect(screen,NULL,colour);}   // Clear the entire screen with black
             void colourFillRect(int id, SDL_Rect *r, Uint32 colour) {SDL_FillRect(surfaces[id],r,colour);}   // Clear the entire screen with black
             void colourFillScreenRect(SDL_Rect *r, Uint32 colour) {SDL_FillRect(screen,r,colour);}   // Clear the entire screen with black
             
             bool renderText(int font_id, const char* text, int x, int y);
             
             SDL_Surface* getScreen() {return screen;}
             
             void loadImageData();
			 void loadFonts();
			 void freeFonts();
             
      private:
              // Pointer to our screen SDL_Surface
              SDL_Surface *screen;
              // Array of SDL_surfaces used to store images
              SDL_Surface *surfaces[IMAGE_COUNT];
	  
              // Pointer to a temporary surface used to render fonts on              
              SDL_Surface *font_surf;
			  
			  void loadImage(unsigned nb, unsigned short *img);
			  
			  static unsigned short image_cube_green[];
			  static unsigned short image_cube_green2[];
			  static unsigned short image_cube_purple[];
			  static unsigned short image_cube_red[];
			  static unsigned short image_cube_yellow[];
			  static unsigned short image_cube_blue[];
	  
			  nSDL_Font *fonts[4];
};

#endif