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
#include "eol_keychain.h"

#include <glib/glist.h>

/**
 * @brief Tiles are a means of constructing levels with components, rather than a fully
 * fleshed out mesh and clip mesh.
 * Tiles can take on any form, including Cubes, sprites or any other type of animated mesh.
 * Tiles can also be filtered based on camera position for drawing.
 * All tiles only add rectangular shapes to the clip mask however.
 * Individual Tiles can take up more than tile, but must be organized continuously.
 * ie: it can take up some integer width and height of tile spaces.  The Footprint.
 */

typedef struct
{
  eolUint        id;        /**<unique tile id.  Must be unique per map*/
  eolUint        footWidth; /**<Number of tile spaces wide this tile takes up*/
  eolUint        footHeight;/**<Number of tile spaces high this tile takes up.  Cannot be 0*/
  eolOrientation ori;       /**<default orientation*/
  eolLine        actorFile; /**<filename of the mode file to load.*/
}eolTileType;

typedef struct
{
  eolUint          id;      /**<which tile type this tile is*/
  eolInt           x,y;     /**<which tile space this tile is in.*/
  eolActor       * actor;   /**<the actor holding the tile*/
  eolOrientation   ori;     /**<orientation of THIS tile*/
}eolTile;

typedef struct
{
  eolFloat    tileWidth;      /**<width of tile spaces in gl coordinate space*/
  eolFloat    tileHeight;     /**<height of tile spaces*/
  eolUint     spaceWidth;     /**<number of tiles wide the space is*/
  eolUint     spaceHeight;    /**<number of tiles high the space is*/
  eolUint     tileIdPool;     /**<makes sure the same tile id is not used twice*/
  GList     * tileSet;        /**<the set of eolTiles that can be in the space*/
  GList     * map;            /**<the list of all tiles in the map*/
}eolTileMap;

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


void eol_tile_map_draw(eolTileMap *map);
void eol_tile_map_add_to_space(eolTileMap *map, cpSpace *space);

/**
 * @brief given coordinates in tile space, return the tile that exists there or NULL on not found
 * @param map the tile map to search
 * @param x the x coordinate to check at
 * @param y the y coordinate to check at
 * @return NULL on error or not found or the tile at the location
 */
eolTile *eol_tile_map_get_by_tilexy(eolTileMap *map,eolInt x, eolInt y);

/**
 * @brief given coordinates in draw space, return the tile that exists there or NULL on not found
 * @param map the tile map to search
 * @param x the x coordinate to check at
 * @param y the y coordinate to check at
 * @return NULL on error or not found or the tile at the location
 */
eolTile *eol_tile_map_get_by_spacexy(eolTileMap *map,eolFloat x, eolFloat y);

/**
 * @brief creates a tile map keychain to be saved to disk
 * @param map the map to create the keychain from
 * @return NULL on error or the configured keychain that describes the tile map
 */
eolKeychain *eol_tile_map_build_keychain(eolTileMap *map);

#endif