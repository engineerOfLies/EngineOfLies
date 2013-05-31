#include "eol_tile.h"
#include "eol_logger.h"
/*local variables*/

/*local function prototypes*/

/*definitions*/

/*tile operations*/

void eol_tile_draw(eolTile *tile,eolFloat tileWidth,eolFloat tileHeight)
{
  eolVec3D position = {0,0,0};
  eol_vec3d_copy(position,tile->ori.position);
  position.x += (tile->x * tileWidth);
  position.y += (tile->y * tileHeight);
  eol_actor_draw(
    tile->actor,
    position,
    tile->ori.rotation,
    tile->ori.scale,
    tile->ori.color,
    tile->ori.alpha
  );
}

void eol_tile_delete(eolTile *tile)
{
  if (!tile)return;
  eol_actor_free(&tile->actor);
  free(tile);
}

void eol_tile_free(eolTile **tile)
{
  if (!tile)return;
  eol_tile_delete(*tile);
  *tile = NULL;
}

eolTile *eol_tile_new()
{
  eolTile *tile;
  tile = (eolTile*)malloc(sizeof(eolTile));
  if (!tile)return NULL;
  memset(tile,0,sizeof(eolTile));
  return tile;
}

void eol_tile_add_to_map_overwrite(eolTileMap *map,eolInt x, eolInt y,eolUint tileIndex)
{
  if (!map)return;
  eol_tile_remove_from_map(map,x,y);
  eol_tile_add_to_map(map,x, y,tileIndex);
}

void eol_tile_add_to_map(eolTileMap *map,eolInt x, eolInt y,eolUint tileIndex)
{
  eolTile *tile;
  if (!map)return;
  if (eol_tile_get_by_tilexy(map,x,y))
  {
    return;
  }
  tile = eol_tile_new();
  if (!tile)return;
  tile->x = x;
  tile->y = y;
  tile->id = tileIndex;
  map->map = g_list_append(map->map,tile);
  /*TODO lookup tileIndex and set ori and actor*/
}

eolBool eol_tile_move_in_map(eolTileMap *map,eolInt newX, eolInt newY,eolTile *tile)
{
  if (!tile)return eolFalse;
  if (eol_tile_get_by_tilexy(map,newX,newY))
  {
    /*a tile already exists there*/
    return eolFalse;
  }
  tile->x = newX;
  tile->y = newY;
  return eolTrue;
}

void eol_tile_remove_from_map(eolTileMap *map,eolInt x, eolInt y)
{
  eolTile *tile;
  GList *tileLink;
  if (!map)return;
  tile = eol_tile_get_by_tilexy(map,x,y);
  if (!tile)
  {
    eol_logger_message(EOL_LOG_INFO,"no tile to remove in map at (%i,%i)");
    return;
  }
  tileLink = g_list_find(map->map,tile);
  eol_tile_delete(tile);
  if (tileLink)
  {
    map->map = g_list_remove_link(map->map,tileLink);
  }
}

eolTile *eol_tile_get_by_tilexy(eolTileMap *map,eolInt x, eolInt y)
{
  GList *t;
  eolTile *tile;
  if (!map)return NULL;
  for(t = map->map;t != NULL;t = t->next)
  {
    if (!t->data)continue;
    tile = (eolTile*)t->data;
    if ((tile->x == x)&&(tile->y == y))
    {
      return tile;
    }
    /*TODO: also check for irregularly shaped tiles*/
  }
  return NULL;
}

eolTile *eol_tile_get_by_spacexy(eolTileMap *map,eolFloat fx, eolFloat fy)
{
  eolUint x,y;
  if ((!map) || (map->tileWidth == 0) || (map->tileHeight == 0))
  {
    return NULL;
  }
  x = (eolInt)(fx / map->tileWidth);
  y = (eolInt)(fy / map->tileHeight);
  return eol_tile_get_by_tilexy(map,x,y);
}

/*tile type section*/


/*map Section*/

void eol_tile_map_draw(eolTileMap *map)
{
  GList *t;
  if (!map)return;
  /*TODO check for tile position relative to camera bounds*/
  for (t = map->map;t != NULL;t = t->next)
  {
    eol_tile_draw(t->data,map->tileWidth,map->tileHeight);
  }
}

eolTileMap * eol_tile_map_new()
{
  eolTileMap *tileMap = NULL;
  tileMap = (eolTileMap*)malloc(sizeof(eolTileMap));
  if (!tileMap)
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_tile_map_new: failed to allocate new tile map");
    return NULL;
  }
  memset(tileMap,0,sizeof(eolTileMap));
  return tileMap;
}

void eol_tile_map_free(eolTileMap **map)
{
  if (!map)return;
  eol_tile_map_delete(*map);
  *map = NULL;
}

void eol_tile_map_delete(eolTileMap *map)
{
  GList *l;
  eolTileType *tileType;
  if (!map)return;
  for (l = map->tileSet;l != NULL;l = l->next)
  {
    if (!l->data)continue;
    tileType = (eolTileType*)l->data;
    free(tileType);
  }
  g_list_free(map->tileSet);
  for (l = map->map;l != NULL;l = l->next)
  {
    eol_tile_delete((eolTile *)l->data);
  }
  g_list_free(map->map);
}


/*saving and loading*/
eolKeychain *eol_tile_set_build_keychain(GList *typeList)
{
  GList *l;
  eolKeychain *tileSet;
  eolKeychain *tileItem;
  eolTileType *tileData;
  if (!typeList)return NULL;
  tileSet = eol_keychain_new_list();
  if (!tileSet)return NULL;
  for (l = typeList;l != NULL;l = l->next)
  {
    if (!l->data)continue;
    tileData = (eolTileType *)l->data;
    tileItem = eol_keychain_new_hash();
    if (!tileItem)continue;
    eol_keychain_hash_insert(tileItem,"id",eol_keychain_new_uint(tileData->id));
    eol_keychain_hash_insert(tileItem,"footWidth",eol_keychain_new_uint(tileData->footWidth));
    eol_keychain_hash_insert(tileItem,"footHeight",eol_keychain_new_uint(tileData->footHeight));
    eol_keychain_hash_insert(tileItem,"ori",eol_keychain_new_orientation(tileData->ori));
    eol_keychain_hash_insert(tileItem,"actorFile",eol_keychain_new_string(tileData->actorFile));
    eol_keychain_list_append(tileSet,tileItem);
  }
  return tileSet;
}

eolKeychain *eol_tile_layout_build_keychain(GList *tileMap)
{
  GList *l;
  eolKeychain *tileLayout;
  eolKeychain *tileItem;
  eolTile *tileData;
  if (!tileMap)return NULL;
  tileLayout = eol_keychain_new_list();
  if (!tileLayout)return NULL;
  for (l = tileMap;l != NULL;l = l->next)
  {
    if (!l->data)continue;
    tileData = (eolTile*)l->data;
    tileItem = eol_keychain_new_hash();
    if (!tileItem)continue;
    eol_keychain_hash_insert(tileItem,"id",eol_keychain_new_uint(tileData->id));
    eol_keychain_hash_insert(tileItem,"x",eol_keychain_new_int(tileData->x));
    eol_keychain_hash_insert(tileItem,"y",eol_keychain_new_int(tileData->y));
    eol_keychain_hash_insert(tileItem,"ori",eol_keychain_new_orientation(tileData->ori));
    eol_keychain_list_append(tileLayout,tileItem);
  }
  return tileLayout;
}

eolKeychain *eol_tile_map_build_keychain(eolTileMap *map)
{
  eolKeychain *mapKeys;
  if (!map)return NULL;
  mapKeys = eol_keychain_new_hash();
  if (!mapKeys)return NULL;
  eol_keychain_hash_insert(mapKeys,"tileWidth",eol_keychain_new_float(map->tileWidth));
  eol_keychain_hash_insert(mapKeys,"tileHeight",eol_keychain_new_float(map->tileHeight));
  eol_keychain_hash_insert(mapKeys,"spaceWidth",eol_keychain_new_uint(map->spaceWidth));
  eol_keychain_hash_insert(mapKeys,"spaceHeight",eol_keychain_new_uint(map->spaceHeight));
  eol_keychain_hash_insert(mapKeys,"tileIdPool",eol_keychain_new_uint(map->tileIdPool));
  eol_keychain_hash_insert(mapKeys,"tileSet",eol_tile_set_build_keychain(map->tileSet));
  eol_keychain_hash_insert(mapKeys,"map",eol_tile_layout_build_keychain(map->map));
  return mapKeys;
}

/*eol@eof*/
