#include "eol_entity.h"
#include "eol_logger.h"
#include "eol_actor.h"
#include "eol_draw.h"
#include "eol_resource.h"

/*local global variables*/
eolUint               _eol_entity_custom_data_size = 0;
eolBool               _eol_entity_initialized = eolFalse;
eolResourceManager  * _eol_entity_manager = NULL;
eolEntity           * _eol_entity_list = NULL;
eolUint               _eol_entity_max = 0;
eolEntityCustomDelete _eol_entity_custom_delete = NULL;
eolUint               _eol_entity_draw_mode = 0;

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
  _eol_entity_draw_mode = eolEntityDrawLighting;
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
  /*physics*/
  cpShapeFree(ent->shape);
  cpBodyFree(ent->body);
  /*
  TODO: make this happen from the config
  eol_config_free(&ent->config);
  */
}

void eol_entity_free(eolEntity **ent)
{
  if (!eol_entity_initialized())return;
  eol_entity_remove_from_space(*ent);
  eol_resource_free_element(_eol_entity_manager,(void **)ent);
}

eolEntity *eol_entity_new()
{
  eolEntity *ent = NULL;
  if (!eol_entity_initialized())
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_entdow:used uninitialized\n");
    return NULL;
  }
  ent = (eolEntity *)eol_resource_new_element(_eol_entity_manager);
  if (ent == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_entity:failed to get new resource\n");
    return NULL;
  }
  if (_eol_entity_custom_data_size > 0)
  {
    ent->customData = (void *)(ent + 1);
  }
  /*set some sane defaults:*/
  ent->self = ent;
  ent->collisionMask = CP_ALL_LAYERS;
  eol_orientation_clear(&ent->ori);
  eol_orientation_clear(&ent->vector);
  eol_orientation_clear(&ent->accel);
  ent->shown = eolTrue;
  ent->id = eol_resource_element_get_id(_eol_entity_manager,ent);
  return ent;
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

void eol_entity_draw_box(eolEntity *ent)
{
  eolRectFloat rect;
  if (!ent)return;
  rect.x = ent->boundingBox.w* -0.5;
  rect.y = ent->boundingBox.h* -0.5;
  rect.w = ent->boundingBox.w;
  rect.h = ent->boundingBox.h;
  eol_draw_rect_3D(rect, ent->ori);
}

void eol_entity_draw_textured(eolEntity *ent)
{
  GList *list;
  if (!ent)return;
  for (list = ent->actorList;list != NULL;list = list->next)
  {
    eol_actor_draw((eolActor *)list->data,
                   ent->ori.position,
                   ent->ori.rotation,
                   ent->ori.scale,
                   ent->ori.color,
                   ent->ori.alpha);
  }
}

void eol_entity_draw(eolEntity *ent)
{
  if (!ent)return;
  if (ent->shown == eolFalse)return;
  /*NOTE: may end up drawing other effects...*/
  if (ent->ori.alpha == 0.0f)return;
}

void eol_entity_draw_all()
{
  eolEntity *ent = NULL;
  if (!eol_entity_initialized())return;
  while ((ent = eol_resource_get_next_data(_eol_entity_manager,ent)) != NULL)
  {
    eol_entity_draw(ent);
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

/*space stuff*/
void eol_entity_add_to_space(eolEntity *ent,cpSpace *space)
{
  if ((!ent) || (!space))
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:passed NULL data to add_to_space.\n");
    return;
  }
  if (ent->body == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:cannot add a NULL body to a space for entity %s.\n",
      ent->name);
    return;
  }
  if (ent->shape == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:cannot add a NULL shape to a space for entity %s.\n",
      ent->name);
    return;
  }
  if (cpSpaceAddBody(space, ent->body) == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:failed to add body to a space for entity %s.\n",
      ent->name);
    return;
  }
  if (cpSpaceAddShape(space, ent->shape) == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:failed to add shape to a space for entity %s.\n",
      ent->name);
    return;
  }
  ent->_space = space;
}

void eol_entity_remove_from_space(eolEntity *ent)
{
  if (!ent)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:passed NULL data to remove_from_space.\n");
    return;
  }
  if (ent->_space == NULL)
  {
    return;
  }
  if (ent->body == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:entity does not have a body!\n");
    return;
  }
  cpSpaceRemoveBody(ent->_space, ent->body);
}

void eol_entity_shape_make_circle(eolEntity *ent)
{
  if (!ent)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:passed NULL data to shape make circle.\n");
    return;
  }
  eol_entity_remove_from_space(ent);
  if (ent->body == NULL)
  {
    ent->body = cpBodyNew(ent->mass, INFINITY);
    if (ent->body == NULL)
    {
      eol_logger_message(
      EOL_LOG_ERROR,
      "eol_entity:failed to create a new body for physics entity.\n");
      return;
    }
  }
  if (ent->shape != NULL)
  {
    cpShapeFree(ent->shape);
  }
  ent->shape = cpCircleShapeNew(ent->body, ent->radius, cpvzero);
  cpShapeSetLayers(ent->shape, ent->collisionMask);
}

void eol_entity_shape_make_rect(eolEntity *ent)
{
  if (!ent)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:passed NULL data to shape make circle.\n");
    return;
  }
  eol_entity_remove_from_space(ent);
  if (ent->body == NULL)
  {
    ent->body = cpBodyNew(ent->mass, INFINITY);
    if (ent->body == NULL)
    {
      eol_logger_message(
        EOL_LOG_ERROR,
        "eol_entity:failed to create a new body for physics entity.\n");
      return;
    }
  }
  if (ent->shape != NULL)
  {
    cpShapeFree(ent->shape);
  }
  ent->shape = cpBoxShapeNew(ent->body, ent->boundingBox.w, ent->boundingBox.h);
  cpShapeSetLayers(ent->shape, ent->collisionMask);
}

/*collision masks*/
void eol_entity_set_collision_mask(eolEntity *ent,cpLayers collisionmask)
{
  if (!ent)return;
  ent->collisionMask = collisionmask;
}

void eol_entity_add_to_collision_mask(eolEntity *ent,cpLayers collisionmask)
{
  if (!ent)return;
  ent->collisionMask |= collisionmask;
}

void eol_entity_remove_entity_from_collision_mask(eolEntity *ent,cpLayers collisionmask)
{
  if (!ent)return;
  ent->collisionMask = ent->collisionMask & ~collisionmask;
}

/*eol@eof*/
