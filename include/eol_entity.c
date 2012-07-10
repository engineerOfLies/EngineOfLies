#include "eol_entity.h"
#include "eol_resource.h"


eolUint              _eol_entity_custom_data_size = 0;
eolBool              _eol_entity_initialized = eolFalse;
eolResourceManager * _eol_entity_manager = NULL;
eolEntity *          _eol_entity_list = NULL;
eolUint              _eol_entity_max = 0; /**<maximum number of entitys that can be loaded*/

/*eol@eof*/
