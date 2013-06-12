//---------------------------------------------------------------//
//                                                               //
//                            MAIN.CPP                           //
//                                                               //
//                              Source code by Marko             //
//                              http://www.gamedesign-online.com //
//---------------------------------------------------------------//

// The game CORE class encapsulates all the multimedia data
// (SDL Surfaces, and image loading function, event handling... etc)
// meaning we can inlcude that to handle the entire game :)
#include "core.h"


int main(void)
{
	//assert_ndless_rev(804);
	
    // instanciate a CORE object    
    core *gameCore = new core();
    
    // Make it perform the game loop until a quit event is detected
    while ( gameCore->gameLoop() == 0 );
    
    // Cleanup before exiting
    delete gameCore;
    
    return 0;
}
