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
#include "eol_spawn.h"

/**
 * @purpose To provide a handler for the generic featured common to all 2D game levels.
 * It will provide support for:
 * Tile Based levels and 3D mesh based levels.
 * layered levels
 */

/**
 * @brief function pointer prototype used to spawn the entities for the level
 * the application will receive a name and a hash.  The hash is keyed on the name
 * of keys (such as position) and a value string (such as "0.3 0.5, 7.4").
 * The function will be called by the level when the level is created
 */
typedef void (*eolSpawnGeneric)(eolSpawn *spawn);

typedef struct
{
  eolUint     width;
  eolUint     height;
  eolUint     tileWidth;
  eolUint     tileHeight;
  eolSprite * tileSet;
  eolUint   * map;
}eolTileMap;

typedef struct
{
  eolOrientation   ori;
  eolLine          modelFile;
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
  eolBool      updateSpace;  /**<its possible to update all, near or only active layer*/
  eolRectFloat bounds;       /**<absolute bounds in model space for the layer*/
  eolBool      usesClipMesh; /**<if true, the layer will build collision data from clip mesh*/
  eolBool      usesTileMap;  /**<if true, the layer will build collision data from tile map*/
  
  eolLine      clipMeshFile; /**<the file to load for a clip mask*/
  eolLine      tileSet;      /**<the group of tiles to use,  Generally a sprite where each cell is a tile*/
  
  eolOrientation clipMaskOri;/**<offset for where the clipmask is set in 3D space*/
  /*allocated data that needs to be cleaned up*/
  eolTileMap   tileMap;      /**<the loaded tile map*/
  GList      * spawnList;    /**<the loaded spawn candidates*/
  GList      * backgrounds;  /**<a list of background display models.*/
  cpSpace    * space;        /**<the collision space for this layer*/
  eolMesh    * clipMesh;     /**<the collision mask as mesh data*/
}eolLevelLayer;

typedef struct
{
  eolWord         idName;     /**<unique level name.  Searchable, should be part of the filename*/
  eolUint         layerCount; /**<how many layers the level contains*/
  eolLevelLayer * layers;     /**<the allocated list of level layers*/
}eolLevel;


/**
* @brief frees all loaded level data.
*/
void eol_level_init();

/**
* @brief loads config for level system from file or sets up defaults.
*/
void eol_level_config();

/**
 * @brief frees all loaded level data.
 */
void eol_level_clear();

eolBool eol_level_initialized();

/**
 * @brief allocates an empty level struct and returns a pointer to it.
 * @return  a pointer to an allocated, but empty level, or NULL on error.
 */
eolLevel *eol_level_new();

/**
 * @brief allocates and loads a level from file.
 * This function will not load any of the assets referred to by the level, such as background images
 * It will merely load the data from the file specified.  A call to eol_level_setup is needed
 * to load the assets and spawn the entities
 * @param filename the name of the level file to load
 * @return a pointer to the loaded level from file.
 */
eolLevel *eol_level_load(char *filename);

/**
 * @brief frees the level passed and sets the pointer to it to NULL
 * @param level a pointer to a level pointer
 */
void eol_level_free(eolLevel **level);

/**
 * @brief sets up a loaded level for use. Including spawning all entities
 * @param level the level to set up
 */
void eol_level_setup(eolLevel *level);

/**
 * @brief steps through an iteration of game time.  Calls collision updates.
 */
void eol_level_update(eolLevel *level);

/**
 * @brief registers the generic entity spawn function with the level system.
 * only one can be registered at a time
 * @param spawnGeneric a pointer to a function to call when spawning entities
 */
void eol_level_register_spawn_generic(eolSpawnGeneric spawnGeneric);



#endif
