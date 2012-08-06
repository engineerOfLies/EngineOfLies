/*
Copyright 2012 Engineer of Lies
This file is part of the Engine of Lies game engine library

The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

EOL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _EOL_GRAPHICS_
#define _EOL_GRAPHICS_

#include "eol_types.h"
#include <SDL.h>

/**
* @purpose The Engine of Lies graphics library is designed to be the point of
* minimal entry into the engine.  It wraps up initialization of graphical
* systems such as SDL and openGL and handles the calls to configure the
* graphics engine of the system.
*/

/*uint32*/
#define EOL_Red         0xDD0000
#define EOL_Green       0x00DD00
#define EOL_Blue        0x0000DD
#define EOL_Yellow      0xDDDD00
#define EOL_Orange      0xEE5522
#define EOL_Violet      0xDD00DD
#define EOL_Brown       0x663311
#define EOL_Grey        0x888888
#define EOL_DarkRed     0x880000
#define EOL_DarkGreen   0x008800
#define EOL_DarkBlue    0x000088
#define EOL_DarkYellow  0x888800
#define EOL_DarkOrange  0x774411
#define EOL_DarkViolet  0x660066
#define EOL_DarkBrown   0x442205
#define EOL_DarkGrey    0x444444
#define EOL_LightRed    0xFF2222
#define EOL_LightGreen  0x33FF33
#define EOL_LightBlue   0x3333FF
#define EOL_LightYellow 0xFFFF11
#define EOL_LightOrange 0xFFDD11
#define EOL_LightViolet 0xFF55FF
#define EOL_LightBrown  0x886622
#define EOL_LightGrey   0xBBBBBB
#define EOL_Black       0x000000
#define EOL_White       0xFEFEFE
#define EOL_Tan         0xFFDD22
#define EOL_Gold        0xBB9900
#define EOL_Silver      0xAAAABB
#define EOL_YellowGreen 0x44EE01
#define EOL_Cyan        0x00FFFF
#define EOL_Magenta     0xFF00FF

/*color constants:*/
enum eol_colors {
  Eol_Red         = 1,
  Eol_Green       = 2,
  Eol_Blue        = 3,
  Eol_Yellow      = 4,
  Eol_Orange      = 5,
  Eol_Violet      = 6,
  Eol_Brown       = 7,
  Eol_Grey        = 8,
  Eol_DarkRed     = 9,
  Eol_DarkGreen   = 10,
  Eol_DarkBlue    = 11,
  Eol_DarkYellow  = 12,
  Eol_DarkOrange  = 13,
  Eol_DarkViolet  = 14,
  Eol_DarkBrown   = 15,
  Eol_DarkGrey    = 16,
  Eol_LightRed    = 17,
  Eol_LightGreen  = 18,
  Eol_LightBlue   = 19,
  Eol_LightYellow = 20,
  Eol_LightOrange = 21,
  Eol_LightViolet = 22,
  Eol_LightBrown  = 23,
  Eol_LightGrey   = 24,
  Eol_Black       = 25,
  Eol_White       = 26,
  Eol_Tan         = 27,
  Eol_Gold        = 28,
  Eol_Silver      = 29,
  Eol_YellowGreen = 30,
  Eol_Cyan        = 31,
  Eol_Magenta     = 32
};

/**
 * Structure contains data associated with the screen.  It may be important to
 * other components to know this type of data.
 */
typedef struct
{
  eolUint           xRes;           /**<horizontal screen resolition*/
  eolUint           yRes;           /**<vertical screen resolution*/
  eolUint           depth;          /**<screen bit depth*/
  eolUint           aMask;          /**<bit mask for alpha*/
  eolUint           rMask;          /**<bit mask for red*/
  eolUint           gMask;          /**<bit mask for green*/
  eolUint           bMask;          /**<bit mask for blue*/
  eolBool           openGLOn;       /**<1 if rendering with openGL,0 uses SDL*/
  eolBool           mipmapping;     /**<true if the engine is using mipmapping*/
  GLint             viewPort[4];    /**<GL view port matrix*/
  GLdouble          modelView[16];  /**<GL model view matrix*/
  GLdouble          projection[16]; /**<GL Projectio matrix*/
}eolGraphicsView;

/**
* @brief Initializes the graphics system for EOL.
* Will load up the default config file in system/graphics.cfg and set up the
* rending system based on the settings in the config file.
*/
void eol_graphics_init();

/**
 * @breif loads up graphics config information from file or sets defaults
 */
void eol_graphics_config();

/**
* @brief Query the graphics ready state.
* @return eolTrue if ready, eolFalse otherwise.
*/
eolBool eol_graphics_initialized();


/**
* @brief relaunches the game screen in the new screen resolution. Invoked an
* registered callbacks.
*
* @param w the new width for the screen
* @param h the new height for the screen
* @param fs set true for fullscreen mode or false for window mode.
*/
void eol_resize_graphics(eolUint w, eolUint h, eolBool fs);

/**
* @brief Sets up a function that will be called after any change to the internal
* graphics settings is made.  It will call the function once to set up initial
* states.
*
* @param callback the function to be invoked when the graphics are resized.
*/
void eol_graphics_register_resize(void (*callback)(eolGraphicsView info));

/*
* @brief takes a screen shot and saves it to filename specififed
*
* @param file the name of the file to save over / create for the screen shot.
*/
void eol_graphics_save_screen_shot(const char *file);

/**
* @brief Given an index, it returns a hex code that corresponds to that color
* @param color the color from eol_colors
* @return the hex EOL_defined useful in direct graphics calls.
*/
eolUint eol_index_color(eolUint color);

/**
 * @brief blanks the drawing buffer to the preset background color.  Should be
 * called befor starting any frame buffer drawing.
 */
void eol_graphics_frame_begin();

/**
* @brief blit the contents of the draw buffer to the screen and toggle the draw
* buffers.  It will prepare a freshly cleared drawing buffer for the next frame
*/
void eol_graphics_frame_end();

/**
 * @brief returns the hax color value of the pixel at the given position in the
 * provided sdl surface.  Note that the surface will need to be locked while
 * this operation is being performed.
 *
 * @param surface the sdl surface that holds the pixel data
 * @param x the x position to check
 * @param y the y position to check
 *
 * @return the hex color value of the color found.
 */
eolUint eol_graphics_get_pixel(SDL_Surface *surface, eolUint x, eolUint y);

/**
 * @brief change a specific pixel in an SDL surface.
 *
 * @param surface a valid SDL_Surface that will be written to
 * @param x the x position in the image
 * @param y the y position in the image
 * @param pixel the hex value of the color to be drawn.
 *
 */
void eol_graphics_put_pixel(
    SDL_Surface *surface,
    eolUint x,
    eolUint y,
    eolUint pixel
  );

/**
 * @brief retrieve the viewports active pixel format
 *
 * @return a pointer to the advice pixel format data.  DO NOT WRITE TO IT.
 */
SDL_PixelFormat *eol_graphics_get_pixel_format();

/**
 * @brief retrieve the current calculated FPS
 *
 * @return the calculated frame per second
 */
eolFloat eol_graphics_get_FPS();

/**
 * @brief retrieves the current graphics view state
 *
 * @param view output.  Current view is loaded into the structure passed.
 */
void eol_graphics_get_view(eolGraphicsView *view);

/**
 * @brief retrieves the current graphics frame time index
 */
eolUint eol_graphics_get_now();

/**
 * @brief retrieves the current graphics frame time index
 * @param w the output width of the view screen
 * @param h the output height of the view screen
 */
void eol_graphics_get_size(eolUint * w, eolUint * h);

#endif

