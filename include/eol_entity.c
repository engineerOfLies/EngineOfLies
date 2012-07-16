#include "eol_entity.h"
#include "eol_logger.h"
#include "eol_actor.h"
#include "eol_resource.h"

/*local global variables*/
eolUint               _eol_entity_custom_data_size = 0;
eolBool               _eol_entity_initialized = eolFalse;
eolResourceManager  * _eol_entity_manager = NULL;
eolEntity           * _eol_entity_list = NULL;
eolUint               _eol_entity_max = 0;
eolEntityCustomDelete _eol_entity_custom_delete = NULL;

/*local function prototypes*/
void eol_entity_close();
void eol_entity_delete(void *entityData);
eolBool eol_entity_load_data_from_file(char * filename,void *data);

/*function definitions*/
void eol_entity_config()
{
  /*TODO: load from config*/
  _eol_entity_max = 1024;
}

void eol_entity_init()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_entity:initializing\n");
  _eol_entity_manager = eol_resource_manager_init(
    "eol_entity_manager",
    _eol_entity_max,
    sizeof(eolEntity) + _eol_entity_custom_data_size,
    eolTrue,
    eol_entity_delete,
    eol_entity_load_data_from_file
    );
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_entity:initialized\n");
  atexit(eol_entity_close);
  _eol_entity_initialized = eolTrue;

}

void eol_entity_close()
{
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_entity:closing\n");
  eol_resource_manager_free(&_eol_entity_manager);
  _eol_entity_max = 0;
  _eol_entity_custom_data_size = 0;
  _eol_entity_custom_delete = NULL;
  _eol_entity_initialized = eolFalse;
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_entity:closed\n");
}

void eol_entity_clear()
{
  if (!eol_entity_initialized())
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_entity:cannot clear, not initialized\n");
    return;
  }
  if (!_eol_entity_manager)return;
  eol_resource_manager_clear(_eol_entity_manager);
}

eolBool eol_entity_initialized()
{
  return _eol_entity_initialized;
}

eolBool eol_entity_load_data_from_file(char * filename,void *data)
{
  return eolTrue;
}

void eol_entity_delete(void *entityData)
{
  eolEntity *ent;
  GList *it = NULL;
  if (!eol_entity_initialized())return;
  if (!entityData)return;
  ent = (eolEntity *)entityData;
  if ((ent->customData != NULL) &&
      (_eol_entity_custom_delete != NULL) &&
      (_eol_entity_custom_data_size != 0))
  {
    _eol_entity_custom_delete(ent->customData);
  }
  for (it = ent->children; it != NULL; it = it->next)
  {
    /*data should be pointing to an eolEntityPointer*/
    if (it->data != NULL)free(it->data);
  }
  if (ent->shape)cpShapeFree(ent->shape);
  for (it = ent->actorList; it != NULL; it = it->next)
  {
    if (it->data)
    {
      eol_actor_free((eolActor**)&it->data);
    }
  }
  it = NULL;
  g_list_free(ent->actorList);
  /*
  TODO: make this happen from the config
  eol_config_free(&ent->config);
  */
}

void eol_entity_free(eolEntity **ent)
{
  if (!eol_entity_initialized())return;
  eol_resource_free_element(_eol_entity_manager,(void **)ent);
}

eolEntity *eol_entity_new()
{
  return NULL;
}

void eol_entity_register_custom_delete(eolEntityCustomDelete delfunc)
{
  if (_eol_entity_initialized)
  {
    eol_logger_message(EOL_LOG_ERROR,
                       "eol_entity:cannot register custom delete after init!\n");
    return;
  }
  _eol_entity_custom_delete = delfunc;
}

void eol_entity_register_custom_data_size(eolUint customSize)
{
  if (_eol_entity_initialized)
  {
    eol_logger_message(EOL_LOG_ERROR,
                       "eol_entity:cannot register custom data size after init!\n");
                       return;
  }
  _eol_entity_custom_data_size = customSize;
}

/*eol@eof*/
