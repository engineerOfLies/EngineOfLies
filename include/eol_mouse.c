#include "eol_mouse.h"
#include "eol_sprite.h"
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
/*eol@eof*/
