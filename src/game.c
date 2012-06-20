#include "eol.h"
#include "eol_graphics.h"
#include "eol_sprite.h"
#include "eol_input.h"
#include "eol_mesh.h"
#include "eol_armature.h"
#include "eol_font.h"
#include "eol_actor.h"
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
  eolMesh *mesh = NULL;
  eolSprite *skin = NULL;
  eolArmature *arm = NULL;
  eolMesh *mesh2 = NULL;
  eolSprite *skin2 = NULL;
  eolArmature *arm2 = NULL;
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
  actor = eol_actor_load("models/bruiser.actor");
  sprite = eol_sprite_load("images/skeleton.png",128,128);
  mesh = eol_mesh_load("models/bruiser/model.obj");
  skin = eol_sprite_load("models/bruiser/skin1.png",-1,-1);
  arm = eol_armature_load("models/bruiser/model.obj");
  mesh2 = eol_mesh_load("models/testmodel/whitecell.obj");
  skin2 = eol_sprite_load("models/testmodel/whitecell.png",-1,-1);
  eol_armature_link_mesh(arm,mesh);
  eol_armature_link_mesh(arm2,mesh2);
  do
  {
    eol_input_update();
  	eol_graphics_frame_begin();
  	sprintf(fps,"FPS: %f",eol_graphics_get_FPS());
    eol_font_draw_text_rj(
      fps,
      640,
      0,
      eol_vec3d(1,0,0),
      1,
      2);

  	eol_sprite_draw(sprite,(eolInt)frame,100,100);
  
//    eol_armature_deform_mesh(arm2, mesh2,frame);
  	eol_mesh_draw(
        mesh2,
        eol_vec3d(2, 0, -10),
        eol_vec3d(-90, 0, rot+=1),
        eol_vec3d(0.3,0.3,0.3),
        eol_vec3d(1,1,1),
        1,
        skin2
      );

    eol_armature_deform_mesh(arm, mesh,frame);
  	eol_mesh_draw(
        mesh,
        eol_vec3d(-2, 0, -10),
        eol_vec3d(-90, 0, rot),
        eol_vec3d(1,1,1),
        eol_vec3d(1, 1, 1),
        1,
        skin
      );

    eol_font_draw_text(
      "TESTING",
      300,
      100,
      eol_vec3d(rot/360,1,1),
      1,
      3);


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

