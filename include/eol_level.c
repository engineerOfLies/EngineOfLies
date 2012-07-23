#include "eol_draw.h"
#include "eol_level.h"
#include "eol_logger.h"
#include "eol_resource.h"

/*local global variables*/
eolBool     _eol_level_initialized = eolFalse;
eolResourceManager * _eol_level_manager = NULL;
eolLevel * _eol_level_list = NULL;
eolLevel * _eol_level_current = NULL;
eolUint    _eol_level_max_layers = 1;
eolUint    _eol_level_layer_draw_range = 0;
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
  /*TODO: load from config*/
  _eol_level_max_layers = 3;
  _eol_level_max = 10;
  _eol_level_layer_draw_range = 1;
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
  GList *s;

  if (!level)return;
  
  eol_sprite_free(&level->tileMap.tileSet);
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

  /*TODO: need to make sure I dont have to handle entities added to this space
  separately.*/
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

/*DRAWING*/

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
  if ((level->active < 0) || (level->active > level->layerCount))return;
  layer = g_list_nth_data(level->layers,level->active);
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

/*eol@eof*/
