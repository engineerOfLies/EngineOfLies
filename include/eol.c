#include "eol.h"
#include "eol_logger.h"
#include "eol_graphics.h"
#include "eol_font.h"
#include "eol_sprite.h"
#include "eol_input.h"
#include "eol_config.h"
#include "eol_mouse.h"
#include "eol_lighting.h"
#include "eol_mesh.h"
#include "eol_armature.h"
#include "eol_loader.h"
#include "eol_model.h"
#include "eol_particle.h"
#include "eol_window.h"
#include <glib.h>
eolUint             _eol_major_version = 0;
eolUint             _eol_minor_version = 1;

void eol_get_version(eolUint *major,eolUint *minor)
{
	if (major != NULL)*major = _eol_major_version;
	if (minor != NULL)*minor = _eol_minor_version;
}

/*TODO: need to check dependencies and either automaticalli include them or report error*/
void eol_init(eolUint components)
{
  eol_logger_init();
  eol_config_init();
  eol_loader_init();
  if (components & EOL_GRAPHICS)
  {
    eol_graphics_config();
    eol_graphics_init();
    if (components & EOL_FONT)
    {
      eol_font_config();
      eol_font_init();
    }

    if (components & EOL_LIGHTING)
    {
      eol_lighting_config();
      eol_lighting_init();
    }

    if (components & EOL_MODEL)
    {
      eol_model_config();
      eol_model_init();
    }
    /*sprite is dependant on graphics*/
    if (components & EOL_SPRITE)
    {
    	eol_sprite_load_config();
    	eol_sprite_init();
      if (components & EOL_MESH)
      {
      	eol_mesh_load_config();
      	eol_mesh_init();
      }
      if (components & EOL_ARMATURE)
      {
      	eol_armature_load_config();
      	eol_armature_init();
      }
    }
    if (components & EOL_PARTICLE)
    {
      eol_particle_config();
      eol_particle_init();
    }
    if (components & EOL_WINDOW)
    {
      eol_window_config();
      eol_window_init();
    }
  }
  if (components & EOL_INPUT)
  {
    eol_mouse_config();
    eol_mouse_init();
    eol_input_load_config();
    eol_input_init();
  }
}


/*eol@eof*/

