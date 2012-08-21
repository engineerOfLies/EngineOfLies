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
  eolEntity *self;
  self = eol_entity_new();
  if (!self)return;
  eol_entity_add_actor(self,eol_actor_load("models/cell.actor"));
  eol_entity_set_actor_bounds(self);

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
