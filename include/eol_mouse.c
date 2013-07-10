#include "eol_mouse.h"
#include "eol_matrix.h"
#include "eol_3d_op.h"
#include "eol_draw.h"
#include "eol_sprite.h"
#include "eol_camera.h"
#include "eol_graphics.h"

typedef struct
{
  eolInt      x,y;
  eolInt      ox,oy;
  eolUint     buttons;
  eolUint     oldButtons;
  eolUint     lastButtons;
  eolUint     timePress; /*this time index*/
  eolUint     lastPress; /*time index of the last time this input was pressed*/
  eolSprite * sprite;
  eolUint     frame;
  eolInt      showHide;
}eolMouse;

eolMouse _eol_mouse;
eolLine  _eol_mouse_file;
eolInt   _eol_mouse_frame_width = -1;
eolInt   _eol_mouse_frame_height = -1;

void eol_mouse_close()
{
  eol_sprite_free(&_eol_mouse.sprite);
  memset(&_eol_mouse,0,sizeof(eolMouse));
}

void eol_mouse_init()
{
  memset(&_eol_mouse,0,sizeof(eolMouse));
  _eol_mouse.sprite = eol_sprite_load(_eol_mouse_file,
                                      _eol_mouse_frame_width,
                                      _eol_mouse_frame_height);
  SDL_ShowCursor(SDL_DISABLE);
  atexit(eol_mouse_close);
}

void eol_mouse_config()
{
  /*TODO; get from config*/
  strncpy(_eol_mouse_file,"images/UI/mouse_icon.png",EOLLINELEN);
}

void eol_mouse_draw()
{
  if (_eol_mouse.showHide <= 0)return;
  eol_sprite_draw(_eol_mouse.sprite,
                  _eol_mouse.frame,
                  _eol_mouse.x,
                  _eol_mouse.y);
}

void eol_mouse_update()
{
  _eol_mouse.ox = _eol_mouse.x;
  _eol_mouse.oy = _eol_mouse.y;
  _eol_mouse.oldButtons = _eol_mouse.buttons;
  _eol_mouse.buttons = SDL_GetMouseState(&_eol_mouse.x,&_eol_mouse.y);
  if (_eol_mouse.buttons != _eol_mouse.oldButtons)
  {
    if (_eol_mouse.oldButtons)
    {
      _eol_mouse.lastButtons = _eol_mouse.oldButtons;
      _eol_mouse.lastPress = _eol_mouse.timePress;
      _eol_mouse.timePress = eol_graphics_get_now();
    }
  }
}

eolBool eol_mouse_old_state(eolUint button)
{
  eolUint mask = 1;
  mask = mask << button;
  return (_eol_mouse.oldButtons & mask);
}

eolBool eol_mouse_last_state(eolUint button)
{
  eolUint mask = 1;
  mask = mask << button;
  return (_eol_mouse.lastButtons & mask);
}

eolBool eol_mouse_input_state(eolUint button)
{
  eolUint mask = 1;
  mask = mask << button;
  return (_eol_mouse.buttons & mask);
}

void eol_mouse_get_position_vec2d(eolVec2D *pos)
{
  if (!pos)return;
  pos->x = (eolFloat)_eol_mouse.x;
  pos->y = (eolFloat)_eol_mouse.y;
}

void eol_mouse_get_position(eolInt *x, eolInt *y)
{
  if (x)*x = _eol_mouse.x;
  if (y)*y = _eol_mouse.y;
}

void eol_mouse_get_motion(eolInt *dx, eolInt *dy)
{
  if (dx)*dx = _eol_mouse.x - _eol_mouse.ox;
  if (dy)*dy = _eol_mouse.y - _eol_mouse.oy;
}

eolBool eol_mouse_in_rect(eolRect rect)
{
  eolVec2D v;
  eolInt x,y;
  eol_mouse_get_position(&x, &y);
  v.x = x;
  v.y = y;
  return eol_vec_in_rect(v,rect);
}

eolBool eol_mouse_input_pressed(eolUint button)
{
  return ((eol_mouse_input_state(button)) &&
          (!eol_mouse_old_state(button)));
}

eolBool eol_mouse_input_released(eolUint button)
{
  return ((!eol_mouse_input_state(button)) &&
          (eol_mouse_old_state(button)));
}

eolBool eol_mouse_input_held(eolUint button)
{
  return ((eol_mouse_input_state(button)) &&
          (eol_mouse_old_state(button)));
}

eolBool eol_mouse_input_double_tapped(eolUint button)
{
  return ((eol_mouse_input_state(button)) &&
          (!eol_mouse_old_state(button)) &&
          (eol_mouse_last_state(button)) &&
          ((_eol_mouse.lastPress - _eol_mouse.lastPress) > 15));
          /*TODO the 15 needs to be brought in fromthe input config*/
}

void eol_mouse_hide()
{
  _eol_mouse.showHide--;
}

void eol_mouse_show()
{
  _eol_mouse.showHide++;
}

void eol_mouse_reset()
{
  _eol_mouse.showHide = 0;
}

void eol_mouse_draw_3d_ray()
{
  eolVec3D position,vector, position2;
  eol_mouse_get_3d_ray(&position,&vector);
  
  eol_vec3d_scale(vector,vector,100);

  eol_vec3d_add(position2,position,vector);

  eol_draw_dot_3D(position,
                  3,
                  eol_vec3d(1,0,0),
                  1);
  eol_draw_line_3D(position,
                   position2,
                   2,
                   eol_vec3d(1,1,0),
                   1);
  eol_draw_dot_3D(position2,
                  5,
                  eol_vec3d(0,1,0),
                  1);
}

eolBool eol_mouse_in_3d_quad(eolVec3D t1,eolVec3D t2,eolVec3D t3,eolVec3D t4)
{
  eolVec3D position,vector;
  eol_mouse_get_3d_ray(&position,&vector);
  return eol_3d_op_ray_in_quad(
    position,
    vector,
    t1,
    t2,
    t3,
    t4,
    NULL);
}

eolBool eol_mouse_in_quad3d(eolQuad3D quad)
{
  eolVec3D position,vector;
  eol_mouse_get_3d_ray(&position,&vector);
  return eol_3d_op_ray_in_quad3d(
    position,
    vector,
    quad,
    NULL);
}

eolBool eol_mouse_get_quad3d_intersect(eolVec3D *contact,eolQuad3D quad)
{
  eolText text;
  eolVec3D position,vector;
  eol_mouse_get_3d_ray(&position,&vector);
  eol_vec3d_scale(vector,vector,10000);
  eol_3d_op_print_quad3d(text,quad);
  return eol_3d_op_ray_in_quad3d(
    position,
    vector,
    quad,
    contact);
}


eolBool eol_mouse_in_3d_triangle(eolVec3D t1,eolVec3D t2,eolVec3D t3)
{
  eolVec3D position,vector;
  eol_mouse_get_3d_ray(&position,&vector);
  return eol_3d_op_ray_in_triangle(
    position,
    vector,
    t1,
    t2,
    t3,
    NULL);
}

eolBool eol_mouse_in_triangle3d(eolTriangle3D tri)
{
  eolVec3D position,vector;
  eol_mouse_get_3d_ray(&position,&vector);
  return eol_3d_op_ray_in_triangle3d(
    position,
    vector,
    tri,
    NULL);
}


void eol_mouse_get_3d_ray(eolVec3D *position,eolVec3D *vector)
{
  eolVec3D cam,pos1,pos2,rotation;
  eolGraphicsView view;
  eol_graphics_get_view(&view);

  eol_camera_get_position(&cam);
  
  gluUnProject(_eol_mouse.x, _eol_mouse.y, 0.99, view.modelView, view.projection, view.viewPort, &pos1.x, &pos1.y, &pos1.z);
  gluUnProject(_eol_mouse.x, _eol_mouse.y, 1, view.modelView, view.projection, view.viewPort, &pos2.x, &pos2.y, &pos2.z);

  pos2.y *= -1;
  pos1.y *= -1;
  eol_vec3d_add(pos1,cam,pos1);
  eol_vec3d_add(pos2,cam,pos2);
  
  if (position)
  {
    eol_vec3d_copy((*position),pos1);
  }
  if (vector)
  {
    eol_vec3d_sub((*vector),pos2,pos1);
    eol_vec3d_normalize(vector);
    eol_camera_get_rotation(&rotation);
    eol_vec3d_rotate_about_x(vector,rotation.x);
    eol_vec3d_rotate_about_y(vector,rotation.y);
    eol_vec3d_rotate_about_z(vector,rotation.z);
    
  }
}

/*eol@eof*/
