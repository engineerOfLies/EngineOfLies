#ifndef __EOL_CONFIG__
#define __EOL_CONFIG__
/*
    Copyright 2012 Engineer of Lies
    This file is part of the Engine of Lies game engine library

    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "eol_types.h"
#include "eol_keychain.h"
#include <yaml.h>


typedef struct
{
  eolLine       filename;
  eolUint       elementCount;
  eolKeychain * _node;
} eolConfig;

/* Variable, Value, Sequence (vector) */
enum eolConfigState { KEY, VAL, SEQ };

/**
 * @brief initializes config system
 */
void eol_config_init();

/**
 * @brief checks initalization status of config
 */
eolBool eol_config_initialized();

/**
 * @brief allocates and initializes an empy config.
 * @return NULL on error, or a valid pointer to a config.
 */
eolConfig *eol_config_new();

/**
 * @brief loads a configuration file into a eolConfig struct
 *
 * @param filename the config file to load.
 * @return a eolConfig pointer with the config structure
 */
eolConfig *eol_config_load(char* filename);

/**
 * @brief returns the config back to the system.
 *
 * @param config a pointer to your config pointer.  Set to NULL upon completion
 */
void eol_config_free(eolConfig **config);

/**
 * @brief gets the element by tag as a keychain.
 * @param output a pointer to a keychain pointer.  Set to the key if found. Null otherwise.
 * @param conf a pointer to your config.
 * @param tag the key to search for.
 * @return eolTrue if found, eolFalse otherwise
 */
eolBool eol_config_get_keychain_by_tag(eolKeychain **output,
                                       eolConfig *conf,
                                       eolLine tag);

eolBool eol_config_get_vec3d_by_tag(
  eolVec3D  *output,
  eolConfig *conf,
  eolLine    tag
);

eolBool eol_config_get_int_by_tag(
  eolInt    *output,
  eolConfig *conf,
  eolLine    tag
);

eolBool eol_config_get_float_by_tag(eolFloat *output, eolConfig *conf, eolLine tag);
eolBool eol_config_get_uint_by_tag(eolUint *output, eolConfig *conf, eolLine tag);
eolBool eol_config_get_bool_by_tag(eolBool *output, eolConfig *conf, eolLine tag);

eolBool eol_config_get_line_by_tag(
  eolLine     output,
  eolConfig * conf,
  eolLine     tag
);

eolBool eol_config_get_list_count_by_tag(
  eolUint    *output,
  eolConfig  *conf,
  eolLine     tag
);

/*TODO; THIS*/
eolBool eol_config_get_list_by_tag(
  eolKeychain ** output,
  eolConfig   * conf,
  eolLine       tag
);

eolBool eol_config_get_rectfloat_by_tag(eolRectFloat *output,
                                        eolConfig *conf,
                                        eolLine tag);

eolConfig *eol_config_load_binary(char* filename);
void eol_config_save_binary(eolConfig *conf, char* filename);


#endif

