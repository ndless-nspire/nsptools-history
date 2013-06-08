/*
  Name: USE-Ultra-Simple-Engine
  Author: Snoolas and the tutorials at the GPWiki.
  Description: An extremely simple C++ codebase for 2D game programming with SDL.
  Features simple image loading, blitting, text rendering, random number generation, etc.
  Date: May/June, 2005
  License: Public Domain. You should have received this file with a file called LICENSE.txt.
  See it for a complete license.
*/

/*}{}{}}{}{}{}{}{}{}{}{}{THIS USE.H FILE IS MODIFIED FOR BLOCKBREAK{}{}{}{}{}{}{}{}{}{}{}{}}{*/

/********************************PREPROCESSOR INSTRUCTIONS *********************************/
#include <OS.h>
#include <cstdlib>    
/*For atexit() to deinitialize the engine at finish and rand() for random number generation as well 
as exit() to terminate execution if something goes wrong.*/

#include <ctime> 
/*For time(0) to seed the random number generator.*/

#include <string>
#include <sstream>
/*For any string functions that we may need.*/

#include <cmath>
/*For floor() that we use in making levels*/

#include "SDL/SDL.h"
//#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
/*SDL Includes. If your include paths are different, change these accordingly.*/
 
//#define SCREEN_WIDTH 640     
//#define SCREEN_HEIGHT 480 
#define COLOR_DEPTH (is_cx ? 16 : 8)
#define POINTSIZE 20
/*Defines that make it easy for us to change the screen properties without even changing the functions.*/

#define COLORKEY 255, 0, 255 
/*The colorkey that will not be included in sprites when they are blitted onto the screen.*/ 
 
using namespace std;
/*Make sure we use good clean up to date ANSI compliant C++!*/
 
/*******************************DECLARATIONS*************************************************/
enum textquality {solid=1, shaded, blended};

void initUSE();
void drawimage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Surface *dstimg, int dx, int dy, int alpha);
SDL_Surface* loadimage(const char *file);
int random(int lowest_number, int highest_number);

SDL_Surface* screen;
/******************************CODE***********************************************************/
void initUSE()
{
  /*The following if clauses call the SDL and SDL_ttf initialization routines and if an error value is returned,
  an error message is printed and a the exit() function is called to terminate the program. */
  
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {   
    printf("Unable to initialize SDL: %s \n", SDL_GetError());
    exit(1);
  }
 
#if 0  
  if (TTF_Init() == -1)
  {
    printf("Unable to initialize SDL_ttf:%s \n", TTF_GetError());
    exit(1);
  }  
#endif
	
  SDL_WM_SetCaption("BlockBreak", "BlockBreak");
  /*Try to set our window caption. This is known not to work fully in AMD64 Linux with
  Xorg.*/
	
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DEPTH, SDL_SWSURFACE); //| SDL_FULLSCREEN);
  if (screen == NULL) {
    printf("Unable to set video mode: %s \n", SDL_GetError());
    exit(1);
  }
  /*Try to set up the main screen surface with the proper flags and properties. If it fails, use the same error 
  reporting method as before.*/
  
  SDL_ShowCursor(SDL_DISABLE);
  /*Make sure that we don't have the cursor sitting in the middle of the game. If your game happens to need the 
  mouse for control, remove this line.*/
  
  srand(static_cast<unsigned>(time(0)));
  /*Seed the random number generator so that it doesn't come up with the same values every time we run the game.*/
}

SDL_Surface* loadimage(const char *file)    
{
  /*This function takes a path to an image file and returns a pointer to the loaded SDL_Surface to be saved into 
  a variable.*/
  SDL_Surface *tmp;                        
  tmp = IMG_Load(file);            

  if (tmp == NULL) {                   
    printf("Error: File could not be opened: %s %s", file, SDL_GetError());
    exit(1);
  } 
  else {                               
    if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(tmp->format, COLORKEY)) == -1)
    printf("Warning: Colorkey will not be used, reason: %s \n", SDL_GetError());
    /*If there are no errors loading the image into tmp, set the color key of the surface, so that our chosen 
    transparent color will be transparent.*/
  }
  return tmp;
}
  
void drawimage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Surface *dstimg, int dx, int dy, int alpha = 255) {
  if ((!srcimg) || (alpha == 0)) return; 
  /*If there's no image, or its 100% transparent.*/
  
  SDL_Rect src, dst;                     
  /*The two rectangles are filled with the information passed to the function.*/   
  src.x = sx;  src.y = sy;  src.w = sw;  src.h = sh;
  dst.x = dx;  dst.y = dy;  dst.w = src.w;  dst.h = src.h;
  
  if (alpha != 255) SDL_SetAlpha(srcimg, SDL_SRCALPHA, alpha); 
  /*Make SDL aware of the desired level of Alpha transparency in the source image.*/
  
  SDL_BlitSurface(srcimg, &src, dstimg, &dst);      
  /*Finally Blit the source on to the destination surface.*/
}

/*This function returns a pseudo-random number between the parameters.*/
int random(int lowest_number, int highest_number)      
{
  int range = highest_number - lowest_number + 1;
  int temp;
  do {
    temp = (rand() % range) + lowest_number;
  } while (temp == 0);
  return temp;
}
