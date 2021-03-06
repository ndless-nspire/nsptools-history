//---------------------------------------------------------------//
//                                                               //
//                            CORE.CPP                           //
//                                                               //
//                              Source code by Marko             //
//                              http://www.gamedesign-online.com //
//---------------------------------------------------------------//

#include "core.h"

// CORE class constructor: initializes SDL, the video mode, window
// title, and enables key repetition
core::core()
{
    // Initialize SDL 
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
        printf ("Couldn't initialize SDL : %s\n", SDL_GetError ());
        exit (1);
    }
    
    atexit (SDL_Quit);

    // Set 800x600 32-bits video mode
    screen = SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT, has_colors ? 16 : 8, SDL_SWSURFACE);
    if (screen == NULL)
    {
        printf ("Couldn't set video mode : %s\n", SDL_GetError ());
        exit (2);
    }
    
    
    // Set the title of our application window handler
    SDL_WM_SetCaption ("Marko's SDL Framework", NULL);
    
    
    // We activate keyboard repetition. Therefore, when a key stays pressed down
    // it will keep moving the image around the screen
    // To see prcisely what this toggle does, just comment the line and recompile
    // the code...
    SDL_EnableKeyRepeat(450, 300);
    
    // Instanciate our graphics controller and load all the image data
    GFXController = new graphics(screen);
    GFXController -> loadImageData();
	GFXController -> loadFonts();
        
    tetris = new game(GFXController);
    gameOver = false;
    gamePaused = false;
	escPressedTime = 0;
    Timer = 0;
    DropTimer = 0;
    
    tmpBuff = new char[64];
}


core::~core()
{
    delete GFXController;
    delete tmpBuff;
    SDL_FreeSurface( screen );
    SDL_Quit();
}


// The gameLoop function is the heart of the game engine
int core::gameLoop()
{
     // #1 - Handle asynchronous events in queue 
     //      scan for new keypresses or window framework actions
     //      This is mainly for "single" event keys and not the 
     //      game keypresses
     while( SDL_PollEvent( &event ) )
     {
            SDL_PumpEvents();
            keys = SDL_GetKeyState(NULL);

            if ( event.type == SDL_QUIT)
                return 1;
			if (keys[SDLK_ESCAPE]) {
				if (escPressedTime) return 1;
				else escPressedTime = SDL_GetTicks();
			}
				
            if ( !gameOver && !gamePaused)
            {
                 if ( keys[SDLK_UP] || keys[SDLK_8] ) { tetris->rotate(); }
                 if ( keys[SDLK_BACKSPACE] ) {  tetris->drop(); }
            }
            if ( keys[SDLK_RETURN] && !gamePaused ) {  tetris->reset(); gameOver = false; }
            if ( keys[SDLK_p] ) {  gamePaused = !gamePaused; }
     }
     
	 // if escape not pressed 2 times in the same second, do not exit
     if ( (SDL_GetTicks()-escPressedTime) > 1000 )
		 escPressedTime = 0;
	 
	 
     // #2 - Handle our game key presses. Since we want our game to run at the same speed
     //      whatever the computer it is tested on, we need to have all our actions, movements,
     //      events, object changes only within this if ( elapsed time > T_FRAME_INTERVAL ) loop.
     //      Also, SDL_EnableKeyRepeat didn't give satisfactory results on multiple keypresses
     //      so we scan all the keyboard for keys that are still pressed in and that wouldn't have been
     //      collected by the SDL_PollEvent function 
     if ( (SDL_GetTicks()-Timer) > T_FRAME_INTERVAL && !gamePaused )
     {
          // - 1 - Update our timer !
              Timer = SDL_GetTicks();

    
// - 2 - handle keys that are kept down  
          //       this will also have to be adapted to your game
            keys = SDL_GetKeyState(NULL);
            
            if ( !gameOver )
            {
                  if ( keys[SDLK_DOWN] || keys[SDLK_2] ) {  tetris->moveDown(); }
                  if ( keys[SDLK_LEFT] || keys[SDLK_4] ) {  tetris->moveLeft(); }
                  if ( keys[SDLK_RIGHT] || keys[SDLK_6] ) {  tetris->moveRight(); }
                  tetris -> updateMoveBoard();
            }
            
     }   
     
     // #3 - Every time our "drop" timer expires, we make the current tetris brick move down...
     //      This is also where all the collision testing is done :)
     if ( (SDL_GetTicks()-DropTimer) > tetris->getDropInterval() && !gamePaused && !gameOver )
     {
              DropTimer = SDL_GetTicks();
              if (tetris->commitMovements())
                 gameOver = true;
     }
     
     // #4 - Blit the surfaces to video buffer then flip ( only if time conditions are correct )
     renderGraphics();
     
     if( SDL_Flip( screen ) == -1 )
                 return 2;
     
     SDL_Delay(35);
     
     return 0;
}


// This function is called by the gameLoop function.
// It handles all the graphics rendering. 
// In this example, we render our metal steel bitmap 
// to cover the screen and then draw the ball above
void core::renderGraphics()
{
	GFXController->drawBackground();

     // Render the GAME LEVEL 
     sprintf(tmpBuff, "Level:  %d", tetris->getGameLevel());
     GFXController->renderText(0, tmpBuff, 230, 15 );
	
     
     // Render the LINE COUNT 
     sprintf(tmpBuff, "Lines:  %d", tetris->getLineCount());
     GFXController->renderText(0, tmpBuff, 230, 15+12 );
	
	 GFXController->renderText(1, "Backspace: Drop", 220, 210 );
	 GFXController->renderText(1, "P: Pause", 220, 210+10 );
	
     // Render the next brick 
     tetris -> renderNextBrickFrame();

     // Render the game board
     tetris -> renderBoardData();
     
     // If the game is over, display the GAME OVER text
     if ( gameOver )
     {
          GFXController->renderText(2, "GAME OVER", 230, 150 );
          GFXController->renderText(3, "Hit Enter", 230, 150+10 );
          GFXController->renderText(3, "to restart", 230, 150+20 );
     }
     
     // If the game is paused, display the GAME PAUSED text
     else if ( gamePaused )
     {
          GFXController->renderText(2, "GAME PAUSED",  225, 150 );
          GFXController->renderText(3, "Press P", 235, 150+10 );
          GFXController->renderText(3, "to resume", 235, 150+20 );
     }
     
     
}
