#include "eol.h"
#include "eol_logger.h"
#include "eol_graphics.h"
#include "eol_font.h"
#include "eol_sprite.h"
#include "eol_input.h"
#include "eol_mesh.h"
#include "eol_armature.h"
#include "eol_loader.h"

eolUint             _eol_major_version = 0;
eolUint             _eol_minor_version = 1;

void eol_get_version(eolUint *major,eolUint *minor)
{
	if (major != NULL)*major = _eol_major_version;
	if (minor != NULL)*minor = _eol_minor_version;
}

void eol_init(eolUint components)
{
  eol_logger_init();
  eol_loader_init();
  if (components & EOL_GRAPHICS)
  {
    eol_graphics_load_config();
    eol_graphics_init();
    if (components & EOL_FONT)
    {
      eol_font_config();
      eol_font_init();
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
  }
  if (components & EOL_INPUT)
  {
  	eol_input_load_config();
    eol_input_init();
  }
}


/*eol@eof*/

