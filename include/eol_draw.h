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

/**
 * @brief gets the 3d space coordinates based on 2d screen coordinates
 * @param out output if NULL, this is a no-op
 * @param in the input coordinates
 */
void eol_draw_get_3d_position(eolVec3D *out,eolVec2D in);

/**
 * @brief gets the 2d screen coordinates based on 3d space coordinates
 * @param out output if NULL, this is a no-op
 * @param in the input coordinates
 */
void eol_draw_get_2d_position(eolVec2D *out,eolVec3D in);

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

void eol_draw_cirlce_2D(eolVec2D point,
                        eolFloat radius,
                        eolUint  segments,
                        eolVec3D color,
                        eolFloat alpha);

void eol_draw_line_3D(eolVec3D p1,
                      eolVec3D p2,
                      eolFloat radius,
                      eolVec3D color,
                      eolFloat alpha);

void eol_draw_line_2D(eolVec2D p1,
                      eolVec2D p2,
                      eolFloat radius,
                      eolVec3D color,
                      eolFloat alpha);

void eol_draw_tri_3D(eolVec3D p1,
                     eolVec3D p2,
                     eolVec3D p3,
                     eolVec3D color,
                     eolFloat alpha);

/**
 * @brief draw a rectangle in 3D space described by the 2D float rect
 * @param rect the rect to draw
 * @param radius the thickness of the lines to draw
 * @param ori the orientation to transform the rect by
 */
void eol_draw_rect_3D(eolRectFloat rect, eolUint radius, eolOrientation ori);

void eol_draw_rect_3D_solid(eolRectFloat rect, eolUint radius, eolOrientation ori);

/**
 * @brief draws a rectangle on the screen in 2D space
 * @param rect the defined rectangle to draw
 * @param color the color of the drawn rect
 * @param alpha the translucency of the drawn rect 
 */
void eol_draw_solid_rect(eolRect rect,eolVec3D color,eolFloat alpha);

void eol_draw_rect(eolRect rect,eolVec3D color,eolFloat alpha);


#endif
