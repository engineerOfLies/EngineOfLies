#include "eol_tile.h"
#include "eol_logger.h"
/*local variables*/

/*local function prototypes*/

/*definitions*/


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
  eolTile *tile;
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
    if (!l->data)continue;
    tile = l->data;
    eol_actor_free(&tile->actor);
    free(tile);
  }
  g_list_free(map->map);
}

eolTile *eol_tile_map_get_by_tilexy(eolTileMap *map,eolInt x, eolInt y)
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

eolTile *eol_tile_map_get_by_spacexy(eolTileMap *map,eolFloat fx, eolFloat fy)
{
  eolUint x,y;
  if ((!map) || (map->tileWidth == 0) || (map->tileHeight == 0))
  {
    return NULL;
  }
  x = (eolInt)(fx / map->tileWidth);
  y = (eolInt)(fy / map->tileHeight);
  return eol_tile_map_get_by_tilexy(map,x,y);
}

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
