#ifndef __EOL_PARTICLE__
#define __EOL_PARTICLE__
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
#include "eol_actor.h"
/**
 * @purpose the particle system provides short lived, dumb, temporary entities.
 * particles do not think, they do not interact with physics.  They exist purely
 * to provide special effects such as bullet trails, explosions, water sprays, etc.
 */
enum eolParticleTypes {
  eolParticlePoint,
  eolParticleTrail,
  eolParticleSprite,
  eolParticleModel,
  eolParticleActor
};

typedef struct
{
  eolUint        type;      /**<which particle type.  see eolParticleTypes*/
  eolTrail       trail;     /**<orientation trail.  Only maintained if drawTrail is set*/
  eolOrientation ori;       /**<orientation now*/
  eolOrientation vector;    /**<orientation change vector*/
  eolOrientation accel;     /**<orientation change vector change vector*/
  eolFloat       radius;    /**<radius used for point and trail particles.*/
  eolActor     * actor;     /**<defined if particl is a sprite, model or actor*/
  eolUint        lifetime;  /**<number of animation frames left to live.*/
  eolBool        drawTrail; /**<if trails should be drawn/maintained*/
  eolBool        fade;      /**<if true, the trail will fade to clear*/
  eolBool        taper;     /**<if true, the trail will dwindle in size*/
}eolParticle;

/*
  Note on acceleration: useful for forces like wind or gravity.  Can be used to
  make effects like fire embers changing from red to yellow to grey...
*/

/**
 * @brief initializes the particle system based on the loaded configuration.
 */
void eol_particle_init();

/**
 * @brief retrieves the initialization status of the particle system
 */
eolBool eol_particle_initialized();

/**
 * @brief loads the global particle configuration.  such as max number of particles.
 */
void eol_particle_config();

/**
 * @brief resets all particles to a zero state.  Useful to stop any particles between levels.
 */
void eol_particle_clear();

/**
 * @brief draws all living particles.
 */
void eol_particle_draw_all();

/**
 * @brief updates the orientations / ticks down the life of all particles.
 */
void eol_particle_update_all();

/**
 * @brief get a pointer to a new particle
 * You do not own the memory location returned.  Do not hold onto it.  Set it
 * and forget it.
 * @return a pointer a newly assigned particle or NULL when no more room.
 * 
 */
eolParticle * eol_particle_new();

void eol_particle_make_point(eolVec3D position,
                             eolVec3D velocity,
                             eolVec3D accel,
                             eolFloat radius,
                             eolVec3D color,
                             eolFloat alpha,
                             eolUint  lifetime);

void eol_particle_make_trail(eolVec3D position,
                             eolVec3D velocity,
                             eolVec3D accel,
                             eolFloat radius,
                             eolVec3D color,
                             eolFloat alpha,
                             eolUint  lifetime);


#endif
