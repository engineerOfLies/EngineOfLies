#ifndef __EOL_COMPONENT_BUTTON__
#define __EOL_COMPONENT_BUTTON__
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

#include "eol_sprite.h"
#include "eol_component.h"
#include "eol_config.h"
#include "eol_types.h"
#include <glib/glist.h>

typedef struct
{
  eolUint     input;                    /**<if defined, the input will operate as a hotkey*/
  eolUint     hotkeymod;
  eolInt      justify;
  eolBool     centered;
  eolUint     fontSize;
  eolFloat    alpha;
  eolLine     buttonText;               /**<text to display over button...should it be a label component?*/
  eolUint     buttonType;               /**<if its an image, or raw text or both*/
  eolSprite * button[eolButtonStateMax];/**<if defined, it will use these over
  stock button images*/
  eolVec3D    textColor[eolButtonStateMax];
  eolVec3D    backgroundColor;
  eolFloat    backgroundAlpha;
  eolVec3D    highlightColor;
  eolVec3D    pressColor;
}eolComponentButton;

void eol_button_configure(eolConfig *conf);

#endif
