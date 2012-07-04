#ifndef __EOL_LEVEL__
#define __EOL_LEVEL__
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
#include "eol_model.h"
#include "eol_sprite.h"
#include <glib/ghash.h>

/**
 * @purpose To provide a handler for the generic featured common to all 2D game levels.
 * It will provide support for:
 * Tile Based levels and 3D mesh based levels.
 * layered levels
 */

typedef struct
{
  eolWord type;
  GHashTable *keys;
}eolSpawn;

/**
 * @brief function pointer prototype used to spawn the entities for the level
 * the application will receive a name and a hash.  The hash is keyed on the name
 * of keys (such as position) and a value string (such as "0.3 0.5, 7.4").
 * The function will be called by the level when the level is created
 */
void (*eolSpawnGeneric)(eolSpawn *spawn);

typedef struct
{
  eolUint     width;
  eolUint     height;
  eolUint     tileWidth;
  eolUint     tileHeight;
  eolSprite * tileset;
  eolUint   * map;
}eolTileMap;

typedef struct
{
  eolOrientation   ori;
  eolModel       * model;
}eolBackground;

/**
 * @brief the structure used to house the level layer data.  The engine supports
 * multiple layers.  Your game does not have to.
 */
typedef struct
{
  eolWord      idName;       /**<unique id for the level layer.  This should be unique*/

  eolFloat     alpha;        /**<translucency to apply to all layer assets.  by setting it to 0, you turn off
                                 rendering for the layer*/
  eolRectFloat bounds;       /**<absolute bounds in model space for the layer*/
  eolBool      usesClipMesh; /**<if true, the layer will build collision data from clip mesh*/
  eolBool      usesTileMap;  /**<if true, the layer will build collision data from tile map*/
  
  eolLine      clipMeshFile; /**<the file to load for a clip mask*/
  eolLine      tileSet;      /**<the group of tiles to use,  Generally a sprite where each cell is a tile*/
  
  eolTileMap   tileMap;      /**<the loaded tile map*/
  
  eolUint      spawnCount;   /**<the size of loaded spawn list*/
  eolSpawn     spawnList;    /**<the loaded spawn candidates*/
  eolBool      updateSpace;  /**<its possible to update all, near or only active layer*/
  cpSpace    * space;        /**<the collision space for this layer*/
}eolLevelLayer;

typedef struct
{
  eolWord         idName;     /**<unique level name.  Searchable, should be part of the filename*/
  eolUint         layerCount; /**<how many layers the level contains*/
  eolLevelLayer * layers;     /**<the allocated list of level layers*/
}eolLevel;

#endif
