#ifndef __EOL_WINDOW__
#define __EOL_WINDOW__
/*
Copyright 2012 EngineerOfLies
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
#include "eol_component.h"
#include <glib/glist.h>

typedef void (*eolWindowCallback)(void *data);

typedef struct eolWindow_S
{
  eolLine             name;          /**<name of the window*/
  eolUint             id;            /**<number ID of the window*/
  eolRect             rect;          /**<rectangle bounds of the window*/
  eolBool             passesInput;   /**<if false, prevent input from hitting windows below this one*/
  eolBool             canHasFocus;   /**<if the window can receive focus*/
  eolBool             hasFocus;      /**<if the window HAS focus*/
  eolBool             drawGeneric;   /**<if the window uses the stock draw*/
  eolUint             componentCount;/**<how many components this window has*/
  GList             * components;    /**<list of window components*/
  eolWindowCallback * callbacks;     /**<callback functions*/
  eolUint             callbackCount; /**<number of callbacks allocated*/
  void              * customData;    /**<window specific data*/
  void                (*custom_delete)(void *data);
                      /**<function to call to delete the custom data*/
  void                (*draw)(struct eolWindow_S *self);
                      /**<custom window draw function*/
  eolBool             (*update)(struct eolWindow_S *self,GList *updates);
                      /**<function to handle updates*/
}eolWindow;

void eol_window_init();
void eol_window_config();
void eol_window_clean();
void eol_window_close();
eolBool eol_window_initialized();

void eol_window_draw_all();
void eol_window_update_all();

eolWindow * eol_window_load_from_file(eolLine file);
eolWindow *eol_window_new();
void eol_window_free(eolWindow **win);
void eol_window_free_if_outside_click(eolWindow **win);
void eol_window_add_component(eolWindow *win,eolComponent *comp);
eolComponent *eol_window_get_component_by_id(eolWindow *win,eolUint id);
void eol_window_allocat_callbacks(eolWindow *win,eolUint count);

eolFloat eol_window_get_relative_position(eolInt position,eolUint range);
eolUint eol_window_get_refcount(eolWindow * window);

#endif
