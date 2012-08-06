#include "test_entity.h"
#include "test_level.h"

#include <eol.h>
#include <eol_input.h>
#include <eol_font.h>
#include <eol_window.h>
#include <eol_mouse.h>
#include <eol_dialog.h>
#include <eol_particle.h>
#include <eol_lighting.h>
#include <eol_camera.h>
#include <eol_entity.h>
#include <eol_level.h>
#include <string.h>
#include <stdio.h>

eolMesh *level = NULL;
eolSprite *levelskin = NULL;
eolMesh *levelclip = NULL;

typedef struct
{
  eolFloat frame;
  eolFloat rot;
}TestData;

void Init_All(const char *argv);

void MakeTestWindow();

int main(int argc, char *argv[])
{
  int done;
  int i;
  int dir = 1;
  float radius = 0.00001;
  eolVec3D point = {0,0,-5};
  eolLine fps;
  eolLight *light = NULL;
  for(i = 1;i < argc;i++)
  {
    if(strcmp(argv[i],"-fs")== 0)
    {
    }
    if(strcmp(argv[i],"-wm")== 0)
    {
    }
  }
  Init_All(argv[0]);
  eol_camera_config();
  eol_camera_set_height(30);
  eol_camera_init();
  done = 0;
  MakeTestWindow();
  eol_mouse_show();
  light = eol_light_area_new();
  eol_light_set_color(light,eol_vec3d(0.5,0.5,0.5));
  eol_light_set_radius(light,radius);
  eol_lighting_setup_rep_plot();
  srandom(SDL_GetTicks());
  CreateTestLevel();
  do
  {
    eol_light_move(light,point);
    eol_light_update(light);

    eol_input_update();
    eol_mouse_update();

    eol_level_update_active();

    eol_entity_think_all();
    eol_entity_update_all();

    eol_particle_update_all();
    eol_window_update_all();

    eol_graphics_frame_begin();
    eol_window_draw_all();
    eol_light_draw(light);

    sprintf(fps,"FPS: %f",eol_graphics_get_FPS());
    eol_font_draw_text_justify(
      fps,
      640,
      0,
      eol_vec3d(1,0,0),
      1,
      2,
      eolJustifyRight);

    eol_mouse_draw();
    eol_graphics_frame_end();

    if (point.x > 5)dir = -1;
    if (point.x < -5)dir = 1;
    point.x += dir * 0.01;
    radius = radius + 0.0001;
    if (radius > 1)radius = 0;

    if((eol_input_quit_check()) ||
      ((eol_input_is_key_pressed(SDLK_F4)) && (eol_input_is_mod_held(KMOD_ALT))))
    {
      eol_dialog_quit();
    }
  }while(!done);
  exit(0);
  return 0;
}

void Init_All(const char *argv)
{
  eol_init(EOL_ALL);
  eol_entity_config();
  eol_entity_init();
  eol_level_config();
  eol_level_init();
}

void TestWindowDraw(eolWindow *win)
{
  TestData *data = (TestData *)win->customData;

  glPushMatrix();

  eol_camera_setup();

  eol_level_draw_current();
  
  eol_entity_draw_all();

  eol_particle_draw_all();

  glPopMatrix();
  
  data->rot = data->rot + 0.25;
  if (data->rot > 360)data->rot -= 360;
  if(data->frame >= 14.0)data->frame = 2.0;
  else data->frame+=0.3;
}

eolBool TestWindowUpdate(eolWindow *win,GList *updates)
{
  int i;
  GList *c;
  eolComponent *comp = NULL;
  eolComponent *labelComp = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    labelComp = eol_window_get_component_by_id(win,0);
    switch (comp->id)
    {
      case 0:
        break;
      case 1:
        for (i = 0; i < 100; i ++)
        {
        eol_particle_make_point(eol_vec3d(crandom()*0.1,0,0),
                                eol_vec3d(crandom()*0.1,crandom()*0.1 + 0.1,crandom()*0.1),
                                eol_vec3d(0,-0.009,0) ,
                                5,
                                eol_vec3d(0,1,random()),
                                1,
                                100);
        }
        return eolTrue;
      case 3:
        spawnTestEnt(eol_vec3d(0,0,0));
        return eolTrue;
    }
  }
  return eolFalse;
}

void MakeTestWindow()
{
  eolFloat lineHeight;
  eolWindow *win = eol_window_new();
  eolComponent *comp = NULL;
  eolUint w,h;
  if (!win)
  {
    fprintf(stdout,"OH NO IT FAILED TO CREATE!\n");
    return;
  }
  eol_graphics_get_size(&w, &h);
  strncpy(win->name,"window",EOLLINELEN);
  eol_rect_copy(&win->rect,eol_rect(0,0,w,h));
  win->canHasFocus = eolTrue;
  win->hasFocus = eolTrue;
  win->drawGeneric = eolFalse;
  win->componentCount = 0;
  win->components = NULL;
  win->customData = NULL;
  win->custom_delete = NULL;
  win->draw = NULL;
  win->update = TestWindowUpdate;
  win->draw = TestWindowDraw;
  win->customData = malloc(sizeof(TestData));
  memset(win->customData,0,sizeof(TestData));
  eol_camera_set_height(30);
  comp = eol_label_new(
    0,
    "main_label",
    eol_rectf(0.05,0.05,1,1),
    win->rect,
    eolTrue,
    "Test Bed for Engine Of Lies",
    eolJustifyLeft,
    eolFalse,
    3,
    NULL,
    eol_vec3d(0.2,1,0.2),
    1
  );
  eol_window_add_component(win,comp);
  
  comp = eol_button_stock_new(
    1,
    "test_button",
    eol_rectf(0.7,0.8,1,1),
    win->rect,
    "Test Particle",
    0,
    0,
    eolFalse
  );
  eol_window_add_component(win,comp);
  
  lineHeight = eol_font_get_text_height_average(3);
  lineHeight += 4;
  comp = eol_line_entry_new(
    2,
    "test_entry",
    eol_rectf(0.1,0.85,0.5,lineHeight/win->rect.h),
    win->rect,
    ""
  );
  eol_window_add_component(win,comp);

  comp = eol_button_stock_new(
    3,
    "test_button2",
    eol_rectf(0.1,0.7,1,1),
    win->rect,
    "Test Entity",
    0,
    0,
    eolFalse
  );
  eol_window_add_component(win,comp);

}

/*eol @ eof*/
