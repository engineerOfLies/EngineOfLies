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
eolUint    _eol_level_max = 0; /**<maximum number of levels that can be loaded
                                   at a time, ie: buffered*/


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
  int i;
  eolLevel *level;
  if (!data)return;
  level = (eolLevel *)data;
  for (i = 0; i < level->layerCount;i++)
  {
    eol_level_delete_layer(&level->layers[i]);
  }
  free(level->layers);
  memset(level,0,sizeof(eolLevel));
}

eolBool eol_level_load_data_from_file(char * filename,void *data)
{
  return eolTrue;
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
  eol_mesh_draw(
    layer->clipMesh,
    layer->clipMaskOri.position,
    layer->clipMaskOri.rotation,
    layer->clipMaskOri.scale,
    layer->clipMaskOri.color,
    layer->clipMaskOri.alpha,
    NULL
  );

}

void eol_level_draw_layer_bounds(eolLevelLayer *layer)
{
  eolOrientation ori;
  if (!layer)return;
  eol_orientation_clear(&ori);
  eol_draw_rect_3D(layer->bounds,ori);
}

/*eol@eof*/
