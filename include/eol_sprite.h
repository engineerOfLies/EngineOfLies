#ifndef __EOL_SPRITE_H__
#define __EOL_SPRITE_H__
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

#include "eol_graphics.h"
#include <SDL_image.h>

/**
  data structure to handle sprite data
*/
typedef struct
{
  eolUint       _refCount;         /**<internal ref count.  DO NOT ACCESS*/
  GLuint        _glImage;          /**<opengl texture handle*/
  SDL_Surface * _sdlSurface;       /**<SDL_surface loaded.*/
  eolLine       filename;          /**<name with path of the file loaded*/
  eolUint       framesPerLine;     /**<How many frames are on each line of the
                                       sprite sheet*/
  eolUint       numFrames;         /**<How Many frames are in the sprite*/
  eolUint       imageWidth;        /**<Width of the image in pixels*/
  eolUint       imageHeight;       /**<Height of the image in pixels*/
  eolFloat      texWidth;          /**<Width of the image as a percentage of the
                                       texture*/
  eolFloat      texHeight;         /**<Height of the image as a percentate of
                                       the texture*/
  eolDouble     x3D;   /**<width of the texture relative to gl
                                       coordinate space*/ 
  eolDouble     y3D;  /**<height of the texture relative to gl
                                       coordinate space*/ 
  eolDouble     z3D;  /**<depth of the texture relative to gl
                                       coordinate space*/ 
  eolUint       frameWidth;        /**<width of an individual frame in pixels*/
  eolUint       frameHeight;       /**<Height of an individual frame in pixels*/
  eolFloat      frameTextureWidth; /**<width of an individual frame as a
                                       pecentage of the texture*/
  eolFloat      frameTextureHeight;/**<height of an individual frame as a
                                       percentage of the texture*/
}eolSprite;

/**
 * @brief loads sprite configuration from file, or sets up defaults if no config
 * file found.
 */
void eol_sprite_load_config();
/**
 * @brief set up the sprite system for use.  Must be called before any sprite
 * functions are used.  eol_graphics_init() must have been previously called.
 */
void eol_sprite_init();

/**
 * @brief Load the sprite from file, or if it has already been loaded, get a
 * pointer to the one in memory
 *
 * @param filename the sprite file to load
 * @param frameWidth the width of an individual frame in pixels
 * @param frameHeight the height of an individual frame in pixels
 */
eolSprite *eol_sprite_load(
    eolLine filename,
    eolUint frameWidth,
    eolUint frameHeight
  );

/**
 * @brief free the given sprite
 *
 * @param sprite a pointer to the sprite pointer.  the sprite will be freed and the
 * pointer to the sprite will be set to NULL;
 */
void eol_sprite_free(eolSprite **sprite);

/**
 * @brief draw a given sprite to the screen buffer with the given transformation
 * information
 *
 *  @param sprite the sprite to be drawn
 *  @param frame the frame to draw
 *  @param x the x position to draw the sprite
 *  @patam y the y position to draw the sprite
 *  @param scaleX the x scaling factor to be aplied
 *  @param scaleY the y scaling factor to be aplied
 *  @param rotation the rotation, in degrees to draw the sprite
 *  @param hFlip if true, the sprite will be flipped horizontally
 *  @param vFlip if true, the sprite will be flipped vertically
 *  @param alpha the transparency of the draw.  0 is clear, 1 is solid
 */
void eol_sprite_draw_transformed(
    eolSprite *sprite,
    eolUint frame,
    eolInt x,
    eolInt y,
    eolFloat scaleX,
    eolFloat scaleY,
    eolFloat rotation,
    eolBool hFlip,
    eolBool vFlip,
    eolFloat alpha
  );

/**
 * @brief draw a given sprite to the screen buffer
 *
 *  @param sprite the sprite to be drawn
 *  @param frame the frame to draw
 *  @param x the x position to draw the sprite
 *  @patam y the y position to draw the sprite
 */
void eol_sprite_draw(
  eolSprite *sprite,
  eolUint    frame,
  eolInt     x,
  eolInt     y
);

/**
 * @brief draw a sprite in 3D space
 *
 * @param sprite the sprite to draw
 * @param frame the frame to draw
 * @param position the position in space to draw the sprite
 * @param scale the scale to apply to the sprite.  1,1,1 is no scale
 * @param rotation the orientation to apply to the sprite
 * @param alpha the transparency of the draw.  0 is clear, 1 is solid
 */
void eol_sprite_draw_transformed_3d(
    eolSprite *sprite,
    eolUint frame,
    eolVec3D position,
    eolVec3D scale,
    eolVec3D rotation,
    eolFloat alpha
  );

/**
 * @brief draw a sprite in 3D space oriented towards the camera in "billboard"
 * style
 *
 * @param sprite the sprite to draw
 * @param frame the frame to draw
 * @param position the position in space to draw the sprite
 * @param scale the scale to apply to the sprite.  1,1,1 is no scale
 * @param alpha the transparency of the draw.  0 is clear, 1 is solid
 */
void eol_sprite_3d(
    eolSprite *sprite,
    eolUint frame,
    eolVec3D position,
    eolVec3D scale,
    eolFloat alpha
  );

/**
 * @brief creates a blank sprite and returns it.  surface and gl texture data
 * are not set.  This could use be used in a custome sprite set up such as
 * storing the test rendered from the font system.
 */
eolSprite *eol_sprite_new();

#endif
