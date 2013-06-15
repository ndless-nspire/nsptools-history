//---------------------------------------------------------------//
//                                                               //
//                            CONFIG.H                           //
//                                                               //
//                              Source code by Marko             //
//                              http://www.gamedesign-online.com //
//---------------------------------------------------------------//

// The main aim of this file is to gather all the game settings
// in a single file and therefore make it easily to change 
// settings and tweak the game to meet your expectations

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <os.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
   
    // SDL Image structure information. Define the maximum number of images the game can load
    const int IMAGE_COUNT = 15;
    
    // Timer definitions. This forces the game to wait at least T_FRAME_INTERVAL milliseconds
    // between each frame. It is the only way to get the game to NOT run faster on a faster
    // computer... Indeed, without this test, the game loop would run as fast as the computer
    // processors allows it
    const int T_FRAME_INTERVAL = 45;     // time interval (ms) used for FPS rendering
    
    const int T_DROP_INTERVAL = 500;     // time interval (ms) used for FPS rendering
    


#endif
