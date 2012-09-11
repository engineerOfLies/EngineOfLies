#include "eol_camera.h"
#include "eol_config.h"
#include "eol_logger.h"
#include "eol_mouse.h"
#include "eol_graphics.h"

/* local data types*/
/*local global variables*/
eolCamera _eol_camera;

/*local function prototypes*/
void eol_camera_editor_update(eolCamera *cam, void *data);
void eol_camera_2d_update(eolCamera *cam, void *data);
void eol_camera_third_update(eolCamera *cam, void *data);
void eol_camera_first_update(eolCamera *cam, void *data);

/*function definitions*/

void eol_camera_config()
{
  eolConfig *conf;
  eolLine temp;
  memset(&_eol_camera,0,sizeof(eolCamera));
  _eol_camera.cameratype = eolCamera2D;
  _eol_camera.followHeight = 10;
  _eol_camera.moveStep = 1;
  conf = eol_config_load("system/camera.cfg");
  if (conf == NULL)return;
  eol_config_get_line_by_tag(temp,conf,"cameraType");
  _eol_camera.cameratype = eol_camera_get_type_by_string(temp);
  eol_config_get_float_by_tag(&_eol_camera.followHeight,conf,"followHeight");
  eol_config_get_float_by_tag(&_eol_camera.moveStep,conf,"moveStep");
  eol_config_free(&conf);
}

void eol_camera_init()
{
  eolGraphicsView view;
  eol_logger_message(EOL_LOG_INFO,"eol_camera:initializing\n");
  eol_graphics_get_view(&view);
  _eol_camera.bounds.x = view.xRes;
  _eol_camera.bounds.y = view.yRes;
  switch(_eol_camera.cameratype)
  {
    case eolCameraEditor:
      _eol_camera.update = eol_camera_editor_update;
      _eol_camera.position.z = _eol_camera.followHeight;
      break;
    case eolCamera2D:
      _eol_camera.update = eol_camera_2d_update;
      _eol_camera.position.z = _eol_camera.followHeight;
      break;
    case eolCameraThird:
      _eol_camera.update = eol_camera_third_update;
      break;
    case eolCameraFirst:
      _eol_camera.update = eol_camera_first_update;
      break;
  }
  eol_logger_message(EOL_LOG_INFO,"eol_camera:initialized\n");
}

void eol_camera_editor_update(eolCamera *cam, void *data)
{
  eolInt x,y;
  /*check input*/
  if (!cam)return;
  if (eol_mouse_input_held(eolMouseCenter))
  {
    eol_mouse_get_motion(&x, &y);
    cam->position.x += (-x * _eol_camera.moveStep);
    cam->position.y += (y * _eol_camera.moveStep);
  }
  if (eol_mouse_input_held(eolMouseLeft) && eol_mouse_input_held(eolMouseRight))
  {
    eol_mouse_get_motion(&x, &y);
    cam->position.z += (-y * _eol_camera.moveStep);
  }
}

void eol_camera_2d_update(eolCamera *cam, void *data)
{
  eolOrientation ori;
  if (_eol_camera.getOri == NULL)return;
  ori = _eol_camera.getOri(_eol_camera.target);
  _eol_camera.position.x = ori.position.x;
  _eol_camera.position.y = ori.position.y;
}

void eol_camera_third_update(eolCamera *cam, void *data)
{
}

void eol_camera_first_update(eolCamera *cam, void *data)
{
}

void eol_camera_update()
{
  if (_eol_camera.update == NULL)return;
  _eol_camera.update(&_eol_camera,NULL);
}

void eol_camera_setup()
{
  /*TODO: verify rotation*/
  glRotatef(-_eol_camera.rotation.x, 1.0f, 0.0f, 0.0f);
  glRotatef(-_eol_camera.rotation.y, 0.0f, 1.0f, 0.0f);
  glRotatef(-_eol_camera.rotation.z, 0.0f, 0.0f, 1.0f);
  glTranslatef(-_eol_camera.position.x,
               -_eol_camera.position.y,
               -_eol_camera.position.z);
}

void eol_camera_set_height(eolFloat dist)
{
  _eol_camera.position.z = _eol_camera.followHeight = dist;
}

void eol_camera_set_target_orientation(eolCameraGetTargetOri targetFunc)
{
  _eol_camera.getOri = targetFunc;
}

void eol_camera_set_target(void *target)
{
  _eol_camera.target = target;
}

eolUint eol_camera_get_type_by_string(char *camtype)
{
  if (!camtype)return 0;
  if (strcmp(camtype,"EDITOR") == 0)
    return eolCameraEditor;
  if (strcmp(camtype,"2D") == 0)
    return eolCamera2D;
  if (strcmp(camtype,"THIRD") == 0)
    return eolCameraThird;
  if (strcmp(camtype,"FIRST") == 0)
    return eolCameraFirst;
  if (strcmp(camtype,"CUSTOM") == 0)
    return eolCameraCustom;
  return 0;
}

/*eol@eof*/
