#ifndef __EOL_COMPONENT_CHECK__
#define __EOL_COMPONENT_CHECK__
/*
    Copyright 2013 Engineer of Lies
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

#include "eol_sprite.h"
#include "eol_component.h"
#include "eol_config.h"
#include "eol_types.h"
#include <glib/glist.h>

/*
  component
  {(button)     label}
  {(button)label     }
  {label(button)     }
  {label     (button)}
  
 */


typedef struct
{
  eolComponent *checkButton;    /**<the button to show when checked*/
  eolComponent *uncheckButton;  /**<the button to show when unchecked*/  
  eolComponent *label;          /**<the Label Component*/
  eolBool       drawLabel;      /**<if true, draw the label, don't otherwise*/
  eolBool       leftAlign;      /**<if left justified, button will be placed at the left end of area*/
  eolBool       packed;         /**<if packed, extra space is ignored and label and button will touch*/
  eolInt        padding;        /**<how many pixels minimum should between button and label*/
  eolLine       checkText;      /**<text to display while checked*/
  eolLine       uncheckText;    /**<text to display while unchecked*/
  eolInt        state;          /**<checked or unchecked*/
}eolComponentCheck;

void eol_check_configure(eolConfig *conf);
eolInt eol_check_get_state_by_line(eolLine stateText);

#endif
