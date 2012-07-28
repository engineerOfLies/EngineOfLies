#ifndef __EOL_CAMERA__
#define __EOL_CAMERA__
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

enum eolCameraTypes {
  eolCameraEditor  = 0, /**<follows editor controls*/
  eolCamera2D      = 1, /**<limits to 2D bounds of play area*/
  eolCameraThird   = 2, /**<over the shoulder third person view of target entity*/
  eolCameraFirst   = 3, /**<first person of the target entity*/
  eolCameraCustom  = 4  /**<custom camera type.*/
};

/**
* @brief function pointer prototype for getting the target's orientation
*/
typedef eolOrientation (*eolCameraGetTargetOri)(void *target);

typedef struct eolCamera_S
{
  eolUint     cameratype;  /**<see eolCameraTypes*/
  eolVec2D    bounds;      /**<width height of screen*/
  eolVec3D    position;    /**<position of the cameratype*/
  eolVec3D    rotation;    /**<rotation of the camera*/
  eolFloat    followDist;  /**<the following distance to the target*/
  eolFloat    followHeight;/**<how high up the camera should be when following a target*/
  void * target;      /**<target entity to watch*/
  eolCameraGetTargetOri getOri; /**<for getting the target orientation*/
  void (*update)(struct eolCamera_S *cam,void *data);  /**<what update function to call for camera*/
  void      * customData;  /**<for custom camera type*/
}eolCamera;

/**
 * @brief function pointer prototype for setting a custom
 */
typedef void (*eolCameraUpdate)(eolCamera *cam, void *data);

/**
 * @brief loads camera configuration from file or sets up defaults.
 */
void eol_camera_config();

/**
 * @brief sets up camera perspective based on graphics view.
 */
void eol_camera_init();

/**
 * @brief updates the camera based on type.  Should be called each frame before setup.
 */
void eol_camera_update();

/**
 * @brief sets up the camera for this frame within the current context
 */
void eol_camera_setup();

/**
* @brief Tell the camera to follow an entity
* @param target which entity to follow
*/
void eol_camera_set_target(void *target);

void eol_camera_set_height(eolFloat dist);
void eol_camera_set_near_clip(eolFloat clip);
void eol_camera_set_far_clip(eolFloat clip);
void eol_camera_set_aspect_ratio(eolFloat aspect);
void eol_camera_set_view_angle(eolFloat clip);
void eol_camera_set_target_orientation(eolCameraGetTargetOri targetFunc);

#endif