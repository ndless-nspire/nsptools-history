//---------------------------------------------------------------//
//                                                               //
//                            CORE.H                             //
//                                                               //
//                              Source code by Marko             //
//                              http://www.gamedesign-online.com //
//---------------------------------------------------------------//

#include "config.h"
#include "entities.h"

// The CORE class is the center part of our game. It contains
// the game loop, a pointer to the GRAPHICS handler class, the
// performance timer (to control FPS), and the event handling
// is done within the gameLoop function of this class...

class core
{
      public:
             core();
             ~core();
             
             int gameLoop();
             void renderGraphics();
             
      private:
              SDL_Surface *screen;
              
              graphics *GFXController;
              
              Uint32 Timer;
              Uint32 DropTimer;
              
              SDL_Event event;
              Uint8* keys;
              
              game *tetris;
              bool gameOver;
              bool gamePaused;
              
              char* tmpBuff; // temp string to dump data in before using GFX->rendertext function
              
              

};
