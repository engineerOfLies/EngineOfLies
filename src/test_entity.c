#include <eol_entity.h>
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
  }
  snprintf(self->name,EOLLINELEN,"TESTENTNAME");
  self->update = testEntUpdate;
  self->think = testEntThink;
  eol_vec3d_set(self->ori.scale,0.4,0.4,0.4);
  self->vector.rotation.z = crandom();
  self->vector.rotation.y = crandom();
  self->vector.rotation.x = crandom();
  eol_vec3d_copy(self->ori.position,position);
  eol_logger_message(
    EOL_LOG_INFO,
    "spawned test entity\n");
}

/*eol@eof*/
