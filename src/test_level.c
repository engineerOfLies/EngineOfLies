#include <eol_level.h>

void CreateTestLevel()
{
  eolLevel *level = NULL;
  eolLevelLayer *layer = NULL;
  eolBackground *back = NULL;
  level = eol_level_new();
  if (!level)return;
  layer = eol_level_add_layer(level);
  if (!layer)
  {
    eol_level_free(&level);
    return;
  }
  back = eol_level_add_background_to_layer(layer);
  if (!back)
  {
    eol_level_free(&level);
    return;
  }
  eol_word_cpy(level->idName,"test_level");
  eol_word_cpy(layer->idName,"test_layer");
  eol_orientation_clear(&back->ori);

  back->model = eol_model_load("models/testlevel.actor");
  layer->clipMesh = eol_mesh_load("models/levelmesh/testlevel_mask.obj");
  if (layer->clipMesh != NULL)
  {
    eol_level_add_mask_to_space(layer);
  }
  eol_orientation_clear(&layer->clipMaskOri);

  eol_rectf_set(&layer->bounds, -5, -5, 10, 10);

  eol_level_set_active_layer(level, 0);
  eol_level_set_current_level(level);
}

/*eol@eof*/
