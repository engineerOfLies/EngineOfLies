#include "eol_model.h"
#include "eol_logger.h"
#include "eol_resource.h"
#include "eol_config.h"

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
  eolConfig *conf;
  _eol_model_max = 2048;
  conf = eol_config_load("system/model.cfg");
  if (!conf)return;
  eol_config_get_uint_by_tag(&_eol_model_max,conf,"model_max");
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
  int i;
  eolKeychain *key,*actions;
  eolLine temp,tempWord;
  eolInt sw = -1,sh = -1;
  eolUint actionCount = 0;
  eolConfig *config = NULL;
  eolModel *model;
  if (data == NULL)
    return eolFalse;
  config = eol_config_load(filename);
  if (!config)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_model:failed to load model %s\n",filename);
    return eolFalse;
  }
  
  model = (eolModel *)data;
  eol_config_get_keychain_by_tag(&actions,config,"actions");
  actionCount = eol_keychain_get_list_count(actions);
  if (actionCount > 0)
  {
    model->_actionList = (eolAction *)malloc(sizeof(eolAction)*actionCount);
    if (model->_actionList == NULL)
    {
      eol_logger_message(
        EOL_LOG_ERROR,
        "eol_model:unable to allocate action list for model\n");
        eol_config_free(&config);
        return eolFalse;
    }

    memset(model->_actionList,0,sizeof(eolAction)*actionCount);
    model->numActions = actionCount;
    for (i = 0; i < actionCount;i++)
    {
      key = eol_keychain_get_list_nth(actions, i);
      if (!key)continue;
      eol_keychain_get_hash_value_as_line(model->_actionList[i].name, key, "action");
      eol_keychain_get_hash_value_as_uint(&model->_actionList[i].begin, key, "begin");
      eol_keychain_get_hash_value_as_uint(&model->_actionList[i].end, key, "end");
      eol_keychain_get_hash_value_as_float(&model->_actionList[i].frameRate, key, "framerate");
      if (eol_keychain_get_hash_value_as_line(tempWord, key, "type"))
      {
        if (strncmp(tempWord,"loop",EOLWORDLEN) == 0)
        {
          model->_actionList[i].type = eolActionLoop;
        }
        if (strncmp(tempWord,"pass",EOLWORDLEN) == 0)
        {
          model->_actionList[i].type = eolActionPass;
        }
        if (strncmp(tempWord,"osci",EOLWORDLEN) == 0)
        {
          model->_actionList[i].type = eolActionOsci;
        }
      }
    }
  }

  eol_config_get_line_by_tag(model->name,config,"model");
  eol_config_get_line_by_tag(model->_meshFile,config,"mesh");
  eol_config_get_line_by_tag(model->_skinFile,config,"skin");
  if (!eol_config_get_vec3d_by_tag(&model->scale,config,"scale"))
  {
    eol_vec3d_set(model->scale,1,1,1);
  }
  eol_config_get_vec3d_by_tag(&model->rotation,config,"rotation");
  eol_config_get_line_by_tag(model->_armFile,config,"arm");
  eol_config_get_line_by_tag(model->_spriteFile,config,"sprite");
  if (eol_config_get_line_by_tag(temp,config,"spritecellsize"))
  {
    sscanf(temp, "%i %i",&sw,&sh);
  }
  eol_config_get_bool_by_tag(&model->_sprite_3D,config,"sprite_is_3D");

  /*load the requested assets*/
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
  eol_config_free(&config);
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
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_model: cannot draw a zero scaled model\n");
    return;
  }
  /*TODO: check to see if the model is on camera...*/
  glPushMatrix();
  glTranslatef(position.x,position.y,position.z);
  glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
  glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
  glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);
  glScalef(scale.x,
           scale.y,
           scale.z);

  if (model->_mesh != NULL)
  {
    if (model->_arm != NULL)
    {
      eol_armature_deform_mesh(model->_arm, model->_mesh,frame);
    }
    eol_mesh_draw(
        model->_mesh,
        model->offset,
        model->rotation,
        model->scale,
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
        model->offset,
        model->scale,
        model->rotation,
        color,
        alpha
      );
    }
    else
    {
      eol_sprite_draw_transformed(
        model->_sprite,
        frame,
        model->offset.x,
        model->offset.y,
        model->scale.x,
        model->scale.y,
        model->rotation.z,
        eolFalse,
        eolFalse,
        color,
        alpha
      );
    }
  }
  glPopMatrix();
}

void eol_model_draw_wire(
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
  scale.x *= model->scale.x;
  scale.y *= model->scale.y;
  scale.z *= model->scale.z;
  eol_vec3d_add(position,model->offset,position);
  eol_vec3d_add(rotation,model->rotation,rotation);
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
    eol_mesh_draw_wire(
        model->_mesh,
        position,
        rotation,
        scale,
        color,
        alpha
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

