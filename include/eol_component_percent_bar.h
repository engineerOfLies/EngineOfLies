#ifndef __EOL_COMPONENT_PERCENT_BAR__
#define __EOL_COMPONENT_PERCENT_BAR__
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
#include <glib/glist.h>

typedef struct
{
  eolBool     vertical;
  eolBool     ascending;
  eolFloat  * max;
  eolFloat  * value;
  eolFloat    percent;
  eolVec3D    statusColor;
  eolVec3D    backgroundColor;
  eolFloat    statusAlpha;
  eolFloat    backgroundAlpha;
}eolComponentPercentBar;


#endif
