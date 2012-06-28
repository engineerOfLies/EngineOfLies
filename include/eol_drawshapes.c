#include "eol_drawshapes.h"
#include "eol_graphics.h"

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
