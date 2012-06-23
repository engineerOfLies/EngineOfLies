#ifndef __EOL_MODEL__
#define __EOL_MODEL__
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

/**
 * @purpose an eolModel type contains all the possible graphics and animation
 *          information that is needed for an animated object.  Like sprites and
 *          models themselves, a single actor object can be referenced by many.
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
  eolLine        name;               /**<name of the model, likely the filename*/
  eolMesh      * _mesh;              /**<optional pointer to the 3d Mesh data*/
  eolLine        _meshFile;          /**<file to load for a mesh*/
  eolSprite    * _skin;              /**<optional pointer to the mesh skin sprite*/
  eolLine        _skinFile;          /**<file to load for a mesh skin*/
  eolArmature  * _arm;               /**<optional pointer to the 3d armature data*/
  eolLine        _armFile;           /**<file to load for a mesh armature*/
  eolSprite    * _sprite;            /**<optional pointer to the rendered sprite*/
  eolLine        _spriteFile;        /**<file to load for a sprite*/
  eolBool        _sprite_3D;      /**<sprites are assumed to be 2D unless this is true*/
  eolUint        numActions;         /**<number of actions for this model*/
  eolAction    * _actionList;        /**<allocated list of actions*/
}eolModel;

/**
 * @brief model system init
 */
void eol_model_init();

/**
 * @brief loads up model system config
 */
void eol_model_config();

/**
 * @brief closes model sytem
 */
void eol_model_close();

/**
 * @brief retrieves model system init status
 * @return eolTrue if initialized, false otherwise
 */
eolBool eol_model_initialized();

/**
 * @brief loads an model from file and returns a pointer to it.
 * based on the file information, it will also load up mesh, sprite, etc data.
 * @param filename the model file to load
 * @return a pointer to the loaded model file or NULL if not found or error
 */
eolModel *eol_model_load(char *filename);

/**
 * @brief frees the model provided and sets your pointer to it to NULL
 *
 * @param model a pointer to a model pointer
 */
void eol_model_free(
    eolModel **model
);

/**
 * @brief sets the model's frame change rate.  this is in float to allow
 * granular speed up and slowdown of an animation
 * Note that actions have their own frame rates that will combine with the
 * model frame rate to determine the final change in frames.
 *
 * @param model the model whose frame rate will change
 * @param rate the amount of change.  Note that a rate of 0 means the model
 * will not animate
 */
void eol_model_set_frame_rate(
    eolModel *model,
    eolFloat rate
  );
  
/**
 * @brief set the model to the begining of the specified action
 *
 * @param model the model to modify
 * @param action the name of the action to set it to.
 *        If name is not found, nothing changes.
 */
void eol_model_set_action(
    eolModel * model,
    eolWord    action
  );

/**
 * @brief steps an model to the next frame in an action.  lets caller know if
 *        if the action has reached the end.
 * if the model was already at the end, it will...
 *        if type is loop: set the frame back to the beginning.
 *        if type is osci: sets the animation direction to -1
 *        if type is pass: frame is not changed
 *
 * @param model the model to animate
 * @return eolTrue if the action has reached the end, false otherwise.
 */
eolBool eol_model_next_frame(eolModel * model);

/**
 * @brief retrieve the current frame of an model
 *
 * @param model the model to retrieve the frame from
 *
 * @return a floating point representing the frame.  Note that when drawing,
 *         it will snap to the floor.
 */
eolFloat eol_model_get_frame(eolModel *model);

/**
 * @brief draw the model under the current conditions
 *
 * @param model pointer to the model to draw
 * @param position the place to draw the model
 * @param rotation how the model is rotated
 * @param scale how the model is scaled
 * @param color color shifting of the model (1,1,1) is no change.
 * @param alpha the tranlucency of the model 0 is clear, 1 is opaque
 * @param frame the frame to draw this model at
 */
void eol_model_draw(
    eolModel *model,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha,
    eolUint  frame
  );

/**
 * @brief searches for the model's action by name
 *
 * @param model the model to search
 * @param aName the name of the action to search for
 *
 * @return a pointer to the action or NULL if not found
 */
eolAction * eol_model_get_action(
    eolModel *model,
    eolWord   aName
  );

#endif

