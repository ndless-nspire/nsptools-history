#include <ctime>
#include "entities.h"

game::game(graphics *GFXController)
{
     srand((unsigned)time(0)); 
     
     for (int i=0; i<GAMEBOARD_X; i++)
         for (int j=0; j<GAMEBOARD_Y; j++)
         {
             gameBoard[i][j] = -1;
             moveBoard[i][j] = -1;
         }
         
     for  ( int i=0; i<GAME_SHAPES; i++ )
          gameShapes[i] = new shape();
     
     initShapes();

     block_status = 0;
     block_x = INIT_BLOCK_X;
     block_y = 0;
     
     GameLevel = 0;
     lineCount = 0;

     shape_id    = getRandom();
     nextShapeID = getRandom();

     nextColourID = getRandomImgID();
     curColourID  = getRandomImgID();
     
     dropInterval = T_DROP_INTERVAL;
     
     updateMoveBoard();
     
     GFX = GFXController;
}

game::~game()
{
             
}

void game::renderBoardData()
{
    for ( int i=0; i<GAMEBOARD_X; i++ )
        for ( int j=0; j<GAMEBOARD_Y; j++ )
            if ( gameBoard[i][j] != -1 )
               GFX->blitImage( gameBoard[i][j], BLOCK_WIDTH_PX*(i+1)+BOARD_X_OFFSET_PX, BLOCK_WIDTH_PX*(j+1)+BOARD_Y_OFFSET_PX );
            else if ( moveBoard[i][j] != -1 )
               GFX->blitImage( moveBoard[i][j], BLOCK_WIDTH_PX*(i+1)+BOARD_X_OFFSET_PX, BLOCK_WIDTH_PX*(j+1)+BOARD_Y_OFFSET_PX );
                

}

void game::renderNextBrickFrame()
{
     for (int i=0; i<4; i++)
         for (int j=0; j<4; j++)
             if ( gameShapes[nextShapeID]->data[0][i][j] == 1)
                GFX->blitImage( nextColourID, NEXT_SHAPE_X_OFFSET_PX+(BLOCK_WIDTH_PX*i), NEXT_SHAPE_Y_OFFSET_PX+BLOCK_WIDTH_PX*j );
}
 

int game::getRandom()
{
    return (rand()%GAME_SHAPES);
}

int game::getRandomImgID()
{
    return (rand()%GAME_IMAGES);
}


int game::getMinXonShape(int _sid)
{
    int val = 3;
    for (int i = 3; i>=0; i--)
         for (int j= 0; j<4; j++)
             if ( gameShapes[_sid]->data[block_status][i][j] == 1 && (val < i) )
                  val = i;
    return val;
}

int game::getMaxXonShape(int _sid)
{
    int val = 0;
    for (int i=0; i<4; i++)
         for (int j=0; j<4; j++)
             if ( gameShapes[_sid]->data[block_status][i][j] == 1 && (val < i) )
                  val = i;
    return val;
}

int game::getMinYonShape(int _sid)
{
    int val = 3;
    for (int i=0; i<4; i++)
         for (int j=3; j>=0; j--)
             if ( gameShapes[_sid]->data[block_status][i][j] == 1 && (val > j) )
                  val = j;
    
    return val;
}

int game::getMaxYonShape(int _sid)
{
    int val = 0;
    for (int i=0; i<4; i++)
         for (int j=0; j<4; j++)
             if ( gameShapes[_sid]->data[block_status][i][j] == 1 && (val < j) )
                  val = j;
    return val;
}

bool game::lineDump()
{
     
     for (int j=GAMEBOARD_Y-1; j>=0; j--)
     {
         int blocksOnLine = 0;
         
         for (int i=0; i<GAMEBOARD_X; i++)
             if ( gameBoard[i][j] != -1 )
                blocksOnLine++;

             // If we counted as many full cells as the game board width, the line is full
             if ( blocksOnLine == GAMEBOARD_X )
             {
                  for (int k=j; k>0; k--)
                      for (int n=0; n<GAMEBOARD_X; n++)
                          gameBoard[n][k] = gameBoard[n][k-1];
                  
                  for (int n=0; n<GAMEBOARD_X; n++)
                      gameBoard[n][0] = -1;
                      
                  lineCount ++;
                  
                  if ( (lineCount - (GameLevel*LINES_PER_LEVEL)) > LINES_PER_LEVEL)
                  {
                     GameLevel++;
                     dropInterval = (int)( (double)dropInterval * SPEED_INCREASE_RATIO );
                  }
                      
                  return true;
             }
     }
     
     return false;
}
              
bool game::commitMovements()
{
     // If we can't move down, we stop the block and respawn
     if ( ! moveDown())
     {
          for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
                 if ( gameShapes[shape_id]->data[block_status][i][j] != 0 )
                    gameBoard[block_x+i][block_y+j] = curColourID;
                    
          while (lineDump());
          
          if (block_y == 0)
             return true;
             
          block_status = 0;
          block_x = INIT_BLOCK_X;
          block_y = 0;
          
          shape_id    = nextShapeID;
          nextShapeID = getRandom();
          
          curColourID  = nextColourID;
          nextColourID = getRandomImgID();
          
          
     }
     return false;
     
}


 void game::updateMoveBoard()
{    
     // Recalculate Moveboard data for rendering
     for (int i = 0; i<GAMEBOARD_X; i++)
             for (int j = 0; j<GAMEBOARD_Y; j++)
                 moveBoard[i][j] = -1;

     for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
                 if (gameShapes[shape_id]->data[block_status][i][j] != 0)
                    moveBoard[block_x+i][block_y+j] = curColourID;
}

void game::moveLeft()
{
     bool canMove = true;
     int limit = getMinXonShape(shape_id);
     
     if ( block_x + limit > 0)
     {
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             {
                 if ( gameShapes[shape_id]->data[block_status][i][j] == 1)
                 {
                      if ( gameBoard[block_x+i-1][block_y+j] != -1 )
                         canMove = false;
                 }
             }
     }
     else canMove = false;
     
     if ( canMove )
        block_x --;
}

void game::moveRight()
{
     bool canMove = true;
     int limit = getMaxXonShape(shape_id);
     
     if ( (block_x+limit) < (GAMEBOARD_X-1) )
     {
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             {
                 if ( gameShapes[shape_id]->data[block_status][i][j] == 1)
                 {
                      if ( gameBoard[block_x+i+1][block_y+j] != -1 )
                         canMove = false;
                 }
             }
     }
     else canMove = false;
     
     if ( canMove )
        block_x ++;
}

bool game::moveDown()
{
     bool canMove = true;
     int limit = getMaxYonShape(shape_id);
     
     if ( (block_y+limit) < (GAMEBOARD_Y-1) )
     {
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             {
                 if ( gameShapes[shape_id]->data[block_status][i][j] == 1)
                 {
                      if ( block_y+j < GAMEBOARD_Y ) 
                         if ( gameBoard[block_x+i][block_y+j+1] != -1 )
                             canMove = false;
                 }
             }
     }
     else canMove = false;
     if ( canMove )
        block_y ++;
        
     return canMove;
}

void game::rotate()
{
     bool rotateOK = true;
     
     int nextShape = block_status - 1;
     if (nextShape < 0)
        nextShape = 3;
        
     // We check for every brick of the rotate shape that it will fit in ok
     for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             {
                 if ( gameShapes[shape_id]->data[nextShape][i][j] == 1)
                 {
                      if ( (block_x+i < GAMEBOARD_X) && (block_x+i >= 0) )
                      {
                         if ( gameBoard[block_x+i+1][block_y+j] != -1 )
                            rotateOK = false;
                      }
                      else rotateOK = false;
                 }
             }
     if ( (block_x+getMaxXonShape(nextShape) >= GAMEBOARD_X) && (block_x+getMaxXonShape(nextShape) <= 0) )
        rotateOK = false;
        
     if (rotateOK)
     {
        if (block_status > -1)
           block_status--;
        if (block_status == -1)
           block_status = 3;
     }
}


void game::drop()
{
     while (moveDown());
}



void game::initShapes()
{
     //       0
     //      000 
     resetShape(0);
     gameShapes[0]->data[0][1][0] = 1;
     gameShapes[0]->data[0][0][1] = 1;
     gameShapes[0]->data[0][1][1] = 1;
     gameShapes[0]->data[0][2][1] = 1;
     
     gameShapes[0]->data[1][0][1] = 1;
     gameShapes[0]->data[1][1][0] = 1;
     gameShapes[0]->data[1][1][1] = 1;
     gameShapes[0]->data[1][1][2] = 1;
     
     gameShapes[0]->data[2][1][2] = 1;
     gameShapes[0]->data[2][0][1] = 1;
     gameShapes[0]->data[2][1][1] = 1;
     gameShapes[0]->data[2][2][1] = 1;
     
     gameShapes[0]->data[3][2][1] = 1;
     gameShapes[0]->data[3][1][0] = 1;
     gameShapes[0]->data[3][1][1] = 1;
     gameShapes[0]->data[3][1][2] = 1;
     
     //       00
     //       00 
     resetShape(1);
     gameShapes[1]->data[0][0][0] = 1;
     gameShapes[1]->data[0][0][1] = 1;
     gameShapes[1]->data[0][1][0] = 1;
     gameShapes[1]->data[0][1][1] = 1;
     gameShapes[1]->data[1][0][0] = 1;
     gameShapes[1]->data[1][0][1] = 1;
     gameShapes[1]->data[1][1][0] = 1;
     gameShapes[1]->data[1][1][1] = 1;
     gameShapes[1]->data[2][0][0] = 1;
     gameShapes[1]->data[2][0][1] = 1;
     gameShapes[1]->data[2][1][0] = 1;
     gameShapes[1]->data[2][1][1] = 1;
     gameShapes[1]->data[3][0][0] = 1;
     gameShapes[1]->data[3][0][1] = 1;
     gameShapes[1]->data[3][1][0] = 1;
     gameShapes[1]->data[3][1][1] = 1;
     
     //       0000
     resetShape(2);
     gameShapes[2]->data[0][1][0] = 1;
     gameShapes[2]->data[0][1][1] = 1;
     gameShapes[2]->data[0][1][2] = 1;
     gameShapes[2]->data[0][1][3] = 1;
     gameShapes[2]->data[1][0][1] = 1;
     gameShapes[2]->data[1][1][1] = 1;
     gameShapes[2]->data[1][2][1] = 1;
     gameShapes[2]->data[1][3][1] = 1;
     gameShapes[2]->data[2][1][0] = 1;
     gameShapes[2]->data[2][1][1] = 1;
     gameShapes[2]->data[2][1][2] = 1;
     gameShapes[2]->data[2][1][3] = 1;
     gameShapes[2]->data[3][0][1] = 1;
     gameShapes[2]->data[3][1][1] = 1;
     gameShapes[2]->data[3][2][1] = 1;
     gameShapes[2]->data[3][3][1] = 1;
     
     //       00
     //        00
     resetShape(3);
     gameShapes[3]->data[0][1][0] = 1;
     gameShapes[3]->data[0][1][1] = 1;
     gameShapes[3]->data[0][0][1] = 1;
     gameShapes[3]->data[0][0][2] = 1;
     gameShapes[3]->data[3][0][0] = 1;
     gameShapes[3]->data[3][1][0] = 1;
     gameShapes[3]->data[3][1][1] = 1;
     gameShapes[3]->data[3][2][1] = 1;
     gameShapes[3]->data[2][2][0] = 1;
     gameShapes[3]->data[2][2][1] = 1;
     gameShapes[3]->data[2][1][1] = 1;
     gameShapes[3]->data[2][1][2] = 1;
     gameShapes[3]->data[1][2][0] = 1;
     gameShapes[3]->data[1][1][0] = 1;
     gameShapes[3]->data[1][1][1] = 1;
     gameShapes[3]->data[1][0][1] = 1;
     
     //       000
     //         0
     resetShape(4);
     gameShapes[4]->data[0][0][1] = 1;
     gameShapes[4]->data[0][1][1] = 1;
     gameShapes[4]->data[0][2][1] = 1;
     gameShapes[4]->data[0][2][2] = 1;
     
     gameShapes[4]->data[1][1][0] = 1;
     gameShapes[4]->data[1][1][1] = 1;
     gameShapes[4]->data[1][1][2] = 1;
     gameShapes[4]->data[1][2][0] = 1;
     
     gameShapes[4]->data[2][0][1] = 1;
     gameShapes[4]->data[2][1][1] = 1;
     gameShapes[4]->data[2][2][1] = 1;
     gameShapes[4]->data[2][0][0] = 1;
     
     gameShapes[4]->data[3][1][0] = 1;
     gameShapes[4]->data[3][1][1] = 1;
     gameShapes[4]->data[3][1][2] = 1;
     gameShapes[4]->data[3][0][2] = 1;
     
     
     //         00
     //        00
     resetShape(5);
     gameShapes[5]->data[0][2][0] = 1;
     gameShapes[5]->data[0][1][0] = 1;
     gameShapes[5]->data[0][1][1] = 1;
     gameShapes[5]->data[0][0][1] = 1;
     gameShapes[5]->data[1][1][2] = 1;
     gameShapes[5]->data[1][1][1] = 1;
     gameShapes[5]->data[1][0][1] = 1;
     gameShapes[5]->data[1][0][0] = 1;
     gameShapes[5]->data[2][2][1] = 1;
     gameShapes[5]->data[2][1][1] = 1;
     gameShapes[5]->data[2][0][0] = 1;
     gameShapes[5]->data[2][1][0] = 1;
     gameShapes[5]->data[3][1][2] = 1;
     gameShapes[5]->data[3][1][1] = 1;
     gameShapes[5]->data[3][0][0] = 1;
     gameShapes[5]->data[3][0][1] = 1;
     
     
     //       000
     //       0
     resetShape(6);
     gameShapes[6]->data[0][0][1] = 1;
     gameShapes[6]->data[0][1][1] = 1;
     gameShapes[6]->data[0][2][1] = 1;
     gameShapes[6]->data[0][0][2] = 1;
     
     gameShapes[6]->data[1][1][0] = 1;
     gameShapes[6]->data[1][1][1] = 1;
     gameShapes[6]->data[1][1][2] = 1;
     gameShapes[6]->data[1][2][2] = 1;
     
     gameShapes[6]->data[2][0][1] = 1;
     gameShapes[6]->data[2][1][1] = 1;
     gameShapes[6]->data[2][2][1] = 1;
     gameShapes[6]->data[2][2][0] = 1;
     
     gameShapes[6]->data[3][1][0] = 1;
     gameShapes[6]->data[3][1][1] = 1;
     gameShapes[6]->data[3][1][2] = 1;
     gameShapes[6]->data[3][0][0] = 1;
}

void game::resetShape(int id)
{
     gameShapes[id]->data[0][0][0] = 0;
     gameShapes[id]->data[0][0][1] = 0;
     gameShapes[id]->data[0][0][2] = 0;
     gameShapes[id]->data[0][0][3] = 0;
     gameShapes[id]->data[0][1][0] = 0;
     gameShapes[id]->data[0][1][1] = 0;
     gameShapes[id]->data[0][1][2] = 0;
     gameShapes[id]->data[0][1][3] = 0;
     gameShapes[id]->data[0][2][0] = 0;
     gameShapes[id]->data[0][2][1] = 0;
     gameShapes[id]->data[0][2][2] = 0;
     gameShapes[id]->data[0][2][3] = 0;
     gameShapes[id]->data[0][3][0] = 0;
     gameShapes[id]->data[0][3][1] = 0;
     gameShapes[id]->data[0][3][2] = 0;
     gameShapes[id]->data[0][3][3] = 0;
     
     gameShapes[id]->data[1][0][0] = 0;
     gameShapes[id]->data[1][0][1] = 0;
     gameShapes[id]->data[1][0][2] = 0;
     gameShapes[id]->data[1][0][3] = 0;
     gameShapes[id]->data[1][1][0] = 0;
     gameShapes[id]->data[1][1][1] = 0;
     gameShapes[id]->data[1][1][2] = 0;
     gameShapes[id]->data[1][1][3] = 0;
     gameShapes[id]->data[1][2][0] = 0;
     gameShapes[id]->data[1][2][1] = 0;
     gameShapes[id]->data[1][2][2] = 0;
     gameShapes[id]->data[1][2][3] = 0;
     gameShapes[id]->data[1][3][0] = 0;
     gameShapes[id]->data[1][3][1] = 0;
     gameShapes[id]->data[1][3][2] = 0;
     gameShapes[id]->data[1][3][3] = 0;
     
     gameShapes[id]->data[2][0][0] = 0;
     gameShapes[id]->data[2][0][1] = 0;
     gameShapes[id]->data[2][0][2] = 0;
     gameShapes[id]->data[2][0][3] = 0;
     gameShapes[id]->data[2][1][0] = 0;
     gameShapes[id]->data[2][1][1] = 0;
     gameShapes[id]->data[2][1][2] = 0;
     gameShapes[id]->data[2][1][3] = 0;
     gameShapes[id]->data[2][2][0] = 0;
     gameShapes[id]->data[2][2][1] = 0;
     gameShapes[id]->data[2][2][2] = 0;
     gameShapes[id]->data[2][2][3] = 0;
     gameShapes[id]->data[2][3][0] = 0;
     gameShapes[id]->data[2][3][1] = 0;
     gameShapes[id]->data[2][3][2] = 0;
     gameShapes[id]->data[2][3][3] = 0;
     
     gameShapes[id]->data[3][0][0] = 0;
     gameShapes[id]->data[3][0][1] = 0;
     gameShapes[id]->data[3][0][2] = 0;
     gameShapes[id]->data[3][0][3] = 0;
     gameShapes[id]->data[3][1][0] = 0;
     gameShapes[id]->data[3][1][1] = 0;
     gameShapes[id]->data[3][1][2] = 0;
     gameShapes[id]->data[3][1][3] = 0;
     gameShapes[id]->data[3][2][0] = 0;
     gameShapes[id]->data[3][2][1] = 0;
     gameShapes[id]->data[3][2][2] = 0;
     gameShapes[id]->data[3][2][3] = 0;
     gameShapes[id]->data[3][3][0] = 0;
     gameShapes[id]->data[3][3][1] = 0;
     gameShapes[id]->data[3][3][2] = 0;
     gameShapes[id]->data[3][3][3] = 0;
}


void game::reset()
{
     for (int i=0; i<GAMEBOARD_X; i++)
         for (int j=0; j<GAMEBOARD_Y; j++)
         {
             gameBoard[i][j] = -1;
             moveBoard[i][j] = -1;
         }
         
     
     block_status = 0;
     block_x = INIT_BLOCK_X;
     block_y = 0;
     
     GameLevel = 0;
     lineCount = 0;

     shape_id    = getRandom();
     nextShapeID = getRandom();

     nextColourID = getRandomImgID();
     curColourID  = getRandomImgID();
     
     dropInterval = T_DROP_INTERVAL;
     
     updateMoveBoard();
}
