/*
    Copyright 2012 EngineerOfLies
    This file is part of the Engine of Lies game engine library

    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "eol_types.h"
#include "eol_logger.h"
#include <math.h>

eolBool eol_equals(eolDouble a, eolDouble b)
{
  return (fabs(a-b) < EOL_EPSILON)?eolTrue:eolFalse;
}

eolVec2D eol_vec2d(eolDouble x, eolDouble y)
{
  eolVec2D vec;
  eol_vec2d_set(vec,x, y);
  return vec;
}

eolVec3D eol_vec3d(eolDouble x, eolDouble y, eolDouble z)
{
  eolVec3D vec;
  eol_vec3d_set(vec,x, y, z);
  return vec;
}

eolVec4D eol_vec4d(eolDouble x, eolDouble y, eolDouble z, eolDouble w)
{
  eolVec4D vec;
  eol_vec4d_set(vec,x, y, z, w);
  return vec;
}

eolFloat eol_vec2d_magnitude (eolVec2D V)
{
  return sqrt (V.x * V.x + V.y * V.y);
}

eolFloat eol_vec3d_magnitude (eolVec3D V)
{
  return sqrt (V.x * V.x + V.y * V.y + V.z * V.z);
}

eolFloat eol_vec4d_magnitude (eolVec4D V)
{
  return sqrt (V.x * V.x + V.y * V.y + V.z * V.z + V.w * V.w);
}

eolFloat eol_vec2d_magnitude_squared(eolVec2D V)
{
  return (V.x * V.x + V.y * V.y);
}

eolFloat eol_vec3d_magnitude_squared(eolVec3D V)
{
  return (V.x * V.x + V.y * V.y + V.z * V.z);
}

eolFloat eol_vec4d_magnitude_squared(eolVec4D V)
{
  return (V.x * V.x + V.y * V.y + V.z * V.z + V.w * V.w);
}

void eol_vec2d_set_magnitude(eolVec2D * V,eolFloat magnitude)
{
  if (!V)return;
  eol_vec2d_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
}

void eol_vec3d_set_magnitude(eolVec3D * V,eolFloat magnitude)
{
  if (!V)return;
  eol_vec3d_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
  V->z *= magnitude;
}

void eol_vec4d_set_magnitude(eolVec4D * V,eolFloat magnitude)
{
  if (!V)return;
  eol_vec4d_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
  V->z *= magnitude;
  V->w *= magnitude;
}

eolBool  eol_vec3d_magnitude_less_than(eolVec3D V,eolFloat size)
{
  return (eol_vec3d_magnitude_squared(V) < (size * size));
}

eolBool eol_distance_between_less_than3d(eolVec3D p1,eolVec3D p2,eolFloat size)
{
  return eol_vec3d_magnitude_less_than(eol_vec3d(p1.x - p2.x,
                                                 p1.y - p2.y,
                                                 p1.z - p2.z),
                                       size);
}

void eol_vec3d_set_angle_by_radians(eolVec3D *out,eolFloat radians)
{
  if(!out)return;
  out->x = cos(radians);
  out->y = sin(radians);
}

void eol_vec4d_set_angle_by_radians(eolVec4D *out,eolFloat radians)
{
  if(!out)return;
  out->x = cos(radians);
  out->y = sin(radians);
}

void eol_vec2d_normalize (eolVec2D *V)
{
  eolFloat M;
  if (!V)return;
  M = eol_vec2d_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
}

void eol_vec3d_normalize (eolVec3D *V)
{
  eolFloat M;
  if (!V)return;
  M= eol_vec3d_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
  V->z *= M;
}

void eol_vec4d_normalize (eolVec4D *V)
{
  eolFloat M;
  if (!V)return;
  M = eol_vec4d_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
  V->z *= M;
  V->w *= M;
}

eolVec2D *eol_vec2d_dup(eolVec2D old)
{
  eolVec2D *duped = NULL;
  duped = eol_vec2d_new();
  if (!duped)return NULL;
  eol_vec2d_copy((*duped),old);
  return duped;
}

eolVec3D *eol_vec3d_dup(eolVec3D old)
{
  eolVec3D *duped = NULL;
  duped = eol_vec3d_new();
  if (!duped)return NULL;
  eol_vec3d_copy((*duped),old);
  return duped;
}

eolVec4D *eol_vec4d_dup(eolVec4D old)
{
  eolVec4D *duped = NULL;
  duped = eol_vec4d_new();
  if (!duped)return NULL;
  eol_vec4d_copy((*duped),old);
  return duped;
}

eolVec2D *eol_vec2d_new()
{
  eolVec2D *vec = NULL;
  vec = (eolVec2D *)malloc(sizeof(eolVec2D));
  if (vec == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_types:failed to allocate a new vector!\n");
    return NULL;
  }
  eol_vec2d_clear((*vec));
  return vec;
}

eolVec3D *eol_vec3d_new()
{
  eolVec3D *vec = NULL;
  vec = (eolVec3D *)malloc(sizeof(eolVec3D));
  if (vec == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_types:failed to allocate a new vector!\n");
    return NULL;
  }
  eol_vec3d_clear((*vec));
  return vec;
}

eolVec4D *eol_vec4d_new()
{
  eolVec4D *vec = NULL;
  vec = (eolVec4D *)malloc(sizeof(eolVec4D));
  if (vec == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_types:failed to allocate a new vector!\n");
    return NULL;
  }
  eol_vec4d_clear((*vec));
  return vec;
}

eolFloat eol_vector_angle(eolFloat x,eolFloat y)
{
  eolFloat angle = atan2(y, x) + M_PI;
  eolFloat fraction = angle * 0.5 / M_PI;
  if (fraction >= 1.0)
  {
    fraction -= 1.0;
  }
  return (fraction * 360);
}

void eol_angle_clamp_degrees(eolFloat *a)
{
  if (!a)return;
  while (*a >= 360)*a -= 360;
  while (*a < 0)*a += 360;
}

eolFloat eol_angle_between_degrees(eolFloat a, eolFloat b)
{
  eolFloat angle;
  angle = fabs(a - b);
  while (angle >= 360)angle -= 360;
  while (angle < 0)angle += 360;
  if (angle > 180)angle -= 180;
  return angle;
}

void eol_vec2d_reflect(eolVec2D *out, eolVec2D normal,eolVec2D in)
{
  eolFloat f;
  f = eol_vec2d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
}

void eol_vec3d_reflect(eolVec3D *out, eolVec3D normal,eolVec3D in)
{
  eolFloat f;
  f = eol_vec3d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
  out->z = in.z - (2 * normal.z * f);
}

void eol_vec4d_reflect(eolVec4D *out, eolVec4D normal,eolVec4D in)
{
  eolFloat f;
  f = eol_vec4d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
  out->z = in.z - (2 * normal.z * f);
  out->w = in.w - (2 * normal.w * f);
}

void eol_vec3d_cross_product(eolVec3D *out, eolVec3D v1, eolVec3D v2)
{
  out->x = v1.y*v2.z - v1.z*v2.y;
  out->y = v1.z*v2.x - v1.x*v2.z;
  out->z = v1.x*v2.y - v1.y*v2.x;
}

void eol_vec3d_rotate_point(eolVec3D *dst, eolVec3D point, eolVec3D origin, eolVec3D rotation)
{
  
}



eolRect eol_rect(
  eolInt  x,
  eolInt  y,
  eolUint w,
  eolUint h
)
{
  eolRect r;
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  return r;
}

eolRectFloat eol_rectf(
  eolDouble  x,
  eolDouble  y,
  eolDouble  w,
  eolDouble  h
)
{
  eolRectFloat r;
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  return r;
}

void eol_rectf_set(
    eolRectFloat * d,
    eolFloat       x,
    eolFloat       y,
    eolFloat       w,
    eolFloat       h
)
{
  if (!d)return;
  d->x = x;
  d->y = y;
  d->w = w;
  d->h = h;
}

void eol_rectf_copy(
    eolRectFloat * d,
    eolRectFloat   s
  )
{
  if (!d)return;
  d->x = s.x;
  d->y = s.y;
  d->w = s.w;
  d->h = s.h;
}

void eol_rect_copy(
    eolRect *d,
    eolRect s
  )
{
  if (!d)return;
  d->x = s.x;
  d->y = s.y;
  d->w = s.w;
  d->h = s.h;
}

eolBool eol_rect_lap_rect(eolRect a, eolRect b)
{
  if ((a.x > (b.x + b.w)) || (a.y > (b.y + b.h)) || (b.x > (a.x + a.w)) || (b.y > (a.y + a.h)))
    return eolFalse;
  return eolTrue;
}


void eol_orientation_copy(eolOrientation *dst,eolOrientation src)
{
  if (!dst)return;
  eol_vec3d_copy(dst->position,src.position);
  eol_vec3d_copy(dst->rotation,src.rotation);
  eol_vec3d_copy(dst->scale,src.scale);
  eol_vec3d_copy(dst->color,src.color);
  dst->alpha = src.alpha;
}

void eol_orientation_clear(eolOrientation *out)
{
  if (!out)return;
  eol_vec3d_set(out->position,0,0,0);
  eol_vec3d_set(out->rotation,0,0,0);
  eol_vec3d_set(out->scale,1,1,1);
  eol_vec3d_set(out->color,1,1,1);
  out->alpha = 1;
}

void eol_orientation_add(eolOrientation * out,
                         eolOrientation   in1,
                         eolOrientation   in2)
{
  if (!out)return;
  eol_vec3d_add(out->position,in1.position,in2.position);
  eol_vec3d_add(out->rotation,in1.rotation,in2.rotation);
  eol_vec3d_add(out->scale,in1.scale,in2.scale);
  eol_vec3d_add(out->color,in1.color,in2.color);
  out->alpha = in1.alpha + in2.alpha;

  if (out->alpha < 0)out->alpha = 0;
  else if (out->alpha > 1)out->alpha = 1;
  
  if (out->color.x < 0)out->color.x = 0;
  else if (out->color.x > 1)out->color.x = 1;
  
  if (out->color.y < 0)out->color.y = 0;
  else if (out->color.y > 1)out->color.y = 1;
  
  if (out->color.z < 0)out->color.z = 0;
  else if (out->color.z > 1)out->color.z = 1;
}

void eol_orientation_mix(eolOrientation * out,
                         eolOrientation   in1,
                         eolOrientation   in2)
{
  if (!out)return;
  eol_vec3d_add(in1.position,in2.position,out->position);
  eol_vec3d_add(in1.rotation,in2.rotation,out->rotation);
  eol_vec3d_add(in1.scale,in2.scale,out->scale);

  out->scale.x = in1.scale.x * in2.scale.x;
  out->scale.y = in1.scale.y * in2.scale.y;
  out->scale.z = in1.scale.z * in2.scale.z;
  
  out->color.x = in1.color.x * in2.color.x;
  out->color.y = in1.color.y * in2.color.y;
  out->color.z = in1.color.z * in2.color.z;
  
  out->alpha = in1.alpha * in2.alpha;

  if (out->alpha < 0)out->alpha = 0;
  else if (out->alpha > 1)out->alpha = 1;

  if (out->color.x < 0)out->color.x = 0;
  else if (out->color.x > 1)out->color.x = 1;

  if (out->color.y < 0)out->color.y = 0;
  else if (out->color.y > 1)out->color.y = 1;

  if (out->color.z < 0)out->color.z = 0;
  else if (out->color.z > 1)out->color.z = 1;
}

void eol_trail_new(eolTrail *trail,eolUint maxLen)
{
  if (!trail)return;
  if (trail->trail != NULL)return;
  if (maxLen == 0)return;
  trail->trail = (eolOrientation *)malloc(sizeof(eolOrientation)*maxLen);
  if (trail->trail == NULL)return;
  memset(trail->trail,0,sizeof(eolOrientation)*maxLen);
  trail->maxLen = maxLen;
  trail->tail = &trail->trail[0];
  trail->head = &trail->trail[0];
}

void eol_trail_free(eolTrail *trail)
{
  if (!trail)return;
  if (!trail->trail)return;
  free(trail->trail);
  memset(trail,0,sizeof(eolTrail));
}

void eol_trail_append(eolTrail *trail,eolOrientation orientation)
{
  if (!trail)return;
  if (!trail->trail)return;
  trail->head++;
  if (trail->head >= &trail->trail[trail->maxLen])
  {
    trail->head = &trail->trail[0];
  }
  if (trail->tail == trail->head)
  {
    trail->tail++;
    if (trail->tail == &trail->trail[trail->maxLen])
    {
      trail->tail = &trail->trail[0];
    }
  }
  memcpy(trail->head,&orientation,sizeof(eolOrientation));
  if (trail->len < trail->maxLen)trail->len++;
}

eolUint eol_trail_get_length(eolTrail *trail)
{
  if (!trail)return 0;
  return trail->len;
}

eolBool eol_trail_get_nth(eolTrail *trail, eolUint n, eolOrientation *ori)
{
  int i;
  eolOrientation *it;
  if (!trail)return eolFalse;
  if (!trail->trail)return eolFalse;
  if (!ori)return eolFalse;
  if (n >= trail->maxLen)return eolFalse;
  it = trail->head;
  for (i = 0;i < n;i++)
  {
    it--;
    if (it < &trail->trail[0])it = &trail->trail[trail->maxLen - 1];
  }
  memcpy(ori,it,sizeof(eolOrientation));
  return eolTrue;
}

eolUint eol_true_from_string(char * truth)
{
  if ((strcmp(truth,"TRUE") == 0) ||
    (strcmp(truth,"true") == 0) ||
    (strcmp(truth,"True") == 0) ||
    (strcmp(truth,"1") == 0))
    return eolTrue;
  return eolFalse;
}

eolUint eol_false_from_string(char * truth)
{
  if ((strcmp(truth,"FALSE") == 0) ||
    (strcmp(truth,"false") == 0) ||
    (strcmp(truth,"False") == 0) ||
    (strcmp(truth,"0") == 0))
    return eolFalse;
  return eolTrue;
}

eolInt eol_bool_from_string(char * truth)
{
  if ((strcmp(truth,"TRUE") == 0) ||
    (strcmp(truth,"true") == 0) ||
    (strcmp(truth,"True") == 0) ||
    (strcmp(truth,"1") == 0))
    return eolTrue;
  if ((strcmp(truth,"FALSE") == 0) ||
    (strcmp(truth,"false") == 0) ||
    (strcmp(truth,"False") == 0) ||
    (strcmp(truth,"0") == 0))
    return eolFalse;
  return -1;
}

char *eol_string_from_bool(eolBool b)
{
  if (b == eolTrue)
  {
    return "true";
  }
  else if (b == eolFalse)
  {
    return "false";
  }
  else return "ERROR";
}


/*eol @ eof*/

