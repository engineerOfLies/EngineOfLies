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

#include "eol_types.h"
#include "eol_mesh.h"
#include "eol_armature.h"
#include "eol_sprite.h"
#include "eol_resource.h"

/**
 * @purpose the purpose of the action type and associated functions is to
 *          provide a common language for managing the animation of assets
 *          unlike generic resources, actors are unique.  One Entity may have
 *          multiple actors.
 */

enum eolActionTypes {
  eolActionLoop  = 0,/**<Action is meant to loop back to the begining, ie: walk*/
  eolActionPass  = 1,/**<Action is meant to make a pass and then end, ie: attack*/
  eolActionOsci  = 2 /**<Action is meant to oscillate between begin and end, ie: idle*/
};

typedef struct
{
  eolWord name;       /**<the name of the action*/
  eolUint begin;      /**<the begin frame of animation for an action*/
  eolUint end;        /**<the end frame of an action*/
  eolInt  type;       /**<the eolActionType for this action*/
  eolFloat frameRate; /**<the frame rate for this action.  Higher numbers mean
                          faster animation. 0 means no animation*/
}eolAction;


/*NOTE: may want to make meshes and sprites possibly lists...*/
typedef struct
{
  eolLine        name;               /**<name of the actor, likely the filename*/
  eolMesh      * _mesh;              /**<optional pointer to the 3d Mesh data*/
  eolLine        _meshFile;          /**<file to load for a mesh*/
  eolSprite    * _skin;              /**<optional pointer to the mesh skin sprite*/
  eolLine        _skinFile;          /**<file to load for a mesh skin*/
  eolArmature  * _arm;               /**<optional pointer to the 3d armature data*/
  eolLine        _armFile;           /**<file to load for a mesh armature*/
  eolSprite    * _sprite;            /**<optional pointer to the rendered sprite*/
  eolLine        _spriteFile;        /**<file to load for a sprite*/
  eolBool        _sprite_3D;      /**<sprites are assumed to be 2D unless this is true*/
  eolWord        currentActionName;  /**<The name of the current action*/
  eolUint        currentActionIndex; /**<index into the actionList of the current action*/
  eolFloat       currentFrame;       /**<the frame that will next be drawn*/
  eolFloat       frameRate;          /**<ammount a frame changes each round*/
  eolInt         frameDirection;     /**<direction of the animation*/
  eolUint        numActions;         /**<number of actions for this actor*/
  eolAction    * _actionList;        /**<allocated list of actions*/
}eolActor;

/**
 * @brief actor system init
 */
void eol_actor_init();

/**
 * @brief loads up actor system config
 */
void eol_actor_config();

/**
 * @brief closes actor sytem
 */
void eol_actor_close();

/**
 * @brief retrieves actor system init status
 * @return eolTrue if initialized, false otherwise
 */
eolBool eol_actor_initialized();

/**
 * @brief loads an actor from file and returns a pointer to it.
 * based on the file information, it will also load up mesh, sprite, etc data.
 * @param filename the actor file to load
 * @return a pointer to the loaded actor file or NULL if not found or error
 */
eolActor *eol_actor_load(char *filename);

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

/**
 * @brief loads an actor from a .actor file returning a pointer to it
 *
 * @param filename the file to load the actor from
 *
 * @return a pointer to the loaded actor, or NULL if not found or other error
 */
eolActor *eol_actor_load(
    char *filename
  );
#endif

