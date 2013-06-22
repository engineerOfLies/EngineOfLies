#ifndef __EOL_COMPONENT_ENTRY__
#define __EOL_COMPONENT_ENTRY__
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

#include "eol_component.h"
#include "eol_types.h"
#include "eol_font.h"
#include <glib.h>

typedef struct
{
  GString * buffer;
  eolInt    bufferLimit; /**<if -1 no limit, otherwise its the maximum character
  that will be added to buffer*/
  eolUint   justify;
  eolBool   wordWrap;
  eolUint   fontSize;    /**<size of the font to use when displaying the text*/
  eolFont * font;    /**<if defined, it will use the custom font to draw text*/
  eolBool   number; /**<if true, limits input to 0-9 - and .*/
  eolVec3D  color;
  eolVec3D  bgcolor;
  eolFloat  alpha;
  eolFloat  cursorBlink;
  eolFloat  cursorBlinkDirection;
  void      (*entry_callback)(void *data,eolLine output);/**< callback when entry is changed*/
  void    * callback_data;/**<custom data to be passed along with the callback*/
}eolComponentEntry;

eolComponent *eol_entry_create_from_config(eolKeychain *def, eolRect parentRect);

#endif
