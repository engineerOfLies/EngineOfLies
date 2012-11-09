#include "eol_particle_emitter.h"

eolVec3D eol_particle_emitter_get_point_by_shape(eolUint shape,
                                                 eolVec3D point,
                                                 eolVec3D variance,
                                                 eolVec3D rotation)
{
  eolVec3D out;
  eol_vec3d_clear(out);
  switch (shape)
  {
    case eolEmmitterPoint:
      out.x = point.x + (crandom() * variance.x);
      out.y = point.y + (crandom() * variance.y);
      out.z = point.z + (crandom() * variance.z);
      break;
    case eolEmmitterSquare:
      out.x = point.x + (crandom() * variance.x);
      out.y = point.y + (crandom() * variance.y);
      /*TODO;rotate about*/
      break;
    case eolEmmitterCircle:
      break;
    case eolEmmitterSphere:
      break;
    case eolEmmitterModel:
      break;
    case eolEmmitterActor:
      break;
  }
  return out;
}

void eol_particle_emitter_from_config(eolConfig *config,
                                      eolVec3D point,
                                      eolVec3D direction,
                                      eolVec3D color,
                                      eolFloat alpha)
{
  eolParticle *part = NULL;
  int particleType = 0;
  eolLine partTypeName;
  int shapeType = 0;
  eolLine shapeName;
  eolUint count = 0,i;
  eolVec3D variance = {0,0,0};
  eolVec3D rotation = {0,0,0};
  if (!config)return;
  
  if (!eol_config_get_uint_by_tag(&count,config,"count"))
  {
    count = 1;
  }

  eol_config_get_vec3d_by_tag(&variance,config,"variance");

  eol_config_get_line_by_tag(partTypeName,config,"type");
  if (eol_line_cmp(partTypeName,"sprite") == 0)
  {
    particleType = eolParticleSprite;
  }
  else if (eol_line_cmp(partTypeName,"trail") == 0)
  {
    particleType = eolParticleTrail;
  }
  else if (eol_line_cmp(partTypeName,"model") == 0)
  {
    particleType = eolParticleModel;
  }
  else if (eol_line_cmp(partTypeName,"actor") == 0)
  {
    particleType = eolParticleActor;
  }
  else
  {
    particleType = eolParticlePoint;
  }

  eol_config_get_line_by_tag(shapeName,config,"shape");
  if (eol_line_cmp(partTypeName,"square") == 0)
  {
    shapeType = eolEmmitterSquare;
  }
  if (eol_line_cmp(partTypeName,"circle") == 0)
  {
    shapeType = eolEmmitterCircle;
  }
  if (eol_line_cmp(partTypeName,"sphere") == 0)
  {
    shapeType = eolEmmitterSphere;
  }
  if (eol_line_cmp(partTypeName,"Model") == 0)
  {
    shapeType = eolEmmitterModel;
  }
  if (eol_line_cmp(partTypeName,"actor") == 0)
  {
    shapeType = eolEmmitterActor;
  }
  
  for (i = 0;i < count;i++)
  {
    part = eol_particle_new();
    if (!part)return;
    part->type = particleType;
    eol_vec3d_copy(part->ori.position,eol_particle_emitter_get_point_by_shape(shapeType,point,variance,rotation));
    switch (part->type)
    {
      case eolParticlePoint:
        break;
      case eolParticleTrail:
        break;
      case eolParticleSprite:
        break;
      case eolParticleModel:
        break;
      case eolParticleActor:
        break;
    }
  }
 /* @param type [point, sprite, actor]
 * @param actor path to the actor to use for this particle
 * @param count uint how many particles are to be created  (for entity, this is per update)
 * @param lifetime uint how long the particles should live before timing out in server ticks
 * @param lifetimeVariance float how much variation should be applied to lifetime
 * @param shape [point|square|circle|sphere] shape of the creation point for the particles
 * @param vector vec3D direction the particles should move in (0,0,0 is viable)
 * @param vectorVariance vec3D percentage that the vector will vary by component
 * @param accelation vec3D direction the particles will acclerate in (0,0,0 is viable)
 * @param AccelVariance vec3D percentage that the acceleration will vary by component
 * @param color vec3D the default color that this should be (will be multiplied by input color)
 * @param colorVariance vec3D percentage that the color will vary by component
 * @param colorVector vec3D change in color over time
 * @param colorVector vec3D change in color over time
 * @param alpha float transparecy
 * @param alphaVariance float percentage that the alpha will vary by component
 * @param alphaVector float change in alpha over time
 * @param drawTrail
 * @param trailTaper
 * @param trailFade*/
}

void eol_particle_emitter_die(eolEntity *self)
{
  self->state = eolEntityDead;
}

void eol_particle_emitter_update(eolEntity *self)
{
  if (!self->config)
  {
    eol_particle_emitter_die(self);
    return;
  }
  eol_particle_emitter_from_config(self->config,
                                   self->ori.position,
                                   self->ori.rotation,
                                   self->ori.color,
                                   self->ori.alpha);
  if (self->state > eolEntityDead)self->state--;
}

eolEntity * eol_particle_spawn_emitter_from_config(eolLine  configFile,
                                                   eolVec3D point,
                                                   eolVec3D vector,
                                                   eolVec3D rotation,
                                                   eolVec3D color,
                                                   eolFloat alpha)
{
  eolConfig *conf = NULL;
  eolEntity *ent = NULL;
  conf = eol_config_load(configFile);
  if (!conf)
  {
    return NULL;
  }
  ent = eol_entity_new();
  if (!ent)
  {
    eol_config_free(&conf);
    return NULL;
  }
  ent->update = eol_particle_emitter_update;
  ent->config = conf;
  eol_vec3d_copy(ent->ori.position,point);
  eol_vec3d_copy(ent->ori.rotation,rotation);
  eol_vec3d_copy(ent->vector.position,vector);
  eol_vec3d_copy(ent->ori.color,color);
  ent->ori.alpha = alpha;

  if(eol_config_get_uint_by_tag(&ent->state,conf,"entityLifetime"))
  {
      ent->state += eolEntityDead;
  }

  eol_config_get_vec3d_by_tag(&ent->vector.color,conf,"entityColorVector");
  eol_config_get_float_by_tag(&ent->vector.alpha,conf,"entityAlphaVector");
  eol_config_get_vec3d_by_tag(&ent->vector.rotation,conf,"entityRotationVector");
  
  return ent;
}
/*eol@eof*/