#include "eol.h"
#include "eol_graphics.h"
#include "eol_sprite.h"
#include "eol_input.h"
#include "eol_font.h"
#include "eol_actor.h"
#include "eol_window.h"
#include <string.h>
#include <stdio.h>

void Init_All(const char *argv);

int main(int argc, char *argv[])
{
  int done;
  int i;
  float rot = 0;
  eolLine fps;
  eolFloat frame = 0;
  eolSprite *sprite = NULL;
  eolActor *actor = NULL;
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
  done = 0;
  actor = eol_actor_new();
  if (actor)eol_actor_load(actor,"models/bruiser.actor");
  sprite = eol_sprite_load("images/skeleton.png",128,128);

  do
  {
    eol_input_update();
  	eol_graphics_frame_begin();
  	sprintf(fps,"FPS: %f",eol_graphics_get_FPS());
    eol_font_draw_text_justify(
      fps,
      640,
      0,
      eol_vec3d(1,0,0),
      1,
      2,
      eolJustifyRight);

  	eol_sprite_draw(sprite,(eolInt)frame,100,100);
  
    eol_actor_draw(
      actor,
      eol_vec3d(0,0,-10),
      eol_vec3d(-90,0,rot),
      eol_vec3d(1,1,1),
      eol_vec3d(1,1,1),
      1
    );
    eol_window_draw_all();
    eol_font_draw_text(
      "TESTING",
      300,
      100,
      eol_vec3d(rot/360,1,1),
      1,
      3);
    rot = rot + 0.25;
    if (rot > 360)rot -= 360;
    eol_graphics_frame_end();
  	if(frame >= 14.0)frame = 2.0;
  	else frame+=0.3;
    if((eol_input_quit_check()) ||
      (eol_input_is_key_pressed(SDLK_ESCAPE)))done = 1;
  }while(!done);
  	fprintf(stdout,"Main Loop exited\n");
  exit(0);
  return 0;
}

void Init_All(const char *argv)
{
	eol_init(EOL_ALL);
}

/*eol @ eof*/

