#include "eol_actor.h"
#include "eol_logger.h"
#include "eol_loader.h"

/*local global variables*/

eolBool              _eol_actor_initialized = eolFalse;
eolUint              _eol_actor_max = 0;
eolResourceManager * _eol_actor_manager = NULL;

/*local function prototypes*/
void eol_actor_delete(void *act);
eolBool eol_actor_load_data_from_file(char * filename,void *data);

void eol_actor_init()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_actor:initializing\n");
_eol_actor_manager = eol_resource_manager_init(
    "eol_actor_manager",
    _eol_actor_max,
    sizeof(eolActor),
    eol_actor_delete,
    eol_actor_load_data_from_file
    );

  eol_logger_message(
      EOL_LOG_INFO,
      "eol_actor:initialized\n");
  atexit(eol_actor_close);
  _eol_actor_initialized = eolTrue;
}

void eol_actor_config()
{
  _eol_actor_max = 2048;
}

void eol_actor_free(
    eolActor **act
  )
{
  if (!eol_actor_initialized())return;
  eol_resource_free_element(_eol_actor_manager,(void **)act);
}

eolActor *eol_actor_load(
    char *filename
  )
{
  eolActor *actor;
  if (!eol_actor_initialized())
  {
    return NULL;
  }
  actor =
    (eolActor *)eol_resource_manager_load_resource(_eol_actor_manager,filename);
  if (actor != NULL)
  {
    fprintf(stdout,"actor loaded %s\n",actor->name);
  }
  else
    fprintf(stdout,"actor failed to load\n");
  return actor;
}
void eol_actor_delete(void *act)
{
  eolActor *actor;
  if (!act)return;
  actor = (eolActor *)(act);
  if (actor->_actionList != NULL)
  {
    free(actor->_actionList);
    actor->_actionList = NULL;
  }
  if (actor->_mesh != NULL)
  {
    eol_mesh_free(&actor->_mesh);
  }
  if (actor->_skin != NULL)
  {
    eol_sprite_free(&actor->_skin);
  }
  if (actor->_sprite != NULL)
  {
    eol_sprite_free(&actor->_sprite);
  }
  if (actor->_arm != NULL)
  {
    eol_armature_free(&actor->_arm);
  }
  memset(actor,0,sizeof(eolActor));
}

void eol_actor_clean()
{
  if (!eol_actor_initialized())
  {
    eol_logger_message(
        EOL_LOG_INFO,
        "eol_actor:cannot clean, not initialized\n");
    return;
  }
  if (!_eol_actor_manager)return;
  eol_resource_manager_clean(_eol_actor_manager);
}

void eol_actor_close()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_actor:closing\n");
  eol_resource_manager_free(&_eol_actor_manager);
  _eol_actor_max = 0;
  _eol_actor_initialized = eolFalse;
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_actor:closed\n");

}

eolBool eol_actor_load_data_from_file(char * filename,void *data)
{
  eolLine buf;
  eolFile *file;
  eolActor *act;
  eolInt temp;
  eolInt sw = -1,sh = -1;
  file = eol_loader_read_file(filename);

  if (file == NULL)
    return eolFalse;
  if (data == NULL)
    return eolFalse;
    
  act = (eolActor *)data;
  while(fscanf(file->file, "%s", buf) != EOF)
  {
    if(strcmp(buf,"actor:") == 0)
    {
      fscanf(file->file, "%s",act->name);
      continue;
    }
    if(strcmp(buf,"mesh:") == 0)
    {
      fscanf(file->file, "%s",act->_meshFile);
      continue;
    }
    if(strcmp(buf,"skin:") == 0)
    {
      fscanf(file->file, "%s",act->_skinFile);
      continue;
    }
    if(strcmp(buf,"arm:") == 0)
    {
      fscanf(file->file, "%s",act->_armFile);
      continue;
    }
    if(strcmp(buf,"sprite:") == 0)
    {
      fscanf(file->file, "%s",act->_spriteFile);
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
        act->_sprite_3D = eolTrue;
      }
      else
      {
        act->_sprite_3D = eolFalse;
      }
      continue;
    }
  }
  eol_loader_close_file(&file);
  if (strlen(act->_spriteFile) > 0)
  {
    act->_sprite = eol_sprite_load(act->_spriteFile,sw,sh);
  }
  if (strlen(act->_skinFile) > 0)
  {
    act->_skin = eol_sprite_load(act->_skinFile,-1,-1);
  }
  if (strlen(act->_meshFile) > 0)
  {
    act->_mesh = eol_mesh_load(act->_meshFile);
  }
  if (strlen(act->_armFile) > 0)
  {
    act->_arm = eol_armature_load(act->_armFile);
    if (act->_mesh != NULL)
    {
      eol_armature_link_mesh(act->_arm,act->_mesh);
    }
  }
  return eolTrue;
}

eolBool eol_actor_initialized()
{
  return _eol_actor_initialized;
}

void eol_actor_draw(
    eolActor *act,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha
  )
{
  if (!act)return;
  if (alpha == 0)
  {
    /*cannot draw a clear actor, so don't waste the math*/
    return;
  }
  if ((scale.x == 0) &&
      (scale.y == 0) &&
      (scale.z == 0))
  {
    /*cannot draw a zero scaled actor*/
    return;
  }
  /*TODO: check to see if the actor is on camera...*/
  if (act->_mesh != NULL)
  {
    if (act->_arm != NULL)
    {
      eol_armature_deform_mesh(act->_arm, act->_mesh,act->currentFrame);
    }
  	eol_mesh_draw(
        act->_mesh,
        position,
        rotation,
        scale,
        color,
        alpha,
        act->_skin
      );    
  }
  if (act->_sprite != NULL) 
  {
    if (act->_sprite_3D)
    {
      eol_sprite_draw_transformed_3d(
        act->_sprite,
        act->currentFrame,
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
        act->_sprite,
        act->currentFrame,
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

