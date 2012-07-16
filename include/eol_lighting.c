#include "eol_lighting.h"
#include "eol_draw.h"
#include "eol_logger.h"
#include "eol_resource.h"

/*local global variables*/
eolBool     _eol_lighting_initialized = eolFalse;
eolResourceManager * _eol_lighting_manager = NULL;
eolLight  * _eol_lighting_list = NULL;
eolUint     _eol_lighting_max = 0;
eolFloat    _eol_global_light_level = 1;
eolVec3D    _eol_global_light_color = {1,1,1};

/*local function prototypes*/
void eol_light_delete(void *light);
eolBool eol_light_load_data_from_file(char * filename,void *data);
void eol_light_update(eolLight *light);

void eol_lighting_config()
{
  _eol_lighting_max = GL_MAX_LIGHTS;
  _eol_global_light_level = 1;
  eol_vec3d_set(_eol_global_light_color,1,1,1);
}

void eol_lighting_close()
{
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_lighting:closing\n"
  );
  _eol_lighting_initialized = eolFalse;
  eol_resource_manager_free(&_eol_lighting_manager);
  _eol_lighting_max = 0;
  eol_logger_message(
      EOL_LOG_INFO,
      "eol_lighting:closed\n"
  );
}

void eol_lighting_init()
{
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_lighting:initializing\n");
  _eol_lighting_manager = eol_resource_manager_init(
    "eol_lighting_manager",
    _eol_lighting_max,
    sizeof(eolLight),
    eolTrue,
    eol_light_delete,
    eol_light_load_data_from_file
    );
  atexit(eol_lighting_close);
  eol_logger_message(
    EOL_LOG_INFO,
    "eol_lighting:initialized\n");
  _eol_lighting_initialized = eolTrue;
}

eolBool eol_lighting_initialized()
{
  return _eol_lighting_initialized;
}

void eol_light_delete(void *light)
{
  eol_light_enable(light,eolFalse);
  memset(light,0,sizeof(eolLight));
}

void eol_light_free(eolLight **light)
{
  if (!eol_lighting_initialized())return;
  if ((light) && (*light))
  {
    eol_light_enable(*light,eolFalse);
  }
  eol_resource_free_element(_eol_lighting_manager,(void **)light);
}

eolBool eol_light_load_data_from_file(char * filename,void *data)
{
  /*load a light from a light config file*/
  return eolTrue;
}

void eol_light_enable(eolLight *light,eolBool enable)
{
  if (!eol_lighting_initialized())return;
  if (!light)return;
  if (light->lightIndex == -1)return;
  if (enable)
  {
    glEnable(GL_LIGHT0 + light->lightIndex);
  }
  else
  {
    glDisable(GL_LIGHT0 + light->lightIndex);
  }
}

eolLight *eol_light_new()
{
  int index = -1;
  eolLight *light = NULL;
  if (!eol_lighting_initialized())
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_lighting:used uninitialized\n");
    return NULL;
  }
  light = (eolLight *)eol_resource_new_element(_eol_lighting_manager);
  if (light == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_lighting:failed to get new resource\n");
    return NULL;
  }
  index = eol_resource_element_get_index(_eol_lighting_manager,light);
  if (index == -1)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_lighting:failed to get valid index for light\n");
    eol_light_delete(light);
    return NULL;
  }
  light->lightIndex = index;
  /*default sane light parameters*/
  light->constantAttenuation = 0.0001f;
  light->linearAttenuation = 0.002f;
  light->QuadraticAttenuation = 0.0000002f;
  light->on = eolTrue;
  eol_light_enable(light,eolTrue);
  return light;
}

void eol_lighting_filter_for_position(eolVec3D pos)
{
  eolLight *light = NULL;
  if (!eol_lighting_initialized())return;
  while ((light = eol_resource_get_next_data(_eol_lighting_manager,light)) != NULL)
  {
    if (!light->on)continue;
    if (light->type > eolLightSpot)continue;
    if (eol_distance_between_less_than(pos,light->position,light->radius))
    {
      eol_light_enable(light,eolTrue);
    }
    else
    {
      eol_light_enable(light,eolFalse);
    }
  }
}

void eol_light_set_color(eolLight *light,eolVec3D color)
{
  if (!eol_lighting_initialized())return;
  if (!light)return;
  eol_vec3d_copy(light->colorSpec,color);
  eol_vec3d_copy(light->colorAmbi,color);
  eol_vec3d_copy(light->colorDiff,color);
}

void eol_light_set_radius(eolLight *light,eolFloat radius)
{
  if (!eol_lighting_initialized())return;
  if (!light)return;
  light->radius = radius;
}

void eol_light_turn_on(eolLight *light)
{
  if (!eol_lighting_initialized())return;
  if (!light)return;
  eol_light_enable(light,eolTrue);
  light->on = eolTrue;
}

void eol_light_turn_off(eolLight *light)
{
  if (!eol_lighting_initialized())return;
  if (!light)return;
  eol_light_enable(light,eolFalse);
  light->on = eolFalse;
}

void eol_light_update(eolLight *light)
{
  eolUint li;
  eolFloat tvec[4];
  if (!eol_lighting_initialized())return;
  if (!light)return;
  if (light->lightIndex <= -1)return;
  li = GL_LIGHT0 + light->lightIndex;
  switch(light->type)
  {
    case eolLightArea:
      eol_vec3d_into_array(tvec,light->position);
      tvec[3] = 0;
      glLightfv(li,GL_POSITION, tvec);
      break;
    case eolLightSpot:
      eol_vec3d_into_array(tvec,light->position);
      tvec[3] = 1;
      glLightfv(li,GL_POSITION, tvec);
      eol_vec3d_into_array(tvec,light->direction);
      tvec[3] = 0;
      glLightfv(li,GL_SPOT_DIRECTION, tvec);
      glLightf(li,GL_SPOT_EXPONENT, light->exponent);
      glLightf(li,GL_SPOT_CUTOFF, light->angle);
      break;
    case eolLightAmbient:
      break;
    case eolLightDirectional:
      eol_vec3d_into_array(tvec,light->direction);
      tvec[3] = 0;
      glLightfv(li,GL_SPOT_DIRECTION, tvec);
      break;
  }
  tvec[3] = 1;
  eol_vec3d_into_array(tvec,light->colorSpec);
  glLightfv(li,GL_SPECULAR,tvec);
  eol_vec3d_into_array(tvec,light->colorAmbi);
  glLightfv(li,GL_AMBIENT,tvec);
  eol_vec3d_into_array(tvec,light->colorDiff);
  glLightfv(li,GL_DIFFUSE,tvec);
  glLightf(li,GL_CONSTANT_ATTENUATION, light->constantAttenuation);
  glLightf(li,GL_LINEAR_ATTENUATION, light->linearAttenuation);
  glLightf(li,GL_QUADRATIC_ATTENUATION, light->QuadraticAttenuation);
}

eolLight *eol_light_area_new()
{
  eolLight *light = eol_light_new();
  if (light != NULL)
  {
    light->type = eolLightArea;
    light->on = eolTrue;
  }
  return light;
}

eolLight *eol_light_spot_new()
{
  eolLight *light = eol_light_new();
  if (light != NULL)
  {
    light->type = eolLightSpot;
    light->on = eolTrue;
  }
  return light;
}

eolLight *eol_light_ambient_new()
{
  eolLight *light = eol_light_new();
  if (light != NULL)
  {
    light->type = eolLightAmbient;
    eol_light_move(light,eol_vec3d(0,0,1));
    light->on = eolTrue;
  }
  return light;
}

eolLight *eol_light_directional_new()
{
  eolLight *light = eol_light_new();
  if (light != NULL)
  {
    light->type = eolLightDirectional;
    eol_light_move(light,eol_vec3d(0,0,1));
    light->on = eolTrue;
  }
  return light;
}

void eol_lighting_setup_rep_plot()
{
  eolLight *light;
  light = eol_light_ambient_new();
  if (!light)return;
  eol_light_set_color(light,eol_vec3d(0.5,0.5,0.5));
  eol_light_update(light);
  
  light = eol_light_area_new();
  if (!light)return;
  eol_light_set_color(light,eol_vec3d(1,0.7,0.7));
  eol_light_move(light,eol_vec3d(-8,4,1));
  eol_light_update(light);
  
  light = eol_light_area_new();
  if (!light)return;
  eol_light_set_color(light,eol_vec3d(0.7,0.7,1));
  eol_light_move(light,eol_vec3d(8,-4,1));
  eol_light_update(light);
}

void eol_light_move(eolLight *light,eolVec3D point)
{
  if (!eol_lighting_initialized())return;
  if (!light)return;
  eol_vec3d_copy(light->position,point);
}

void eol_light_draw(eolLight *light)
{
  if (!eol_lighting_initialized())return;
  if (!light)return;
  eol_draw_dot_3D(light->position,
                  5,
                  light->colorAmbi,
                  1);
}

/*eol@eof*/
