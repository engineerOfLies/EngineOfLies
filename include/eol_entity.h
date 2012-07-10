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
  
  eolRectFloat      boundingBox;/**<bounding box offset from orientation*/
  eolFloat          radius;     /**<for circle based collisions*/
  cpShape         * shape;      /**<link to physics engine*/
  cpLayers          layermask;  /**<layes that this entity collides with*/
  
  eolTrail          trail;     /**<the last so many orientations of the entity*/
  eolBool           drawTrail; /**<if the trail should be drawn*/
  eolOrientation    ori;       /**<orientation now*/
  eolOrientation    vector;    /**<orientation change vector*/
  eolOrientation    accel;     /**<orientation change vector change vector*/

  GList           * actorList; /**<general use will have only 1 actor, but its possible to have
                                   composite entities*/

  /*function pointers*/
  /**<the custom draw function, if defined will override standard draw function*/
  void (*draw)(struct Entity_S * self);
  /**<the think function is used for AI and input handling.  Should be called every frame for player.*/
  void (*think)(struct Entity_S * self);
  eolInt            thinkRate;/**<how often, in milliseconds, think is called.*/

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
  void *customData;   /**<pointer to the custom game data packed with the entity.*/
}eolEntity;

typedef struct
{
  eolEntity *ent; /**<this pointer is valid only if the following ID matches this*/
  eolUint    id;
}eolEntityPointer;


#endif
