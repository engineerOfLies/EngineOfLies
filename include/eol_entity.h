#ifndef __EOL_ENTITY__
#define __EOL_ENTITY__
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

#include "eol_spawn.h"
#include "eol_trace.h"
#include "eol_actor.h"
#include "eol_config.h"
#include "eol_types.h"
#include <glib/glist.h>

/**
 * @purpose the Entity is the structure that defines anything that can act or
 * or be acted upon in the game.  This included everything from the player, to
 * projectiles, monsters, npcs, doors, platforms and item pickups.
 */

/**<states for how the entity is treated by the entity system*/
enum eolEntityStates {
  eolEntitySpawn  = 0,  /**<spawned entities are running through their begining animations*/
  eolEntityActive = 1,  /**<active entities are normal*/
  eolEntityDying  = 2,  /**<dying entities should be running through death animations and no longer clip*/
  eolEntityDead   = 3   /**<dead entities are deleted after the end of the frame by the entity system*/
};

enum eolEntityShapes {
  eolEntityNoShape = 0, /**<until specified, an entity has no shape*/
  eolEntityCircle  = 1,
  eolEntityRect    = 2
};

enum eolEntityDrawModes {
  eolEntityDrawBounds    = 0,
  eolEntityDrawWireframe = 1,
  eolEntityDrawMesh      = 2,
  eolEntityDrawTextured  = 3,
  eolEntityDrawLighting  = 4,
  eolEntityDrawShaded    = 5
};

enum eolEntityClipLayers {
  eolEntityClipLevel = 1
};


typedef struct Entity_S
{
  eolUint           id;         /**<unique entity resource id*/
  eolUint           levelId;    /**<unique to the level id.*/
  eolLine           name;       /**<name of the entity*/
  struct Entity_S * self;       /**<pointer to itself*/

  struct Entity_S * parent;     /**<a pointer to a parent entity.  ie: a shooter is the parent of a gun*/
  eolUint           parentId;   /**<unique id of the parent entity.  if this doesnt match, parent died.*/

  GList           * children;   /**<this will be a list of eolEntityPointer*/

  eolBool           shown;      /**<if the entity should be drawn*/
  eolBool           physical;   /**<if the entity should be added to physics space*/
  eolUint           state;      /**<see eolEntityState for details*/

  eolUint           shapeType;  /**<see eolEntityShapes for supported shape types*/
  eolRectFloat      boundingBox;/**<bounding box offset from orientation*/
  eolFloat          radius;     /**<for circle based collisions*/
  eolFloat          mass;       /**<for physics simulation.  default should be about 0.001*/
  cpBody          * body;       /**<links to the physics engine*/
  cpShape         * shape;      /**<link to physics engine*/
  cpSpace         * _space;     /**<pointer to the space this entity is a part of*/
  cpLayers          collisionMask;/**<collision mask for space.  Not the same as level layer.*/
  
  eolTrail          trail;     /**<the last so many orientations of the entity*/
  eolBool           trackTrail;/**<if the trail should be tracked.*/
  eolBool           drawTrail; /**<if the trail should be drawn*/
  eolOrientation    ori;       /**<orientation now*/
  eolOrientation    vector;    /**<orientation change vector*/
  eolOrientation    accel;     /**<orientation change vector change vector*/

  eolActor        * actor;     /**<most common use case is only have 1 actor, this will point to first*/
  GList           * actorList; /**<general use will have only 1 actor, but its possible to have
                                   composite entities*/

  eolBool           grounded; /**<set true by the physics pass if the entity is touching the ground*/
  eolBool           wallTouch;/**<set true by the physics pass if the entity is touching a wall*/

  /*function pointers*/
  /**<the custom draw function, if defined will override standard draw function*/
  void (*draw)(struct Entity_S * self);
  /**<the think function is used for AI and input handling.  Should be called every frame for player.*/
  void (*think)(struct Entity_S * self);
  eolInt            thinkRate;/**<how often, in milliseconds, think is called.*/
  eolUint           thinkNext;/**<the next time this entity should think*/

  /**<update function is used for maintenance for things like positional update and frame animation.
      called every frame*/
  void (*update)(struct Entity_S * self);

  /**<the touch function is called when two entities touch each other.  It will be called for each entity
      order is not guaranteed*/
  void (*touch)(struct Entity_S * self,struct Entity_S * other);

  /**<the touch funciton called when the entity collides with the world*/
  void (*levelTouch)(struct Entity_S * self,eolTrace *trace);

  /**<a function called when the entity is "activated", for instance a door opened, or a trap sprung*/
  void (*activate)(struct Entity_S * self,struct Entity_S * other);

  /**<a function called when the entity "dies" sets up dying animation, drops items, gives exp, etc.*/
  void (*die)(struct Entity_S * self);

  /*spawn candidate data*/
  eolSpawn  * spawn;  /**<points to the data in the level.  It should not change*/
  eolConfig * config; /**<points to the config data for this entity*/

  /*pointer to game-specific data to be handled by individual games*/
  void *customData;   /**<pointer to the custom game data packed with the entity.
                          !!!THIS MUST REMAIN THE LAST ELEMENT IN THIS STRUCT!!!!*/
}eolEntity;

typedef struct
{
  eolEntity *ent; /**<this pointer is valid only if the following ID matches this*/
  eolUint    id;
}eolEntityPointer;

/**
 * @brief configure entity system from file or defaults
 */
void eol_entity_config();

/**
* @brief sets up entity system based on defaults
*/
void eol_entity_init();

/**
* @brief deleted loaded entity data resetting system back to startup
*/
void eol_entity_clear();

/**
* @brief returns initialization status of entity system
*/
eolBool eol_entity_initialized();

/**
* @brief Frees the entity passed.  Sets pointer to NULL
* @param ent a pointer to an entity pointer.
*/
void eol_entity_free(eolEntity **ent);

/**
* @brief Get a newly allocated freshly initialized entity.
*/
eolEntity *eol_entity_new();

/**
 * @brief sync entity states with the collision space.  Update desired forces,
 * note that position and velocity are updated automatically here.
 * add and remove entities from the space. delete entities marked for death.
 */
void eol_entity_presync_all();

/**
 * @brief sync entity states from the collision space.  Update positions, call
 * touch functions.
 */
void eol_entity_postsync_all();

/**
 * @brief calls all update functions for entities.
 */
void eol_entity_update_all();

/**
* @brief calls all think functions for entities if they need to think
*/
void eol_entity_think_all();

/**
 * @brief draws all entities in the current context.
 */
void eol_entity_draw_all();

/**
 * @brief this is the function prototype for a customData delete function
 * a delete function must be provided if using customData
 * @Param the function must take a pointer to the custom data.  It should be cast
 * internally as the customData type.
 */
typedef void (*eolEntityCustomDelete)(void *customData);

/**
 * @brief this function registers the custome delete function with the entity system
 * note that for ALL entities, there can be only one custom delete.
 * note that this must be called BEFORE the entity system is initialized
 * @param a pointer to the function that will be called when deleting custom data.
 */
void eol_entity_register_custom_delete(eolEntityCustomDelete delfunc);

/**
 * @brief this function registers the size of the custom data segment
 * note that this must be called BEFORE the entity system is initialized
 * @param customSize this should be the sizeof(YOURDATATYPE)
 */
void eol_entity_register_custom_data_size(eolUint customSize);

/**
 * @brief Adds an entity to a physics space for physics calculations.
 * automatically sets flags to enable physics for this entity.
 * Entity must already have a defined body or this will fail.
 * @param ent the entity to add
 * @param space the space to add the entity to
 */
void eol_entity_add_to_space(eolEntity *ent,cpSpace *space);

/**
 * @brief Removed the entity from the space that it is a part of.
 * @param ent the entity to remove.
 */
void eol_entity_remove_from_space(eolEntity *ent);

/**
 * @brief sets up the internal body and shape for the entity so it can be added to a space as a circle
 * uses ent->radius for making the radius of the circle.
 * destroys previous shape and body data and must be re-added to the space.
 * @param ent the entity to become a circle
 */
void eol_entity_shape_make_circle(eolEntity *ent);

/**
 * @brief sets up the internal body and shape for the entity so it can be added to a space as a rect
 * uses ent->bounds for making the dimensions of the rect.
 * destroys previous shape and body data and must be re-added to the space.
 * @param ent the entity to become a rect
 */
void eol_entity_shape_make_rect(eolEntity *ent);

void eol_entity_set_collision_mask(eolEntity *ent,cpLayers collisionmask);

void eol_entity_add_to_collision_mask(eolEntity *ent,cpLayers collisionmask);

void eol_entity_remove_entity_from_collision_mask(eolEntity *ent,cpLayers collisionmask);

void eol_entity_add_actor(eolEntity *ent,eolActor *act);

void eol_entity_set_actor_bounds(eolEntity *ent);

#endif
