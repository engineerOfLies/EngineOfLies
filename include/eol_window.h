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
  eolBool             hidden;        /**<hidden windows do not get drawn*/
  eolBool             sleeping;      /**<sleeping windows do not update*/
  eolBool             passesInput;   /**<if false, prevent input from hitting windows below this one*/
  eolBool             canHasFocus;   /**<if the window can receive focus*/
  eolBool             hasFocus;      /**<if the window HAS focus*/
  eolUint             inputFocusNext;/**<what input steps through to next focus component*/
  eolUint             modFocusNext;  /**<what input steps through to next focus component*/
  eolUint             inputFocusPrev;/**<what input steps through to previous focus component*/
  eolUint             modFocusPrev;  /**<what input steps through to next focus component*/
  eolComponent      * focusComponent;/**<if defined, points to the component that has focus*/
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

/**
 * @brief initializes the window system
 */

void eol_window_init();

/**
 * @brief configures the window system defaults
 */
void eol_window_config();

/**
 * @brief frees unused window resources
 */
void eol_window_clean();

/**
 * @brief closes the window system.  Called automatically at exit
 */
void eol_window_close();

/**
 * @brief checks window system initialization status
 * @return eolTrue if initialized, eolFalse otherwise
 */
eolBool eol_window_initialized();

/**
 * @brief Draws all visible windows to screen.  Should be called every frame in main loop.
 */
void eol_window_draw_all();

/**
 * @brief Updates all active windows.  Should be called every frame.
 */
void eol_window_update_all();

/**
 * @brief Loads a window from config file and moves it to the position specified
 * @param file the window .def file to load
 * @param position the screen position for the upper left corner of the window
 * @return a pointer to the newly created window or NULL on error.
 */
eolWindow * eol_window_load_from_file_to(eolLine file,eolVec2D position);

/**
 * @brief Loads a window from config file
 * @param file the window .def file to load
 * @return a pointer to the newly created window or NULL on error.
 */
eolWindow * eol_window_load_from_file(eolLine file);

/**
 * @brief creates an initialized window.  Must be provided with data before it is used.
 * @return a pointer to the newly created window or NULL on error.
 */
eolWindow * eol_window_new();

/**
 * @brief moves the specified window to new coordinates
 * @param win the window to move
 * @param position the new position for the window
 */
void eol_window_move_to(eolWindow *win,eolVec2D position);

/**
 * @brief let the window system know you are done with the window.  Set your pointer to the window to NULL.
 * @param win a pointer to your window pointer.  Set to NULL after successfully freeing the window
 */
void eol_window_free(eolWindow **win);

/**
 * @brief automatically frees the window provided if the mouse has clicked outside of the window frame
 * @param win a pointer to your window pointer.  If freed, it will be set to NULL.
 */
void eol_window_free_if_outside_click(eolWindow **win);

/**
 * @brief check the mouse has been clicked outside the window
 * @param win the window context to check
 * @return eolTrue if the mouse has been clicked outside the window,
 *         eolFalse on error or otherwise
 */
eolBool eol_window_mouse_outside(eolWindow *win);


/**
 * @brief check the mouse has been clicked inside the window
 * @param win the window context to check
 * @return eolTrue if the mouse has been clicked inside the window,
 *         eolFalse on error or otherwise
 */
eolBool eol_window_mouse_inside(eolWindow *win);

/**
 * @brief Adds a component to the window.  Component should already be set up.
 * @param win the window to get the new component
 * @param comp the component to add to the window
 */
void eol_window_add_component(eolWindow *win,eolComponent *comp);

/**
 * @brief returns a pointer to the window component with the id provided
 * @param win the window to search through
 * @param id the id, as provided to the component.  Should be unique per window.
 * @return a pointer to the first (in the case of multiple) component with the
 *         provided id, or NULL if not found.
 */
eolComponent *eol_window_get_component_by_id(eolWindow *win,eolUint id);

/**
 * @brief returns a pointer to the window component with the name provided
 * @param win the window to search through
 * @param name the name, as provided to the component.  Should be unique per window.
 * @return a pointer to the first (in the case of multiple) component with the
 *         provided name, or NULL if not found.
 */
eolComponent *eol_window_get_component_by_name(eolWindow *win,eolLine name);

/**
 * @brief Allocated window callbacks for the window
 * @param win the window to get more callback options
 * @param count the number of callbacks to allocate.
 * Note: if the allocation failed, see the log for errors.
 */
void eol_window_allocat_callbacks(eolWindow *win,eolUint count);

eolFloat eol_window_get_relative_position(eolInt position,eolUint range);

/**
 * @brief get the number of references to the window
 * @param window the window to check for references of
 * @return the number of references to the provided window
 */
eolUint eol_window_get_refcount(eolWindow * window);

/**
 * @brief hides a window (prevents it from being drawn
 */
void eol_window_hide(eolWindow *win);

/**
 * @brief marks a window and allows it to be drawn
 */
void eol_window_show(eolWindow *win);

/**
 * @brief puts a window to "sleep" preventing updates
 */
void eol_window_sleep(eolWindow *win);

/**
 * @brief "wakes" a window and resumes updates
 */
void eol_window_wakeup(eolWindow *win);
#endif
