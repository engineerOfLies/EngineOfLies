#include "eol_entity.h"
#include "eol_logger.h"
#include "eol_actor.h"
#include "eol_draw.h"
#include "eol_resource.h"
#include "eol_types.h"

/*local data types*/
typedef void (*eolEntityDrawType)(eolEntity *ent);

/*local global variables*/
eolUint               _eol_entity_custom_data_size = 0;
eolBool               _eol_entity_initialized = eolFalse;
eolResourceManager  * _eol_entity_manager = NULL;
eolEntity           * _eol_entity_list = NULL;
eolUint               _eol_entity_max = 1024;
eolEntityCustomDelete _eol_entity_custom_delete = NULL;
eolUint               _eol_entity_trail_max = 16;
eolUint               _eol_entity_draw_mode = 0;
eolEntityDrawType     _eol_entity_draw_func = NULL; /*we bring the func*/

/*local function prototypes*/
void eol_entity_close();
void eol_entity_delete(void *entityData);
eolBool eol_entity_load_data_from_file(char * filename,void *data);
static void eol_entity_handle_touch(cpBody *body, cpArbiter *arbiter, void *data);
void eol_entity_handle_world_touch(eolEntity *ent);

void eol_entity_draw_textured(eolEntity *ent);
void eol_entity_draw_wire(eolEntity *ent);
void eol_entity_draw_box(eolEntity *ent);
void eol_entity_draw_textured_bound(eolEntity *ent);


/*function definitions*/
void eol_entity_config()
{
  eolConfig *conf = NULL;
  eolLine    buf;
  _eol_entity_max = 1024;
  _eol_entity_trail_max = 16;
  _eol_entity_draw_mode = eolEntityDrawBounds;

  conf = eol_config_load("system/entity.cfg");
  if (conf != NULL)
  {
    eol_config_get_uint_by_tag(&_eol_entity_max,conf,"entityMax");
    eol_config_get_uint_by_tag(&_eol_entity_trail_max,conf,"trailLenth");
    eol_config_get_line_by_tag(buf,conf,"entityDraw");
    if (strlen(buf))
    {
      if ((eol_line_cmp(buf,"box")==0) ||
          (eol_line_cmp(buf,"bounds")==0))
      {
        _eol_entity_draw_mode = eolEntityDrawBounds;
      }
      else if ((eol_line_cmp(buf,"wire")==0) ||
               (eol_line_cmp(buf,"wireframe")==0))
      {
        _eol_entity_draw_mode = eolEntityDrawWireframe;
      }
      else if ((eol_line_cmp(buf,"flat")==0) ||
               (eol_line_cmp(buf,"mesh")==0))
      {
        _eol_entity_draw_mode = eolEntityDrawMesh;
      }
      else if ((eol_line_cmp(buf,"skin")==0) ||
               (eol_line_cmp(buf,"texture")==0))
      {
        _eol_entity_draw_mode = eolEntityDrawTextured;
      }
      else if (eol_line_cmp(buf,"light")==0)
      {
        _eol_entity_draw_mode = eolEntityDrawLighting;
      }
      else if ((eol_line_cmp(buf,"shader")==0) ||
               (eol_line_cmp(buf,"full")==0))
      {
        _eol_entity_draw_mode = eolEntityDrawShaded;
      }
      else if ((eol_line_cmp(buf,"texture_bounds")==0) ||
               (eol_line_cmp(buf,"bounds_texture")==0))
      {
        _eol_entity_draw_mode = eolEntityDrawTexturedBound;
      }
    }
    eol_config_free(&conf);
  }
  switch(_eol_entity_draw_mode)
  {
    case eolEntityDrawBounds:
      _eol_entity_draw_func = eol_entity_draw_box;
      break;
    case eolEntityDrawWireframe:
      _eol_entity_draw_func = eol_entity_draw_wire;
      break;
    case eolEntityDrawMesh:
      _eol_entity_draw_func = eol_entity_draw_textured;
      break;
    case eolEntityDrawTextured:
      _eol_entity_draw_func = eol_entity_draw_textured;
      break;
    case eolEntityDrawLighting:
      _eol_entity_draw_func = eol_entity_draw_textured;
      break;
    case eolEntityDrawShaded:
      _eol_entity_draw_func = eol_entity_draw_textured;
      break;
    case eolEntityDrawTexturedBound:
      _eol_entity_draw_func = eol_entity_draw_textured_bound;
      break;
  }
}

void eol_entity_init()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_entity:initializing");
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
      "eol_entity:initialized");
  atexit(eol_entity_close);
  _eol_entity_initialized = eolTrue;

}

void eol_entity_close()
{
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_entity:closing");
  eol_resource_manager_free(&_eol_entity_manager);
  _eol_entity_max = 0;
  _eol_entity_custom_data_size = 0;
  _eol_entity_custom_delete = NULL;
  _eol_entity_initialized = eolFalse;
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_entity:closed");
}

void eol_entity_clear()
{
  if (!eol_entity_initialized())
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_entity:cannot clear, not initialized");
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
  if ((_eol_entity_custom_delete != NULL) &&
      (_eol_entity_custom_data_size != 0))
  {
    _eol_entity_custom_delete(ent->customData);
  }
  if (ent->children != NULL)
  {
    for (it = ent->children; it != NULL; it = it->next)
    {
      /*data should be pointing to an eolEntityPointer*/
      if (it->data != NULL)free(it->data);
    }
  }
  for (it = ent->actorList; it != NULL; it = it->next)
  {
    if (it->data)
    {
      eol_actor_free((eolActor**)&it->data);
    }
  }
  it = NULL;
  g_list_free(ent->actorList);
  ent->actor = NULL;
  ent->actorList = NULL;
  /*physics*/
  if (ent->body != NULL)cpBodyFree(ent->body);
  if (ent->shape != NULL)cpShapeFree(ent->shape);
  eol_config_free(&ent->config);
 
  memset(ent,0,sizeof(eolEntity));
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
      "eol_entdow:used uninitialized");
    return NULL;
  }
  ent = (eolEntity *)eol_resource_new_element(_eol_entity_manager);
  if (ent == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_entity:failed to get new resource");
    return NULL;
  }
  if (_eol_entity_custom_data_size > 0)
  {
    /*the resource system has allocated the custom data in the same continuous block
      as this entity*/
    ent->customData = (void *)(ent + 1);
  }
  /*set some sane defaults:*/
  ent->self = ent;
  ent->collisionMask = CP_ALL_LAYERS;
  eol_orientation_clear(&ent->ori);
  eol_trail_new(&ent->trail,_eol_entity_trail_max);
  ent->shown = eolTrue;
  ent->team = CP_NO_GROUP;
  ent->id = eol_resource_element_get_id(_eol_entity_manager,ent);
  return ent;
}

void eol_entity_register_custom_delete(eolEntityCustomDelete delfunc)
{
  if (_eol_entity_initialized)
  {
    eol_logger_message(EOL_LOG_ERROR,
                       "eol_entity:cannot register custom delete after init!");
    return;
  }
  _eol_entity_custom_delete = delfunc;
}

void eol_entity_register_custom_data_size(eolUint customSize)
{
  if (_eol_entity_initialized)
  {
    eol_logger_message(EOL_LOG_ERROR,
                       "eol_entity:cannot register custom data size after init!");
                       return;
  }
  _eol_entity_custom_data_size = customSize;
}

eolUint eol_entity_get_ref_count(eolEntity * ent)
{
  return eol_resource_element_get_refcount(_eol_entity_manager ,ent);
}

/*sync with physics system*/

void eol_entity_postsync(eolEntity * ent)
{
  cpVect p;
  eolUint count = 0;
  eolFloat factor = 0;
  eolVec3D * tempVec;
  GList *it;
  if (!ent)return;
  /*sync with the physics body*/
  if (ent->body == NULL)return;
  p = cpBodyGetPos(ent->body);
  ent->ori.position.x = p.x;
  ent->ori.position.y = p.y;
  if (ent->trackTrail)
  {
    eol_trail_append(&ent->trail,ent->ori);
  }

  /*entity-entity collisions are most important...*/
  for (it = ent->normals;it != NULL;it = it->next)
  {
    if (it->data)free(it->data);
  }
  g_list_free(ent->normals);
  ent->normals = NULL;
  
  cpBodyEachArbiter(ent->body, (cpBodyArbiterIteratorFunc)eol_entity_handle_touch, ent);
  eol_vec3d_clear(ent->normal);
  for (it = ent->normals,count = 0;it != NULL;it = it->next,count++)
  {
    if (it->data != NULL)
    {
      tempVec = (eolVec3D *)it->data;
      eol_vec3d_add(ent->normal,
                    (*tempVec),
                    ent->normal);
    }
  }
  if (count > 0)
  {
    factor = 1.0f/(float)count;
    eol_vec3d_scale(ent->normal,ent->normal,factor);
    eol_vec3d_normalize(&ent->normal);
    /*TODO this needs to compare based on local gravity direction*/
    if (ent->normal.y < -0.5)ent->grounded = eolTrue;
    if (ent->grounded)
    {
      eol_vec3d_clear(ent->gravity);
    }
    eol_entity_handle_world_touch(ent);
  }
}

void eol_entity_presync(eolEntity *ent)
{
  if (!ent)return;
  if (ent->state == eolEntityDead)
  {
    eol_entity_free(&ent);
    return;
  }

  if (ent->dampening)
  {
    if (fabs(ent->vector.position.x) > ent->dampening)
    {
      if (ent->vector.position.x < 0)ent->vector.position.x += ent->dampening;
      else ent->vector.position.x -= ent->dampening;
    }
    else ent->vector.position.x = 0;
    if (fabs(ent->vector.position.y) > ent->dampening)
    {
      if (ent->vector.position.y < 0)ent->vector.position.y += ent->dampening;
      else ent->vector.position.y -= ent->dampening;
    }
    else ent->vector.position.y = 0;
    if (fabs(ent->vector.position.z) > ent->dampening)
    {
      if (ent->vector.position.z < 0)ent->vector.position.z += ent->dampening;
      else ent->vector.position.z -= ent->dampening;
    }
    else ent->vector.position.z = 0;

  }

  eol_orientation_add(&ent->vector,
                        ent->vector,
                        ent->accel);

  if ( !eol_vec3d_magnitude_less_than(ent->vector.position,ent->topSpeed))
  {
    eol_vec3d_set_magnitude(&ent->vector.position,ent->topSpeed);
  }
  /*always apply gravity after personal vector movement*/
  if (!ent->grounded)
  {
    eol_vec3d_add(ent->vector.position,ent->vector.position,ent->gravityAccel);
  }
  
  ent->grounded = eolFalse;
  ent->bounced = eolFalse;
  ent->wallTouch = eolFalse;

  if (ent->body != NULL)
  {
    cpBodySetPos(ent->body, cpv(ent->ori.position.x,ent->ori.position.y));
    cpBodySetVel(ent->body, cpv(ent->vector.position.x,ent->vector.position.y));
    cpBodyActivate(ent->body);
  }
}

void eol_entity_presync_all()
{
  eolEntity *ent = NULL;
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
          (ent->thinkNext < now))
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
  eolOrientation ori;
  if (!ent)return;
  switch (ent->shapeType)
  {
    case eolEntityRect:
      rect.x = ent->boundingBox.w* -0.5;
      rect.y = ent->boundingBox.h* -0.5;
      rect.w = ent->boundingBox.w;
      rect.h = ent->boundingBox.h;
      eol_orientation_copy(&ori,ent->ori);
      eol_vec3d_clear(ori.rotation);
      eol_draw_rect_3D(rect, 2,ori);
      break;
    case eolEntityCircle:
      eol_draw_cirlce_3D(ent->ori.position,
                        ent->radius,
                        32,
                        ent->ori.color,
                        ent->ori.alpha);
      break;
    default:
      eol_draw_dot_3D(ent->ori.position,
                      ent->radius,
                      ent->ori.color,
                      ent->ori.alpha);
  }
}

void eol_entity_draw_wire(eolEntity *ent)
{
  GList *list;
  if (!ent)return;
  for (list = ent->actorList;list != NULL;list = list->next)
  {
    if (list->data != NULL)
    {
      eol_actor_draw_wire((eolActor *)list->data,
                     ent->ori.position,
                     ent->ori.rotation,
                     ent->ori.scale,
                     ent->ori.color,
                     ent->ori.alpha);
    }
  }
}

void eol_entity_draw_textured(eolEntity *ent)
{
  GList *list;
  if (!ent)return;
  for (list = ent->actorList;list != NULL;list = list->next)
  {
    if (list->data != NULL)
    {
      eol_actor_draw((eolActor *)list->data,
                    ent->ori.position,
                    ent->ori.rotation,
                    ent->ori.scale,
                    ent->ori.color,
                    ent->ori.alpha);
    }
  }
}

void eol_entity_draw_textured_bound(eolEntity *ent)
{
  GList *list;
  if (!ent)return;
  for (list = ent->actorList;list != NULL;list = list->next)
  {
    if (list->data != NULL)
    {
      eol_entity_draw_box(ent);
      eol_actor_draw((eolActor *)list->data,
                    ent->ori.position,
                    ent->ori.rotation,
                    ent->ori.scale,
                    ent->ori.color,
                    ent->ori.alpha);
    }
  }
}

void eol_entity_draw(eolEntity *ent)
{
  if (!ent)return;
  if (ent->shown == eolFalse)
  {
    eol_logger_message(EOL_LOG_INFO,"eol_entity:skipping draw re: not shown");
    return;
  }
  /*NOTE: may end up drawing other effects...*/
  if (ent->ori.alpha == 0.0f)
  {
    eol_logger_message(EOL_LOG_INFO,"eol_entity:skipping draw re: 0 alpha");
    return;
  }
  if (_eol_entity_draw_func != NULL)
  {
    _eol_entity_draw_func(ent);
  }
  else
  {
    eol_entity_draw_textured(ent);
  }
  if (ent->drawTrail)
  {
    eol_draw_trail(&ent->trail,
                   ent->trailWidth,
                   eolFalse,
                   eolFalse);
  }
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

void eol_entity_handle_world_touch(eolEntity *ent)
{
  if (!ent)return;
  if (ent->bounces)
  {
    eol_vec3d_reflect(&ent->vector.position, ent->normal,ent->vector.position);
    ent->bounced = eolTrue;
  }
  if (ent->levelTouch != NULL)
  {
    ent->levelTouch(ent,NULL);
  }
}

static void eol_entity_handle_touch(cpBody *body, cpArbiter *arbiter, void *data)
{
  eolUint count = 0;
  int i;
  eolEntity *self = (eolEntity *)data;
  eolEntity *other = NULL;
  cpBody *counterBody = NULL;
  cpShape *counterShape = NULL;
  cpShape *s1,*s2;
  cpVect cpnormal,cppoint;
  eolVec3D normal,point;
  if (self == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,"Given bad pointer to self in HandleEntityTouch.");
    return;
  }
  if ((self->touch == NULL) && (self->levelTouch == NULL))
  {
    /*no need to do any more work here*/
    return;
  }
  count = cpArbiterGetCount(arbiter);
  cpArbiterGetShapes(arbiter, &s1, &s2);
  for (i = 0; i < count;i++)
  {
    cpnormal = cpArbiterGetNormal(arbiter, i);
    cppoint = cpArbiterGetPoint(arbiter, i);
    eol_vec3d_set(normal,cpnormal.x,cpnormal.y,0);
    eol_vec3d_set(point,cppoint.x,cppoint.y,0);
    if (s1[i].body == body)
    {
      counterBody = s2[i].body;
      counterShape = &s2[i];
    }
    else
    {
      counterBody = s1[i].body;
      counterShape = &s1[i];
    }
    if(cpBodyIsStatic(counterBody))
    {
      /*world collision handler*/
      if (eol_equals(eol_vec3d_magnitude_squared(normal),1))
        self->normals = g_list_append(self->normals,eol_vec3d_dup(normal));
      return;
    }
    if (self->touch != NULL)
    {
      other = (eolEntity *)counterBody->data;
      self->touch(self,other);
    }
  }
}

/*space stuff*/
void eol_entity_add_to_space(eolEntity *ent,cpSpace *space)
{
  if ((!ent) || (!space))
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:passed NULL data to add_to_space.");
    return;
  }
  if (ent->body == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:cannot add a NULL body to a space for entity %s.",
      ent->name);
    return;
  }
  if (ent->shape == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:cannot add a NULL shape to a space for entity %s.",
      ent->name);
    return;
  }
  if (cpSpaceAddBody(space, ent->body) == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:failed to add body to a space for entity %s.",
      ent->name);
    return;
  }
  ent->shape->collision_type = eolEntityClipEntities;
  if (cpSpaceAddShape(space, ent->shape) == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:failed to add shape to a space for entity %s.",
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
      "eol_entity:passed NULL data to remove_from_space.");
    return;
  }
  if (ent->_space == NULL)
  {
    return;
  }
  if (ent->body != NULL)
  {
    cpSpaceRemoveBody(ent->_space, ent->body);
    cpBodyFree(ent->body);
    ent->body = NULL;
  }
  if (ent->shape != NULL)
  {
    cpSpaceRemoveShape(ent->_space, ent->shape);
    cpShapeFree(ent->shape);
    ent->shape = NULL;
  }
  ent->_space = NULL;
}

void eol_entity_shape_make_circle(eolEntity *ent)
{
  if (!ent)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:passed NULL data to shape make circle.");
    return;
  }
  /*cleanup any potential old space data*/
  eol_entity_remove_from_space(ent);
  ent->body = cpBodyNew(ent->mass, INFINITY);
  if (ent->body == NULL)
  {
    eol_logger_message(
    EOL_LOG_ERROR,
    "eol_entity:failed to create a new body for physics entity.");
    return;
  }
  ent->shapeType = eolEntityCircle;
  ent->shape = cpCircleShapeNew(ent->body, ent->radius, cpvzero);
  ent->body->data = ent;
  cpShapeSetGroup(ent->shape, ent->team);
  cpShapeSetLayers(ent->shape, ent->collisionMask);
}

void eol_entity_shape_make_rect(eolEntity *ent)
{
  if (!ent)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_entity:passed NULL data to shape make circle.");
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
        "eol_entity:failed to create a new body for physics entity.");
      return;
    }
  }
  if (ent->shape != NULL)
  {
    cpShapeFree(ent->shape);
  }
  ent->shapeType = eolEntityRect;
  ent->shape = cpBoxShapeNew(ent->body, ent->boundingBox.w, ent->boundingBox.h);
  ent->body->data = ent;
  cpShapeSetGroup(ent->shape, ent->team);
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

/*entity data*/
void eol_entity_set_actor_bounds(eolEntity *ent)
{
  eolActor * act;
  if ((!ent) || (!ent->actor))return;
  act = ent->actor;
  if ((act->model != NULL )&&(act->model->_mesh != NULL))
  {
    ent->boundingBox.x = -act->model->_mesh->bounds.x/2;
    ent->boundingBox.y = -act->model->_mesh->bounds.y/2;
    ent->boundingBox.w = act->model->_mesh->bounds.x;
    ent->boundingBox.h = act->model->_mesh->bounds.y;
    ent->radius = act->model->_mesh->bounds.y/2 * 0.6;
  }
}

void eol_entity_add_actor(eolEntity *ent,eolActor *act)
{
  if ((!ent)||(!act))return;
  ent->actorList = g_list_append(ent->actorList,act);
  if (ent->actor == NULL)
  {
    ent->actor = act;
  }
}

/*eol@eof*/
