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
  eolUint        footwidth; /**<Number of tile spaces wide this tile takes up*/
  eolUint        footheight;/**<Number of tile spaces high this tile takes up.  Cannot be 0*/
  eolOrientation ori;       /**<default orientation*/
  eolLine        modelFile; /**<filename of the mode file to load.*/
}eolTileType;

typedef struct
{
  eolUint          x,y;     /**<which tile space this tile is in.*/
  eolActor       * actor;   /**<the actor holding the tile*/
  eolOrientation   ori;     /**<orientation of THIS tile*/
}eolTile;

typedef struct
{
  eolFloat    tilewidth;      /**<width of tile spaces in gl coordinate space*/
  eolFloat    tileheight;     /**<height of tile spaces*/
  eolUint     spaceWidth;     /**<number of tiles wide the space is*/
  eolUint     spaceHeight;    /**<number of tiles high the space is*/
  eolUint     tileIdPool;     /**<makes sure the same tile id is not used twice*/
  GList     * tileSet;        /**<the set of eolTiles that can be in the space*/
  GList     * map;            /**<the list of all tiles in the map*/
}eolTileMap;

eolTileMap * eol_tile_map_new();
void eol_tile_map_delete(eolTileMap *map);
void eol_tile_map_draw(eolTileMap *map);
void eol_tile_map_add_to_space(eolTileMap *map, cpSpace *space);
#endif