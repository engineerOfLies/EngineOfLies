#include <eol_entity.h>
#include <eol_level.h>
#include <eol_actor.h>
#include <eol_logger.h>

void testEntUpdate(eolEntity *self)
{
}

void testEntThink(eolEntity *self)
{
}

void spawnTestEnt(eolVec3D position)
{
  eolActor *act;
  eolEntity *self;
  self = eol_entity_new();
  if (!self)return;
  act = eol_actor_new();
  if (act)
  {
    eol_actor_load(act, "models/cell.actor");
    eol_entity_add_actor(self,act);
    if ((act->model != NULL )&&(act->model->_mesh != NULL))
    {
      self->boundingBox.x = -act->model->_mesh->bounds.x/2;
      self->boundingBox.y = -act->model->_mesh->bounds.y/2;
      self->boundingBox.w = act->model->_mesh->bounds.x;
      self->boundingBox.h = act->model->_mesh->bounds.y;
      self->radius = act->model->_mesh->bounds.y/2 * 0.6;
    }
  }
  eol_line_cpy(self->name,"TESTENTNAME");
  self->update = testEntUpdate;
  self->think = testEntThink;
  eol_vec3d_set(self->ori.scale,0.4,0.4,0.4);
  eol_vec3d_set(self->ori.color,1,1,0.4);
  self->vector.position.x = crandom() * 0.01;
  self->vector.position.y = crandom() * 0.01;
  
  self->vector.rotation.z = crandom();
  self->vector.rotation.y = crandom();
  self->vector.rotation.x = crandom();

  eol_vec3d_copy(self->ori.position,position);

  self->mass = 10;
  eol_entity_set_collision_mask(self,CP_ALL_LAYERS);
  eol_entity_shape_make_circle(self);
  eol_level_add_entity_to_active_layer(self);
}

/*eol@eof*/
