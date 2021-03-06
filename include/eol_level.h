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

/**
 * @brief function pointer prototype used to spawn the entities for the level
 * the application will receive a name and a hash.  The hash is keyed on the name
 * of keys (such as position) and a value string (such as "0.3 0.5, 7.4").
 * The function will be called by the level when the level is created
 */
typedef void (*eolSpawnGeneric)(eolSpawn *spawn);

enum EOL_TRANSITION_DIRECTIONS
{
  ETD_BI = 0, /**<transition works in both directions*/
  ETD_UP = 1, /**<only from first layer to second layer*/
  ETD_DN = 2  /**<only from second layer to first layer*/
};

typedef struct
{
  eolOrientation   ori;
  eolBool          hidden;    /**<to draw or not*/
  eolBool          useAsClip; /**<if true, this background will add its geometry to the collision space*/
  eolBool          useAsFloor;/**<if true, game entities will seek this level as the floor*/
  eolLine          modelFile;
  eolFloat         followCam; /**<For paralax effect. 0 if not, 1 if follows completely*/
  eolModel       * model;
}eolBackground;


/**
 * @brief transition regions are for allowing entities to pass between different layer spaces seamlessly
 * when an entity enters a transition region for one layer, they will be checked for intersection with the
 * other layer's transition region.  While in both, the entity will be added to both physics spaces.  When
 * the entity leaves one region, it will be removed from that regions layer.
 * great for things like stairs between layers or transitions between layers that use different orientations
 *   (A)   [A+B]     {B}
 * (     {       )       }
 */
typedef struct
{
  eolRectFloat regions[2];/**<the region where two layers intersect*/
  eolUint      layers[2]; /**<the two layers to transition between*/
  eolUint      direction; /**<see EOL_TRANSITION_DIRECTIONS enum*/
  eolBool      disabled;  /**<if true, this transition is not working*/
}eolTransitionRegion;

/**
 * @brief the structure used to house the level layer data.  The engine supports
 * multiple layers.  Your game does not have to.
 */
typedef struct
{
  eolLine       idName;       /**<unique id for the level layer.  This should be unique*/

  eolOrientation ori;         /**<orientation applied to the whole layer*/
  eolBool       hidden;       /**<if this layer should be drawn or not*/
  eolRectFloat  bounds;       /**<absolute bounds in model space for the layer*/
  eolBool       usesClipMesh; /**<if true, the layer will build collision data from backgrounds marked as collision masks*/
  eolBool       usesTileMap;  /**<if true, the layer will build collision data from tile map*/
  eolBool       usesStaticFloorPlane;/**<if true all entities will be snapped to the z height of the layer*/
  eolBool       useFloorTiles;/**<if true, the layer will check against floor tiles for floor height*/
  eolBool       useFloorBackgrounds; /**<if true, the layer will check against floor backgrounds for floor heights*/
  
  
  /*allocated data that needs to be cleaned up*/
  eolTileMap  * tileMap;      /**<the loaded tile map*/
  eolKeychain * keys;        /**<config keys for the layer*/
  GList       * spawnList;    /**<the loaded spawn candidates*/
  GList       * backgrounds;  /**<a list of background display models.*/
  
  /*live data*/
  cpSpace     * space;        /**<the collision space for this layer*/
  GList       * entities;     /**<entites that have been added to the layer*/
}eolLevelLayer;

typedef struct
{
  eolLine         idName;     /**<unique level name.  Searchable, should be part of the filename*/
  eolUint         layerCount; /**<how many layers the level contains*/
  eolUint         active;     /**<the layer that is active*/
  eolFloat        cameraDist; /**<Camera follow distance*/
  eolUint         spawnPool;  /**<spawn unique index pool*/
  eolKeychain   * keys;       /**<level configuration keys*/
  GList         * layers;     /**<the allocated list of level layers*/
  eolTileSet    * tileSet;    /**<the set of eolTiles that can be in the level  References layer tiles, do not edit*/
  GList         * transitions;/**<list of transition regions*/
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

/**
 * @brief get level config parameters
 */
eolFloat eol_level_clip_step();
eolUint eol_level_clip_iterations();
eolFloat eol_level_slop();
eolFloat eol_level_bias();

/**
 * @brief enable / disable drawing of backgrounds
 * NOTE: does not apply to backgrounds used as masks
 * @param enable set to true to turn it on, false to turn it off
 */
void eol_level_enable_background_draw(eolBool enable);

/**
 * @brief enable / disable drawing of collision masks
 * NOTE: includes level bounds and tile outlines
 * @param enable set to true to turn it on, false to turn it off
 */
void eol_level_enable_collision_draw(eolBool enable);

/**
 * @brief enable / disable drawing of tiles
 * @param enable set to true to turn it on, false to turn it off
 */
void eol_level_enable_tile_draw(eolBool enable);

/**
 * @brief enable / disable drawing of the tile grid
 * @param enable set to true to turn it on, false to turn it off
 */
void eol_level_enable_tile_grid_draw(eolBool enable);

/**
 * @brief sets up the level manager
 * @return eolFalse on error
 */
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
 * @brief deletes the nth layer of the level, freeing all assets
 * No Op if the level is null or the layer does not exist
 * @param level the level to delete a layer from
 * @param layerIndex the index of the layer to be deleted
 */
void eol_level_delete_Layer_n(eolLevel *level,eolUint layerIndex);

/**
 * @brief changes the place in the layer list of the nth layer to one position up
 * No op if layer doesn't exist, cant move further up or any missing data
 * @param level the level to alter
 * @param n the layer to move up
 */
void eol_level_move_layer_n_up(eolLevel * level,eolUint n);

/**
 * @brief changes the place in the layer list of the nth layer to one position down
 * No op if layer doesn't exist, cant move further up or any missing data
 * @param level the level to alter
 * @param n the layer to move down
 */
void eol_level_move_layer_n_down(eolLevel * level,eolUint n);

/**
 * @brief get the nth layer of a level
 * @param level a pointer to the level to search
 * @param n the index of the layer to get
 * @return NULL on miss or error, a pointer to the layer otherwise
 */
eolLevelLayer *eol_level_get_layer_n(eolLevel *level,eolUint n);

/**
 * @brief sets the hidden status of a layer
 * @param level the level that owns the layer
 * @param n the layer index to hide/unhide
 * @param hide if true, set the layer to hidden, false to unhide
 */
void eol_level_hide_layer(eolLevel *level, eolUint n, eolBool hide);

/**
 * @brief gets the layer bounding quad in 3d space.
 * 
 * @param layer the owning layer
 * @return the quad in 3d of the level bounds.  
 */
eolQuad3D eol_level_layer_get_bounding_quad(eolLevelLayer *layer);

/**
 * @brief gets the layer bounding quad in 3d space transformed as per layer's orientation
 *
 * @param layer the owning layer
 * @return the quad in 3d of the level bounds rotated, moved and scaled
 */
eolQuad3D eol_level_layer_get_bounding_quad_transformed(eolLevelLayer *layer);

/**
 * @brief gets the point that the mouse is directly over in the layer plane.
 * 
 * @param point output. The point the mouse intersects with the layer plane.
 * @param layer the layer to check against
 * @return eolTrue if point was set, eolFalse on error or if mouse is outside the bounds
 * of the layer.
 */
eolBool eol_level_layer_get_mouse_point(eolVec3D *point,eolLevelLayer *layer);

/*background editing*/

/**
 * @brief allocated an empty background, adds it to the layer in question and returns a pointer
 * to the newly created background.
 * @param layer the layer to add a background to.
 * @return a pointer to an allocated, but empty layer, or NULL on error.
 */
eolBackground *eol_level_add_background_to_layer(eolLevelLayer *layer);

/**
 * @brief free the model and delete the background from the layer
 * @param layer the layer to delete a background from
 * @param n the index of the background in the layer
 */
void eol_level_delete_background(eolLevelLayer *layer,eolUint n);

/**
 * @brief change the hidden status of a background in a layer
 * @param layer the layer owning the background
 * @param n the background index to hide or unhide
 * @param hide eolTrue to hide the background, eolFalse to show the background
 */
void eol_level_hide_background(eolLevelLayer *layer,eolUint n, eolBool hide);

void eol_level_move_background_down(eolLevelLayer *layer,eolUint n);
void eol_level_move_background_up(eolLevelLayer *layer,eolUint n);

/**
 * @brief gets the nth background from the level
 * @param layer the layer that owns the background
 * @param n the index of the background to get
 * @return a pointer to the background or NULL if not found or error
 */
eolBackground *eol_level_get_background(eolLevelLayer *layer,eolUint n);

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
 * @param level the level owning the layer
 * @param layer the layer to setup
 */
void eol_level_setup_layer(eolLevel *level, eolLevelLayer *layer);

/**
 * @brief sets up the level for use, including all layers
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
 * @return a pointer to the newly activated layer
 */
eolLevelLayer* eol_level_set_active_layer(eolLevel *level, eolUint layer);

/**
 * @brief rename the specified layer
 * @param level the parent level
 * @param n the layer index to alter
 * @param newName the new name for the layer
 */
void eol_level_rename_layer_n(eolLevel * level,eolUint n, eolLine newName);

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

/**
 * @brief adds the loaded level mask model into the collision space as collision mask
 * 
 * @param layer the layer to alter
 */
void eol_level_add_mask_to_space(eolLevelLayer *layer);

/**
 * @brief adds a static line segment to the provide colllision space
 * 
 * @param sx the starting point x coordinate
 * @param sy the starting point y coordinate
 * @param ex the ending point x coordinate
 * @param ey the ending point y coordinate
 * @param space the collision space to update
 * @return the cp shape of the added line
 */
cpShape *eol_level_add_segment_to_space(eolFloat sx,
                                        eolFloat sy,
                                        eolFloat ex,
                                        eolFloat ey,
                                        cpSpace *space);

/*
 * *** Tiles ***
 */

/**
 * @brief loads a set of tile definitions from file and uses it as the tileset for the level
 * This will blow out any previously loaded tile set, and re-sync active tiles to the new set.
 * 
 * @param level the level to load the tile set for
 * @param filename the file containing tile set definitions
 */
void eol_level_load_tile_set(eolLevel *level, eolLine filename);

/**
 * @brief load a tileset from the keychain provided
 * @param level the level to load the tile set for
 * @param keychain the keychain containing the tile definitions
 */
void eol_level_load_tile_set_from_keychain(eolLevel *level, eolKeychain *keychain);

/**
 * @brief loads a single tile definition into the existing map.  Will assign it a new ID
 * @param level the level to get the new tile
 * @param filename the file containing tile definition
 */
void eol_level_load_tile_type(eolLevel *level, eolLine filename);

/**
 * @brief replace a tile with a new one from disk.  Re-syncs existing tiles to match the one loaded
 * @param level the level to get the replacement tile type
 * @param filename the file containing the tile data
 * @param id the id of the tile type to replace
 */
void eol_level_replace_tile_type(eolLevel *level, eolLine filename, eolUint id);

/**
 * @brief delete the tile type from the set and all associated tiles using that type in ALL layers
 * @param level the level to be cleaned
 * @param tileIndex the tile Index to be destroyed
 */
void eol_level_delete_tile_type(eolLevel *level, eolUint tileIndex);

/**
 * @brief gets the nth tile in the tile set of the level
 * @param level the level to query
 * @param n the index of the tile to return (not the tile type's id)
 * @return NULL on error or not found, a pointer otherwise.  This pointer is not yours to free.
 */
eolTileType *eol_level_get_tile_set_nth(eolLevel *level,eolUint n);

/**
 * @brief gets the tile type in the tile set of the level by its id
 * @param level the level to query
 * @param id the id of the tile to return (not the tile type's index in the list)
 * @return NULL on error or not found, a pointer otherwise.  This pointer is not yours to free.
 */
eolTileType *eol_level_get_tile_set_by_id(eolLevel *level,eolUint id);

/**
 * @brief get the number of tiles in the level's tile set
 * @param level the level to query
 * @return 0 on error or empty, the number otherwise
 */
eolUint eol_level_get_tile_set_count(eolLevel *level);

/**
 * @brief get the index x,y by the mouse position
 * NOTE: this returns a position if there is a tile under the mouse or not, as long
 * as the mouse is within the tile space
 * @param layer the owning layer to seach
 * @param x optional output.  the X position of the tile map under the mouse
 * @param y optional output.  the Y position of the tile map under the mouse
 * @return eolFalse on error or the mouse is outside the map.  eolTrue otherwise
 */
eolBool eol_level_get_layer_tilexy_by_mouse(eolLevelLayer *layer, eolUint *x, eolUint *y);

/*

  Spawn

 */

/**
 * @brief add a new spawn to a level layer
 * @param layer the layer to add the spawn too
 * @param spawnTemplate the spawn candidate to base the new spawn on
 * @param position the position of the spawn candidate
 */
void eol_level_layer_add_spawn(
  eolLevelLayer *layer,
  eolSpawn *spawnTemplate,
  eolVec3D  position,
  eolUint   id
);

/**
 * @brief gets a unique spawn id for the level
 * @param level the level to get an ID for
 * @return 0 on error or any other number is a unique id for this level
 */
eolUint eol_level_get_new_spawn_id(eolLevel *level);

/**
 * @brief adds a new spawn to a provided level layer
 * @param level the level to add a spawn to
 * @param layer the layer of the level to add the spawn to
 * @param spawnTemplate the spawn candidate to use as the template
 * @param position where in the level layer to add the spawn to
 */
void eol_level_add_spawn_to_layer(
  eolLevel *level,
  eolLevelLayer *layer,
  eolSpawn *spawnTemplate,
  eolVec3D position);

/**
 * @brief adds a new spawn to a provided level layer by index
 * @param level the level to add a spawn to
 * @param n the nth layer gets the spawn
 * @param spawnTemplate the spawn candidate to use as the template
 * @param position where in the level layer to add the spawn to
 */
void eol_level_add_spawn_to_layer_n(
  eolLevel *level,
  eolUint   n,
  eolSpawn *spawnTemplate,
  eolVec3D position);

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
