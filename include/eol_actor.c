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
  }
  eol_loader_close_file(&file);
  return eolTrue;
}

eolBool eol_actor_initialized()
{
  return _eol_actor_initialized;
}

/*eol@eof*/

