/*
    Copyright 2012 Donald Kehoe
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
#include <math.h>

eolBool eol_equals(eolDouble a, eolDouble b)
{
	return (fabs(a-b) < EOL_EPSILON)?eolTrue:eolFalse;
}

eolVec3D eol_vec3d(eolDouble x, eolDouble y, eolDouble z)
{
	eolVec3D vec;
  eol_vector_set_3D(x, y, z, vec);
	return vec;
}

eolFloat eol_vec3d_magnitude (eolVec3D V)
{
  return sqrt (V.x * V.x + V.y * V.y + V.z * V.z);
}

void eol_vec3d_normalize (eolVec3D *V)
{
  eolFloat M = eol_vec3d_magnitude (*V);
  
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
  V->z *= M;
}

eolRect eol_rect(
  eolInt  x,
  eolInt  y,
  eolUint w,
  eolUint h
)
{
  eolRect r = {x,y,w,h};
  return r;
}


/*eol @ eof*/

