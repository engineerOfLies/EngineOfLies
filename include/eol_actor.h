#ifndef __EOL_ACTOR__
#define __EOL_ACTOR__
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

#include "eol_model.h"

/**
 * @purpose the purpose of an actor type is to provide a specific instance of an
 *          animated model.  Actors might be used by entities, particles or UI
 * unlike other resources, the actor is not meant to be allocated or de-allocated.
 */

typedef struct
{
  eolLine     name;
  eolModel  * model;
  eolWord     actionName;
  eolAction * action;
  eolFloat    frame;
  eolFloat    frameRate;
  eolInt      frameDirection;
}eolActor;

/**
 * @brief loads an actor from file and returns a pointer to it.
 * based on the file information, it will also load up mesh, sprite, etc data.
 *
 * @param act the actor data to load into.  It will clear any existing data.
 * @param filename the model file to load
 */
void eol_actor_load(eolActor *act, char *filename);

/**
 * @brief clears any of the actor information at the location specified
 */
void eol_actor_clear(eolActor *act);

/**
 * @brief allocated empty actor data;
 *
 * @return a pointer to a valid empty actor or NULL on error
 */
eolActor * eol_actor_new();

/**
 * @brief frees all data pointed to by actor
 *
 * @param actor a pointer to an actor pointer.  Make sure data can be freed.
 */
void eol_actor_free(eolActor **actor);

/**
 * @brief sets the actor's frame change rate.  this is in float to allow
 * granular speed up and slowdown of an animation
 * Note that actions have their own frame rates that will combine with the
 * actor frame rate to determine the final change in frames.
 *
 * @param act the actor whose frame rate will change
 * @param rate the amount of change.  Note that a rate of 0 means the actor
 * will not animate
 */
void eol_actor_set_frame_rate(
    eolActor *act,
    eolFloat rate
  );
  
/**
 * @brief set the actor to the begining of the specified action
 *
 * @param act the actor to modify
 * @param action the name of the action to set it to.
 *        If name is not found, nothing changes.
 */
void eol_actor_set_action(
    eolActor * act,
    eolWord    action
  );

/**
 * @brief steps an actor to the next frame in an action.  lets caller know if
 *        if the action has reached the end.
 * if the actor was already at the end, it will...
 *        if type is loop: set the frame back to the beginning.
 *        if type is osci: sets the animation direction to -1
 *        if type is pass: frame is not changed
 *
 * @param act the actor to animate
 * @return eolTrue if the action has reached the end, false otherwise.
 * looping never returns false
 * pass returns true after a single pass
 * oscillate returns true after a single pass forward and back.
 */
eolBool eol_actor_next_frame(eolActor * act);

/**
 * @brief retrieve the current frame of an actor
 *
 * @param act the actor to retrieve the frame from
 *
 * @return a floating point representing the frame.  Note that when drawing,
 *         it will snap to the floor.
 */
eolFloat eol_actor_get_frame(eolActor *act);

/**
 * @brief draw the actor under the current conditions
 *
 * @param act pointer to the actor to draw
 * @param position the place to draw the actor
 * @param rotation how the actor is rotated
 * @param scale how the actor is scaled
 * @param color color shifting of the actor (1,1,1) is no change.
 * @param alpha the tranlucency of the actor 0 is clear, 1 is opaque
 */
void eol_actor_draw(
    eolActor *act,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha
  );

#endif

