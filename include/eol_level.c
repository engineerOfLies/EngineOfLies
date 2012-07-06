#include "eol_level.h"
#include "eol_resource.h"

/*local global variables*/
eolBool     _eol_level_initialized = eolFalse;
eolResourceManager * _eol_level_manager = NULL;
eolLevel * _eol_level_list = NULL;
eolLevel * _eol_level_current = NULL;
eolUint    _eol_level_max_layers = 1;
eolUint    _eol_level_max = 0; /**<maximum number of levels that can be loaded
                                   at a time, ie: buffered*/


/*local function prototypes*/

/*function definitions*/
void eol_level_init()
{
  cpInitChipmunk();
}

/*eol@eof*/
