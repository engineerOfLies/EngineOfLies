#ifndef __EOL_FONT__
#define __EOL_FONT__
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

#include "eol_types.h"
#include <SDL_ttf.h>

/**
 * @brief these enumerations will be used as a shorthand access to the default
 *        loaded fonts at different point sizes
 */
enum eolFontSizes {
  eolFontTiny   = 0,  /**<10 point loaded font*/
  eolFontSmall  = 1,  /**<12 point loaded font*/
  eolFontMedium = 2,  /**<14 point loaded font*/
  eolFontLarge  = 3,  /**<18 point loaded font*/
  eolFontHuge   = 4   /**<24 point loaded font*/
};
/*below should be one larger than largest font*/
#define eolFontSizeMax 5

enum eolTextJustify {
  eolJustifyLeft      = 0,
  eolJustifyCenter    = 1,
  eolJustifyRight     = 2
};

typedef struct
{
  eolUint    _refCount;
  eolLine    filename;
  eolInt     point;
  eolInt     index;
  TTF_Font * _font;
}eolFont;

void eol_font_init();
eolBool eol_font_initialized();
void eol_font_config();
void eol_font_close();

/**
 * @brief loads a specific ttf font file from disk.  Returns a pointer to the
 *        loaded font or NULL on failure.
 * @param filename the file of the font to load.
 * @param pointSize the pointsize to load the font at
 */
eolFont * eol_font_load(
    char   *filename,
    eolInt  pointSize
  );

/**
 * @brief frees a font resource and sets the provided pointer to NULL
 * 
 * @param a pointer to a font pointer.
 */
void eol_font_free(
    eolFont **font
  );

/**
 * @brief Renders text to screen using the default font of the specified size
 *
 * @param text the text to render.  does nothing if NULL or string length is zero
 * @param x the x position (in pixels) to draw from (left).
 * @param y the y position (in pixels) to draw from (top). 
 * @param color the (r,g,b) color to render the font in
 * @param alpha the translucency to render the font it
 * @param size which of the enumerated sizes to draw the font with
 */
void eol_font_draw_text(
    char   * text,
    eolInt   x,
    eolInt   y,
    eolVec3D color,
    eolFloat alpha,
    eolUint  size
  );
  
/**
 * @brief same as eol_font_draw_text, except it will be right justified.
 * @param x the x position (in pixels) to draw from (right).
 * @param y the y position (in pixels) to draw from (top). 
 * @param color the (r,g,b) color to render the font in
 * @param alpha the translucency to render the font it
 * @param size which of the enumerated sizes to draw the font with
 * @param justify see enumerations for how it will be justified
 */
void eol_font_draw_text_justify(
    char   * text,
    eolInt   x,
    eolInt   y,
    eolVec3D color,
    eolFloat alpha,
    eolUint  size,
    eolUint  justify
  );

void eol_font_draw_text_justify_custom(
    char    * text,
    eolInt    x,
    eolInt    y,
    eolVec3D  color,
    eolFloat  alpha,
    eolFont * font,
    eolUint   justify
  );

/**
 * @brief Renders text to screen using the default font of the specified size
 *
 * @param text the text to render.  does nothing if NULL or string length is zero
 * @param x the x position (in pixels) to draw from (left).
 * @param y the y position (in pixels) to draw from (top). 
 * @param color the (r,g,b) color to render the font in
 * @param alpha the translucency to render the font it
 * @param font a pointer to a specific font to draw with
 */    
void eol_font_draw_text_custom(
    char    *text,
    eolInt   x,
    eolInt   y,
    eolVec3D color,
    eolFloat alpha,
    eolFont *font
  );

/**
 * @brief Renders text to screen using the default font of the specified size
 *
 * @param text the text to render.  does nothing if NULL or string length is zero
 * @param x the x position (in pixels) to draw from (left).
 * @param y the y position (in pixels) to draw from (top). 
 * @param w the width (in pixels) to limit writing to
 * @param h the height (in pixels) to limit writing to.  if 0, no limit applied
 * @param color the (r,g,b) color to render the font in
 * @param alpha the translucency to render the font it
 * @param size which of the enumerated sizes to draw the font with
 */
void eol_font_draw_text_block(
    char   * text,
    eolInt   x,
    eolInt   y,
    eolInt   w,
    eolInt   h,
    eolVec3D color,
    eolFloat alpha,
    eolInt   size
  );

/**
 * @brief Renders text to screen using the default font of the specified size
 *
 * @param text the text to render.  does nothing if NULL or string length is zero
 * @param block the bounding rectangle to limit drawing by.  
 *        X and Y components are used for position
          if the H component is 0, no bottom limit is applied.
 * @param color the (r,g,b) color to render the font in
 * @param alpha the translucency to render the font it
 * @param font a pointer to the specific font to draw with
 */
void eol_font_draw_text_block_custom(
    char   * text,
    eolRect  block,
    eolVec3D color,
    eolFloat alpha,
    eolFont *font
  );

/**
 * @brief returns the bounding rect ( in screen pixels ) of the text.
 * 
 * @param text the text to use in determining the size
 * @param size which default font to use for determining the size
 *
 * @return a rectangle that contains the bounding rectangle of the text.
 *         The x and y components will be empty.
 */
eolRect eol_font_get_bounds(
    char * text,
    eolInt size
  );

/**
 * @brief returns the bounding rect ( in screen pixels ) of the text.
 * 
 * @param text the text to use in determining the size
 * @param font a pointer to the font to use in determining size
 *
 * @return a rectangle that contains the bounding rectangle of the text.
 *         The x and y components will be empty.
 */
eolRect eol_font_get_bounds_custom(
    char    * text,
    eolFont * font
  );

/**
 * @brief returns the bounding rect ( in screen pixels ) of the text.
 * 
 * @param text the text to use in determining the size
 * @param size which default font to use for determining the size
 * @param w the width limit to apply to the block.
 * @param h the height limit to apply to the block.
 * 
 * @return a rectangle that contains the bounding rectangle of the text.
 *         The x and y components will be empty.  Note that the w and h 
 *         provided may not be the actual bounds produced.
 */
eolRect eol_font_get_block_bounds(
    char * text,
    eolInt size,
    eolUint w,
    eolUint h
  );

/**
 * @brief returns the bounding rect ( in screen pixels ) of the text.
 * 
 * @param text the text to use in determining the size
 * @param font pointer of the font to use in determining the size
 * @param w the width limit to apply to the block.
 * @param h the height limit to apply to the block.
 * 
 * @return a rectangle that contains the bounding rectangle of the text.
 *         The x and y components will be empty.  Note that the w and h 
 *         provided may not be the actual bounds produced.
 */
eolRect eol_font_get_block_bounds_custom(
    char    * text,
    eolFont * font,
    eolUint   w,
    eolUint   h
  );


void eol_font_draw_text_3D(
    char *text,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha,
    eolInt   size
  );

void eol_font_draw_text_3D_custom(
    char *text,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha,
    eolFont *font
  );

void eol_font_draw_text_3D_block(
    char *text,
    eolRect  block,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha,
    eolInt   size
  );

void eol_font_draw_text_3D_block_custom(
    char *text,
    eolRect  block,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha,
    eolFont *font
  );

eolUint eol_font_get_text_height_average(eolUint fontsize);

eolUint eol_font_get_text_height_average_custom(eolFont *font);

#endif

