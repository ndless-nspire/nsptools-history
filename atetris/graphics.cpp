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
        
    for (int i=0; i<FONT_COUNT; i++)
        fonts[i] = NULL;
}

// GRAPHICS destructor : for each allocated image, we perform a 
// "clean" release :)
graphics::~graphics()
{
    for (int i=0; i<IMAGE_COUNT; i++)
       if (surfaces[IMAGE_COUNT])
           if( surfaces[i] ) // Is it pointing to anything?
               SDL_FreeSurface( surfaces[i] );

    for (int i=0; i<FONT_COUNT; i++)
       if (fonts[FONT_COUNT])
           if( fonts[i] ) // Cannot destroy fonts that doesn't exist...
               TTF_CloseFont( fonts[i] );

   TTF_Quit(); 
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


// This function is what enables transparency in SDL_Surfaces. Without 
// the alpha key colour, SDL graphics would be extremely limited. 
// All you need to do here is specify an image ID and a colour,
// and that colour will become transparent for that specific
// surface      
void graphics::setAlphaColour(int id, Uint32 colour)
{
     if (id <IMAGE_COUNT && id >-1)
     {
        SDL_SetColorKey(surfaces[id],SDL_SRCCOLORKEY|SDL_RLEACCEL,colour);
        surfaces[id] = SDL_DisplayFormatAlpha(surfaces[id]);
     }
}


// Same transparency colour key function as above except it takes
// direct R,G,B values, making it much easier to handle
void graphics::setAlphaColour(int id, int R, int G, int B)
{
     if (id <IMAGE_COUNT && id >-1)
     {
        Uint32 colour = SDL_MapRGB(screen->format, R, G, B);
        setAlphaColour(id, colour);
     }
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


// The function below loads all the images defined in the data/bitmap_images.dat
// making it simple for coders to handle images. It has quite a bit of 
// checkups to ensure the data loaded is "clean" (no negative or crazy colour
// values). You can always add a little more security to this section if
// you're willing to
void graphics::loadBitmapImageData()
{
    FILE *f_conf = fopen("data/bitmap_images.dat","r");
    if ( !f_conf )
       printf("*** ERROR *** can't load data/game_images.dat config file\n");
     else 
     {
          printf("Opening bitmap image data config file\n");
          printf("==========================================\n");
          char* line = new char[256];
          char* path = new char[256];
          char* data = new char[256];
          
          int nb = 0;
          int R, G, B;
          
          while (fgets(line,256,f_conf)!=NULL)
          {
              R = -1;
              B = -1;
              G = -1;
              nb = -1;
                
              if ( !strstr( line, "#" ) && (strlen(line)>3) )
              {
                    char* r = line;
                    char* w = data;
                    
                    // Load cell id
                    while (r && *r!='\n' && *r!='\r' && *r!='[')
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']')
                          *w++ = *r++;
                    *w = 0;
                    nb = atoi(data);
                    w = path;
                    
                    // Load image path id
                    while (r && *r!='\n' && *r!='\r' && *r!='[' )
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']' )
                          *w++ = *r++;
                    *w = 0;
                    w = data;
                    
                    // Load RGB transparency key colour
                    while (r && *r!='\n' && *r!='\r' && *r!='[')
                          r++;
                    r++;
                        // Load R
                        while (r && *r!='\n' && *r!='\r' && *r!=' ' && *r!='\t')
                          *w++ = *r++;
                        *w = 0;
                        r++;
                        R = atoi(data);
                        w = data;
                        
                        // Load G
                        while (r && *r!='\n' && *r!='\r' && *r!=' ' && *r!='\t')
                          *w++ = *r++;
                        *w = 0;
                        r++;
                        G = atoi(data);
                        w = data;
                        
                        // Load B
                        while (r && *r!='\n' && *r!='\r' && ( *r!=' ' || *r!=']') )
                          *w++ = *r++;
                        *w = 0;
                        B = atoi(data);
                        
                    bool ok = true;
                    
                    if ( R==-1 && G==-1 && B==-1 )
                    {
                         if ( nb > -1 && nb < IMAGE_COUNT)
                         {
                              if ( ! surfaces[nb] )
                            {
                                  printf ("\tLoading image '%s' to cell [%d]\n", path, nb);
                                  SDL_Surface* temp = SDL_LoadBMP(path);
                                  surfaces[nb] = SDL_DisplayFormat(temp);
                                  SDL_FreeSurface(temp);
                             }
                             else
                                  printf ("\t***ERROR*** cell [%d] has already been allocated... can't load image '%s'\n",nb,path); 
                         }
                         else
                             printf ("\t***ERROR*** Error, can't load image in non allocated memory cell [%d] '%s'!!!!\n",nb, line); 
                         
                    }
                    else
                    {
    
                        if ( (R < 0 || R > 255) || (G < 0 || G > 255) || (B < 0 || B > 255) )
                        {
                           ok = false;
                           printf ("\t***ERROR*** RGB values (%d,%d,%d) aren't located between 0 and 255 on line '%s'!!!!\n",R,G,B, line); 
                        }
                        
                        if ( ok )
                        {
                            if ( ! surfaces[nb] )
                            {
                                  printf ("\tLoading image '%s' to cell [%d] with alpha colour [%d, %d, %d]\n",path,nb,R,G,B);
                                  SDL_Surface* temp = SDL_LoadBMP(path);
                                  surfaces[nb] = SDL_DisplayFormat(temp);
                                  setAlphaColour(nb, R, G, B);
                                  SDL_FreeSurface(temp);
                             }
                             else
                                  printf ("\t***ERROR*** cell [%d] has already been allocated... can't load image '%s'\n",nb,path); 
                         }
                      }
              }
          }
          delete line;
          delete path;
          delete data;
          fclose(f_conf);
          printf("\n\n");
     }
}



// The function below loads all the images defined in the data/images.dat
// making it simple for coders to handle images. It has quite a bit of 
// checkups to ensure the data loaded is "clean" (no negative or crazy colour
// values). You can always add a little more security to this section if
// you're willing to
void graphics::loadImageData()
{
    FILE *f_conf = fopen("data/images.dat","r");
    if ( !f_conf )
       printf("*** ERROR *** can't load data/images.dat config file\n");
    else 
    {
          printf("Opening image data config file\n");
          printf("==========================================\n");
          char* line = new char[256];
          char* path = new char[256];
          char* data = new char[256];
          
          int nb = 0;
          int alpha;
          
          while (fgets(line,256,f_conf)!=NULL)
          {
              alpha = -1;
              nb = -1;
                
              if ( !strstr( line, "#" ) && (strlen(line)>3) )
              {
                    char* r = line;
                    char* w = data;
                    
                    // Load cell id
                    while (r && *r!='\n' && *r!='\r' && *r!='[')
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']')
                          *w++ = *r++;
                    *w = 0;
                    nb = atoi(data);
                    w = path;
                    
                    // Load image path id
                    while (r && *r!='\n' && *r!='\r' && *r!='[' )
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']' )
                          *w++ = *r++;
                    *w = 0;
                    w = data;
                    
                    // Load image alpha boolean
                    while (r && *r!='\n' && *r!='\r' && *r!='[' )
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']' )
                          *w++ = *r++;
                    *w = 0;
                    alpha = atoi(data);
                    
                    if ( alpha==0 || alpha==1 )
                    {
                         if ( nb > -1 && nb < IMAGE_COUNT)
                         {
                            if ( ! surfaces[nb] )
                            {
                                 if (alpha == 0)
                                 {
                                    printf ("\tLoading image '%s' to cell [%d]\n", path, nb);
                                    SDL_Surface* temp = IMG_Load(path);
                                    surfaces[nb] = SDL_DisplayFormat(temp);
                                    SDL_FreeSurface(temp);
                                 }
                                 else
                                 {
                                    printf ("\tLoading image '%s' to cell [%d] using its alpha channel\n", path, nb);
                                    SDL_Surface* temp = IMG_Load(path);
                                    surfaces[nb] = SDL_DisplayFormatAlpha(temp);
                                    SDL_FreeSurface(temp);
                                 }
                             }
                             else
                                  printf ("\t***ERROR*** cell [%d] has already been allocated... can't load image '%s'\n",nb,path); 
                         }
                         else
                             printf ("\t***ERROR*** Error, can't load image in non allocated memory cell [%d] '%s'!!!!\n",nb, path); 
                         
                    }
                    else
                        printf ("\t***ERROR*** Error, can't load image '%s' : alpha channel value is incorrect !!!!\n",path); 
              }
          }

          delete line;
          delete path;
          delete data;
          fclose(f_conf);
          printf("\n\n");
     }
}


// The function below loads all the fonts defined in the data/fonts.dat
void graphics::loadFontData()
{
    FILE *f_conf = fopen("data/fonts.dat","r");
    if ( !f_conf )
       printf("*** ERROR *** can't load data/fonts.dat config file\n");
    else 
    {
          printf("Opening font data config file\n");
          printf("==========================================\n");
          char* line = new char[256];
          char* path = new char[256];
          char* data = new char[256];
          
          int nb = 0;
          int font_size = 0;
          
          while (fgets(line,256,f_conf)!=NULL)
          {
              nb = -1;
                
              if ( !strstr( line, "#" ) && (strlen(line)>3) )
              {
                    char* r = line;
                    char* w = data;
                    
                    // Load cell id
                    while (r && *r!='\n' && *r!='\r' && *r!='[')
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']')
                          *w++ = *r++;
                    *w = 0;
                    nb = atoi(data);
                    w = path;
                    
                    // Load font path 
                    while (r && *r!='\n' && *r!='\r' && *r!='[' )
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']' )
                          *w++ = *r++;
                    *w = 0;
                    w = data;
                    
                    // Load font size 
                    while (r && *r!='\n' && *r!='\r' && *r!='[' )
                          r++;
                    r++;
                    while (r && *r!='\n' && *r!='\r' && *r!=']' )
                          *w++ = *r++;
                    *w = 0;
                    font_size = atoi( data );
                    
                    if ( nb > -1 && nb < FONT_COUNT)
                    {
                            if ( ! fonts[nb] )
                            {
                                 fonts[nb] = TTF_OpenFont( path, font_size );
                                 printf ("\tLoaded font '%s' to cell [%d] using font size '%d'\n", path, nb, font_size); 
                            }
                             else
                                  printf ("\t***ERROR*** cell [%d] has already been allocated... can't load font '%s'\n",nb,path); 
                     }
                     else
                         printf ("\t***ERROR*** Error, can't load font in non allocated memory cell [%d] '%s'!!!!\n",nb, path); 
              }
          }

          delete line;
          delete path;
          delete data;
          fclose(f_conf);
          printf("\n\n");
     }
}

// Font rendering function. The font_id param is used to specify which font you want
// to render on the screen. The next params are the text you want to print out
// and its position on screen...
bool graphics::renderText(int font_id, char* text, int R, int G, int B, int x, int y)
{
     if ( font_id < FONT_COUNT && font_id > -1 && fonts[font_id] )
     {
          SDL_Color textColor = { R, G, B };
          font_surf = TTF_RenderText_Solid( fonts[font_id], text, textColor );
          blitImage( font_surf, x, y );
          SDL_FreeSurface( font_surf ); // No more memory leaks! \o/
          font_surf = NULL; // Probably a good idea to make it point to NULL. Just in case... 
          return true;
     }
     return false;
}
