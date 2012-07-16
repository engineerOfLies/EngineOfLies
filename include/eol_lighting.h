#ifndef __EOL_LIGHTING__
#define __EOL_LIGHTING__
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

enum eolLightTypes {
  eolLightArea,
  eolLightSpot,
  eolLightAmbient,
  eolLightDirectional
};

typedef struct
{
  eolUint  _inUse;
  eolUint  type;
  eolInt   lightIndex; /**<the GL_LIGHT* index  if -1 light is invalid*/
  eolBool  on;
  eolFloat radius;
  eolVec3D position;
  eolVec3D colorSpec;
  eolVec3D colorAmbi;
  eolVec3D colorDiff;
  eolFloat constantAttenuation;
  eolFloat linearAttenuation;
  eolFloat QuadraticAttenuation;
  eolVec3D direction;
  eolFloat exponent;
  eolFloat angle;
}eolLight;

/**
 * @brief configures the lighting system
 */
void eol_lighting_config();

/**
 * @brief initializes the lighting system
 */
void eol_lighting_init();

/**
 * @brief retrieves initialization status for the lighting system
 */
eolBool eol_lighting_initialized();

/**
 * @brief allocates a new light and returns a pointer to it.
 */
eolLight * eol_light_new();

/**
 * @brief frees the passed in light and sets pointer to NULL
 *
 * @param light a pointer to a light pointer to be freed
 */
void eol_light_free(eolLight **light);

/**
 * @brief allocates new lights of the type specified
 */
eolLight *eol_light_area_new();
eolLight *eol_light_spot_new();
eolLight *eol_light_ambient_new();
eolLight *eol_light_directional_new();

/**
 * @brief Any changes to the lighting struct will not be applied to the rendering light
 * until this is called.
 */
void eol_light_update(eolLight *light);

/**
 * @brief sets up a basic 3 point lighting system useful for most applications
 * use the rep plot when you dont need specific lights, but want to see your 3D
 * scene.  Destroy it will a call to eol_lighting_clear();
 */
void eol_lighting_setup_rep_plot();

/**
 * @brief removes all active lights.  Used during scene transition.  WARNING, it
 * will invalidate all active pointers to lights.
 */
void eol_lighting_clear();

/**
 * @brief turns on or off a light without killing the pointer.  
 */
void eol_light_enable(eolLight *light,eolBool enable);

/**
 * @brief points a spot light at a specific point in 3d space
 * does nothing unless light type is spot
 * @brief the point to point the light at.  Note that if the point is equal to
          the light's position, it will do nothing.
 */
void eol_light_point_at(eolLight *light,eolVec3D point);

/**
 * @brief sets lighting position to the point specified.
 * does nothing if spot is ambient or directional
 * @param light the light to move
 * @Param point the point to move the light to
 */
void eol_light_move(eolLight *light,eolVec3D point);

/**
 * @brief sets the ambient diffuse and specular lighting to color
 * @param light  the light whose color needs to be changed.
 * @param color color as mapped from (x,y,z) to (r,g,b)
 */
void eol_light_set_color(eolLight *light,eolVec3D color);

/**
 * @brief sets the light radius for a light.  The light is disabled for anything
 * outside the radius of the light.
 */
void eol_light_set_radius(eolLight *light,eolFloat radius);

/**
 * @brief goes through each light and enables / disabled them based on distance
 * to the point passed.  Only applies to position based lights.
 * @param pos the position to check against
 */
void eol_lighting_filter_for_position(eolVec3D pos);

/**
 * @brief turns light on and enables it for rendering.
 * @param light the light to turn on
 */
void eol_light_turn_on(eolLight *light);

/**
* @brief turns light off and disables it for rendering.
* @param light the light to turn off
*/
void eol_light_turn_off(eolLight *light);

/**
 * @brief draws a point at the position of the light in the light's color
 */
void eol_light_draw();

#endif
