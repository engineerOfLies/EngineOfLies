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
#include <glib/glist.h>

typedef struct eolWindow_S
{
  eolLine   name;
  eolUint   id;
  eolRect   rect;
  eolBool   canHasFocus;
  eolBool   hasFocus;
  eolBool   drawGeneric;
  eolUint   componentCount;
  GList   * components;
  void    * customData;
  void      (*custom_delete)(void *data);
  void      (*draw)(struct eolWindow_S *self);
  int       (*update)(struct eolWindow_S *self);
}eolWindow;

void eol_window_init();
void eol_window_config();
void eol_window_clean();
void eol_window_close();
eolBool eol_window_initialized();

void eol_window_draw_all();
void eol_window_update_all();

eolWindow *eol_window_new();
void eol_window_free(eolWindow **win);

#endif
