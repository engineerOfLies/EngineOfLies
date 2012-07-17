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
static void eol_entity_handle_touch(cpBody *body, cpArbiter *arbiter, void *data);

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

/*sync with physics system*/

void eol_entity_postsync(eolEntity * ent)
{
  cpVect p;
  if (!ent)return;
  /*sync with the physics body*/
  if (ent->body == NULL)return;
  p = cpBodyGetPos(ent->body);
  ent->ori.position.x = p.x;
  ent->ori.position.y = p.y;
  cpBodyEachArbiter(ent->body, (cpBodyArbiterIteratorFunc)eol_entity_handle_touch, ent);
}

void eol_entity_presync(eolEntity *ent)
{
  if (!ent)return;
  eol_orientation_add(&ent->vector,
                      ent->vector,
                      ent->accel);
  eol_orientation_add(&ent->ori,
                      ent->ori,
                      ent->vector);
  if (ent->trackTrail)
  {
    eol_trail_append(&ent->trail,ent->ori);
  }

  cpBodySetVel(ent->body, cpv(ent->vector.position.x,ent->vector.position.y));
  if (cpvlengthsq(cpv(ent->vector.position.x,ent->vector.position.y)))
  {
    cpBodyActivate(ent->body);
  }
}

void eol_entity_presync_all()
{
  eolEntity *ent = NULL;
  if (!eol_entity_initialized())return;
  if (!eol_entity_initialized())return;
  while ((ent = eol_resource_get_next_data(_eol_entity_manager,ent)) != NULL)
  {
    eol_entity_presync(ent);
  }
}

void eol_entity_postsync_all()
{
  eolEntity *ent = NULL;
  if (!eol_entity_initialized())return;
  if (!eol_entity_initialized())return;
  while ((ent = eol_resource_get_next_data(_eol_entity_manager,ent)) != NULL)
  {
    eol_entity_postsync(ent);
  }
}

void eol_entity_update_all()
{
  eolEntity *ent = NULL;
  if (!eol_entity_initialized())return;
  while ((ent = eol_resource_get_next_data(_eol_entity_manager,ent)) != NULL)
  {
    if (ent->update != NULL)
    {
      ent->update(ent);
    }
  }
}

void eol_entity_think_all()
{
  eolUint now;
  eolEntity *ent = NULL;
  if (!eol_entity_initialized())return;
  now = eol_graphics_get_now();
  while ((ent = eol_resource_get_next_data(_eol_entity_manager,ent)) != NULL)
  {
    if (ent->think != NULL)
    {
      if ((ent->thinkRate == -1) ||
          (ent->thinkNext > now))
      {
        ent->think(ent);
        if (ent->thinkRate != -1)
        {
          ent->thinkNext += ent->thinkRate;
        }
      }
    }
  }
}

/*physics sync*/
static void eol_entity_handle_touch(cpBody *body, cpArbiter *arbiter, void *data)
{
  eolEntity *self = (eolEntity *)data;
  eolEntity *other = NULL;
  cpBody *a,*b;
  if (self == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,"Given bad pointer to self in HandleEntityTouch.\n");
    return;
  }
  if (self->touch == NULL)
  {
    /*no need to do any more work here*/
    return;
  }
  cpArbiterGetBodies(arbiter, &a, &b);
  if ((a == NULL)||(b == NULL))
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "Arbiter returned bad participants!\n");
    return;
  }
  if (a->data == (void *)self)
  {
    if (b->data == NULL)
    {
      return;
    }
    other = (eolEntity *)b->data;
  }
  else if (b->data == self)
  {
    if (a->data == NULL)
    {
      return;
    }
    other = (eolEntity *)a->data;
  }
  else
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "neither body attached to this arbiter is self!\n");
    return;
  }
  self->touch(self,other);
}

/*eol@eof*/
