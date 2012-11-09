#ifndef __EOL_PARTICLE_EMITTER__
#define __EOL_PARTICLE_EMITTER__
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
#include "eol_config.h"
#include "eol_particle.h"
#include "eol_entity.h"

enum eolEmitterShapes {
  eolEmmitterPoint  = 0,
  eolEmmitterSquare = 1,
  eolEmmitterCircle = 2,
  eolEmmitterSphere = 3,
  eolEmmitterModel  = 4,
  eolEmmitterActor  = 5
};

/**
 * @brief here is a list of options that may occur in the config:
 * @param entityLiftime how long the entity should live
 * @param entityRotationVector change of entity rotation
 * @param entityColorVector change in color for entity
 * @param entityAlphaVector change in alpha for entity
 * @param type [point, sprite, actor]
 * @param actor path to the actor to use for this particle
 * @param count uint how many particles are to be created  (for entity, this is per update)
 * @param lifetime uint how long the particles should live before timing out in server ticks
 * @param lifetimeVariance float how much variation should be applied to lifetime
 * @param shape [point|square|circle|sphere] shape of the creation point for the particles
 * @param vector vec3D direction the particles should move in (0,0,0 is viable)
 * @param vectorVariance vec3D percentage that the vector will vary by component
 * @param accelation vec3D direction the particles will acclerate in (0,0,0 is viable)
 * @param AccelVariance vec3D percentage that the acceleration will vary by component
 * @param color vec3D the default color that this should be (will be multiplied by input color)
 * @param colorVariance vec3D percentage that the color will vary by component
 * @param colorVector vec3D change in color over time
 * @param colorVector vec3D change in color over time
 * @param alpha float transparecy
 * @param alphaVariance float percentage that the alpha will vary by component
 * @param alphaVector float change in alpha over time
 * @param drawTrail
 * @param trailTaper
 * @param trailFade
 */

/**
 * @brief spawn a bunch of particles based on a config setup
 * @param config the loaded config file that contains the particle emitter info
 * @param point the point where all the particles should be created from
 * @param rotation the direction the particles will move in (factors of config will affect this)
 * @param color the color the particles should be (config will affect this)
 * @param alpha the transparency of the particles created (config affects this)
 */
void eol_particle_emitter_from_config(eolConfig *config,
                                      eolVec3D point,
                                      eolVec3D rotation,
                                      eolVec3D color,
                                      eolFloat alpha);

/**
 * @brief spawn an entity that will emit particles over its lifetime based on a config setup
 * @param configfile the file to load for the config
 * @param point the point where all the particles should be created from
 * @param vector the direction the entity will move in
 * @param color the color the particles should be (config will affect this)
 * @param alpha the transparency of the particles created (config affects this)
 * @return a pointer to the set up entity or NULL on error.
 * NOTE: the entity will not be added to any space for collision, as particles tend not to interact.
 * You are given the pointer to the entity to do with what you will however.
 */
eolEntity * eol_particle_spawn_emitter_from_config(eolLine  configfile,
                                                   eolVec3D point,
                                                   eolVec3D vector,
                                                   eolVec3D rotation,
                                                   eolVec3D color,
                                                   eolFloat alpha);

#endif
