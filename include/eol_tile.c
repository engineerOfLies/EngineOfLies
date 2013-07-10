#include "eol_tile.h"
#include "eol_logger.h"
#include "eol_mouse.h"
#include "eol_draw.h"
#include "eol_config.h"

/*TODO: make the tile system use the resource manager for tile types at the least*/
/*local variables*/

/*local function prototypes*/
eolTileType *eol_tile_set_get_tiletype_by_id(eolTileSet *set,eolUint id);
void eol_tile_type_delete(eolTileType *tile);
void eol_tile_map_delete_tiles_outside(eolTileMap *map);

/*definitions*/

/*tile type operations*/

eolTileSet *eol_tile_set_new()
{
  eolTileSet *set;
  set = (eolTileSet*)malloc(sizeof(eolTileSet));
  if (!set)return NULL;
  memset(set,0,sizeof(eolTileSet));
  /*setup the hash map*/
  set->tileSet = g_hash_table_new_full(g_str_hash,
                                       g_str_equal,
                                       (GDestroyNotify)eol_g_string_free,
                                       (GDestroyNotify)eol_tile_type_delete);
  return set;
}

void eol_tile_set_delete(eolTileSet *set)
{
  if (!set)return;
  /*handle the hashmap*/
  g_hash_table_destroy(set->tileSet);
  free(set);
}

void eol_tile_set_free(eolTileSet **set)
{
  if (!set)return;
  eol_tile_set_delete(*set);
  *set = NULL;
}

void eol_tile_map_assign_set(eolTileMap *map, eolTileSet *set)
{
  if ((!map)||(!set))return;
  map->tileSet = set;
}

void eol_tile_set_build_from_definition(eolTileSet *set,eolKeychain *tileTypeList)
{
  eolKeychain *link;
  eolLine filename;
  int i,count;
  if ((!set)||(!tileTypeList))return;
  count = eol_keychain_get_list_count(tileTypeList);
  for (i = 0; i < count;i++)
  {
    link = eol_keychain_get_list_nth(tileTypeList, i);
    if (eol_keychain_get_hash_value_as_line(filename,link,"typeFile"))
    {
      eol_tile_set_add_type_from_file(set,filename,NULL);
    }
    else
    {
      eol_tile_set_add_type(set,link,NULL);
    }
  }
}

void eol_tile_set_load_tile_set(eolTileSet *set,eolLine filename)
{
  eolKeychain *tileTypeList;
  eolConfig *conf;
  if (!set)return;
  conf = eol_config_load(filename);
  if (!conf)return;
  if (!eol_config_get_keychain_by_tag(&tileTypeList,conf,"tileSet"))
  {
    return;
  }
  eol_tile_set_build_from_definition(set,tileTypeList);
}

void eol_tile_type_delete(eolTileType *tile)
{
  if (!tile)return;
  free(tile);
}

void eol_tile_type_free(eolTileType **tile)
{
  if (!tile)
  {
    return;
  }
  eol_tile_type_delete(*tile);
  *tile = NULL;
}

eolTileType *eol_tile_type_new()
{
  eolTileType *newTile = NULL;
  newTile = (eolTileType *)malloc(sizeof(eolTileType));
  if (!newTile)return NULL;
  memset(newTile,0,sizeof(eolTileType));
  eol_orientation_clear(&newTile->ori);
  return newTile;
}

eolTileType *eol_tile_type_load(eolKeychain *tileType)
{
  eolTileType *newTile = NULL;
  if (!tileType)return NULL;
  newTile = eol_tile_type_new();
  if (!newTile)return NULL;
  eol_keychain_get_hash_value_as_uint(&newTile->footWidth, tileType,"footWidth");
  eol_keychain_get_hash_value_as_uint(&newTile->footHeight, tileType,"footHeight");
  eol_keychain_get_hash_value_as_orientation(&newTile->ori, tileType,"ori");
  eol_keychain_get_hash_value_as_line(newTile->actorFile, tileType,"actorFile");
  eol_keychain_get_hash_value_as_bool(&newTile->walkBlock, tileType,"walkBlock");
  eol_keychain_get_hash_value_as_bool(&newTile->hitBlock, tileType,"hitBlock");
  eol_keychain_get_hash_value_as_bool(&newTile->sightBlock, tileType,"sightBlock");
  return newTile;
}

void eol_tile_set_remove_type(eolTileSet *set,eolUint id)
{
  eolLine key;
  eolTileType *tile;
  if (!set)return;
  tile = eol_tile_set_get_tiletype_by_id(set,id);
  if (!tile)return;
  snprintf(key,EOLLINELEN,"%i",id);
  g_hash_table_remove(set->tileSet,key);
}

void eol_tile_set_add_type_overwrite_from_file(eolTileSet *set,eolLine filename,eolUint id)
{
  eolConfig *conf;
  if (!set)return;
  conf = eol_config_load(filename);
  if (!conf)return;
  eol_tile_set_add_type_overwrite(set,conf->_node,id);
  eol_config_free(&conf);
}

void eol_tile_set_add_type_overwrite(eolTileSet *set,eolKeychain *tileType,eolUint id)
{
  if ((!set)||(!tileType))return;
  eol_tile_set_remove_type(set,id);
  eol_tile_set_add_type(set,tileType,&id);
}

eolBool eol_tile_set_add_type_from_file(eolTileSet *set,eolLine filename,eolUint *id)
{
  eolBool rc = eolFalse;
  eolConfig *conf;
  if (!set)return eolFalse;
  conf = eol_config_load(filename);
  if (!conf)return eolFalse;
  rc = eol_tile_set_add_type(set,conf->_node,id);
  eol_config_free(&conf);
  return rc;
}

eolBool eol_tile_set_add_type(eolTileSet *set,eolKeychain *tileType,eolUint *id)
{
  eolUint newId;
  eolLine key;
  eolTileType *newTile;
  if ((!set) || (!tileType))return eolFalse;
  /*id Priority is from parameter, then hash, then new*/
  if (id)
  {
    newId = *id;
  }
  else if (!eol_keychain_get_hash_value_as_uint(&newId, tileType,"id"))
  {
    newId = set->idPool;
  }
  /* check for collision */
  newTile = eol_tile_set_get_tiletype_by_id(set,newId);
  if (newTile)
  {
    eol_logger_message(EOL_LOG_WARN,"Not adding new tilem collision with id %i",newId);
    return eolFalse;
  }
  
  newTile = eol_tile_type_load(tileType);
  if (!newTile)return eolFalse;
  newTile->id = newId;
  if (newId == set->idPool)
  {
    set->idPool++;
  }
  snprintf(key,EOLLINELEN,"%i",newId);
  g_hash_table_insert(set->tileSet,g_strdup(key),newTile);
  return eolTrue;
}

eolTileType *eol_tile_set_get_nth(eolTileSet *set,eolUint n)
{
  eolTileType *nth = NULL;
  GList *typeList;
  if (!set)return NULL;

  typeList = g_hash_table_get_values(set->tileSet);
  nth = g_list_nth_data(typeList,n);
  if (typeList)
  {
    g_list_free(typeList);
  }
  return nth;
}

eolUint eol_tile_set_get_count(eolTileSet *set)
{
  if (!set)return 0;
  return g_hash_table_size(set->tileSet);
}

eolTileType *eol_tile_set_get_tiletype_by_id(eolTileSet *set,eolUint id)
{
  eolLine key;
  if (!set)
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_tile_set_get_tiletype_by_id: passed a null set to search");
    return NULL;
  }
  snprintf(key,EOLLINELEN,"%i",id);
  return g_hash_table_lookup(set->tileSet,key);
}

/*tile operations*/

void eol_tile_draw(eolTile *tile,eolFloat tileWidth,eolFloat tileHeight)
{
  eolVec3D position = {0,0,0};
  eol_vec3d_copy(position,tile->ori.position);
  position.x += (tile->x * tileWidth) + (0.5 * tileWidth);
  position.y += (tile->y * tileHeight) + (0.5 * tileHeight);
  /*apply parent orientation*/
  glPushMatrix();
  if (tile->parentType)
  {
    glTranslatef(tile->parentType->ori.position.x,
                 tile->parentType->ori.position.y,
                 tile->parentType->ori.position.z);
    glRotatef(tile->parentType->ori.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(tile->parentType->ori.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(tile->parentType->ori.rotation.z, 0.0f, 0.0f, 1.0f);
    glScalef(tile->parentType->ori.scale.x,
             tile->parentType->ori.scale.y,
             tile->parentType->ori.scale.z);
  }
  
  eol_actor_draw(
    tile->actor,
    position,
    tile->ori.rotation,
    tile->ori.scale,
    tile->ori.color,
    tile->ori.alpha
  );
  glPopMatrix();
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
  eol_orientation_clear(&tile->ori);
  return tile;
}

void eol_tile_map_add_tile_by_definition(eolTileMap *map,eolKeychain *def)
{
  eolInt x;
  eolInt y;
  eolUint tileIndex;
  eolTile * tile;
  if ((!map)||(!def))return;
  eol_keychain_get_hash_value_as_int(&x,def,"x");
  eol_keychain_get_hash_value_as_int(&y,def,"y");
  eol_keychain_get_hash_value_as_uint(&tileIndex,def,"tileIndex");
  tile = eol_tile_add_to_map(map,x, y,tileIndex);
  if (!tileIndex)return;
  eol_keychain_get_hash_value_as_orientation(&tile->ori,def,"ori");
}

void eol_tile_add_to_map_overwrite(eolTileMap *map,eolInt x, eolInt y,eolUint tileIndex)
{
  if (!map)return;
  eol_tile_remove_from_map(map,x,y);
  eol_tile_add_to_map(map,x, y,tileIndex);
}

void eol_tile_sync_to_type(eolTileMap *map,eolTile *tile, eolTileType *tileType)
{
  if ((!map) || (!tile))return;
  if (!tileType)
  {
    /*check if the tile already has an association*/
    tileType = eol_tile_set_get_tiletype_by_id(map->tileSet,tile->tileType);
    if (tileType == NULL)
    {
      eol_logger_message(EOL_LOG_WARN,"eol_tile_sync_to_type: tile type %i does not exist",tile->tileType);
      return;
    }
  }
  else
  {
    tile->tileType = tileType->id;
  }
  tile->parentType = tileType;
  if (tile->actor)
  {
    eol_actor_free(&tile->actor);
  }
  tile->actor = eol_actor_load(tileType->actorFile);
  tile->walkBlock = tileType->walkBlock;
  tile->hitBlock = tileType->hitBlock;
  tile->sightBlock = tileType->sightBlock;
}

eolTile *eol_tile_add_to_map(eolTileMap *map,eolInt x, eolInt y,eolUint tileIndex)
{
  eolTile *tile;
  eolTileType *tileType;
  if (!map)return NULL;
  tileType = eol_tile_set_get_tiletype_by_id(map->tileSet,tileIndex);
  if (!tileType)
  {
    eol_logger_message(EOL_LOG_ERROR,"eol_tile_add_to_map: tile type with index %i not found.",tileIndex);
    return NULL;
  }
  if (eol_tile_get_by_tilexy(map,x,y))
  {
    eol_logger_message(EOL_LOG_INFO,"eol_tile_add_to_map: tile already at location (%i,%i).",x,y);
    return NULL;
  }
  tile = eol_tile_new();
  if (!tile)return NULL;
  tile->x = x;
  tile->y = y;
  /*TODO check for id collision*/
  tile->id = map->tileIdPool++;
  eol_tile_sync_to_type(map,tile, tileType);
  map->map = g_list_append(map->map,tile);
  return tile;
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

eolTile *eol_tile_get_by_mouse(eolTileMap *map, eolOrientation LayerOri)
{
  eolVec2D mousePosition;

  if (!eol_tile_get_spacexy_by_mouse(map, LayerOri, &mousePosition))
  {
    return NULL;
  }
  return eol_tile_get_by_spacexy(map,mousePosition.x, mousePosition.y);
}

eolBool eol_tile_get_tilexy_by_mouse(eolTileMap *map, eolOrientation LayerOri, eolUint *x, eolUint *y)
{
  eolVec2D mouseOut;
  if (!eol_tile_get_spacexy_by_mouse(map, LayerOri, &mouseOut))
  {
    return eolFalse;
  }
  if (!eol_tile_map_position_to_tile_xy(map,mouseOut,x, y))
  {
    return eolFalse;
  }
  return eolTrue;
}

eolBool eol_tile_get_spacexy_by_mouse(eolTileMap *map, eolOrientation LayerOri, eolVec2D *mouseOut)
{
  eolVec3D mousePosition;
  eolQuad3D quad;

  quad = eol_tile_map_get_bounding_quad(map);
  
  /*apply bounding quad orientation*/
  eol_3d_op_transform_quad_by_ori(&quad, quad,LayerOri);
  
  if (!eol_mouse_get_quad3d_intersect(&mousePosition,quad))
  {
    return eolFalse;
  }
  if (!mouseOut)return eolTrue;
  
  eol_3d_op_transform_vec3d_inverse_by_ori(&mousePosition, mousePosition,LayerOri);
  
  mouseOut->x = mousePosition.x;
  mouseOut->y = mousePosition.y;
  return eolTrue;
}
/*tile type section*/


/*map Section*/

eolQuad3D eol_tile_map_get_bounding_quad(eolTileMap *map)
{
  eolQuad3D quad =
  {
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0}
  };
  if (!map)
  {
    eol_logger_message(EOL_LOG_WARN,"eol_tile_map_get_bounding_quad:passed a NULL map");
    return quad;
  }
  quad.t1.x = map->mapRect.x;
  quad.t1.y = map->mapRect.y;
  
  quad.t2.x = map->mapRect.x + map->mapRect.w;
  quad.t2.y = map->mapRect.y;

  quad.t3.x = map->mapRect.x + map->mapRect.w;
  quad.t3.y = map->mapRect.y + map->mapRect.h;
  
  quad.t4.x = map->mapRect.x;
  quad.t4.y = map->mapRect.y + map->mapRect.h;
  return quad;
}


eolBool eol_tile_map_position_to_tile_xy(eolTileMap *map,eolVec2D position,eolUint *x, eolUint *y)
{
  eolVec2D positionInMap;
  if (!map)return eolFalse;
  if (!map->tileWidth)return eolFalse;
  if (!map->tileHeight)return eolFalse;
  if (!eol_vec_in_rect(position,map->mapRect))return eolFalse;
  eol_vec2d_sub(positionInMap,position,map->mapRect);/*this works because the rect starts off with an x and a y*/
  if (x)*x = (eolUint)(positionInMap.x/map->tileWidth);
  if (y)*y = (eolUint)(positionInMap.y/map->tileHeight);
  return eolTrue;
}

eolBool eol_tile_map_get_tile_rect(eolTileMap *map,eolRectFloat *rect,eolVec2D position)
{
  eolUint x,y;
  if (!map)return eolFalse;
  if (!eol_vec_in_rect(position,map->mapRect))return eolFalse;
  if (!eol_tile_map_position_to_tile_xy(map,position,&x, &y))
  {
    return eolFalse;
  }
  if (rect)
  {
    rect->x = (x * map->tileWidth) + map->mapRect.x;
    rect->y = (y * map->tileHeight) + map->mapRect.y;
    rect->w = map->tileWidth;
    rect->h = map->tileHeight;
  }
  return eolTrue;
}

void eol_tile_map_set_tile_size(eolTileMap *map,eolFloat tileWidth, eolFloat tileHeight)
{
  if (!map)return;
  if ((tileWidth == 0) || (tileHeight == 0))return;
  map->tileWidth = tileWidth;
  map->tileHeight = tileHeight;
  map->mapRect.w = map->tileWidth * map->spaceWidth;
  map->mapRect.h = map->tileHeight * map->spaceHeight;
  eol_vec2d_set(map->mapRect,
                map->mapRect.w*-0.5,
                map->mapRect.h*-0.5);
}

void eol_tile_map_set_space_size(eolTileMap *map,eolUint spaceWidth, eolUint spaceHeight)
{
  if (!map)return;
  if ((spaceWidth == 0) || (spaceHeight == 0))return;
  map->spaceWidth = spaceWidth;
  map->spaceHeight = spaceHeight;
  map->mapRect.w = map->tileWidth * map->spaceWidth;
  map->mapRect.h = map->tileHeight * map->spaceHeight;
  eol_vec2d_set(map->mapRect,
                map->mapRect.w*-0.5,
                map->mapRect.h*-0.5);
  /*remove tiles outside the new size*/
  eol_tile_map_delete_tiles_outside(map);
}


void eol_tile_grid_draw(eolTileMap *map, eolVec3D color)
{
  int i;
  eolOrientation ori;
  if (!map)return;
  eol_orientation_clear(&ori);
  eol_vec3d_copy(ori.color,color);
  eol_draw_rect_3D(map->mapRect,1, ori);
  for (i = 1; i < map->spaceWidth;i++)
  {
    eol_draw_line_3D(eol_vec3d(map->mapRect.x + (i * map->tileWidth),
                               map->mapRect.y,
                               0),
                     eol_vec3d(map->mapRect.x + (i * map->tileWidth),
                               map->mapRect.y + map->mapRect.h,
                               0),
                     1,
                     color,
                     1);
  }
  for (i = 1; i < map->spaceHeight;i++)
  {
    eol_draw_line_3D(eol_vec3d(map->mapRect.x,
                               map->mapRect.y + (i * map->tileHeight),
                               0),
                     eol_vec3d(map->mapRect.x + map->mapRect.w,
                               map->mapRect.y + (i * map->tileHeight),
                               0),
                     1,
                     color,
                     1);
  }
}

void eol_tile_map_draw(eolTileMap *map)
{
  GList *t;
  if (!map)return;
  /*TODO check for tile position relative to camera bounds*/
  glPushMatrix();
  glTranslatef(map->mapRect.x,
               map->mapRect.y,
               0);
  
  for (t = map->map;t != NULL;t = t->next)
  {
    eol_tile_draw(t->data,map->tileWidth,map->tileHeight);
  }
  glPopMatrix();
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
  /*set some defaults*/
  tileMap->tileWidth = 1;
  tileMap->tileHeight = 1;
  tileMap->spaceWidth = 16;
  tileMap->spaceHeight= 16;
  tileMap->mapRect.x = -8;
  tileMap->mapRect.y = -8;
  tileMap->mapRect.w = 16;
  tileMap->mapRect.h = 16;
  
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
  if (!map)return;
  for (l = map->map;l != NULL;l = l->next)
  {
    eol_tile_delete((eolTile *)l->data);
  }
  g_list_free(map->map);
}

void eol_tile_map_delete_tiles_outside(eolTileMap *map)
{
  int i;
  eolTile *tile;
  if (!map)return;
  while (i < g_list_length(map->map))
  {
    tile = g_list_nth_data(map->map,i);
    if ((tile->x >= map->spaceWidth)||(tile->y >= map->spaceHeight))
    {
      eol_tile_remove_from_map(map,tile->x, tile->y);
    }
    else
    {
      i++;
    }
  }
}

void eol_tile_map_delete_tiles_by_type(eolTileMap *map, eolUint tileIndex)
{
  int i;
  eolTile *tile;
  if (!map)return;
  while (i < g_list_length(map->map))
  {
    tile = g_list_nth_data(map->map,i);
    if (tile->tileType == tileIndex)
    {
      eol_tile_remove_from_map(map,tile->x, tile->y);
    }
    else
    {
      i++;
    }
  }
}
  
void eol_tile_map_sync_tiles(eolTileMap *map)
{
  GList *t;
  if ((!map) || (!map->tileSet) || (!map->map))return;
  for (t = map->map;t != NULL;t = t->next)
  {
    if (!t->data)continue;
    eol_tile_sync_to_type(map,t->data, NULL);
  }
}


/*saving and loading*/
eolKeychain *eol_tile_set_build_keychain(eolTileSet *set)
{
  GList *l;
  GList *typeList;
  eolKeychain *tileSet;
  eolKeychain *tileItem;
  eolTileType *tileData;
  if (!set)return NULL;
  typeList = g_hash_table_get_values(set->tileSet);
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
    eol_keychain_hash_insert(tileItem,"walkBlock",eol_keychain_new_bool(tileData->walkBlock));
    eol_keychain_hash_insert(tileItem,"hitBlock",eol_keychain_new_bool(tileData->hitBlock));
    eol_keychain_hash_insert(tileItem,"sightBlock",eol_keychain_new_bool(tileData->sightBlock));
    eol_keychain_list_append(tileSet,tileItem);
  }
  g_list_free(typeList);
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
  eolKeychain *key;
  if (!map)return NULL;
  mapKeys = eol_keychain_new_hash();
  if (!mapKeys)return NULL;
  eol_keychain_hash_insert(mapKeys,"tileWidth",eol_keychain_new_float(map->tileWidth));
  eol_keychain_hash_insert(mapKeys,"tileHeight",eol_keychain_new_float(map->tileHeight));
  eol_keychain_hash_insert(mapKeys,"spaceWidth",eol_keychain_new_uint(map->spaceWidth));
  eol_keychain_hash_insert(mapKeys,"spaceHeight",eol_keychain_new_uint(map->spaceHeight));
  eol_keychain_hash_insert(mapKeys,"tileIdPool",eol_keychain_new_uint(map->tileIdPool));
  key = eol_tile_layout_build_keychain(map->map);
  if (key != NULL)
  {
    eol_keychain_hash_insert(mapKeys,"map",key);
  }
  return mapKeys;
}

eolTileMap *eol_tile_map_build_from_definition(eolKeychain *def)
{
  eolTileMap *map;
  if (!def)return NULL;
  map = eol_tile_map_new();
  if (!map)return NULL;
  eol_keychain_get_hash_value_as_float(&map->tileWidth,def,"tileWidth");
  eol_keychain_get_hash_value_as_float(&map->tileHeight,def,"tileHeight");
  eol_keychain_get_hash_value_as_uint(&map->spaceWidth,def,"spaceWidth");
  eol_keychain_get_hash_value_as_uint(&map->spaceHeight,def,"spaceHeight");
  eol_keychain_get_hash_value_as_uint(&map->tileIdPool,def,"tileIdPool");
  /*TODO build tile map from definition*/
  return map;
}

/*eol@eof*/
