#include "test_entity.h"

#include <eol.h>
#include <eol_graphics.h>
#include <eol_sprite.h>
#include <eol_input.h>
#include <eol_font.h>
#include <eol_window.h>
#include <eol_mouse.h>
#include <eol_dialog.h>
#include <eol_particle.h>
#include <eol_lighting.h>
#include <eol_camera.h>
#include <eol_entity.h>
#include <string.h>
#include <stdio.h>

eolSprite *sprite = NULL;
eolActor *actor = NULL;
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
  eol_camera_init();
  done = 0;
  actor = eol_actor_new();
  if (actor)
  {
    eol_actor_load(actor,"models/bruiser.actor");
    eol_actor_set_action(actor,"idle");
  }
  sprite = eol_sprite_load("images/skeleton.png",128,128);
  MakeTestWindow();
  eol_mouse_show();
  light = eol_light_area_new();
  eol_light_set_color(light,eol_vec3d(0.5,0.5,0.5));
  eol_light_set_radius(light,radius);
  //  eol_lighting_setup_rep_plot();
  srandom(SDL_GetTicks());
  do
  {
    eol_light_move(light,point);
    eol_light_update(light);

    eol_input_update();
    eol_mouse_update();
    eol_entity_presync_all();
    /*TODO: run physics... right here*/
    eol_entity_postsync_all();
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
      (eol_input_is_key_pressed(SDLK_ESCAPE)))
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
}

void TestWindowDraw(eolWindow *win)
{
  TestData *data = (TestData *)win->customData;
  eol_sprite_draw(sprite,(eolInt)data->frame,100,100);

  glPushMatrix();

  eol_camera_setup();

  eol_entity_draw_all();
  eol_actor_draw(
    actor,
    eol_vec3d(0,0,0),
    eol_vec3d(-90,0,0/*data->rot*/),
    eol_vec3d(.5,.5,.5),
    eol_vec3d(1,1,1),
    0.5
  );

  eol_actor_next_frame(actor);
  eol_particle_draw_all();

  glPopMatrix();
  
  data->rot = data->rot + 0.25;
  if (data->rot > 360)data->rot -= 360;
  if(data->frame >= 14.0)data->frame = 2.0;
  else data->frame+=0.3;
}

void TestWindowUpdate(eolWindow *win,GList *updates)
{
  int i;
  GList *c;
  eolComponent *comp = NULL;
  eolComponent *labelComp = NULL;
  if ((win == NULL)||(updates == NULL))return;
  
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
        break;
      case 3:
        spawnTestEnt(eol_vec3d(crandom() * 3,crandom() * 3,0));
        break;
    }
  }
}

void MakeTestWindow()
{
  eolFloat lineHeight;
  eolWindow *win = eol_window_new();
  eolComponent *comp = NULL;
  if (!win)
  {
    fprintf(stdout,"OH NO IT FAILED TO CREATE!\n");
    return;
  }
  strncpy(win->name,"window",EOLLINELEN);
  win->id = 1;
  eol_rect_copy(&win->rect,eol_rect(32,32,640 - 64,480 - 64));
  win->canHasFocus = eolTrue;
  win->hasFocus = eolTrue;
  win->drawGeneric = eolTrue;
  win->componentCount = 0;
  win->components = NULL;
  win->customData = NULL;
  win->custom_delete = NULL;
  win->draw = NULL;
  win->update = TestWindowUpdate;
  win->draw = TestWindowDraw;
  win->customData = malloc(sizeof(TestData));
  memset(win->customData,0,sizeof(TestData));
  comp = eol_label_new(
    0,
    "main_label",
    eol_rectf(0.25,0.1,1,1),
    win->rect,
    eolTrue,
    "This is a test label",
    eolJustifyLeft,
    eolFalse,
    3,
    NULL,
    eol_vec3d(1,1,1),
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
    eol_rectf(0.2,0.8,1,1),
    win->rect,
    "Test Entity",
    0,
    eolFalse
  );
  eol_window_add_component(win,comp);

}

/*eol @ eof*/
