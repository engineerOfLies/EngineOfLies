#include "eol_model.h"
#include "eol_logger.h"
#include "eol_loader.h"
#include "eol_resource.h"

/*local global variables*/

eolBool              _eol_model_initialized = eolFalse;
eolUint              _eol_model_max = 0;
eolResourceManager * _eol_model_manager = NULL;

/*local function prototypes*/
void eol_model_delete(void *model);
eolBool eol_model_load_data_from_file(char * filename,void *data);

void eol_model_init()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_model:initializing\n");
_eol_model_manager = eol_resource_manager_init(
    "eol_model_manager",
    _eol_model_max,
    sizeof(eolModel),
    eolFalse,
    eol_model_delete,
    eol_model_load_data_from_file
    );

  eol_logger_message(
      EOL_LOG_INFO,
      "eol_model:initialized\n");
  atexit(eol_model_close);
  _eol_model_initialized = eolTrue;
}

void eol_model_config()
{
  _eol_model_max = 2048;
}

void eol_model_free(
    eolModel **model
  )
{
  if (!eol_model_initialized())return;
  eol_resource_free_element(_eol_model_manager,(void **)model);
}

eolModel *eol_model_load(
    char *filename
  )
{
  eolModel *model;
  if (!eol_model_initialized())
  {
    return NULL;
  }
  model =
    (eolModel *)eol_resource_manager_load_resource(_eol_model_manager,filename);
  return model;
}

void eol_model_delete(void *modelData)
{
  eolModel *model;
  if (!modelData)return;
  model = (eolModel *)(modelData);
  if (model->_actionList != NULL)
  {
    free(model->_actionList);
    model->_actionList = NULL;
  }
  if (model->_mesh != NULL)
  {
    eol_mesh_free(&model->_mesh);
  }
  if (model->_skin != NULL)
  {
    eol_sprite_free(&model->_skin);
  }
  if (model->_sprite != NULL)
  {
    eol_sprite_free(&model->_sprite);
  }
  if (model->_arm != NULL)
  {
    eol_armature_free(&model->_arm);
  }
  memset(model,0,sizeof(eolModel));
}

void eol_model_clean()
{
  if (!eol_model_initialized())
  {
    eol_logger_message(
        EOL_LOG_INFO,
        "eol_model:cannot clean, not initialized\n");
    return;
  }
  if (!_eol_model_manager)return;
  eol_resource_manager_clean(_eol_model_manager);
}

void eol_model_close()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_model:closing\n");
  eol_resource_manager_free(&_eol_model_manager);
  _eol_model_max = 0;
  _eol_model_initialized = eolFalse;
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_model:closed\n");
}

eolBool eol_model_load_data_from_file(char * filename,void *data)
{
  eolLine buf;
  eolFile *file;
  eolModel *model;
  eolInt temp;
  eolInt sw = -1,sh = -1;
  file = eol_loader_read_file(filename);

  if (file == NULL)
    return eolFalse;
  if (data == NULL)
    return eolFalse;
    
  model = (eolModel *)data;
  while(fscanf(file->file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"model:") == 0)
    {
      fscanf(file->file, "%s",model->name);
      continue;
    }
    if(strcmp(buf,"mesh:") == 0)
    {
      fscanf(file->file, "%s",model->_meshFile);
      continue;
    }
    if(strcmp(buf,"skin:") == 0)
    {
      fscanf(file->file, "%s",model->_skinFile);
      continue;
    }
    if(strcmp(buf,"arm:") == 0)
    {
      fscanf(file->file, "%s",model->_armFile);
      continue;
    }
    if(strcmp(buf,"sprite:") == 0)
    {
      fscanf(file->file, "%s",model->_spriteFile);
      continue;
    }
    if(strcmp(buf,"spritecellsize:") == 0)
    {
      fscanf(file->file, "%i %i",&sw,&sh);
      continue;
    }
    if(strcmp(buf,"sprite_is_3D") == 0)
    {
      fscanf(file->file, "%i",&temp);
      if (temp != eolFalse)
      {
        model->_sprite_3D = eolTrue;
      }
      else
      {
        model->_sprite_3D = eolFalse;
      }
      continue;
    }
  }
  eol_loader_close_file(&file);
  if (strlen(model->_spriteFile) > 0)
  {
    model->_sprite = eol_sprite_load(model->_spriteFile,sw,sh);
  }
  if (strlen(model->_skinFile) > 0)
  {
    model->_skin = eol_sprite_load(model->_skinFile,-1,-1);
  }
  if (strlen(model->_meshFile) > 0)
  {
    model->_mesh = eol_mesh_load(model->_meshFile);
  }
  if (strlen(model->_armFile) > 0)
  {
    model->_arm = eol_armature_load(model->_armFile);
    if (model->_mesh != NULL)
    {
      eol_armature_link_mesh(model->_arm,model->_mesh);
    }
  }
  return eolTrue;
}

eolAction * eol_model_get_action(
    eolModel *model,
    eolWord   aName
  )
{
  int i;
  if (!model)return NULL;
  if (!eol_model_initialized())return NULL;
  for (i = 0; i < model->numActions;i++)
  {
    if (strncmp(model->_actionList[i].name,aName,EOLWORDLEN) == 0)
    {
      return &model->_actionList[i];
    }
  }
  return NULL;
}

eolBool eol_model_initialized()
{
  return _eol_model_initialized;
}

void eol_model_draw(
    eolModel *model,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha,
    eolUint  frame
  )
{
  if (!model)return;
  if (alpha == 0)
  {
    /*cannot draw a clear model, so don't waste the math*/
    return;
  }
  if ((scale.x == 0) &&
      (scale.y == 0) &&
      (scale.z == 0))
  {
    /*cannot draw a zero scaled model*/
    return;
  }
  /*TODO: check to see if the model is on camera...*/
  if (model->_mesh != NULL)
  {
    if (model->_arm != NULL)
    {
      eol_armature_deform_mesh(model->_arm, model->_mesh,frame);
    }
  	eol_mesh_draw(
        model->_mesh,
        position,
        rotation,
        scale,
        color,
        alpha,
        model->_skin
      );    
  }
  if (model->_sprite != NULL)
  {
    if (model->_sprite_3D)
    {
      eol_sprite_draw_transformed_3d(
        model->_sprite,
        frame,
        position,
        scale,
        rotation,
        color,
        alpha
      );
    }
    else
    {
      eol_sprite_draw_transformed(
        model->_sprite,
        frame,
        position.x,
        position.y,
        scale.x,
        scale.y,
        rotation.z,
        eolFalse,
        eolFalse,
        color,
        alpha
      );
    }
  }
}

/*eol@eof*/

