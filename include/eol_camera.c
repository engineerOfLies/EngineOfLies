#include "eol_camera.h"
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
  memset(&_eol_camera,0,sizeof(eolCamera));
  _eol_camera.cameratype = eolCamera2D;
  _eol_camera.followHeight = 10;
}

void eol_camera_init()
{
  eolGraphicsView view;
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
}

void eol_camera_editor_update(eolCamera *cam, void *data)
{
  /*check input*/
}

void eol_camera_2d_update(eolCamera *cam, void *data)
{
  eolOrientation ori;
  if ((_eol_camera.getOri == NULL) ||
      (_eol_camera.target == NULL))return;
  ori = _eol_camera.getOri(_eol_camera.target);
}

void eol_camera_third_update(eolCamera *cam, void *data)
{
}

void eol_camera_first_update(eolCamera *cam, void *data)
{
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

/*eol@eof*/
