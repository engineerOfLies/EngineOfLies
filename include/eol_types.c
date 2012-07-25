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
#include <math.h>

eolBool eol_equals(eolDouble a, eolDouble b)
{
	return (fabs(a-b) < EOL_EPSILON)?eolTrue:eolFalse;
}

eolVec3D eol_vec3d(eolDouble x, eolDouble y, eolDouble z)
{
	eolVec3D vec;
  eol_vec3d_set(vec,x, y, z);
	return vec;
}

eolFloat eol_vec3d_magnitude (eolVec3D V)
{
  return sqrt (V.x * V.x + V.y * V.y + V.z * V.z);
}

eolFloat eol_vec3d_magnatude_squared(eolVec3D V)
{
  return (V.x * V.x + V.y * V.y + V.z * V.z);
}

eolBool  eol_vec3d_magnatude_less_than(eolVec3D V,eolFloat size)
{
  return (eol_vec3d_magnatude_squared(V) < (size * size));
}

eolBool eol_distance_between_less_than(eolVec3D p1,eolVec3D p2,eolFloat size)
{
  return eol_vec3d_magnatude_less_than(eol_vec3d(p1.x - p2.x,
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

void eol_vec3d_normalize (eolVec3D *V)
{
  eolFloat M = eol_vec3d_magnitude (*V);
  
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
  V->z *= M;
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
  eol_vector_add_3D(in1.position,in2.position,out->position);
  eol_vector_add_3D(in1.rotation,in2.rotation,out->rotation);
  eol_vector_add_3D(in1.scale,in2.scale,out->scale);
  eol_vector_add_3D(in1.color,in2.color,out->color);
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

/*eol @ eof*/

