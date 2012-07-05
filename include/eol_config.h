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
#include <glib.h>
#include <yaml.h>


typedef struct
{
  eolLine   filename;
  eolUint   elementCount;
  GNode   * _node;
}eolConfig;

/* Variable, Value, Sequence (vector) */
enum eolConfigState { VAR, VAL, SEQ };


/**
 * @brief initializes config system
 */
void eol_config_init();

/**
 * @brief loads a configuration file into a eolConfig struct
 *
 * @return a eolConfig pointer with the config structure
 */
eolConfig *eol_config_load(char* filename);

void eol_config_destroy(eolConfig *config);

/**
 * @brief writes config from a GNode object into a file
 *
 */
void eol_config_dump(char* filename, GNode* data);

/* it would be nice to be able to access your library like this: */


eolInt eol_config_get_int_by_tag(
    eolConfig *conf,
    eolLine    tag
  );

void eol_config_get_line_by_tag(
    eolLine     output,
    eolConfig * conf,
    eolLine     tag
  );

eolUint eol_config_get_list_count_by_tag(
    eolConfig * conf,
    eolLine     tag
  );

GList * eol_config_get_list_by_tag(
    eolConfig * conf,
    eolLine     tag
  );

#endif

