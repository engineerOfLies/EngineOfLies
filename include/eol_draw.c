#include "eol_draw.h"
#include "eol_graphics.h"

void eol_draw_trail(eolTrail * trail,
                    eolFloat   radius,
                    eolBool    fade,
                    eolBool    taper)
{
  int i;
  eolFloat fadeFactor = 1;
  eolFloat taperFactor = 1;
  eolFloat alpha = 1;
  eolOrientation ori[2];
  if (!trail)return;
  if (!trail->trail)return;
  if (trail->len <= 1)return;
  if (fade)fadeFactor = 0.75;
  if (taper)taperFactor = 0.75;
  i = 1;
  if (!eol_trail_get_nth(trail, i, &ori[0]))return;
  while (!eol_trail_get_nth(trail, i, &ori[1]))
  {
    eol_draw_line_3D(ori[0].position,
                     ori[1].position,
                     radius,
                     ori[0].color,
                     (alpha * ori[0].alpha));
    radius *= taperFactor;
    alpha *= fadeFactor;
    memcpy(&ori[0],&ori[1],sizeof(eolOrientation));
  }
}

void eol_draw_cirlce_3D(eolVec3D point,
                        eolFloat radius,
                        eolUint  segments,
                        eolVec3D color,
                        eolFloat alpha)
{
  eolVec3D p1,p2;
  eolFloat step;
  int i;
  if (segments < 3)return;
  step = EOL_2PI / segments;
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(color.x,color.y,color.z,alpha);
  glPushMatrix();

  glTranslatef(point.x,point.y,point.z);
  
  glBegin( GL_LINES );
  
  for (i = 0; i < segments;i++)
  {
    eol_vec3d_set_angle_by_radians(&p1,i * step);
    eol_vec3d_set_angle_by_radians(&p2,(i + 1) * step);
    p1.x *= radius;
    p1.y *= radius;
    p2.x *= radius;
    p2.y *= radius;
    glVertex3f(p1.x,p1.y,0);
    glVertex3f(p2.x,p2.y,0);
  }
  
  glEnd( );
  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  
}

void eol_draw_dot_3D(eolVec3D point,
                     eolFloat radius,
                     eolVec3D color,
                     eolFloat alpha)
{
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(color.x,color.y,color.z,alpha);
  glPushMatrix();
  glEnable(GL_POINT_SMOOTH);
  glPointSize(radius);
  glBegin( GL_POINTS );
  glVertex3f(point.x,point.y,point.z);
  glEnd( );
  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
}

void eol_draw_line_3D(eolVec3D p1,
                      eolVec3D p2,
                      eolFloat radius,
                      eolVec3D color,
                      eolFloat alpha)
{
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(color.x,color.y,color.z,alpha);
  glPushMatrix();
  glLineWidth(radius);
  glBegin( GL_LINES );
  glVertex3f(p1.x,p1.y,p1.z);
  glVertex3f(p2.x,p2.y,p2.z);
  glEnd( );
  
  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
}

void eol_draw_tri_3D(eolVec3D p1,
                     eolVec3D p2,
                     eolVec3D p3,
                     eolVec3D color,
                     eolFloat alpha)
{
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(color.x,color.y,color.z,alpha);
  glPushMatrix();
  glBegin( GL_TRIANGLES );
  glVertex3f(p1.x,p1.y,p1.z);
  glVertex3f(p2.x,p2.y,p2.z);
  glVertex3f(p3.x,p3.y,p3.z);
  glEnd( );
  
  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
}


void eol_draw_rect_3D(eolRectFloat rect, eolOrientation ori)
{
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(ori.color.x,ori.color.y,ori.color.z,ori.alpha);
  glPushMatrix();

  glTranslatef(ori.position.x,ori.position.y,ori.position.z);
  glRotatef(ori.rotation.x, 1.0f, 0.0f, 0.0f);
  glRotatef(ori.rotation.y, 0.0f, 1.0f, 0.0f);
  glRotatef(ori.rotation.z, 0.0f, 0.0f, 1.0f);
  glScalef(ori.scale.x,ori.scale.y,ori.scale.z);
  
  
  glBegin( GL_LINES );

  glVertex3f(rect.x,rect.y,0);
  glVertex3f(rect.x + rect.w,rect.y,0);
  
  glVertex3f(rect.x + rect.w,rect.y,0);
  glVertex3f(rect.x + rect.w,rect.y + rect.h,0);

  glVertex3f(rect.x + rect.w,rect.y + rect.h,0);
  glVertex3f(rect.x,rect.y + rect.h,0);

  glVertex3f(rect.x,rect.y + rect.h,0);
  glVertex3f(rect.x,rect.y,0);
  glEnd( );
  
  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
}

void eol_draw_rect(eolRect rect,eolVec3D color,eolFloat alpha)
{
    eolVec3D pos,pos2;
  eolGraphicsView view;

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(color.x,color.y,color.z,alpha);
  
  eol_graphics_get_view(&view);
  eol_opengl_get_gl_coordinate(
      rect.x,
      rect.y,
      EOL_2DPLANE_,
      view.modelView,
      view.projection,
      view.viewPort,
      &pos.x,
      &pos.y,
      &pos.z
  );

  eol_opengl_get_gl_coordinate(
      rect.x + rect.w,
      rect.y + rect.h,
      EOL_2DPLANE_,
      view.modelView,
      view.projection,
      view.viewPort,
      &pos2.x,
      &pos2.y,
      &pos2.z
  );

  glPushMatrix();
  
  glBegin( GL_LINES );
  glVertex3f(pos.x,pos.y,pos.z);
  glVertex3f(pos2.x,pos.y,pos.z);
  
  glVertex3f(pos2.x,pos.y,pos.z);
  glVertex3f(pos2.x,pos2.y,pos.z);
  
  glVertex3f(pos2.x,pos2.y,pos.z);
  glVertex3f(pos.x,pos2.y,pos.z);

  glVertex3f(pos.x,pos2.y,pos.z);
  glVertex3f(pos.x,pos.y,pos.z);
  glEnd( );

  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

}

void eol_draw_solid_rect(eolRect rect,eolVec3D color,eolFloat alpha)
{
  eolVec3D pos,pos2;
  eolGraphicsView view;

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glColor4f(color.x,color.y,color.z,alpha);
  
  eol_graphics_get_view(&view);
  eol_opengl_get_gl_coordinate(
      rect.x,
      rect.y,
      EOL_2DPLANE_,
      view.modelView,
      view.projection,
      view.viewPort,
      &pos.x,
      &pos.y,
      &pos.z
  );

  eol_opengl_get_gl_coordinate(
      rect.x + rect.w,
      rect.y + rect.h,
      EOL_2DPLANE_,
      view.modelView,
      view.projection,
      view.viewPort,
      &pos2.x,
      &pos2.y,
      &pos2.z
  );

  glPushMatrix();
  
  glBegin( GL_QUADS );
  glVertex3f(pos.x,pos.y,pos.z);
  glVertex3f(pos2.x,pos.y,pos.z);
  glVertex3f(pos2.x,pos2.y,pos.z);
  glVertex3f(pos.x,pos2.y,pos.z);
  glEnd( );

  glPopMatrix();
  glColor4f(1,1,1,1);
  glDisable(GL_BLEND);
  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
}
/*eol@eof*/
