#ifndef __EOL_COMPONENTS__
#define __EOL_COMPONENTS__
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

/**
 * @purpose This will create the components used for window construction.
    Similar in concept to a gtkWidget, but severely limited to the components
    needed in a game
 */

enum eolComponentTypes {
  eolNullComponent   = 0,  /**<this type is set by default*/
  eolLabelComponent  = 1,
  eolButtonComponent = 2,
  eolEntryComponent  = 3,
  eolSliderComponent = 4,
  eolImageComponent  = 5,
  eolActorComponent  = 6,
  eolListComponent   = 7
};

enum eolButtonTypes {
  eolButtonStock  = 0,
  eolButtonText   = 1,
  eolButtonHidden = 2,
  eolButtonCustom = 3
};
enum eolButtonStates {
  eolButtonIdle       = 0,
  eolButtonHighlight  = 1,
  eolButtonPressed    = 2
};
#define eolButtonStateMax 4

enum eolListTypes {
  eolListLines  = 0, /**<items are drawn from top to bottom fit within bounding
                         rect*/
  eolListBlock  = 1, /**<items are drawn left to right, then top to bottom fit
                         within bounding rect*/
  eolListDock   = 2  /**<items are drawn left to right.  fit within bounding rect*/
};


/**
  @brief this structure serves as a header for all components
 */
typedef struct eolComponent_S
{
  eolUint       id;
  eolWord       name;
  eolRectFloat  rect;
  eolRect       bounds;
  eolBool       canHasFocus;    /**<I apologize for the lolcat reference*/
  eolBool       hasFocus;
  eolInt        state;
  eolInt        oldState;
  eolUint       type;
  void        * componentData;
  void          (*data_free)(struct eolComponent_S *component);
  void          (*data_draw)(struct eolComponent_S *component,eolRect bounds);
  eolBool       (*data_update)(struct eolComponent_S *component);
  eolInt        (*data_get_state)(struct eolComponent_S *component);
  eolBool       (*data_changed)(struct eolComponent_S *component);
}eolComponent;

/*@brief loads default component assets.*/
void eol_component_config();

eolComponent * eol_component_new();
void eol_component_free(eolComponent **component);
eolBool eol_component_update(eolComponent *component);
void eol_component_set_focus(eolComponent *component,eolBool focus);
eolBool eol_component_has_changed(eolComponent *component);
eolInt eol_component_get_state(eolComponent *component);
void eol_component_draw(eolComponent *component,eolRect bounds);

/**
 * @brief checks if a component has changed since last frame
 *
 * @param component the component to check
 * @return eolTrue if the component has changed, eolFalse otherwise
 */
eolBool eol_component_changed(eolComponent *component);

void eol_label_set_text(eolComponent *comp,char *text);

void eol_button_get_stock_size(eolUint *w, eolUint *h);

eolComponent *eol_label_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    eolBool        canHasFocus,
    char         * text,
    eolUint        justify,
    eolBool        wordWrap,
    eolInt         fontSize,
    char         * fontName,
    eolVec3D       color,
    eolFloat       alpha
  );

eolComponent *eol_button_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    char         * buttonText,
    eolInt         buttonType,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolBool        center,
    char         * buttonFileUp,
    char         * buttonFileHigh,
    char         * buttonFileDown
  );

eolComponent *eol_button_stock_new(
    eolUint        id,
    eolWord        name,
    eolRectFloat   rect,
    eolRect        bounds,
    char         * buttonText,
    eolInt         buttonHotkey,
    eolUint        buttonHotkeymod,
    eolBool        center
  );

void eol_entry_delete_char(eolComponent *component);
void eol_entry_append_char(eolComponent *component,
                           char          newchar);
                           
void eol_entry_assign_output(eolComponent *component);

eolComponent *eol_entry_new(
    eolUint       id,
    eolWord       name,
    eolRectFloat  rect,
    eolRect       bounds,
    char        * output,
    eolInt        outputLimit,
    eolUint       justify,
    eolBool       wordWrap,
    eolUint       fontSize,
    eolLine       fontName,
    eolBool       number,
    eolVec3D      color,
    eolFloat      alpha,
    eolVec3D      bgcolor
);

eolComponent *eol_line_entry_new(
    eolUint       id,
    eolWord       name,
    eolRectFloat  rect,
    eolRect       bounds,
    eolLine       output
);

#endif
