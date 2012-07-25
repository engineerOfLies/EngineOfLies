#ifndef __EOL_DRAWSHAPES__
#define __EOL_DRAWSHAPES__
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

void eol_draw_trail(eolTrail * trail,
                    eolFloat   radius,
                    eolBool    fade,
                    eolBool    taper);

void eol_draw_dot_3D(eolVec3D point,
                     eolFloat radius,
                     eolVec3D color,
                     eolFloat alpha);

void eol_draw_cirlce_3D(eolVec3D point,
                        eolFloat radius,
                        eolUint  segments,
                        eolVec3D color,
                        eolFloat alpha);

void eol_draw_line_3D(eolVec3D p1,
                      eolVec3D p2,
                      eolFloat radius,
                      eolVec3D color,
                      eolFloat alpha);

void eol_draw_tri_3D(eolVec3D p1,
                     eolVec3D p2,
                     eolVec3D p3,
                     eolVec3D color,
                     eolFloat alpha);

/**
 * @brief draws a rectangle on the screen in 2D space
 * @param rect the defined rectangle to draw
 * @param color the color of the drawn rect
 * @param alpha the translucency of the drawn rect 
 */
void eol_draw_solid_rect(eolRect rect,eolVec3D color,eolFloat alpha);

void eol_draw_rect_3D(eolRectFloat rect, eolOrientation ori);

#endif
