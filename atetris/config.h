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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

    // SDL Video mode information
    const int SCREEN_WIDTH = 600;
    const int SCREEN_HEIGHT = 600;
    const int SCREEN_BPP = 32;
    
    // SDL Image structure information. Define the maximum number of images the game can load
    const int IMAGE_COUNT = 128;
    
    // SDL TTF Font structure information. Defines the maximum number of fonts loaded
    const int FONT_COUNT = 8;
    
    // SDL Audio information
    const int AUDIO_SOUND_COUNT  = 32;
    const int AUDIO_SAMPLE_FRQ = 22050;// Sampling frequency of audio 
    const int AUDIO_CHUNK_SIZE  = 4096;// This value determines the size of the memory chunks 
                                       // used for storage and playback of samples. A value of
                                       // 4096 should be suitable for most games. Increasing 
                                       // the value will decrease the CPU load but will also 
                                       // decrease responsiveness of playback. If you find the 
                                       // mixer's play/stop response times to be too slow, you 
                                       // may wish to decrease this value.
    
    
    // Timer definitions. This forces the game to wait at least T_FRAME_INTERVAL milliseconds
    // between each frame. It is the only way to get the game to NOT run faster on a faster
    // computer... Indeed, without this test, the game loop would run as fast as the computer
    // processors allows it
    const int T_FRAME_INTERVAL = 45;     // time interval (ms) used for FPS rendering
    
    const int T_DROP_INTERVAL = 500;     // time interval (ms) used for FPS rendering
    


#endif
