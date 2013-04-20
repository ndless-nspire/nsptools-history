/**
 * @file nspireio.h
 * @author  Julian Mackeben aka compu <compujuckel@googlemail.com>
 * @version 3.0
 *
 * @section LICENSE
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 *
 * @section DESCRIPTION
 *
 * Nspire I/O 3.0 header file
 */
#include <os.h>

#ifndef NSPIREIO2_H
#define NSPIREIO2_H

struct nio_console
{
	char* data;
	char* color;
	int cursor_x;
	int cursor_y;
	int max_x;
	int max_y;
	int offset_x;
	int offset_y;
	char default_background_color;
	char default_foreground_color;
	BOOL drawing_enabled;
    BOOL cursor_enabled;
	int cursor_type;
	int cursor_line_width;
	unsigned char cursor_custom_data[6];
	BOOL cursor_blink_enabled;
	BOOL cursor_blink_status;
	unsigned cursor_blink_timestamp;
	unsigned cursor_blink_duration;
};
typedef struct nio_console nio_console;

#define NIO_CURSOR_BLOCK 0
#define NIO_CURSOR_UNDERSCORE 1
#define NIO_CURSOR_VERTICAL 2
#define NIO_CURSOR_CUSTOM 3

#define NIO_CHAR_WIDTH 6
#define NIO_CHAR_HEIGHT 8

#define NIO_MAX_ROWS 30
#define NIO_MAX_COLS 53

/** Draws a string to the screen on the given position. For internal use.
	@param offset_x x offset in px
	@param offset_y y offset in px
	@param x x position in columns (px*6)
	@param y y position in rows (px*8)
	@param str String
	@param bgColor Background color
	@param textColor text color
*/
void nio_grid_puts(const int offset_x, const int offset_y, const int x, const int y, const char *str, const char bgColor, const char textColor);

/** Draws a char to the screen on the given position. For internal use.
	@param offset_x x offset in px
	@param offset_y y offset in px
	@param x x position in columns (px*6)
	@param y y position in rows (px*8)
	@param c Char
	@param bgColor Background color
	@param textColor text color
*/
void nio_grid_putc(const int offset_x, const int offset_y, const int x, const int y, const char ch, const char bgColor, const char textColor);

/** Loads a console from a file on flash storage.
    @param path File path
	@param c Console
*/
void nio_load(const char* path, nio_console* c);

/** Saves a console to a file in flash storage.
	@param path File path
	@param c Console
*/
void nio_save(const char* path, const nio_console* c);

/** Sets a default console that will be used for all functions without console argument, e.g. nio_puts()
	@param c Console
*/
void nio_set_default(nio_console* c);

/** Clears a console.
	@param c Console
*/
void nio_clear(nio_console* c);

/** Scrolls a console one line down.
	@param c Console
*/
void nio_scroll(nio_console* c);

/** Draws a char from the console to the screen. For internal use.
	@param c Console
	@param pos_x x position
	@param pos_y y position
*/
void nio_csl_drawchar(nio_console* c, const int pos_x, const int pos_y);

/** Saves a char in a console without drawing it. For internal use.
	@param c Console
	@param ch Char
	@param pos_x x position
	@param pos_y y position
*/
void nio_csl_savechar(nio_console* c, const char ch, const int pos_x, const int pos_y);

/** Immediately gets a char from the keyboard. For internal use.
    @param c Console
	@return Char
*/
char nio_getch(nio_console* c);

/** Sets the background- and text color of a console. Possible values are 0-15.
	@param c Console
	@param background_color Background color
	@param foreground_color Text color
*/
void nio_color(nio_console* c, const char background_color, const char foreground_color);

/** Changes the drawing behavior of a console.
	@param c Console
	@param enable_drawing If this is true, a console will automatically be updated if text is written to it.
*/
void nio_drawing_enabled(nio_console* c, const BOOL enable_drawing);

/** Initializes a console.
	@param c Console
	@param size_x console width
	@param size_y console height
	@param offset_x x position
	@param offset_y y position
	@param background_color Background color
	@param foreground_color Text color
    @param drawing_enabled See nio_enable_drawing()
*/
void nio_init(nio_console* c, const int size_x, const int size_y, const int offset_x, const int offset_y, const char background_color, const char foreground_color, const BOOL drawing_enabled);

/** Uninitializes a console. This should always be called before the program ends.
	@param c Console
*/
void nio_free(nio_console* c);

/** For use with NIO_REPLACE_STDIO. Use at the beginning of your program.
*/
void nio_use_stdio(void);

/** For use with NIO_REPLACE_STDIO. Use at the end of your program.
*/
void nio_free_stdio(void);

/** See [fflush](http://www.cplusplus.com/reference/clibrary/cstdio/fflush/)
	\note This is useful for consoles with enable_drawing set to false. Using this function will result in the console being drawn.
*/
int nio_fflush(nio_console* c);

/** See [fputc](http://www.cplusplus.com/reference/clibrary/cstdio/fputc/)
*/
char nio_fputc(char ch, nio_console* c);

/** See [putchar](http://www.cplusplus.com/reference/clibrary/cstdio/putchar/)
*/
char nio_putchar(const char ch);

/** See [fputs](http://www.cplusplus.com/reference/clibrary/cstdio/fputs/)
*/
int nio_fputs(const char* str, nio_console* c);

/** See [puts](http://www.cplusplus.com/reference/clibrary/cstdio/puts/)
*/
int nio_puts(const char* str);

/** See [fgetc](http://www.cplusplus.com/reference/clibrary/cstdio/fgetc)
*/
char nio_fgetc(nio_console* c);

/** See [getchar](http://www.cplusplus.com/reference/clibrary/cstdio/getchar)
*/
char nio_getchar(void);

/** See [fgets](http://www.cplusplus.com/reference/clibrary/cstdio/fgets/)
    \todo Do not ignore num
*/
char* nio_fgets(char* str, int num, nio_console* c);

/** See [gets](http://www.cplusplus.com/reference/clibrary/cstdio/gets/)
*/
char* nio_gets(char* str);

//int nio_vfprintf(nio_console* c, const char* format, va_list* arglist);

/** See [fprintf](http://www.cplusplus.com/reference/clibrary/cstdio/fprintf/)
*/
int nio_fprintf(nio_console* c, const char* format, ...);

/** See [printf](http://www.cplusplus.com/reference/clibrary/cstdio/printf/)
*/
int nio_printf(const char* format, ...);

/** See [perror](http://www.cplusplus.com/reference/clibrary/cstdio/perror/)
*/
void nio_perror(const char* str);

// Macro of nio_fgetc
#define nio_getc nio_fgetc

// Macro of nio_fputc
#define nio_putc nio_fputc

/** Stores binary data in a file.
	@param dataptr Pointer to the data to be stored
	@param size Length in bytes
	@param regpath Path to file
	@return 0 on success, -1 on failure
*/
int reg_store(void* dataptr, size_t size, char* regpath);

/** Reads binary data from a file.
	@param regpath Path to file
	@return Pointer to the data, NULL on failure
*/
void* reg_get(char* regpath);

/** Checks if there is data available at the serial port.
	@return TRUE if new data is available.
*/
BOOL uart_ready(void);

/** See [getchar](http://www.cplusplus.com/reference/clibrary/cstdio/getchar/)
	@return Char
*/
char uart_getchar(void);

/** See [gets](http://www.cplusplus.com/reference/clibrary/cstdio/gets/)
	@return Destination
*/
char* uart_gets(char* str);

/** See [putchar](http://www.cplusplus.com/reference/clibrary/cstdio/putchar/)
*/
char uart_putchar(char character);

/** See [puts](http://www.cplusplus.com/reference/clibrary/cstdio/puts/)
    \note This DOES NOT append a newline (\\n) character.
*/
int uart_puts(const char *str);

/** See [printf](http://www.cplusplus.com/reference/clibrary/cstdio/printf/)
*/
void uart_printf(char *format, ...);

/** Returns the current time.
	@return Current RTC time
*/
inline unsigned nio_time_get();

/** Draws the cursor of the console, if enabled.
	@param c Console
*/
void nio_cursor_draw(nio_console* c);

/** Erases the cursor of the console, if enabled.
	@param c Console
*/
void nio_cursor_erase(nio_console* c);

/** Draws a blinking cursor, if enabled. Blinking occurs on an interval set inside the console.
	@param c Console
*/
void nio_cursor_blinking_draw(nio_console* c);

/** Resets the blinking cursor timer.
	@param c Console
*/
void nio_cursor_blinking_reset(nio_console* c);

/** Enables the console cursor.
	@param c Console
	@param enable_cursor When this is true, a cursor will be drawn to the screen, false: no cursor shown.
*/
void nio_cursor_enable(nio_console* c, BOOL enable_cursor);

/** Enables console cursor blinking.
	@param c Console
	@param enable_cursor_blink When this is true, the cursor will blink, false: no cursor blinking will occur.
*/
void nio_cursor_blinking_enable(nio_console* c, BOOL enable_cursor_blink);

/** Sets the console cursor blink duration (the time it takes to switch on or off)
	@param c Console
	@param duration The time (in seconds) it takes to switch on or off.
*/
void nio_cursor_blinking_duration(nio_console* c, int duration);

/** Sets the console cursor type.
	@param c Console
	@param cursor_type The cursor type. 0 is a block cursor (default, like a
	Linux X11 terminal), 1 is an underscore cursor (like a Windows Command
	Prompt window), 2 is a vertical bar cursor (like a regular text box),
	and 3 is a custom cursor that is set via SetCursorCustom.
	
	If you specify an invalid value, NspireIO will silenty fail and set the
	cursor type to 0, a block cursor.
	
	You may also use the predefined types as arguments. (NIO_CURSOR_*)
*/
void nio_cursor_type(nio_console* c, int cursor_type);

/** Sets the console cursor width.
	@param c Console
	@param cursor_width The cursor line width. This only applies to cursors
	1 and 2 (underscore and vertical bar). All others cursor types will not
	be affected by this setting.
	
	For the underscore cursor, it must be greater than 0 and less than or
	equal to CHAR_HEIGHT (as defined by	charmap.h). At the time of writing,
	CHAR_HEIGHT is 8. Therefore, for an underscore cursor,
	0 < cursor_width <= 8.
	
	For a vertical bar cursor, it must be greater than 0 and less than or
	equal to CHAR_WIDTH (as defined by charmap.h). At the time of writing,
	CHAR_WIDTH is 6. Therefore, for a vertical bar cursor, 0 < cursor_width < 6.
	
	If you wish to draw a blank cursor, you probably should disable the cursor
	altogether with EnableCursor(nio_console, FALSE).
	
	Note that if you specify an out-of-range value, NspireIO will silently fail
	and reset the cursor width to 1.
*/
void nio_cursor_width(nio_console* c, int cursor_width);

/** Sets the console cursor width.
	@param c Console
	@param cursor_data The custom cursor data. This is in the form of a char[6]
	array. This pretty much uses the format (and the drawing code) for character
	drawing, so take a look at charmap.h for examples. Note that the characters
	in charmap.h are truncated, so they will display differently.
	
	By default, if this is not specified and the cursor type is set to the
	custom cursor type (3), the custom cursor will be set to
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} (a block cursor).
*/
void nio_cursor_custom(nio_console* c, unsigned char cursor_data[6]);

#ifdef NIO_KEEP_COMPATIBILITY
#define nio_InitConsole(a,b,c,d,e,f,g)  nio_init(a,b,c,d,e,f,g,TRUE)
#define nio_DrawConsole                 nio_fflush
#define nio_Clear                       nio_clear
#define nio_PrintChar(a,b)              nio_fputc(b,a)
#define nio_EnableDrawing               nio_drawing_enabled
#define nio_PrintStr(a,b)               nio_fputs(b,a)
#define nio_GetChar                     nio_fgetc
#define nio_GetStr(a,b)                 nio_fgets(b,1000,a)
#define nio_SetColor                    nio_color
#define nio_CleanUp                     nio_free
#define setPixel                        nio_pixel_set
#define putChar                         nio_pixel_putc
#define putStr                          nio_pixel_puts
#define nio_drawstr                     nio_grid_puts
#define nio_drawch                      nio_grid_putc
#define nio_ScrollDown                  nio_scroll
#define nio_DrawChar                    nio_csl_drawchar
#define nio_SetChar                     nio_csl_savechar
#define nio_printf                      nio_fprintf

#define uart_putc                       uart_putchar
#define uart_getc                       uart_getchar

#define get_current_time				nio_time_get
#define nio_DrawCursor					nio_cursor_draw
#define nio_EraseCursor					nio_cursor_erase
#define nio_DrawBlinkingCursor			nio_cursor_blinking_draw
#define nio_ResetBlinkingCursor			nio_cursor_blinking_reset
#define nio_EnableCursor				nio_cursor_enable
#define nio_EnableCursorBlink			nio_cursor_blinking_enable
#define nio_SetCursorBlinkDuration		nio_cursor_blinking_duration
#define nio_SetCursorType				nio_cursor_type
#define nio_SetCursorWidth				nio_cursor_width
#define nio_SetCursorCustom				nio_cursor_custom
#endif

#ifdef NIO_REPLACE_STDIO
#define putchar                         nio_putchar
#define puts                            nio_puts
#define getchar                         nio_getchar
#define gets                            nio_gets
#define printf                          nio_printf
#define perror                          nio_perror
#endif

#ifdef UART_REPLACE_STDIO
#define putchar                         uart_putchar
#define puts                            uart_puts
#define getchar                         uart_getchar
#define gets                            uart_gets
#define printf                          uart_printf
#endif

#endif