//& ****************************************************************************
//&
//& Copyright (C) 2004 HP-GCC Team
//&
//& ****************************************************************************
//&
//& This file is part of HP-GCC.
//&
//& HP-GCC is free software; you can redistribute it and/or modify
//& it under the terms of the GNU General Public License as published by
//& the Free Software Foundation; either version 2, or (at your option)
//& any later version.
//& 
//& HP-GCC is distributed in the hope that it will be useful,
//& but WITHOUT ANY WARRANTY; without even the implied warranty of
//& MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//& GNU General Public License for more details.
//& 
//& You should have received a copy of the GNU General Public License
//& along with HP-GCC; see the file COPYING. 
//& 
//& As a special exception, you may use this file as part of a free software
//& library without restriction.  Specifically, if other files instantiate
//& templates or use macros or inline functions from this file, or you compile
//& this file and link it with other files to produce an executable, this
//& file does not by itself cause the resulting executable to be covered by
//& the GNU General Public License.  This exception does not however
//& invalidate any other reasons why the executable file might be covered by
//& the GNU General Public License.
//&
//& ****************************************************************************

#ifndef __HPGRAPHICS_H
#define __HPGRAPHICS_H

/*!
 * \file hpgraphics.h
 *
 * \brief Graphics routines for the HP49G+ calculator.
 */

/*!
 * \mainpage HPG Graphics Library
 *
 * \section overview Overview
 *
 * HPG implements basic graphics functionality on the calculator display.  Its
 * features include:
 *
 * \li Monochrome, 4-color grayscale, and 16-color grayscale modes
 * \li Double buffering with vsync and hardware page flipping
 * \li Drawing to and blitting to and from off-screen images
 * \li Loading the Xpm (X-Windows pixmap) file format
 * \li Drawing of pixels, lines, circles, rectangles, and polygons
 * \li Filling of circles, rectangles, and polygons with arbitrary patterns
 * \li Text drawing in multiple selectable fonts, or in user-defined fonts
 * \li Rectangular clipping regions for each surface
 * \li Control of LCD indicators on the calculator screen
 *
 * \section quickstart Quick Start
 *
 * A simple graphics program written with HPG should perform several tasks:
 *
 * \li Set a display mode (optional)
 * \li Configure the graphics context (optional)
 * \li Draw to the graphics context
 * \li Display the new graphics image (double-buffered modes only)
 *
 * \subsection qssample Quick Start Sample
 *
 * On the theory that it is best to learn by immersion, here are two
 * quick samples of using the HPG library.  The first sample is a simple
 * "Hello, world!" application.
 *
 * \code
 * #include <hpgcc49.h>
 * #include <hpgraphics.h>
 * 
 * int main(void)
 * {
 *     hpg_clear();
 *     hpg_draw_text("Hello, world!", 0, 0);
 *     while(!keyb_isON());
 * 
 *     return 0;
 * }
 * \endcode
 *
 * Next is a more involved sample, demonstrating each of the steps from the
 * list above.  You may find it helpful to refer to this example as each
 * step is explained below.  This sample uses 16-color grayscale, a
 * non-default font, and double-buffering.
 *
 * \code
 * #include <hpgcc49.h>
 * #include <hpgraphics.h>
 * int main(void)
 * {
 *     hpg_set_font(hpg_stdscreen, hpg_get_bigfont());
 *     hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_10);
 * 
 *     hpg_clear();
 *     hpg_draw_text("Hello, world!", 0, 0);
 * 
 *     hpg_flip(); //updates the screen with the new image (needed in double-buffered mode)
 *     while(!keyb_isON());
 * 
 *     return 0;
 * }
 * \endcode
 *
 * Here is a program that draws some lines, text and shapes to the screen.
 *
 * \code
 * #include <hpgcc49.h>
 * #include <hpgraphics.h>
 * 
 * int main(void){
 *    hpg_clear(); //clear the screen
 *    hpg_set_color(hpg_stdscreen, HPG_COLOR_BLACK);
 *    hpg_draw_text("minifont", 0, 0);
 *    hpg_set_font(hpg_stdscreen, hpg_get_bigfont()); //change to a big font
 *    hpg_draw_text("big font", 0, 20);
 *    hpg_set_font(hpg_stdscreen, hpg_get_minifont()); //and back to small
 *    hpg_draw_text("minifont again", 0, 35);
 *    hpg_draw_line(70,5,70,25);// draw a line
 *    hpg_draw_rect(50,60,55,70); //and a rectangle
 *    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_8); //grey    
 *    hpg_fill_rect(50,0,60,10); //fill in a rectangle
 *    hpg_set_color(hpg_stdscreen, HPG_COLOR_GRAY_5); //lightish grey
 *    hpg_fill_circle(70,70,4); //draw a circle
 *    hpg_flip(); //updates the screen with the new image (needed in double-buffered mode)
 *    while(!keyb_isON()); //wait until ON pressed
 * }
 * \endcode


 * \subsection mode Setting a Display Mode
 *
 * The HPG library implements provides three different display modes for the
 * calculator display.  They are:
 *
 * \li 1-bit-per-pixel monochrome, single or double buffered
 * \li 2-bit-per-pixel (4-color) grayscale, single or double buffered
 * \li 4-bit-per-pixel (16-color) grayscale, single or double buffered
 *
 * HPG defaults to using a single-buffered monochrome screen.  If this is the
 * screen you want, then you may skip this step.
 *
 * After choosing a display mode, you should call one of ::hpg_set_mode_mono,
 * ::hpg_set_mode_gray4, or ::hpg_set_mode_gray16.  Each of these functions
 * accepts one additional parameter, which will enable double-buffering if it
 * is set.
 *
 * When double buffering is enabled, your images are drawn to an off-screen
 * buffer, and diplayed as a single action when the ::hpg_flip function is
 * called.  Until that time, the display will show the previous completed
 * screen in its entirety.  This prevents your users from viewing partially
 * drawn screens, and can prevent an appearance of "flicker".  HPG's
 * implementation of double-buffering uses a combination of vsync and hardware
 * page flipping to ensure that only complete frames are drawn.
 *
 * Regardless of the current display mode, the HPG library provides a choice
 * of 256 virtual colors.  However, these colors are mapped in a mode-specific
 * manner to the colors of the physical screen.  In 1-bpp monochrome mode, for
 * example, colors 0-127 appear as white, while colors 128-255 appear as
 * black.  By contrast, 16-color grayscale mode displays only colors 0-15 as
 * white, and 240-255 as black, while the intermediate colors appear as shades
 * of gray.
 *
 * \subsection config Configuring the Graphics Context
 *
 * HPG defines a global variable, ::hpg_stdscreen, to refer to the screen's
 * graphics context.  This graphics context defines several properties that
 * affect how drawing operations modify the screen.  These include:
 * 
 * \li The color for all drawing operations
 * \li The drawing mode, either ::HPG_MODE_PAINT or ::HPG_MODE_XOR
 * \li The font for drawing text
 * \li The fill patterns for filling shapes
 *
 * Before drawing, you may modify any of these attributes.  The library
 * defaults to drawing in paint mode, black, the minifont, and a solid fill.
 * These defaults may be modified by using the following functions:
 *
 * \li ::hpg_set_color
 * \li ::hpg_set_mode
 * \li ::hpg_set_font
 * \li ::hpg_set_pattern
 *
 * More information about these options may be found in the reference
 * documentation.
 *
 * \subsection drawing Drawing to the Graphics Context
 *
 * HPG provides a number of functions to draw to a graphics context.  These
 * include any functions that begin with \c hpg_draw_ or \c hpg_fill_
 * prefixes.  You may browse the reference documentation for more information
 * on each of these functions.
 *
 * The ::hpg_clear function is used to clear the screen.  The initial contents
 * of the screen are undefined, and it is recommended that you clear the
 * screen prior to beginning your drawing.
 *
 * \subsection display Displaying a New Graphics Image
 *
 * If you chose a single-buffered display mode, your drawing operations are
 * already visible on the screen.  If you chose a double-buffered mode,
 * however, then your changes to the screen are not yet visible.  You need to
 * inform the library that you are done drawing, so it can display the
 * completed image to the screen.  This is done by calling ::hpg_flip.
 *
 * \subsection cleaning Cleaning Up the Display
 *
 * The display needs to be restored to a very specific state prior to exiting
 * your application.  HPG provides a method called ::hpg_cleanup to do this.
 * 
 * \attention All applications which use HPG should call ::hpg_cleanup just
 * prior to exiting, after the last call to any other HPG function.
 *
 * And that's all there is to it!
 *
 * \section general General Information
 *
 * \subsection coordinates Screen Coordinates
 *
 * The HP49G+ screen is 131 by 80 pixels in size.  The coordinate system of
 * HPG is a standard screen coordinate plane (which is inverted in the Y
 * dimension from a standard mathematical 2D coordinate plane).  The pixel
 * at coordinate (0, 0) is in the upper-left of the screen.  X coordinates
 * increase to the right, and Y coordinates increase toward the bottom.  The
 * lower-right pixel of the screen is (130, 79).
 *
 * \subsection clipping Clipping Regions
 *
 * Drawing operations in HPG may be clipped to subregions of the screen.  The
 * three functions used for this purpose are ::hpg_clip, ::hpg_clip_set, and
 * ::hpg_clip_reset.  All clipping regions in HPG are axis-aligned rectangles.
 * By default, drawing operations are clipped to the size of the screen.
 *
 * \subsection images Off-screen Images
 *
 * Occasionally, it is useful to draw temporarily to an off-screen area,
 * which can be copied to the screen on demand.  HPG uses images to
 * provide this functionality.
 *
 * An image is created by calling any of ::hpg_alloc_mono_image,
 * ::hpg_alloc_gray4_image, or ::hpg_alloc_gray16_image.  The size of the
 * image is passed as parameters to the function.  Images can be arbitrary
 * size, limited by available memory.  The result of the allocation function
 * is a pointer to an ::hpg_t, which can be used with drawing function in the
 * normal manner.  You will need to use functions with the \c _on suffix, as
 * opposed to those that draw directly to the screen.
 *
 * \note Newly allocated images have undefined contents.  You should ensure
 * that you either use ::hpg_clear_on to clear any old contents prior to
 * drawing, or guarantee that your drawing code overwrites every pixel of the
 * screen.  Failure to do so can result in garbage data within the image,
 * which may be later copied to the screen.
 *
 * When you wish to copy a portion of the image to the screen, the ::hpg_blit
 * function can accomplish this task.
 *
 * \subsection xpm Xpm Image Loading
 *
 * Preliminary code exists to load Xpm image files from a file on disk.  The
 * steps required to do so may be unusual to programmers who have not worked
 * with C-compilable file formats before.  Here is a sample of loading and
 * drawing an Xpm file to the screen:
 *
 * \code
 * #include "monalisa.xpm"
 *
 * int main(void)
 * {
 *     hpg_set_mode_gray16(0);
 *     hpg_t *img = hpg_load_xpm_gray16(monalisa_xpm);
 *
 *     hpg_blit(img, 0, 0, 80, 80, hpg_stdscreen, 0, 0);
 *
 *     while(!keyb_isON());
 *     hpg_free_image(img);
 *
 *     return 0;
 * }
 * \endcode
 *
 * The first line of the example above includes the image file into the
 * application code.  Unlike most image formats, Xpm files consist of C code
 * that can be compiled.  This C code declares a data structure called
 * \c \<name\>_xpm, which contains the image data.  The data is then loaded into
 * an off-screen image, and drawn to the screen.
 */

/*!
 * \brief A graphics context for a surface that can be used for drawing.
 * 
 * The ::hpg_t type represents a surface that can be drawn upon.  It might
 * represent the physical screen, or an off-screen image.  An ::hpg_t may
 * contain a visible frame buffer (which is shown) and an effective frame
 * buffer (which is drawn upon), and the two may be swapped via the
 * ::hpg_flip method.
 *
 * Each ::hpg_t maintains its own state, including the current color, fill
 * pattern, font, and clipping region.
 *
 * Most applications will primarily use one instance of ::hpg_t, which is
 * accessed through ::hpg_stdscreen.  Additional instances of ::hpg_t may be
 * created to draw to off-screen images, by using the functions
 * ::hpg_alloc_mono_image, ::hpg_alloc_gray4_image, and
 * ::hpg_alloc_gray16_image.  Off-screen images should later be released by
 * a call to ::hpg_free_image.
 */
typedef struct hpg_graphics hpg_t;

/*!
 * \brief A font for drawing text.
 *
 * The ::hpg_font_t type represents a font for drawing text.  The font defines
 * glyphs, as well as an advance and height.  Every ::hpg_t has a current font
 * that it uses for all drawing operations.
 *
 * The functions ::hpg_get_minifont and ::hpg_get_bigfont retrieve pointers
 * to the two standard fonts bundled with HPG.  Custom fonts can be created
 * with ::hpg_alloc_font, and should be released with ::hpg_free_font when
 * they are no longer in use.
 *
 * The current font affects the behavior of ::hpg_draw_letter, ::hpg_draw_text,
 * ::hpg_draw_letter_on, and ::hpg_draw_text_on.
 */
typedef struct hpg_font hpg_font_t;

/*!
 * \brief A pattern for filling shapes.
 *
 * Setting a fill pattern causes shape fill operations (\c hpg_fill_*) to
 * become partially transparent.  Patterns may also be used for drawing
 * predefined single-color images to the screen with transparency.
 *
 * Patterns are currently limited to be exactly eight pixels in width.  They
 * may either be fixed (so they repeat over the entire screen beginning from
 * the top-left corner) or relative (so they begin at a reference location,
 * which is generally the top-left corner of a shape that's being drawn).
 *
 * Patterns can be created with ::hpg_alloc_pattern.  When no longer in use,
 * they should be released using ::hpg_free_pattern.
 *
 * The current fill pattern affects the behavior of ::hpg_fill_rect,
 * ::hpg_fill_circle, and ::hpg_fill_polygon.
 */
typedef struct hpg_pattern hpg_pattern_t;

/*!
 * \brief A graphics context representing the physical screen.
 *
 * This variable points to the graphics context for the physical screen.
 * It is initialized by a call to ::hpg_init, which must be called before
 * any drawing to the screen.
 *
 * \note Some HPG functions have convenience versions that automatically
 * operate on ::hpg_stdscreen.  These include ::hpg_clear, ::hpg_draw_line,
 * and any other functions which have versions with and without the \c _on
 * prefix.  Other functions can \b only validly operate on ::hpg_stdscreen,
 * including ::hpg_flip, and ::hpg_set_mode_mono and its kin.  These functions
 * do not accept a parameter, but nevertheless operate on ::hpg_stdscreen.
 */
extern hpg_t *hpg_stdscreen;

/*!
 * \brief A drawing mode for painting on the screen.
 * 
 * In paint mode, any pixel drawn to the screen will replace the current
 * pixel at that location.  The value of the current pixel has no effect on
 * the result of drawing.  This is the default drawing mode.
 */
#define HPG_MODE_PAINT 0

/*!
 * \brief A drawing mode to XOR bits reversibly onto the screen.
 *
 * In XOR drawing mode, drawing to the screen will perform a bitwise XOR
 * between the old color of the pixel at that location and the most
 * significant bits of the new screen color.  For example, in 16-color
 * grayscale (4 bits per pixel), the 4 most significant bits of the current
 * color are combined with the current pixel in an XOR operation.  In
 * monochrome (1 bit per pixel) only the most significant bit is used.
 *
 * Drawing a second time to the same pixel with the same color will return
 * it to its normal state.  It is this property which makes XOR drawing mode
 * particularly interesting.
 *
 * XOR mode acts exactly like paint mode when drawing to a white screen.
 * Drawing in XOR mode with white as the current color has no effect at all.
 * Generally speaking, darker colors will cause the colors on the screen to
 * change more than lighter colors -- however, this is not always the case;
 * the above mathematical description of the result of drawing operations
 * should be considered authoritative.
 */
#define HPG_MODE_XOR 1

/*!
 * \brief The color white.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_WHITE    0x00

/*!
 * \brief The 1st shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_1   0x11

/*!
 * \brief The 2nd shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_2   0x22

/*!
 * \brief The 3rd shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_3   0x33

/*!
 * \brief The 4th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_4   0x44

/*!
 * \brief The 5th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_5   0x55

/*!
 * \brief The 6th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 *
 * \warning Use of this color in 16-color grayscale modes may result in
 * visible flickering of the screen.  Choice of other colors is recommended.
 */
#define HPG_COLOR_GRAY_6   0x66

/*!
 * \brief The 7th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_7   0x77

/*!
 * \brief The 8th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 *
 * \warning Use of this color in 16-color grayscale modes may result in
 * visible flickering of the screen.  Choice of other colors is recommended.
 */
#define HPG_COLOR_GRAY_8   0x88

/*!
 * \brief The 9th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_9   0x99

/*!
 * \brief The 10th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_10  0xAA

/*!
 * \brief The 11th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_11  0xBB

/*!
 * \brief The 12th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_12  0xCC

/*!
 * \brief The 13th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_13  0xDD

/*!
 * \brief The 14th shade of gray.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_GRAY_14  0xEE

/*!
 * \brief The color black.
 *
 * \note Colors are nominally 8-bit values, with white as 0 and black as
 * 255. Only the most significant bits of these colors will be used for
 * drawing operations, depending on the current LCD mode.
 */
#define HPG_COLOR_BLACK    0xFF

/*
 * These constants represent the six indicators at the top of the HP49G+
 * screen.  They should be used with ::hpg_set_indicator to change the color
 * of the indicators.
 */

/*!
 * \brief The remote indicator at the right side of the screen.
 */
#define HPG_INDICATOR_REMOTE  0

/*!
 * \brief The left shift indicator.
 */
#define HPG_INDICATOR_LSHIFT  1

/*!
 * \brief The right shift indicator.
 */
#define HPG_INDICATOR_RSHIFT  2

/*!
 * \brief The alpha indicator.
 */
#define HPG_INDICATOR_ALPHA   3

/*!
 * \brief The low battery indicator.
 */
#define HPG_INDICATOR_BATTERY 4

/*!
 * \brief The wait/hourglass indicator.
 */
#define HPG_INDICATOR_WAIT    5

/*!
 * \brief Initializes the hpg module.
 *
 * This function is no longer required
 */
void hpg_init(void);

/*!
 * \brief Restores the graphics hardware to the default state.
 *
 * This function is no longer required
 */
void hpg_cleanup(void);

/*!
 * \brief Sets the screen to monochrome mode.
 *
 * Monochrome is the default mode for the screen.  Generally, this function is
 * only used if you wish to change to a double-buffered monochrome mode.
 *
 * \param dbuf If non-zero, sets up two buffers and draws to the unseen
 *             buffer; ::hpg_flip can be used to switch to the other buffer
 *             at any time.
 */
void hpg_set_mode_mono(int dbuf);

/*!
 * \brief Sets the screen to 4-color grayscale mode.
 *
 * 4-color grayscale is a palette mode, and a palette of reasonable colors
 * is set by calling this function.  Gray levels 0, 7, 10, and 15
 * are used, based on a visual comparison of the grays for the most stable
 * values.
 *
 * \param dbuf If non-zero, sets up two buffers and draws to the unseen
 *             buffer; ::hpg_flip can be used to switch to the other buffer
 *             at any time.
 */
void hpg_set_mode_gray4(int dbuf);

/*!
 * \brief Sets the screen to 16-color grayscale mode.
 *
 * \param dbuf If non-zero, sets up two buffers and draws to the unseen
 *             buffer; ::hpg_flip can be used to switch to the other buffer
 *             at any time.
 */
void hpg_set_mode_gray16(int dbuf);

/*!
 * \brief Retrieves the width of a surface.
 *
 * \param g  A drawing surface.
 * \return The width of the surface, in pixels.
 */
int hpg_get_width(hpg_t *g);

/*!
 * \brief Retrieves the height of a surface.
 *
 * \param g  A drawing surface.
 * \return The height of the surface, in pixels.
 */
int hpg_get_height(hpg_t *g);

/*!
 * \brief Sets the state of an LCD indicator.
 *
 * \param indicator The indicator to set.  This may be any of
 *                  ::HPG_INDICATOR_REMOTE, ::HPG_INDICATOR_LSHIFT,
 *                  ::HPG_INDICATOR_RSHIFT, ::HPG_INDICATOR_ALPHA,
 *                  ::HPG_INDICATOR_BATTERY, or ::HPG_INDICATOR_WAIT.
 * \param color The target color for the indicator.  Typically, white
 *              indicates off and black indicates on.  However, other colors
 *              may be used.
 */
void hpg_set_indicator(unsigned char indicator, unsigned char color);

/*!
 * \brief Swaps buffers on a double-buffered screen.
 *
 * If hardware page flipping is not being used, this function has no effect.
 *
 * After this method is complete, the contents of the back buffer are
 * undefined.  The application must either clear the back buffer prior to
 * drawing onto it (with ::hpg_clear, for example) or otherwise guarantee
 * this it has overwritten every pixel of the screen in order to avoid
 * undefined behavior.
 */
void hpg_flip(void);

/*!
 * \brief Resets the clipping region to the entire screen.
 *
 * This function may be used to undo any clipping performed by earlier
 * operations.
 *
 * \param g The graphics context to which this function applies
 */
void hpg_clip_reset(hpg_t *g);

/*!
 * \brief Sets the clipping region to an absolute value.
 *
 * Sets the clipping region for a given graphics context.  Use of this
 * function is discouraged in general-purpose graphics code, in favor of
 * ::hpg_clip which preserves existing clipping regions as well.  An attempt
 * to set the clipping region larger than the size of the graphics context
 * will cause the clipping region to shrink in order to fit on the screen.
 *
 * This function is equivalent to calling ::hpg_clip_reset, followed by
 * ::hpg_clip.
 *
 * \param g The graphics context to which this function applies
 * \param x1 The left-most x coordinate of the new clipping region
 * \param y1 The top-most y coordinate of the new clipping region
 * \param x2 The right-most x coordinate of the new clipping region
 * \param y2 The bottom-most y coordinate of the new clipping region
 */
void hpg_clip_set(hpg_t *g, int x1, int y1, int x2, int y2);

/*!
 * \brief Clips the display to a specified region.
 *
 * This function sets the clipping region to the geometric intersection
 * of the current clipping region and the rectangle given.  Therefore, any
 * existing clipping continues to take effect.  If this is not what you
 * want, see ::hpg_clip_set instead.
 *
 * \param g The graphics context to which this function applies
 * \param x1 The left-most x coordinate of the new clipping region
 * \param y1 The top-most y coordinate of the new clipping region
 * \param x2 The right-most x coordinate of the new clipping region
 * \param y2 The bottom-most y coordinate of the new clipping region
 */
void hpg_clip(hpg_t *g, int x1, int y1, int x2, int y2);

/*!
 * \brief Clears the given buffer.
 *
 * The entire buffer will be set to white following the completion of this
 * function.  Note that the current clipping region is \b not respected, nor
 * is current color and fill pattern used.  To set the buffer to arbitrary
 * colors, use the current clipping region, or specify a fill pattern, you
 * should use ::hpg_fill_rect_on instead.
 *
 * \param g The graphics context to which this function applies
 */
void hpg_clear_on(hpg_t *g);

/*!
 * \brief Clears the screen.
 *
 * The entire screen will be set to white following the completion of this
 * function.  Note that the current clipping region is \b not respected, nor
 * is current color and fill pattern used.  To set the screen to arbitrary
 * colors, use the current clipping region, or specify a fill pattern, you
 * should use ::hpg_fill_rect instead.
 */
void hpg_clear(void);

/*!
 * \brief Draws a pixel onto a buffer.
 *
 * \param g The graphics context to which this function applies
 * \param x The x coordinate of the pixel to draw
 * \param y The y coordinate of the pixel to draw
 */
void hpg_draw_pixel_on(hpg_t *g, int x, int y);

/*!
 * \brief Draws a pixel onto the screen.
 *
 * \param x The x coordinate of the pixel to draw
 * \param y The y coordinate of the pixel to draw
 */
void hpg_draw_pixel(int x, int y);

/*!
 * \brief Draws a line onto a buffer.
 *
 * \param g The graphics context to which this function applies
 * \param x1 The x coordinate of the starting point of the line
 * \param y1 The y coordinate of the starting point of the line
 * \param x2 The x coordinate of the ending point of the line
 * \param y2 The y coordinate of the ending point of the line
 */
void hpg_draw_line_on(hpg_t *g, int x1, int y1, int x2, int y2);

/*!
 * \brief Draws a line onto the screen.
 *
 * \param x1 The x coordinate of the starting point of the line
 * \param y1 The y coordinate of the starting point of the line
 * \param x2 The x coordinate of the ending point of the line
 * \param y2 The y coordinate of the ending point of the line
 */
void hpg_draw_line(int x1, int y1, int x2, int y2);

/*!
 * \brief Draws a rectangle onto a buffer.
 *
 * The result is an axis-aligned rectangle spanning between opposite corners at
 * (x1, y1) and (x2, y2).  To draw an arbitrary rectangle at angles to the
 * coordinate axes, use ::hpg_draw_polygon_on instead.
 *
 * \param g The graphics context to which this function applies
 * \param x1 The x coordinate of one corner of the rectangle
 * \param y1 The y coordinate of one corner of the rectangle
 * \param x2 The x coordinate of the opposite corner of the rectangle
 * \param y2 The y coordinate of the opposite corner of the rectangle
 */
void hpg_draw_rect_on(hpg_t *g, int x1, int y1, int x2, int y2);

/*!
 * \brief Draws a rectangle onto the screen.
 *
 * The result is an axis-aligned rectangle spanning between opposite corners at
 * (x1, y1) and (x2, y2).  To draw an arbitrary rectangle at angles to the
 * coordinate axes, use ::hpg_draw_polygon instead.
 *
 * \param x1 The x coordinate of one corner of the rectangle
 * \param y1 The y coordinate of one corner of the rectangle
 * \param x2 The x coordinate of the opposite corner of the rectangle
 * \param y2 The y coordinate of the opposite corner of the rectangle
 */
void hpg_draw_rect(int x1, int y1, int x2, int y2);

/*!
 * \brief Fills a rectangle on a buffer.
 *
 * The result is an axis-aligned rectangle spanning between opposite corners at
 * (x1, y1) and (x2, y2).  To fill an arbitrary rectangle at angles to the
 * coordinate axes, use ::hpg_fill_polygon_on instead.
 *
 * \param g The graphics context to which this function applies
 * \param x1 The x coordinate of one corner of the rectangle
 * \param y1 The y coordinate of one corner of the rectangle
 * \param x2 The x coordinate of the opposite corner of the rectangle
 * \param y2 The y coordinate of the opposite corner of the rectangle
 */
void hpg_fill_rect_on(hpg_t *g, int x1, int y1, int x2, int y2);

/*!
 * \brief Fills a rectangle on the screen.
 *
 * The result is an axis-aligned rectangle spanning between opposite corners at
 * (x1, y1) and (x2, y2).  To fill an arbitrary rectangle at angles to the
 * coordinate axes, use ::hpg_fill_polygon instead.
 *
 * \param x1 The x coordinate of one corner of the rectangle
 * \param y1 The y coordinate of one corner of the rectangle
 * \param x2 The x coordinate of the opposite corner of the rectangle
 * \param y2 The y coordinate of the opposite corner of the rectangle
 */
void hpg_fill_rect(int x1, int y1, int x2, int y2);

/*!
 * \brief Draws a circle onto a buffer.
 *
 * \param g The graphics context to which this function applies
 * \param cx The x coordinate of the center of the circle
 * \param cy The y coordinate of the center of the circle
 * \param r The radius of the circle
 */
void hpg_draw_circle_on(hpg_t *g, int cx, int cy, int r);

/*!
 * \brief Draws a circle onto the screen.
 *
 * \param cx The x coordinate of the center of the circle
 * \param cy The y coordinate of the center of the circle
 * \param r The radius of the circle
 */
void hpg_draw_circle(int cx, int cy, int r);

/*!
 * \brief Fills a circle on a buffer.
 *
 * \param g The graphics context to which this function applies
 * \param cx The x coordinate of the center of the circle
 * \param cy The y coordinate of the center of the circle
 * \param r The radius of the circle
 */
void hpg_fill_circle_on(hpg_t *g, int cx, int cy, int r);

/*!
 * \brief Fills a circle on the screen.
 *
 * \param cx The x coordinate of the center of the circle
 * \param cy The y coordinate of the center of the circle
 * \param r The radius of the circle
 */
void hpg_fill_circle(int cx, int cy, int r);

/*!
 * \brief Draws a polygon onto a buffer.
 *
 * Draws a closed polygon with the vertices from the given arrays.  Edges are
 * drawn between each pair of consecutive vertices, and between the first and
 * last vertices to close the shape.
 *
 * \param g The graphics context to which this function applies
 * \param vx The x coordinates of the vertices of the polygon
 * \param vy The y coordinates of the vertices of the polygon
 * \param len The number of vertices in the polygon
 */
void hpg_draw_polygon_on(hpg_t *g, int vx[], int vy[], int len);

/*!
 * \brief Draws a polygon onto the screen.
 *
 * Draws a closed polygon with the vertices from the given arrays. Edges are
 * drawn between each pair of consecutive vertices, and between the first and
 * last vertices to close the shape.
 *
 * \param vx The x coordinates of the vertices of the polygon
 * \param vy The y coordinates of the vertices of the polygon
 * \param len The number of vertices in the polygon
 */
void hpg_draw_polygon(int vx[], int vy[], int len);

/*!
 * \brief Fills a polygon on a buffer.
 *
 * Fills in a closed polygon with the vertices from the given arrays.  Edges
 * are assumed between each pair of consecutive vertices, and between the
 * first and last vertices to close the shape.
 *
 * \param g The graphics context to which this function applies
 * \param vx The x coordinates of the vertices of the polygon
 * \param vy The y coordinates of the vertices of the polygon
 * \param len The number of vertices in the polygon
 */
void hpg_fill_polygon_on(hpg_t *g, int vx[], int vy[], int len);

/*!
 * \brief Fills a polygon on the screen.
 *
 * Fills in a closed polygon with the vertices from the given arrays.  Edges
 * are assumed between each pair of consecutive vertices, and between the
 * first and last vertices to close the shape.
 *
 * \param vx The x coordinates of the vertices of the polygon
 * \param vy The y coordinates of the vertices of the polygon
 * \param len The number of vertices in the polygon
 */
void hpg_fill_polygon(int vx[], int vy[], int len);

/*!
 * \brief Retrieves the minifont.
 *
 * The minifont is a font with a height of 6 pixels and an advance of 4
 * pixels, and is the smallest font that is reasonably useful on the
 * calculator display.  This is the default font for the HPG library.
 *
 * \return The minifont
 */
hpg_font_t *hpg_get_minifont(void);

/*!
 * \brief Retrieves the bigfont.
 *
 * The bigfont is a font with a height of 8 pixels and an advance of 6
 * pixels.  It is the standard font for the HP49G+ calculator environment,
 * and is both larger and easier to read than the minifont, but results in
 * less text fitting on the screen.
 *
 * \return The bigfont
 */
hpg_font_t *hpg_get_bigfont(void);

/*!
 * \brief Returns the height of the given font.
 *
 * The height of the font is the number of vertical pixels in the font.  This
 * includes space for tails (a.k.a. the "descent") and inter-line spacing if
 * applicable.  It is the number of pixels between the top of a glyph on one
 * line and the top of the glyph on the next line.
 *
 * \param font The font
 * \return The height in pixels
 */
int hpg_font_get_height(hpg_font_t *font);

/*!
 * \brief Returns the advance of the given font.
 *
 * The advance of a font is the number of horizontal pixels in the font.  This
 * includes space inter-character spacing if applicable.  It is the number of
 * pixels between the left side of a glyph and the left side of the next glyph
 * in the line.
 *
 * \param font The font
 * \return The advance in pixels
 */
int hpg_font_get_advance(hpg_font_t *font);

/*!
 * \brief Draws a character onto a buffer.
 *
 * The top-left corner of the character is at point (x, y), and the character
 * extends to the right and bottom of that point by the font's advance and
 * height, respectively.
 *
 * \param g The graphics context to which this function applies
 * \param a The character to draw
 * \param x The x coordinate of the left side of the character block
 * \param y The y coordinate of the top edge of the character block
 */
void hpg_draw_letter_on(hpg_t *g, char a, int x, int y);

/*!
 * \brief Draws a character onto the screen.
 *
 * The top-left corner of the character is at point (x, y), and the character
 * extends to the right and bottom of that point by the font's advance and
 * height, respectively.
 *
 * \param a The character to draw
 * \param x The x coordinate of the left side of the character block
 * \param y The y coordinate of the top edge of the character block
 */
void hpg_draw_letter(char a, int x, int y);

/*!
 * \brief Draws a text string onto a buffer.
 *
 * The top-left corner of the string is at point (x, y), and the character
 * extends to the right and bottom of that point by the number of lines and
 * columns of the string, respectively.  The string may contain newline
 * characters, which will move the insertion point to the next line, and to
 * the original x coordinate for the block.
 *
 * \param g The graphics context to which this function applies
 * \param s A null-terminated string, containing the text to draw
 * \param x The x coordinate of the left side of the text block
 * \param y The y coordinate of the top edge of the text block
 */
void hpg_draw_text_on(hpg_t *g, char *s, int x, int y);

/*!
 * \brief Draws a text string onto the screen.
 *
 * The top-left corner of the string is at point (x, y), and the character
 * extends to the right and bottom of that point by the number of lines and
 * columns of the string, respectively.  The string may contain newline
 * characters, which will move the insertion point to the next line, and to
 * the original x coordinate for the block.
 *
 * \param s A null-terminated string, containing the text to draw
 * \param x The x coordinate of the left side of the text block
 * \param y The y coordinate of the top edge of the text block
 */
void hpg_draw_text(char *s, int x, int y);

/*!
 * \brief Retrieves the current color.
 *
 * \param g The graphics context to which this function applies
 * \return The current color for drawing operations to the context
 */
unsigned char hpg_get_color(hpg_t *g);

/*!
 * \brief Sets the current color.
 *
 * The new color will take effect until it is changed with another call to
 * ::hpg_set_color.
 *
 * \param g The graphics context to which this function applies
 * \param color The new color to set for the context
 */
void hpg_set_color(hpg_t *g, unsigned char color);

/*!
 * \brief Retrieves the current drawing mode.
 *
 * \param g The graphics context to which this function applies
 * \return The drawing mode for the context; either ::HPG_MODE_PAINT
 *         or ::HPG_MODE_XOR
 */
unsigned char hpg_get_mode(hpg_t *g);

/*!
 * \brief Sets the current drawing mode.
 *
 *
 * \param g The graphics context to which this function applies
 * \param mode The new drawing mode to set for the context; either
 *             ::HPG_MODE_PAINT or ::HPG_MODE_XOR.  See the descriptions
 *             of those constants for more information about the drawing
 *             modes they describe.
 */
void hpg_set_mode(hpg_t *g, unsigned char mode);

/*!
 * \brief Retrieves the current fill pattern.
 *
 * \param g The graphics context to which this function applies
 * \return The current fill pattern for the context; or \c NULL if there is
 *         no fill pattern
 */
hpg_pattern_t *hpg_get_pattern(hpg_t *g);

/*!
 * \brief Sets the current fill pattern.
 *
 * Patterns used with this function should be allocated with
 * ::hpg_alloc_pattern, and disposed of with ::hpg_free_pattern when they are
 * no longer in use.
 *
 * To remove all fill patterns from the context, pass \c NULL for the fill
 * pattern.
 *
 * \param g The graphics context to which this function applies
 * \param pattern The new fill pattern to use for the context; or \c NULL
 *                for no pattern
 */
void hpg_set_pattern(hpg_t *g, hpg_pattern_t *pattern);

/*!
 * \brief Retrieves the current font.
 *
 * \param g The graphics context to which this function applies
 * \return The current text font for the context
 */
hpg_font_t *hpg_get_font(hpg_t *g);

/*!
 * \brief Sets the current font.
 *
 * Fonts can be retrieved via the ::hpg_get_minifont and ::hpg_get_bigfont
 * functions, or they can be created by the user.  User fonts should be
 * allocated with ::hpg_alloc_font, and disposed of with ::hpg_free_font when
 * they are no longer in use.
 *
 * \param g The graphics context to which this function applies
 * \param font The new text font to use for the context
 */
void hpg_set_font(hpg_t *g, hpg_font_t *font);

/*!
 * \brief Allocates a new fill pattern.
 *
 * The pattern data is passed as a pointer, and should point to a valid buffer
 * at least as long as \a height, in bytes.  Height specifies the number of
 * rows of the pattern, and \a fixed is zero for a floating pattern (which is
 * drawn relative to a reference location on the shape being drawn) or
 * non-zero for a fixed pattern, which is drawn relative to the upper-left
 * corner of the screen.
 *
 * The data in buffer is a bitmask of opaque pixels in the pattern, with one
 * byte per line.  At the current time, all patterns are eight pixels wide.
 * The least significant bit represents the left-most pixel, and the most
 * significant bit represents the right-most pixel.  This bit order is
 * contrary to many expectations, and should be noted.
 *
 * Patterns allocated via this method should be deallocated by calling
 * ::hpg_free_pattern when they are no longer in use.
 *
 * \param buffer The buffer containing the pattern's pixel data
 * \param height The number of pixels of height for the pattern
 * \param fixed Non-zero if the pattern should be fixed, zero if floating
 * \return Pointer to a newly allocated fill pattern; or \c NULL if there was
 *         not enough memory to complete this operation
 */
hpg_pattern_t *hpg_alloc_pattern(char *buffer, int height, int fixed);

/*!
 * \brief Releases memory used by a fill pattern after it is no longer in use.
 *
 * The pattern data buffer is not released, since it is owned by the client
 * code. If the data buffer is allocated dynamically, then it should be freed
 * separately.
 *
 * \param pattern The pattern to be freed.
 */
void hpg_free_pattern(hpg_pattern_t *pattern);

/*!
 * \brief Allocates a new font.
 *
 * The font data is the concatenation of a fill pattern for each character
 * of the font, and should be at least as long as (count * height) bytes.
 * The advance should be less than or equal to eight pixels.
 *
 * Fonts allocated via this method should be deallocated by calling
 * ::hpg_free_font when they are no longer in use.
 *
 * \param buffer The pixel data for characters
 * \param count The number of characters
 * \param height The number of pixels of height for characters
 * \param advance The number of horizontal pixels between the beginning of
 *                one character in this font and the beginning of the next
 * \return Pointer to the newly allocated font; or \c NULL if there was not
 *         enough memory to allocate the font
 */
hpg_font_t *hpg_alloc_font(char *buffer, int count, int height, int advance);

/*!
 * \brief Releases memory used by a font after it is no longer in use.
 *
 * The font data buffer is not released, since it is owned by the client code.
 * If it is allocated dynamically, then it should be freed separately.
 *
 * \param font The font to be freed
 */
void hpg_free_font(hpg_font_t *font);

/*!
 * \brief Retrieves the color of a pixel.
 *
 * \param g The graphics context to which this function applies
 * \param x The x coordinate of the pixel to read
 * \param y The y coordinate of the pixel to read
 * \return The color of the pixel, extended to a full color value
 */
unsigned char hpg_get_pixel(hpg_t *g, int x, int y);

/*!
 * \brief Allocates a off-screen image for drawing in monochrome.
 *
 * \param width The width of the image
 * \param height The height of the image
 * \return A graphics context to draw to the new image
 */
hpg_t *hpg_alloc_mono_image(int width, int height);

/*!
 * \brief Allocates a off-screen image for drawing in 4-color gray.
 *
 * \param width The width of the image
 * \param height The height of the image
 * \return A graphics context to draw to the new image
 */
hpg_t *hpg_alloc_gray4_image(int width, int height);

/*!
 * \brief Allocates a off-screen image for drawing in 16-color gray.
 *
 * \param width The width of the image
 * \param height The height of the image
 * \return A graphics context to draw to the new image
 */
hpg_t *hpg_alloc_gray16_image(int width, int height);

/*!
 * \brief Frees the memory used by an off-screen image.
 *
 * \warning Do not pass ::hpg_stdscreen to this function.  Only graphics
 *          contexts for off-screen images need to be freed.
 *
 * \param img The image to be freed
 */
void hpg_free_image(hpg_t *img);

/*!
 * \brief Copy a region of one buffer to another.
 *
 * This function is used to copy any arbitrary region of a buffer to any
 * arbitrary location on the destination buffer.  The image may not be
 * resized or stretched during the blit operation.  The clipping region of
 * the target screen is effective for this operation.
 *
 * \note Blitting is possible between buffers of any color depths, even if
 * they do not match.  However, the operation may be much faster, and its
 * results will be simpler to understand, for buffers of the same depth.  It
 * is recommended that you use off-screen images of the same color depth as
 * the physical screen.
 *
 * \param src The graphics context to copy from
 * \param sx  The left x coordinate of the area to copy on the source
 * \param sy  The top y coordinate of the area to copy on the source
 * \param w   The width of the area to copy, in pixels
 * \param h   The height of the area to copy, in pixels
 * \param dst The graphics context to copy to
 * \param dx  The left x coordinate of the area to copy to the destination
 * \param dy  The top y coordinate of the area to copy to the destination
 */
void hpg_blit(hpg_t *src, int sx, int sy, int w, int h,
              hpg_t *dst, int dx, int dy);

/*!
 * \brief Loads a Xpm file to a monochrome image.
 *
 * The resulting image will be the same size as the original pixmap, but will
 * be converted to monochrome.  Colors that appear lighter than a medium
 * gray will appear white, while colors darker than a medium gray will appear
 * black.  Non-gray colors are mapped to grayscale according to an approach
 * very similar to ITU Recommendation 709, which uses a model of typical human
 * perception of color to convert color images to grays.
 *
 * ::hpg_free_image should be called when the application is no longer
 * using the image.
 *
 * \param xpm The result of compiling an Xpm image as C code
 * \return Pointer to an ::hpg_t representing the image.
 */
hpg_t *hpg_load_xpm_mono(char *xpm[]);

/*!
 * \brief Loads a Xpm file to a 4-color grayscale image.
 *
 * The resulting image will be the same size as the original pixmap, but will
 * be converted to 4-color gray.  Grays will be stored as the nearest shade
 * Non-gray colors are mapped to grayscale according to an approach very
 * similar to ITU Recommendation 709, which uses a model of typical human
 * perception of color to convert color images to grays.
 *
 * ::hpg_free_image should be called when the application is no longer
 * using the image.
 *
 * \param xpm The result of compiling an Xpm image as C code
 * \return Pointer to an ::hpg_t representing the image.
 */
hpg_t *hpg_load_xpm_gray4(char *xpm[]);

/*!
 * \brief Loads a Xpm file to a 16-color grayscale image.
 *
 * The resulting image will be the same size as the original pixmap, but will
 * be converted to 16-color gray.  Grays will be stored as the nearest shade
 * Non-gray colors are mapped to grayscale according to an approach very
 * similar to ITU Recommendation 709, which uses a model of typical human
 * perception of color to convert color images to grays.
 *
 * ::hpg_free_image should be called when the application is no longer
 * using the image.
 *
 * \param xpm The result of compiling an Xpm image as C code
 * \return Pointer to an ::hpg_t representing the image.
 */
hpg_t *hpg_load_xpm_gray16(char *xpm[]);

/*!
 * \example example_set_pattern.c
 *
 * This example demonstrates setting a checker pattern to fill shapes on the
 * screen.
 */
 
 #endif // __HPGRAPHICS_H
 
