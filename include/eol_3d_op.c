#include "eol_3d_op.h"
#include "eol_matrix.h"

/*
 * *** triangle basics ***
 */

eolTriangle3D eol_triangle_3d(
  eolVec3D t1,
  eolVec3D t2,
  eolVec3D t3
)
{
  eolTriangle3D tri;
  eol_vec3d_copy(tri.t1,t1);
  eol_vec3d_copy(tri.t2,t2);
  eol_vec3d_copy(tri.t3,t3);
  return tri;
}

void eol_triangle_3d_set(
  eolTriangle3D *tri,
  eolVec3D t1,
  eolVec3D t2,
  eolVec3D t3
)
{
  if (!tri)return;
  eol_vec3d_copy(tri->t1,t1);
  eol_vec3d_copy(tri->t2,t2);
  eol_vec3d_copy(tri->t3,t3);
}


eolTriangle2D eol_triangle_2d(
  eolVec2D t1,
  eolVec2D t2,
  eolVec2D t3
)
{
  eolTriangle2D tri;
  eol_vec2d_copy(tri.t1,t1);
  eol_vec2d_copy(tri.t2,t2);
  eol_vec2d_copy(tri.t3,t3);
  return tri;
}

void eol_triangle_2d_set(
  eolTriangle2D *tri,
  eolVec2D t1,
  eolVec2D t2,
  eolVec2D t3
)
{
  if (!tri)return;
  eol_vec2d_copy(tri->t1,t1);
  eol_vec2d_copy(tri->t2,t2);
  eol_vec2d_copy(tri->t3,t3);
}

eolQuad3D eol_quad_3d(
  eolVec3D t1,
  eolVec3D t2,
  eolVec3D t3,
  eolVec3D t4
)
{
  eolQuad3D quad;
  eol_vec3d_copy(quad.t1,t1);
  eol_vec3d_copy(quad.t2,t2);
  eol_vec3d_copy(quad.t3,t3);
  eol_vec3d_copy(quad.t4,t4);
  return quad;
}

void eol_quad_3d_set(
  eolQuad3D *quad,
  eolVec3D t1,
  eolVec3D t2,
  eolVec3D t3,
  eolVec3D t4
)
{
  if (!quad)return;
  eol_vec3d_copy(quad->t1,t1);
  eol_vec3d_copy(quad->t2,t2);
  eol_vec3d_copy(quad->t3,t3);
  eol_vec3d_copy(quad->t4,t4);
}


eolQuad2D eol_quad_2d(
  eolVec2D t1,
  eolVec2D t2,
  eolVec2D t3,
  eolVec2D t4
)
{
  eolQuad2D quad;
  eol_vec2d_copy(quad.t1,t1);
  eol_vec2d_copy(quad.t2,t2);
  eol_vec2d_copy(quad.t3,t3);
  eol_vec2d_copy(quad.t4,t4);
  return quad;
}

void eol_quad_2d_set(
  eolQuad2D *quad,
  eolVec2D t1,
  eolVec2D t2,
  eolVec2D t3,
  eolVec2D t4
)
{
  if(!quad)return;
  eol_vec2d_copy(quad->t1,t1);
  eol_vec2d_copy(quad->t2,t2);
  eol_vec2d_copy(quad->t3,t3);
  eol_vec2d_copy(quad->t4,t4);
}

/*
 * *** edge operations ***
 */

eolFloat eol_2d_edge_length(eolVec2D e1,eolVec2D e2)
{
  return eol_vec2d_magnitude(eol_vec2d(e1.x-e2.x,e1.y-e2.y));
}

eolFloat eol_3d_edge_length(eolVec3D e1,eolVec3D e2)
{
  return eol_vec3d_magnitude(eol_vec3d(e1.x-e2.x,e1.y-e2.y,e1.z-e2.z));
}


eolBool eol_2d_op_ray_cross_edge(
  eolVec2D start,
  eolVec2D v,
  eolVec2D e1,
  eolVec2D e2,
  eolVec3D *contact)
{
  eolFloat testx, testy;
  eolFloat endx, endy;
  eolFloat Ua,Ub,Uden;
  
  endx = start.x + v.x;
  endy = start.y + v.y;
  
  Uden = ((e2.y - e1.y)*(endx - start.x)) - ((e2.x - e1.x)*(endy - start.y));
  if(Uden == 0)
  {
    return eolFalse;/*parallel, can't hit*/
  }
    
  Ua = (((e2.x - e1.x)*(start.y - e1.y))-((e2.y - e1.y)*(start.x - e1.x))) / Uden;
  Ub = (((endx - start.x)*(start.y - e1.y))-((endy - start.y)*(start.x - e1.x))) / Uden;
  
  testx = start.x + (Ua * (endx - start.x));
  testy = start.y + (Ua * (endy - start.y));
  
  if(contact != NULL)
  {
    contact->x = testx;
    contact->y = testy;
  }
  
  if((Ua >= 0) && (Ua <= 1) && (Ub >= 0) && ( Ub <= 1))
  {
    return eolTrue;
  }
  return eolFalse;  
}

/*
 * *** transformation operations
 */

void eol_3d_op_transform_quad_by_ori(eolQuad3D *out, eolQuad3D in,eolOrientation ori)
{
  eol_3d_op_transform_quad(out, in,ori.position, ori.rotation, ori.scale);
}

void eol_3d_op_transform_quad(eolQuad3D *out, eolQuad3D in,eolVec3D translation, eolVec3D rotation, eolVec3D scale)
{
  if (!out)return;
  eol_3d_op_transform_vec3d(&out->t1, in.t1,translation, rotation, scale);
  eol_3d_op_transform_vec3d(&out->t2, in.t2,translation, rotation, scale);
  eol_3d_op_transform_vec3d(&out->t3, in.t3,translation, rotation, scale);
  eol_3d_op_transform_vec3d(&out->t4, in.t4,translation, rotation, scale);
}

void eol_3d_op_transform_quad_inverse_by_ori(eolQuad3D *out, eolQuad3D in,eolOrientation ori)
{
  eol_3d_op_transform_quad_inverse(out, in,ori.position, ori.rotation, ori.scale);
}

void eol_3d_op_transform_quad_inverse(eolQuad3D *out, eolQuad3D in,eolVec3D translation, eolVec3D rotation, eolVec3D scale)
{
  if (!out)return;
  eol_3d_op_transform_vec3d_inverse(&out->t1, in.t1,translation, rotation, scale);
  eol_3d_op_transform_vec3d_inverse(&out->t2, in.t2,translation, rotation, scale);
  eol_3d_op_transform_vec3d_inverse(&out->t3, in.t3,translation, rotation, scale);
  eol_3d_op_transform_vec3d_inverse(&out->t4, in.t4,translation, rotation, scale);
}


void eol_3d_op_rotate(eolVec3D *out, eolVec3D in,eolVec3D rotation)
{
  if (!out)return;
  eol_3d_op_rotate_about_z(out, in,rotation.z);
  eol_3d_op_rotate_about_y(out, *out,rotation.y);
  eol_3d_op_rotate_about_x(out, *out,rotation.x);
}

void eol_3d_op_rotate_inverse(eolVec3D *out, eolVec3D in,eolVec3D rotation)
{
  eolVec3D locRot;
  if (!out)return;
  eol_vec3d_negate(locRot,rotation);
  eol_3d_op_rotate_about_x(out, in,locRot.x);
  eol_3d_op_rotate_about_y(out, *out,locRot.y);
  eol_3d_op_rotate_about_z(out, *out,locRot.z);
}

void eol_3d_op_rotate_about_x(eolVec3D *out, eolVec3D in,eolFloat angle)
{
  eolMat4 m;
  if (!out)return;
  angle *= EOL_DEGTORAD;
  m_eol_mat4(
    m,
    1,        0,          0,            0,
    0,        cos(angle), sin(angle),  0,
    0,        -sin(angle), cos(angle),   0,
    0,        0,          0,            1);
  eol_mat4_mult_vert(
    out,
    m,
    in
  );
}

void eol_3d_op_rotate_about_y(eolVec3D *out, eolVec3D in,eolFloat angle)
{
  eolMat4 m;
  if (!out)return;
  angle *= EOL_DEGTORAD;
  m_eol_mat4(
    m,
    cos(angle), 0,          -sin(angle), 0,
    0,          1,          0,          0,
    sin(angle),0,          cos(angle), 0,
    0,          0,          0,          1);
  eol_mat4_mult_vert(
    out,
    m,
    in
  );
}

void eol_3d_op_rotate_about_z(eolVec3D *out, eolVec3D in,eolFloat angle)
{
  eolMat4 m;
  if (!out)return;
  angle *= EOL_DEGTORAD;
  m_eol_mat4(
    m,
    cos(angle), sin(angle),0,          0,
    -sin(angle), cos(angle), 0,          0,
    0,          0,          1,          0,
    0,          0,          0,          1);
  eol_mat4_mult_vert(
    out,
    m,
    in
  );
}

void eol_3d_op_scale(eolVec3D *out, eolVec3D in,eolVec3D scale)
{
  eolMat4 m;
  if (!out)return;
  m_eol_mat4(
    m,
    scale.x,    0,          0,          0,
    0,          scale.y,    0,          0,
    0,          0,          scale.z,    0,
    0,          0,          0,          1);
  eol_mat4_mult_vert(
    out,
    m,
    in
  );
}

void eol_3d_op_scale_inverse(eolVec3D *out, eolVec3D in,eolVec3D scale)
{
  eolMat4 m;
  eolVec3D lScale = {1,1,1};
  if (!out)return;
  if (scale.x != 0)lScale.x = 1/scale.x;
  if (scale.y != 0)lScale.y = 1/scale.y;
  if (scale.z != 0)lScale.z = 1/scale.z;
  m_eol_mat4(
    m,
    lScale.x,    0,           0,          0,
    0,           lScale.y,    0,          0,
    0,           0,            lScale.z,  0,
    0,           0,            0,          1);
  eol_mat4_mult_vert(
    out,
    m,
    in
  );
}


void eol_3d_op_translate(eolVec3D *out, eolVec3D in,eolVec3D translation)
{
  if (!out)return;
  out->x = in.x + translation.x;
  out->y = in.y + translation.y;
  out->z = in.z + translation.z;
}

void eol_3d_op_transform_vec3d_by_ori(eolVec3D *out, eolVec3D in,eolOrientation ori)
{
  eol_3d_op_transform_vec3d(out, in,ori.position, ori.rotation, ori.scale);
}

void eol_3d_op_transform_vec3d(eolVec3D *out, eolVec3D in,eolVec3D translation, eolVec3D rotation, eolVec3D scale)
{
  if (!out)return;
  eol_3d_op_scale(out, in,scale);
  eol_3d_op_rotate(out, *out,rotation);
  eol_3d_op_translate(out,*out,translation);
}

void eol_3d_op_transform_vec3d_inverse_by_ori(eolVec3D *out, eolVec3D in,eolOrientation ori)
{
  eol_3d_op_transform_vec3d_inverse(out, in,ori.position, ori.rotation, ori.scale);
}

void eol_3d_op_transform_vec3d_inverse(eolVec3D *out, eolVec3D in,eolVec3D translation, eolVec3D rotation, eolVec3D scale)
{
  eolVec3D move;
  if (!out)return;
  eol_vec3d_negate(move,translation);
  eol_3d_op_translate(out,in,move);
  eol_3d_op_rotate_inverse(out, *out,rotation);
  eol_3d_op_scale_inverse(out, *out,scale);
}

/*
 * *** triangle ops ***
 */
void eol_3d_op_get_triangle_normal(eolVec3D t1, eolVec3D t2, eolVec3D t3, eolVec3D *normal)
{
  if (normal)
  {
    normal->x = (t1.y * (t2.z - t3.z)) + (t2.y * (t3.z - t1.z)) + (t3.y * (t1.z - t2.z));
    normal->y = (t1.z * (t2.x - t3.x)) + (t2.z * (t3.x - t1.x)) + (t3.z * (t1.x - t2.x));
    normal->z = (t1.x * (t2.y - t3.y)) + (t2.x * (t3.y - t1.y)) + (t3.x * (t1.y - t2.y));
    eol_vec3d_normalize(normal);
  }
}

void eol_3d_op_get_triangle_plane(eolVec3D t1, eolVec3D t2, eolVec3D t3, eolVec3D *normal, eolFloat *D)
{
  eol_3d_op_get_triangle_normal(t1, t2, t3, normal);
  if (D)
  {
    *D = -((t1.x *(t2.y * t3.z - t3.y * t2.z)) +
          (t2.x *(t3.y * t1.z - t1.y * t3.z)) +
          (t3.x *(t1.y * t2.z - t2.y * t1.z)));
  }
}

eolFloat eol_3d_op_ray_in_plane(
  eolVec3D start,
  eolVec3D dir,
  eolVec3D normal,
  eolFloat D,
  eolVec3D *contact)
{
  eolFloat denom,t;
  denom = ((normal.x * dir.x) + (normal.y * dir.y) + (normal.z * dir.z));
  if(denom == 0)return 0;
  t = - (((normal.x * start.x) + (normal.y * start.y) + (normal.z * start.z) + D) / denom);
  if((t > 0)&&(t <= 1))
  {
    if (contact)
    {
      contact->x = start.x + (dir.x * t);
      contact->y = start.y + (dir.y * t);
      contact->z = start.z + (dir.z * t);
    }
    return 1;
  }
  if (contact)
  {
    contact->x = start.x + (dir.x * t);
    contact->y = start.y + (dir.y * t);
    contact->z = start.z + (dir.z * t);
  }
  return -1;
}

eolBool eol_3d_op_point_in_triangle(
  eolVec3D point,
  eolVec3D t1,
  eolVec3D t2,
  eolVec3D t3,
  eolVec3D *normal)
{
  eolVec3D n;
  eolVec2D rayTest = {0,0};
  eolUint intersectCount = 0;
  if (!normal)
  {
    eol_3d_op_get_triangle_normal(t1, t2, t3, &n);
    normal = &n;
  }

  rayTest.y = eol_3d_edge_length(t1,t2) + eol_3d_edge_length(t2,t3) + eol_3d_edge_length(t3,t1);
  
  if (normal->x > (MAX(normal->y,normal->z)))
  {
    /*project triangle to yz plane*/
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.y,point.z),
      rayTest,
      eol_vec2d(t1.y,t1.z),
      eol_vec2d(t2.y,t2.z),
      NULL))
    {
      intersectCount++;
    }
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.y,point.z),
      rayTest,
      eol_vec2d(t2.y,t2.z),
      eol_vec2d(t3.y,t3.z),
      NULL))
    {
      intersectCount++;
    }
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.y,point.z),
      rayTest,
      eol_vec2d(t3.y,t3.z),
      eol_vec2d(t1.y,t1.z),
      NULL))
    {
      intersectCount++;
    }    
  }
  else if (normal->y > (MAX(normal->x,normal->z)))
  {
    /*project triangle to xz plane*/
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.x,point.z),
      rayTest,
      eol_vec2d(t1.x,t1.z),
      eol_vec2d(t2.x,t2.z),
      NULL))
    {
      intersectCount++;
    }
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.x,point.z),
      rayTest,
      eol_vec2d(t2.x,t2.z),
      eol_vec2d(t3.x,t3.z),
      NULL))
    {
      intersectCount++;
    }
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.x,point.z),
      rayTest,
      eol_vec2d(t3.x,t3.z),
      eol_vec2d(t1.x,t1.z),
      NULL))
    {
      intersectCount++;
    }
  }
  else
  {
    /*project triangle to xy plane*/
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.x,point.y),
      rayTest,
      eol_vec2d(t1.x,t1.y),
      eol_vec2d(t2.x,t2.y),
      NULL))
    {
      intersectCount++;
    }
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.x,point.y),
      rayTest,
      eol_vec2d(t2.x,t2.y),
      eol_vec2d(t3.x,t3.y),
      NULL))
    {
      intersectCount++;
    }
    if (eol_2d_op_ray_cross_edge(
      eol_vec2d(point.x,point.y),
      rayTest,
      eol_vec2d(t3.x,t3.y),
      eol_vec2d(t1.x,t1.y),
      NULL))
    {
      intersectCount++;
    }
  }
  if (intersectCount % 2)
  {
    return eolTrue;
  }
  return eolFalse;
}

eolBool eol_3d_op_ray_in_triangle3d(
  eolVec3D start,
  eolVec3D dir,
  eolTriangle3D tri,
  eolVec3D *contact)
{
  return eol_3d_op_ray_in_triangle(
    start,
    dir,
    tri.t1,
    tri.t2,
    tri.t3,
    contact);
}

eolBool eol_3d_op_ray_in_triangle(
  eolVec3D start,
  eolVec3D dir,
  eolVec3D t1,
  eolVec3D t2,
  eolVec3D t3,
  eolVec3D *contact)
{
  eolFloat t;
  eolVec3D normal;
  eolFloat D;
  eolVec3D intersectPoint;
  
  eol_3d_op_get_triangle_plane(t1,t2,t3,&normal,&D);
  t = eol_3d_op_ray_in_plane(
    start,
    dir,
    normal,
    D,
    &intersectPoint);
  if ((t <= 0)|| (t > 1))
  {
    return eolFalse;
  }
  if (eol_3d_op_point_in_triangle(
    intersectPoint,
    t1,
    t2,
    t3,
    &normal))
  {
    if (contact)
    {
      eol_vec3d_copy((*contact),intersectPoint);
    }
    return eolTrue;
  }
  return eolFalse;
}

eolBool eol_3d_op_ray_in_quad3d(
  eolVec3D start,
  eolVec3D dir,
  eolQuad3D quad,
  eolVec3D *contact)
{
  return eol_3d_op_ray_in_quad(
    start,
    dir,
    quad.t1,
    quad.t2,
    quad.t3,
    quad.t4,
    contact);
}

eolBool eol_3d_op_ray_in_quad(
  eolVec3D start,
  eolVec3D dir,
  eolVec3D t1,
  eolVec3D t2,
  eolVec3D t3,
  eolVec3D t4,
  eolVec3D *contact)
{
  eolFloat t;
  eolVec3D normal;
  eolFloat D;
  eolVec3D intersectPoint;

  eol_3d_op_get_triangle_plane(t1,t2,t3,&normal,&D);
  
  t = eol_3d_op_ray_in_plane(
    start,
    dir,
    normal,
    D,
    &intersectPoint);
  
  if ((t <= 0)|| (t > 1))
  {
    return eolFalse;
  }
  if (eol_3d_op_point_in_triangle(
    intersectPoint,
    t1,
    t2,
    t3,
    &normal))
  {
    if (contact)
    {
      eol_vec3d_copy((*contact),intersectPoint);
    }
    return eolTrue;
  }
  if (eol_3d_op_point_in_triangle(
    intersectPoint,
    t1,
    t3,
    t4,
    &normal))
  {
    if (contact)
    {
      eol_vec3d_copy((*contact),intersectPoint);
    }
    return eolTrue;
  }
  return eolFalse;
}

void eol_3d_op_print_quad3d(eolText out,eolQuad3D quad)
{
  eolLine line1,line2,line3,line4;
  eol_vec3d_print(line1, quad.t1);
  eol_vec3d_print(line2, quad.t2);
  eol_vec3d_print(line3, quad.t3);
  eol_vec3d_print(line4, quad.t4);
  snprintf(out,EOLTEXTLEN,"{%s,%s,%s,%s}",line1,line2,line3,line4);
}
  


/*eol@eof*/
