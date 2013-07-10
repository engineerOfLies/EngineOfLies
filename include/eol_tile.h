#ifndef __EOL_TILE__
#define __EOL_TILE__
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
#include "eol_actor.h"
#include "eol_3d_op.h"
#include "eol_keychain.h"

#include <glib.h>

/**
 * @brief Tiles are a means of constructing levels with components, rather than a fully
 * fleshed out mesh and clip mesh.
 * Tiles can take on any form, including Cubes, sprites or any other type of animated mesh.
 * Tiles can also be filtered based on camera position for drawing.
 * All tiles only add rectangular shapes to the clip mask however.
 * Individual Tiles can take up more than tile, but must be organized continuously.
 * ie: it can take up some integer width and height of tile spaces.  The Footprint.
 */

/**
 * @brief the eolTileType is a tile archetype that will define the tiles in the map.
 * The benefit of using tiles is to have uniform blocks to build the level with.  This
 * type describes what those blocks should look like.
 */
typedef struct
{
  eolUint        id;        /**<unique tile id.  Must be unique per map*/
  eolUint        footWidth; /**<Number of tile spaces wide this tile takes up*/
  eolUint        footHeight;/**<Number of tile spaces high this tile takes up.  Cannot be 0*/
  eolOrientation ori;       /**<default orientation*/
  eolLine        actorFile; /**<filename of the mode file to load.*/
  eolBool        walkBlock; /**<if true, this tile type cannot be walked upon*/
  eolBool        hitBlock;  /**<if true this tile type will block weapons and projectiles*/
  eolBool        sightBlock;/**<if true this tile type will block vision*/
}eolTileType;

/**
 * @brief the eol tile set contains a list of tile definitions to be referenced by tile instances
 *
 * tile sets are going to be created from a few ways: First they will be loaded from config in any
 * order without an ID.  Second they could be modified while editing a map.  Finally, they will be
 * loaded from the level file.  idPool and individual IDs need to be preserved
 */
typedef struct
{
  GHashTable * tileSet;
  eolUint      idPool;
}eolTileSet;


/**
 * @brief the eolTile describes a specific instance of a tile.  Based on a tile type
 * these are the actual building blocks that make up a tile map.
 */
typedef struct
{
  eolUint          id;        /**<the unique id of this tile in the map*/
  eolUint          tileType;  /**<which tile type this tile is*/
  eolTileType    * parentType;/**<pointer to parent type*/
  eolInt           x,y;       /**<which tile space this tile is in.*/
  eolActor       * actor;     /**<the actor holding the tile*/
  eolOrientation   ori;       /**<orientation of THIS tile*/
  eolBool          walkBlock; /**<if true, this tile cannot be walked upon*/
  eolBool          hitBlock;  /**<if true this tile will block weapons and projectiles*/
  eolBool          sightBlock;/**<if true this tile will block vision*/
}eolTile;

/**
 * @brief the eolTileMap is the container for all the tiles and tile definitions for a layer
 */
typedef struct
{
  eolFloat      tileWidth;      /**<width of tile spaces in gl coordinate space*/
  eolFloat      tileHeight;     /**<height of tile spaces*/
  eolUint       spaceWidth;     /**<number of tiles wide the space is*/
  eolUint       spaceHeight;    /**<number of tiles high the space is*/
  eolRectFloat  mapRect;       /**<position and size of tile space in this map*/
  eolUint       tileIdPool;     /**<makes sure the same tile id is not used twice*/
  eolTileSet  * tileSet;        /**<tile set to reference for these tiles*/
  GList       * map;            /**<the list of all tiles in the map*/
}eolTileMap;

/*
 * *** TILE MAP ***
 */
/**
 * @brief sets the size of a tile in openGL space.  Generally a good idea to make this uniform
 * @param map the map to set the parameters on
 * @param tileWidth the size in openGL space for a single tile's width cannot set to 0
 * @param tileHeight the size in openGL space for a single tile's height cannot set to 0
 */
void eol_tile_map_set_tile_size(eolTileMap *map,eolFloat tileWidth, eolFloat tileHeight);

/**
 * @brief sets the size of the tile space available for this map
 * @param map the map to set the parameters on
 * @param spaceWidth the number of tiles wide this map will be  cannot set to 0
 * @param spaceHeight the number of tiles high this map will be  cannot set to 0
 */
void eol_tile_map_set_space_size(eolTileMap *map,eolUint spaceWidth, eolUint spaceHeight);

/**
 * @brief given the position, return the tile dimensions that it clips.
 * @param map use the tile dimensions of this map
 * @param rect output optional
 * @param position the position to check, ignoring Z value
 * @return eolFalse on error or if the position lies outside fo the map
 */
eolBool eol_tile_map_get_tile_rect(eolTileMap *map,eolRectFloat *rect,eolVec2D position);

/**
 * @brief allocated and initializes a new tile map
 * @return NULL on error or a valid, zero'd eolTileMap
 */
eolTileMap * eol_tile_map_new();

/**
 * @brief free's all data associated with the tile map
 * @param map the tile map to be destroyed
 */
void eol_tile_map_delete(eolTileMap *map);

/**
 * @brief free's all data associated with the tile map and sets pointer to NULL
 * @param map a pointer to your map pointer.
 */
void eol_tile_map_free(eolTileMap **map);

/**
 * @brief returns a 3D Quad bounding the tile map
 * @param map the map to query
 * @return a 3D quad marking the corners of the map
 * NOTE: the Z values will all be Zero.
 * NOTE: If the map is transformed, the quad will need to be too
 */
eolQuad3D eol_tile_map_get_bounding_quad(eolTileMap *map);

/**
 * @brief converts tile space coordinates to tile indices
 * @param map the context to convert
 * @param postion the 3d space coordinates (ignoring Z)
 * @param x output optional. the tile space index of the X position
 * @param y output optional. the tile space index of the Y position
 * @return eolFalse on error or the position lies outside the map.  eolTrue otherwise
 */
eolBool eol_tile_map_position_to_tile_xy(eolTileMap *map,eolVec2D position,eolUint *x, eolUint *y);

/**
 * @brief get the tile space position of the mouse
 * @param map the map to check
 * @param layerOri parent orientation.  This needs to be inclusive of all
 *                 transformations affecting the tiles to make this accurate
 * @param mouseOut output optional.  the mouse position in tile space. If not
 *                 specified, this still returns if the mouse is over the tile map
 * @return eolFalse on error or if the mouse is outside the tile map, eolTrue otherwise
 */
eolBool eol_tile_get_spacexy_by_mouse(eolTileMap *map, eolOrientation LayerOri, eolVec2D *mouseOut);

/**
 * @brief get the tile position of the mouse.
 * @param map the map to check
 * @param layerOri parent orientation.  This needs to be inclusive of all
 *                 transformations affecting the tiles to make this accurate
 * @param x output optional the x tile position of the mouse
 * @param y output optional the y tile position of the mouse
 * @return eolFalse on error or mouse is not over the map, eolTrue otherwise
 */
eolBool eol_tile_get_tilexy_by_mouse(eolTileMap *map, eolOrientation LayerOri, eolUint *x, eolUint *y);

/**
 * @brief get the tile that the mouse is over
 * @param map the map to check
 * @param layerOri parent orientation.  This needs to be inclusive of all
 *                 transformations affecting the tiles to make this accurate
 * @return NULL on error or no tile found.  A tile pointer otherwise
 */
eolTile *eol_tile_get_by_mouse(eolTileMap *map, eolOrientation LayerOri);


/*
 * *** TILE TYPES ***
 */
/**
 * @brief load a type type from config
 * @param tileType a keychain describing a tile type
 * @return a pointer to the created and configured tile type
 */
eolTileType *eol_tile_type_load(eolKeychain *tileType);

/**
 * @brief load an entire set from the keychain provided
 * @param set the set to load into.  previous data will be lost
 * @param keychain the keychain containing a tileSet list
 */
void eol_tile_set_build_from_definition(eolTileSet *set,eolKeychain *keychain);

/**
 * @brief Adds a new tile type to the tile set, overwriting a type with the same id
 *  if no ID is speicified in the keychain, one will be assign
 * @param sat the set to add the tile type to
 * @param tileType a keychain describing the tileType to add
 * @param id the id to overwrite if there is a collision
 */
void eol_tile_set_add_type_overwrite(eolTileSet *set,eolKeychain *tileType,eolUint id);

/**
 * @brief Adds a new tile type to the tile set, overwriting a type with the same id
 *  if no ID is speicified in the config file provided, one will be assign
 * @param sat the set to add the tile type to
 * @param filename a config file describing the tileType to add
 * @param id the id to overwrite if there is a collision
 */
void eol_tile_set_add_type_overwrite_from_file(eolTileSet *set,eolLine filename,eolUint id);

/**
 * @brief Adds a new tile type to the tile set, no-op on collision
 * if no id is specified in parameter or keychain, an id will be assigned
 * @param set the set to add the tile type to
 * @param tileType a keychain describing the tileType to add
 * @param id optional if set, it will override any id in the keychain or pool.
 */
eolBool eol_tile_set_add_type(eolTileSet *set,eolKeychain *tileType,eolUint *id);

/**
 * @brief Adds a new tile type to the tile set, no-op on collision
 * if no id is specified in parameter or file, an id will be assigned
 * @param set the set to add the tile type to
 * @param filename a config file defining a tile type
 * @param id optional if set, it will override any id in the file or pool.
 */
eolBool eol_tile_set_add_type_from_file(eolTileSet *set,eolLine filename,eolUint *id);

/**
 * @brief removes and deletes the tile type with the id from the set
 * @param set the set to remove the tile type from
 * @param id the tile type to remove
 */
void eol_tile_set_remove_type(eolTileSet *set,eolUint id);

/**
 * @brief Finds the tile type by the id specified
 * @param set the set to search through
 * @param id the id to seach for
 */
eolTileType *eol_tile_set_get_tiletype_by_id(eolTileSet *set,eolUint id);

/**
 * @brief Destructive add tile to the map.  If a tile already exists in the desired location,
 * it is first removed and then this new tile is added in its place.
 * @param map the map to add a tile to
 * @param x the target X coordinate
 * @param y the target Y coordinate
 * @param tileIndex the tile type index to define the tile
 */
void eol_tile_add_to_map_overwrite(eolTileMap *map,eolInt x, eolInt y,eolUint tileIndex);

/**
 * @brief non-destructive add tile to the map.  If a tile already exists in the desired location,
 * this operation fails.
 * @param map the map to add a tile to
 * @param x the target X coordinate
 * @param y the target Y coordinate
 * @param tileIndex the tile type index to define the tile
 * @return NULL on collision or error, valid tile pointer on success
 */
eolTile * eol_tile_add_to_map(eolTileMap *map,eolInt x, eolInt y,eolUint tileIndex);

/**
 * @brief moves a tile that already exists in the map to another location.  It is non-destructive
 * if a tile already exists in that location this fails.
 * @param map the map within to move
 * @param newX the new X coordinate in map space
 * @param newY the new Y coordinate in map space
 * @param tile the tile to move
 * @return eolFalse on error or collision, eolTrue if successful
 */
eolBool eol_tile_move_in_map(eolTileMap *map,eolInt newX, eolInt newY,eolTile *tile);

/**
 * @brief deletes a tile from the map found at the provided location
 * if no tile exists in the target location, this is a no op.
 * @param map the map to delete a tile from
 * @param x the target x coordinate
 * @param y the target y coordinate
 */
void eol_tile_remove_from_map(eolTileMap *map,eolInt x, eolInt y);

/**
 * @brief draw the tile map to the screen
 * @param map the map to draw
 */
void eol_tile_map_draw(eolTileMap *map);

/**
 * @brief draw the tile grid
 * @param map the map to draw
 * @param color the color to use
 */
void eol_tile_grid_draw(eolTileMap *map, eolVec3D color);

/**
 * @brief reset tiles in the map to be in line with the tile set data
 * Should be called after any changes are made to the tile set
 * @param map the map to sync
 */
void eol_tile_map_sync_tiles(eolTileMap *map);

/**
 * @brief assign the level's tile set to the layer's tile map
 * if Either parameter is NULL, this is a no-op
 * @param map the map to get the set
 * @param set the set to assign
 */
void eol_tile_map_assign_set(eolTileMap *map, eolTileSet *set);

/*TODO*/
void eol_tile_map_add_to_space(eolTileMap *map, cpSpace *space);

/**
 * @brief given coordinates in tile space, return the tile that exists there or NULL on not found
 * @param map the tile map to search
 * @param x the x coordinate to check at
 * @param y the y coordinate to check at
 * @return NULL on error or not found or the tile at the location
 */
eolTile *eol_tile_get_by_tilexy(eolTileMap *map,eolInt x, eolInt y);

/**
 * @brief given coordinates in draw space, return the tile that exists there or NULL on not found
 * @param map the tile map to search
 * @param x the x coordinate to check at
 * @param y the y coordinate to check at
 * @return NULL on error or not found or the tile at the location
 */
eolTile *eol_tile_get_by_spacexy(eolTileMap *map,eolFloat x, eolFloat y);

/**
 * @brief delete all tiles defined by the specified type
 * @param map the map to delete the tiles from
 * @param tileIndex the index of the tile type to delete
 */
void eol_tile_map_delete_tiles_by_type(eolTileMap *map, eolUint tileIndex);

/**
 * @brief creates a tile map keychain to be saved to disk
 * @param map the map to create the keychain from
 * @return NULL on error or the configured keychain that describes the tile map
 */
eolKeychain *eol_tile_map_build_keychain(eolTileMap *map);

/**
 * @brief given a loaded keychain, build a tile map from the data
 */
eolTileMap *eol_tile_map_build_from_definition(eolKeychain *def);

/**
 * @brief creates a new empty tile set
 * @return NULL on error or a newly created, but empty, tile set
 */
eolTileSet *eol_tile_set_new();

/**
 * @brief retrieves the nth tile type from the tile set.
 * NOTE: the set is stored as a hash, order is not dependant on id
 * @param set the set to search through
 * @param n the index to retrieve
 * @return NULL on error or not found or a pointer to the tile type
 */
eolTileType *eol_tile_set_get_nth(eolTileSet *set,eolUint n);

/**
 * @brief gets the number of tile types in the provided tile set
 * @param set the tile set to query
 * @return 0 on error or empty, the count otherwise.
 */
eolUint eol_tile_set_get_count(eolTileSet *set);

/**
 * @brief frees the passed in tile set and sets the provided pointer to NULL
 * @param set a pointer to the tile set pointer to free and NULLify
 */
void eol_tile_set_free(eolTileSet **set);


#endif