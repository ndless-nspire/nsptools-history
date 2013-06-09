#include "graphics.h"

#define GAMEBOARD_X 10
#define GAMEBOARD_Y 14
#define GAME_SHAPES 7
#define GAME_IMAGES 6
#define LINES_PER_LEVEL 5        // Defines after how many lines the level increases :)
#define SPEED_INCREASE_RATIO 0.9  // Defines how much the speed is decreased every time the level increases


// Tetris shapes are stored in this structure.
//        data is a 4x4x4 array to identify 4 shapes (1 shape & its 3 rotations)
//        in addition to a 4x4 matrix to define the shape itself
//        finally, the image ID holds the texture ID used by this shape
struct shape
{
       int data[4][4][4];
};


class game
{
      public:
             game(graphics *GFXController);
             ~game();
             
             bool commitMovements(); // This is what makes the bricks "fall" with time...
             void moveLeft();
             void moveRight();
             bool moveDown();
             void rotate();
             void drop();  // When space bar is pressed, it makes the brisk fall as low as possible
             
             void initShapes(); // Creates all our brick shapes
             void resetShape(int id); // Function used to initialise a shape and all it's rotations
             void reset(); // Used mainly after game over or simply to reset the game :)
             
             void renderBoardData(); // Draw the tetris board on screen
             void renderNextBrickFrame();
             
             void updateMoveBoard(); // Function used to refresh the movement board depending on the shape data, block_x and block_y
             
             inline int getDropInterval() { return dropInterval;}
             inline int getGameLevel() { return GameLevel;}
             inline int getLineCount() { return lineCount;}
             
      private:
              // Array to compute the current status of the game board
              int gameBoard[GAMEBOARD_X][GAMEBOARD_Y];
              // Array to compute the current position of the moving brick
              int moveBoard[GAMEBOARD_X][GAMEBOARD_Y];
              
              int getRandom();
              int getRandomImgID();
              
              int dropInterval;
              
              // The functions below get the min / max offset of shapes to determine 
              // when they collide with the world boudaries or with other shapes
              int getMinXonShape(int _sid);
              int getMaxXonShape(int _sid);
              int getMinYonShape(int _sid);
              int getMaxYonShape(int _sid);
              
              bool lineDump(); // Function to clean full lines out
              
              shape *gameShapes[GAME_SHAPES];
              
              int block_status; // Defines the rotation "angle" of the shape
              int block_x; // Shape X position 
              int block_y; // Shape Y position 
              int shape_id; // Shape ID => defines which shape is being used
              
              int nextShapeID;
              int nextColourID;
              int curColourID;
              
              int GameLevel;
              int lineCount;
              
              graphics *GFX;
};
