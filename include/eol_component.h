#ifndef __EOL_COMPONENTS__
#define __EOL_COMPONENTS__
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

#include "eol_font.h"
#include "eol_sprite.h"
#include "eol_actor.h"
#include "eol_types.h"

/**
 * @purpose This will create the components used for window construction.
    Similar in concept to a gtkWidget, but severely limited to the components
    needed in a game
 */

enum eolComponentTypes {
  eolNullComponent   = 0,  /**<this type is set by default*/
  eolLabelComponent  = 1,
  eolButtonComponent = 2,
  eolInputComponent  = 3,
  eolSliderComponent = 4,
  eolImageComponent  = 5,
  eolModelComponent  = 6,
  eolListComponent   = 7
};

enum eolButtonStates {
  eolButtonIdle       = 0,
  eolButtonHighlight  = 1,
  eolButtonPressed    = 2
};

enum eolTextJustify {
  eolJustifyLeft      = 0,
  eolJustifyCenter    = 1,
  eolJustifyRight     = 2
};

#define eolButtonStateMax 3

/**
  @brief this structure serves as a header for all components
 */
typedef struct
{
  eolUint   id;
  eolWord   name;
  eolRect   rect;
  eolBool   hasFocus;
  eolInt    state;
  eolUint   type;
  void    * componentData;
}eolComponent;

typedef struct
{
  char     * buffer;
  eolInt     justify;
  eolBool    wordWrap;
  eolUint    fontSize;
  eolVec3D   color;
  eolFont  * font;    /**<if defined, it will use the custom font to draw text*/
}eolComponentLabel;

typedef struct
{
  eolLine     buttonText;
  eolSprite * button[eolButtonStateMax];/**<if defined, it will use these over
                                            stock button images*/
}eolComponentButton;

typedef struct
{
  char    * buffer;
  eolInt    bufferLimit; /**<if -1 no limit, otherwise its the maximum character
                            that will be added to buffer*/
  eolUint   fontSize;    /**<size of the font to use when displaying the text*/
  eolFont * font;    /**<if defined, it will use the custom font to draw text*/
  eolBool   number; /**<if true, limits input to 0-9 - and .*/
}eolComponentTextInput;

typedef struct
{
  eolActor * actor;
}eolComponentActor;

#endif
