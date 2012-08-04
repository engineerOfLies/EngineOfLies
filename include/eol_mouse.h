#ifndef __EOL_MOUSE__
#define __EOL_MOUSE__
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

enum eolMouseStates {
  eolMouseLeft       = 0,
  eolMouseCenter     = 1,
  eolMouseRight      = 2,
  eolMouseWheelUp    = 3,
  eolMouseWheelDown  = 4
};

/**
* @brief Initializes internal mouse data based on loaded config.
*/
void eol_mouse_init();

/**
* @brief Loads mouse configs or sets up defaults
*/
void eol_mouse_config();

/**
* @brief draws the mouse to the screen based on current states
*/
void eol_mouse_draw();

/**
* @brief Stops drawing the mouse
*/
void eol_mouse_hide();

/**
* @brief Starts drawing the mouse
*/
void eol_mouse_show();

/**
  * @brief resests the show/hiding of a mouse.  Sets it to hiden.
  * Useful when moving into a new context
  */
void eol_mouse_reset();

/**
 * @brief updates the internal mouse information.  Relies on global input update.
 */
void eol_mouse_update();

/**
 * @brief returns the state of the mouse button specified
 *
 * @param button which mouse button to check
 * @return true if the button is in the pressed state.
*/
eolBool eol_mouse_input_state(eolUint button);

/**
* @brief checks if the mouse button is pressed this frame
*
* @param button which mouse button to check
* @return true if the button was pressed this frame
*/
eolBool eol_mouse_input_pressed(eolUint button);

/**
* @brief checks if the mouse button is released this frame
*
* @param button which mouse button to check
* @return true if the button was released
*/
eolBool eol_mouse_input_released(eolUint button);

/**
* @brief checks if the mouse button is held
*
* @param button which mouse button to check
* @return true if the button is held
*/
eolBool eol_mouse_input_held(eolUint button);

/**
* @brief checks if the mouse button was double-tapped
*
* @param button which mouse button to check
* @return true if the button was double tapped
*/
eolBool eol_mouse_input_double_tapped(eolUint button);

/**
 * @brief gets the current mouse position
 *
 * @param x putput pointer to where the X position can be written to.
 * @param y putput pointer to where the Y position can be written to.
 */
void eol_mouse_get_position(eolInt *x, eolInt *y);

/**
* @brief gets the change in mouse position since the last input frame
*
* @param x putput pointer to where the change in X position can be written to.
* @param y putput pointer to where the change in Y position can be written to.
*/
void eol_mouse_get_motion(eolInt *dx, eolInt *dy);

/**
* @brief checks if the mouse is within the rect specified.
*
* @param rect the bounding rect to check for.
* @return eolTrue if the mouse is in the rect, eolFalse otherwise.
*/
eolBool eol_mouse_in_rect(eolRect rect);

#endif
