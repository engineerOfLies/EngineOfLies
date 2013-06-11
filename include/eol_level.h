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

#include "eol_keychain.h"
#include "eol_tile.h"
#include "eol_types.h"
#include "eol_model.h"
#include "eol_spawn.h"
#include "eol_entity.h"
#include <glib/glist.h>

/**
 * @purpose To provide a handler for the generic featured common to all 2D game levels.
 * It will provide support for:
 * Tile Based levels and 3D mesh based levels.
 * layered levels
 */

enum eolLevelDrawModes {
  eolLevelDrawClip,
  eolLevelDrawMesh,
  eolLevelDrawFull
};

/**
 * @brief function pointer prototype used to spawn the entities for the level
 * the application will receive a name and a hash.  The hash is keyed on the name
 * of keys (such as position) and a value string (such as "0.3 0.5, 7.4").
 * The function will be called by the level when the level is created
 */
typedef void (*eolSpawnGeneric)(eolSpawn *spawn);

typedef struct
{
  eolOrientation   ori;
  eolLine          modelFile;
  eolFloat         followCam; /**<For paralax effect. 0 if not, 1 if follows completely*/
  eolModel       * model;
}eolBackground;

/**
 * @brief the structure used to house the level layer data.  The engine supports
 * multiple layers.  Your game does not have to.
 */
typedef struct
{
  eolLine       idName;       /**<unique id for the level layer.  This should be unique*/

  eolOrientation ori;         /**<orientation applied to the whole layer*/

  eolRectFloat  bounds;       /**<absolute bounds in model space for the layer*/
  eolBool       usesClipMesh; /**<if true, the layer will build collision data from clip mesh*/
  eolBool       usesTileMap;  /**<if true, the layer will build collision data from tile map*/
  
  eolLine       clipMeshFile; /**<the file to load for a clip mask*/
  eolLine       tileSet;      /**<the group of tiles to use,  Generally a sprite where each cell is a tile*/
  
  eolOrientation clipMaskOri;/**<offset for where the clipmask is set in 3D space*/
  /*allocated data that needs to be cleaned up*/
  eolTileMap  * tileMap;      /**<the loaded tile map*/
  eolKeychain * keys;        /**<config keys for the layer*/
  GList       * spawnList;    /**<the loaded spawn candidates*/
  GList       * backgrounds;  /**<a list of background display models.*/
  
  /*live data*/
  cpSpace     * space;        /**<the collision space for this layer*/
  eolMesh     * clipMesh;     /**<the collision mask as mesh data*/
  GList       * entities;     /**<entites that have been added to the layer*/
}eolLevelLayer;

typedef struct
{
  eolLine         idName;     /**<unique level name.  Searchable, should be part of the filename*/
  eolUint         layerCount; /**<how many layers the level contains*/
  eolUint         active;     /**<the layer that is active*/
  eolFloat        cameraDist; /**<Camera follow distance*/
  eolKeychain   * keys;       /**<level configuration keys*/
  GList         * layers;     /**<the allocated list of level layers*/
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
 * @brief allocated an empty level layer, adds it to the level in question and returns a pointer
 * to the newly created layer.
 * @param level the level to add a layer to.
 * @return  a pointer to an allocated, but empty layer, or NULL on error.
 */
eolLevelLayer *eol_level_add_layer(eolLevel *level);

/**
 * @brief get the nth layer of a level
 * @param level a pointer to the level to search
 * @param n the index of the layer to get
 * @return NULL on miss or error, a pointer to the layer otherwise
 */
eolLevelLayer *eol_level_get_layer_n(eolLevel *level,eolUint n);

/**
 * @brief allocated an empty background, adds it to the layer in question and returns a pointer
 * to the newly created background.
 * @param layer the layer to add a background to.
 * @return a pointer to an allocated, but empty layer, or NULL on error.
 */
eolBackground *eol_level_add_background_to_layer(eolLevelLayer *layer);

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
 * @brief saves the level information to disk into the file name specified.
 * @param filename what this file should be known as.
 * @param level the level to be saved.
 */
void eol_level_save(char *filename,eolLevel *level);

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

/**
* @brief draws the current level.
*/
void eol_level_draw_current();

/**
 * @brief draws the level focused on the active layer.
 * @param level the level to draw.
 */
void eol_level_draw(eolLevel *level);

/**
 * @brief draws the active layer's clip mask and level bounds
 * @param level the level to draw.
 */
void eol_level_draw_clip(eolLevel *level);

/**
 * @brief sets which layer in the level has the focus and gets updated.
 * @param level the level to set
 * @param layer layer number for the level to make active
 */
void eol_level_set_active_layer(eolLevel *level, eolUint layer);

/**
 * @brief sets the passed in level as the active level for rendering and updating.
 * @param level to the level to make active
 */
void eol_level_set_current_level(eolLevel *level);

/**
 * @brief runs the physics simulation for the active layer.
 * Runs pre and post sync for entities
 */
void eol_level_update_active();

void eol_level_add_mask_to_space(eolLevelLayer *layer);

/*
  *** ENTITY ***
*/

/**
 * @brief adds and entity's body and shape to the layer space for physics.
 * The entity should have the body and shape defined previously.
 * @param layer the layer of the level to add the player to.
 * @param ent the entity to be adde
 */
void eol_level_add_entity_to_layer(eolLevelLayer *layer, eolEntity *ent);

void eol_level_add_entity_to_active_layer(eolEntity *ent);

cpBody *eol_level_get_active_layer_body();

#endif
