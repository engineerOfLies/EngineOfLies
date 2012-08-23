#include "eol_level.h"
#include "eol_draw.h"
#include "eol_logger.h"
#include "eol_resource.h"
#include "eol_config.h"

/*local global variables*/
eolBool     _eol_level_initialized = eolFalse;
eolResourceManager * _eol_level_manager = NULL;
eolLevel * _eol_level_list = NULL;
eolLevel * _eol_level_current = NULL;
eolUint    _eol_level_max_layers = 1;
eolUint    _eol_level_clip_iterations = 10;
eolFloat   _eol_level_clip_step = 0.1;
eolFloat   _eol_level_clip_thickness = 0.5;
eolFloat   _eol_level_slop = 0.1;
eolFloat   _eol_level_bias = 0.1;
eolUint    _eol_level_layer_draw_range = 0;
eolUint    _eol_level_draw_mode = 0;
eolUint    _eol_level_max = 0; /**<maximum number of levels that can be loaded
                                   at a time, ie: buffered*/
eolSpawnGeneric _eol_level_spawn_generic = NULL;



/*local function prototypes*/
void eol_level_close();
void eol_level_delete(void *leveldata);
eolBool eol_level_load_data_from_file(char * filename,void *data);

/*function definitions*/
void eol_level_init()
{
  cpInitChipmunk();
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_level:initializing\n");
  _eol_level_manager = eol_resource_manager_init(
    "eol_level_manager",
    _eol_level_max,
    sizeof(eolLevel),
    eolTrue,
    eol_level_delete,
    eol_level_load_data_from_file
    );
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_level:initialized\n");
  atexit(eol_level_close);
  _eol_level_initialized = eolTrue;

}

eolBool eol_level_initialized()
{
  return _eol_level_initialized;
}

void eol_level_config()
{
  eolConfig *conf = NULL;
  _eol_level_max_layers = 3;
  _eol_level_max = 10;
  _eol_level_layer_draw_range = 1;
  _eol_level_draw_mode = eolLevelDrawFull;
  conf = eol_config_load("system/level.cfg");
  if (conf == NULL)return;
  eol_config_get_uint_by_tag(&_eol_level_draw_mode,conf,"drawLevel");
  eol_config_get_uint_by_tag(&_eol_level_layer_draw_range,conf,"drawDepth");
  eol_config_get_uint_by_tag(&_eol_level_clip_iterations ,conf,"iterations");
  eol_config_get_float_by_tag(&_eol_level_clip_step ,conf,"clipStep");
  eol_config_get_float_by_tag(&_eol_level_clip_thickness ,conf,"clipThickness");
  eol_config_get_float_by_tag(&_eol_level_slop,conf,"clipSlop");
  eol_config_get_float_by_tag(&_eol_level_bias,conf,"clipBias");
  
}

void eol_level_set_draw_mode(eolUint mode)
{
  _eol_level_draw_mode = mode;
}

void eol_level_register_spawn_generic(eolSpawnGeneric spawnGeneric)
{
  _eol_level_spawn_generic = spawnGeneric;
}

void eol_level_clear()
{
  if (!eol_level_initialized())
  {
    eol_logger_message(
        EOL_LOG_INFO,
        "eol_level:cannot clean, not initialized\n");
    return;
  }
  if (!_eol_level_manager)return;
  eol_resource_manager_clean(_eol_level_manager);

}

void eol_level_close()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_level:closing\n"
  );
  eol_resource_manager_free(&_eol_level_manager);
  _eol_level_max = 0;
  _eol_level_max_layers = 1;
  _eol_level_initialized = eolFalse;
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_level:closed\n"
  );
}

void eol_level_free(eolLevel **level)
{
  if (!eol_level_initialized())return;
  if (_eol_level_current == *level)
  {
    _eol_level_current = NULL;
  }
  eol_resource_free_element(_eol_level_manager,(void **)level);
}

void eol_level_delete_layer(eolLevelLayer * level)
{
  eolBackground *b = NULL;
  GList *s,*e;
  if (!level)return;

  /*TODO: free tile set*/
  eol_mesh_free(&level->clipMesh);
  
  if (level->tileMap.map != NULL)free(level->tileMap.map);
  
  for (s = level->spawnList; s != NULL; s = s->next)
  {
    eol_spawn_free((eolSpawn *)s->data);
  }
  s = NULL;
  g_list_free(level->spawnList);
  for (s = level->backgrounds; s != NULL; s = s->next)
  {
    if (s->data == NULL)continue;
    b = (eolBackground *)s->data;
    eol_model_free(&b->model);
    free(b);
  }
  s = NULL;
  g_list_free(level->backgrounds);

  for (e = level->entities;e != NULL;e = e->next)
  {
    eol_entity_remove_from_space(e->data);
  }
  g_list_free(level->entities);/*just free our pointers to em, they can delete themselves*/
  if (level->space != NULL)cpSpaceFree(level->space);
}

void eol_level_delete(void *data)
{
  eolLevel *level;
  GList *l = NULL;
  if (!data)return;
  level = (eolLevel *)data;
  for (l = level->layers; l != NULL;l = l->next)
  {
    eol_level_delete_layer(l->data);
  }
  g_list_free(level->layers);
  memset(level,0,sizeof(eolLevel));
}

eolBool eol_level_load_data_from_file(char * filename,void *data)
{
  return eolTrue;
}

eolLevelLayer *eol_level_add_layer(eolLevel *level)
{
  eolLevelLayer *layer = NULL;
  if (!level)return NULL;
  layer = (eolLevelLayer*)malloc(sizeof(eolLevelLayer));
  if (layer == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_level:failed to allocated a layer for level %s\n",
      level->idName);
    return NULL;
  }
  memset(layer,0,sizeof(eolLevelLayer));

  layer->space = cpSpaceNew();
  if (layer->space == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "Unable to create a physics space for new layer in level %s!\n",
      level->idName);
    eol_level_delete_layer(layer);
    return NULL;
  }
  layer->space->iterations = _eol_level_clip_iterations;
  layer->space->sleepTimeThreshold = 999999;
  cpSpaceSetCollisionSlop(layer->space, _eol_level_slop);
  cpSpaceSetCollisionBias(layer->space, _eol_level_bias);
  
  level->layers = g_list_append(level->layers,layer);
  level->layerCount++;
  return layer;
}

eolBackground *eol_level_add_background_to_layer(eolLevelLayer *layer)
{
  eolBackground *back;
  if (!layer)return NULL;
  back = (eolBackground*)malloc(sizeof(eolBackground));
  if (back == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_level:failed to allocated a background for layer %s\n",
      layer->idName);
    return NULL;
  }
  layer->backgrounds = g_list_append(layer->backgrounds,back);
  return back;
}

eolLevel *eol_level_new()
{
  eolLevel *level = NULL;
  if (!eol_level_initialized())
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_entdow:used uninitialized\n");
    return NULL;
  }
  level = (eolLevel *)eol_resource_new_element(_eol_level_manager);
  if (level == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_level:failed to get new resource\n");
    return NULL;
  }
  return level;
}

/*
  *** DRAWING ***
*/

void eol_level_draw_current()
{
  if (!eol_level_initialized())return;
  if (_eol_level_current == NULL)return;
  switch (_eol_level_draw_mode)
  {
    case eolLevelDrawFull:
      eol_level_draw(_eol_level_current);
      break;
    case eolLevelDrawClip:
      eol_level_draw_clip(_eol_level_current);
      break;
  }
}

void eol_level_draw_background(eolBackground * back)
{
  if (!back)return;
  if (!back->model)return;
  eol_model_draw(
    back->model,
    back->ori.position,
    back->ori.rotation,
    back->ori.scale,
    back->ori.color,
    back->ori.alpha,
    0
  );
}

void eol_level_draw_layer_backgrounds(eolLevelLayer *layer)
{
  GList *b;
  if (!layer)return;
  for (b = layer->backgrounds; b != NULL; b= b->next)
  {
    eol_level_draw_background((eolBackground *)b->data);
  }
}

void eol_level_draw_layer_clipmask(eolLevelLayer *layer)
{
  if (!layer)return;
  eol_mesh_draw_wire(
    layer->clipMesh,
    layer->clipMaskOri.position,
    layer->clipMaskOri.rotation,
    layer->clipMaskOri.scale,
    layer->clipMaskOri.color,
    layer->clipMaskOri.alpha
  );

}

void eol_level_draw_layer_bounds(eolLevelLayer *layer)
{
  eolOrientation ori;
  if (!layer)return;
  eol_orientation_clear(&ori);
  eol_draw_rect_3D(layer->bounds,ori);
}

void eol_level_draw(eolLevel *level)
{
  int i;
  GList *layerNode = NULL;
  eolLevelLayer *layer = NULL;
  float alpha = 1;
  if (!level)return;
  if (!level->layers)return;
  i = level->active - _eol_level_layer_draw_range;
  if (i < 0)i = 0;
  layerNode = g_list_nth(level->layers,i);
  for (;(layerNode != NULL) && (i < level->active + _eol_level_layer_draw_range);
        layerNode = layerNode->next,i++)
  {
    if (layerNode->data == NULL)continue;
    if (i >= level->active)alpha = 0.25;
    layer = (eolLevelLayer *)layerNode->data;
    layer->alpha = alpha;
    eol_level_draw_layer_backgrounds(layer);
  }
}

void eol_level_draw_clip(eolLevel *level)
{
  eolLevelLayer *layer;
  if (!level)return;
  if (!level->layers)return;
  layer = g_list_nth_data(level->layers,level->active);
  if (layer == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "cannot get layer %n for level %s\n",
      level->active,
      level->idName);
    return;
  }
  eol_level_draw_layer_bounds(layer);
  eol_level_draw_layer_clipmask(layer);
}

void eol_level_set_active_layer(eolLevel *level, eolUint layer)
{
  if (!level)return;
  if (!level->layers)return;
  if (layer >= level->layerCount)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_level:cannot set active layer %i for level %s\n",
      layer,
      level->idName);
    return;
  }
  level->active = layer;
}

void eol_level_set_current_level(eolLevel *level)
{
  if (!level)return;
  if (!level->layers)return;
  _eol_level_current = level;
}

/*
 *** physics ***
*/

cpShape *eol_level_add_segment_to_space(eolFloat sx,
                                        eolFloat sy,
                                        eolFloat ex,
                                        eolFloat ey,
                                        cpSpace *space)
{
  cpShape *shape;
  float friction = 0.1;
  if(space == NULL)return NULL;
  shape = cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(sx,sy), cpv(ex,ey), _eol_level_clip_thickness);
  if (sx == ex)
  {
    friction = 0;
  }
  if(shape != NULL)
  {
    shape->e = 0;
    shape->u = friction;
    shape->collision_type = eolEntityClipLevel;
    cpShapeSetLayers (shape, eolEntityClipLevel);
    cpSpaceAddStaticShape(space, shape);
  }
  return shape;
}

void eol_level_add_triangle_to_space(cpSpace *space,eolFace *face,eolMesh *mask)
{
  if ((!space)||(!face)||(!mask))return;
  /*TODO dont add duplicate edges to the space*/
  eol_level_add_segment_to_space(mask->_vertices[face->vertices[0]].x,
                                 mask->_vertices[face->vertices[0]].y,
                                 mask->_vertices[face->vertices[1]].x,
                                 mask->_vertices[face->vertices[1]].y,
                                 space);

  eol_level_add_segment_to_space(mask->_vertices[face->vertices[1]].x,
                                 mask->_vertices[face->vertices[1]].y,
                                 mask->_vertices[face->vertices[2]].x,
                                 mask->_vertices[face->vertices[2]].y,
                                 space);

  eol_level_add_segment_to_space(mask->_vertices[face->vertices[2]].x,
                                 mask->_vertices[face->vertices[2]].y,
                                 mask->_vertices[face->vertices[1]].x,
                                 mask->_vertices[face->vertices[1]].y,
                                 space);
  /*TODO see if I need to catch these and free em up later.*/
}

void eol_level_add_mask_to_space(eolLevelLayer *layer)
{
  int i;
  if (!layer)return;
  if ((!layer->space)||(!layer->clipMesh))
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "cannot add clipmesh to space %s: missing data\n",
      layer->idName);
    return;
  }
  for (i = 0; i < layer->clipMesh->_numFaces;i++)
  {
    eol_level_add_triangle_to_space(layer->space,
                                    &layer->clipMesh->_faces[i],
                                    layer->clipMesh);
  }
}

void eol_level_update_active()
{
  eol_level_update(_eol_level_current);
}

void eol_level_update(eolLevel *level)
{
  eolLevelLayer *layer = NULL;
  if (!level)return;
  layer = g_list_nth_data(level->layers,level->active);
  if (layer == NULL)return;
  /*presync*/
  eol_entity_presync_all();

  cpSpaceStep(layer->space,_eol_level_clip_step);

  /*post sync*/
  eol_entity_postsync_all();
}

void eol_level_add_entity_to_active_layer(eolEntity *ent)
{
  if (!_eol_level_current)return;
  eol_level_add_entity_to_layer(g_list_nth_data(_eol_level_current->layers,
                                                _eol_level_current->active),
                                ent);
}

void eol_level_add_entity_to_layer(eolLevelLayer *layer, eolEntity *ent)
{
  if ((!layer) || (!ent))return;
  eol_entity_add_to_space(ent,layer->space);
  if (ent->_space != layer->space)return;
  layer->entities = g_list_append(layer->entities,ent);
}

void eol_level_remove_entity_from_layer(eolLevelLayer *layer, eolEntity *ent)
{
  if ((!layer) || (!ent))return;
  layer->entities = g_list_remove(layer->entities,ent);
  eol_entity_remove_from_space(ent);
}

void eol_level_remove_entity_from_active_layer(eolEntity *ent)
{
  if (!_eol_level_current)return;
  eol_level_remove_entity_from_layer(g_list_nth_data(_eol_level_current->layers,
                                                     _eol_level_current->active),
                                     ent);
}
/*eol@eof*/
