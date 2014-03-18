#include "eol_particle.h"
#include "eol_draw.h"
#include "eol_logger.h"

/*local global variables*/
eolUint        _eol_particle_max = 0;
eolUint        _eol_particle_trail_len = 0;
eolParticle  * _eol_particle_list = NULL;
eolOrientation _eol_particle_global_motion;
eolBool        _eol_particle_initialized = eolFalse;

/*local prototypes*/

void eol_particle_update(eolParticle *part);
void eol_particle_draw(eolParticle *part);
void eol_particle_kill(eolParticle *part);
void eol_particle_delete(eolParticle *part);/*deletes trail info*/
void eol_particle_create(eolParticle *part);/*creates trail info*/

/*local definitions*/

void eol_particle_close()
{
  int i;
  eol_logger_message(EOL_LOG_INFO,"eol_particle:closing\n");
  eol_particle_clear();
  for (i = 0;i < _eol_particle_max; i++)
  {
    eol_particle_delete(&_eol_particle_list[i]);
  }
  free(_eol_particle_list);
  _eol_particle_list = NULL;
  _eol_particle_max = 0;
  _eol_particle_trail_len = 0;
  _eol_particle_initialized = eolFalse;
  eol_logger_message(EOL_LOG_INFO,"eol_particle:closed\n");
}

void eol_particle_init()
{
  int i;
  eol_logger_message(EOL_LOG_INFO,"eol_particle:initializing\n");
  _eol_particle_list = (eolParticle *)malloc(sizeof(eolParticle)*_eol_particle_max);
  if (_eol_particle_list == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_particle: unable to allocate particle list!\n");
    return;
  }
  memset(_eol_particle_list,0,sizeof(eolParticle)*_eol_particle_max);
  for (i = 0;i < _eol_particle_max; i++)
  {
    eol_particle_create(&_eol_particle_list[i]);
  }
  atexit(eol_particle_close);
  _eol_particle_initialized = eolTrue;
  eol_logger_message(EOL_LOG_INFO,"eol_particle:initialized\n");
}

eolBool eol_particle_initialized()
{
  return _eol_particle_initialized;
}

void eol_particle_config()
{
  /*TODO: load from config*/
  _eol_particle_max = 8392;
  _eol_particle_trail_len = 1;
}

void eol_particle_clear()
{
  int i;
  if (!eol_particle_initialized())return;
  for (i = 0; i < _eol_particle_max;i++)
  {
    eol_particle_kill(&_eol_particle_list[i]);
  }
}

void eol_particle_kill(eolParticle *part)
{
  if (!eol_particle_initialized())return;
  if (!part)return;
  if (part->actor != NULL)
  {
    eol_actor_free(&part->actor);
  }
  memset(part,0,sizeof(eolParticle));
}

void eol_particle_draw_all()
{
  int i;
  if (!eol_particle_initialized())return;
  for (i = 0; i < _eol_particle_max;i++)
  {
    if (_eol_particle_list[i].lifetime > 0)
    {
      eol_particle_draw(&_eol_particle_list[i]);
    }
  }
}

void eol_particle_update_all()
{
  int i;
  if (!eol_particle_initialized())return;
  for (i = 0; i < _eol_particle_max;i++)
  {
    if (_eol_particle_list[i].lifetime > 0)
    {
      eol_particle_update(&_eol_particle_list[i]);
    }
  }
}

void eol_particle_update(eolParticle *part)
{
  if (!part)return;
  /*this is really all there should be to particles:
   1) tick down the life
   2) update orientation and trail
   */
  part->lifetime--;
  if (part->lifetime == 0)
  {
    eol_particle_kill(part);
    return;
  }
  eol_orientation_add(&part->vector,
                      part->vector,
                      part->accel);
  eol_orientation_add(&part->ori,
                      part->ori,
                      part->vector);
  if ((part->type == eolParticleTrail) ||
      (part->drawTrail == eolTrue))
  {
    eol_trail_append(&part->trail,part->ori);
  }
  eol_actor_next_frame(part->actor);
}

void eol_particle_draw(eolParticle *part)
{
  if (!part)return;
  switch(part->type)
  {
    case eolParticlePoint:
      eol_draw_dot_3D(part->ori.position,
                      part->radius,
                      part->ori.color,
                      part->ori.alpha);
      break;
    case eolParticleTrail:
      eol_draw_trail(&part->trail,
                     part->radius,
                     part->fade,
                     part->taper);
      break;
    case eolParticleSprite:
    case eolParticleModel:
    case eolParticleActor:
      eol_actor_draw(part->actor,
                     part->ori.position,
                     part->ori.rotation,
                     part->ori.scale,
                     part->ori.color,
                     part->ori.alpha);
      break;
  }
}

void eol_particle_delete(eolParticle *part)
{
  if (!part)return;
  eol_trail_free(&part->trail);
}

void eol_particle_create(eolParticle *part)
{
  if (!part)return;
  eol_trail_new(&part->trail,_eol_particle_trail_len);
}

eolParticle * eol_particle_new()
{
  int i;
  if (!eol_particle_initialized())return NULL;
  for (i = 0; i < _eol_particle_max;i++)
  {
    if (_eol_particle_list[i].lifetime == 0)
    {
      eol_vec3d_set(_eol_particle_list[i].ori.scale,1,1,1);
      return &_eol_particle_list[i];
    }
  }
  return NULL;
}

void eol_particle_make_point(eolVec3D position,
                             eolVec3D velocity,
                             eolVec3D accel,
                             eolFloat radius,
                             eolVec3D color,
                             eolFloat alpha,
                             eolUint  lifetime)
{
  eolParticle *part;
  part = eol_particle_new();
  if (part == NULL)return;
  eol_vec3d_copy(part->ori.position,position);
  eol_vec3d_copy(part->vector.position,velocity);
  eol_vec3d_copy(part->accel.position,accel);
  eol_vec3d_copy(part->ori.color,color);
  part->radius = radius;
  part->ori.alpha = alpha;
  part->lifetime = lifetime;
  part->type = eolParticlePoint;
}

void eol_particle_make_trail(eolVec3D position,
                             eolVec3D velocity,
                             eolVec3D accel,
                             eolFloat radius,
                             eolVec3D color,
                             eolFloat alpha,
                             eolUint  lifetime)
{
  eolParticle *part;
  part = eol_particle_new();
  if (part == NULL)return;
  eol_vec3d_copy(part->ori.position,position);
  eol_vec3d_copy(part->vector.position,velocity);
  eol_vec3d_copy(part->accel.position,accel);
  eol_vec3d_copy(part->ori.color,color);
  part->radius = radius;
  part->ori.alpha = alpha;
  part->lifetime = lifetime;
  part->type = eolParticleTrail;
}

/*eol@eof*/
