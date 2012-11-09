#ifndef __EOL_MESSAGE___
#define __EOL_MESSAGE__
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

#include "eol_window.h"
#include "eol_types.h"

/**
 * @purpose The EOL Message Window is meant as  a singleton.  The window in any game view
 * where the user will get updates about events in the game. "Job Done" "Attack Missed"
 * or possibly as text updates from other players
 */

/**
 * @brief set up message window based on config or uses defaults
 */
void eol_message_setup();

/**
 * @brief moves the message window to a new screen position
 * @param position the new position to draw the window at
 */
void eol_message_move(eolVec2D position);

/**
 * @brief Sets the message window to shown
 */
void eol_message_show();

/**
 * @brief stops the message window from drawing
 */
void eol_message_hide();

/**
 * @brief clears the buffered messages from the window
 */
void eol_message_clear();

/**
 * @brief sets how many messages will be drawn.
 * @param newLimit how many messages to show.  0 is nop.
 */
void eol_message_set_limit(eolUint newLimit);

/**
 * @brief sets how long (in milliseconds) a message lasts before fadeout
 * @param newLimit how many messages to show.  0 is nop.
 */
void eol_message_set_timeout(eolUint timeout);

/**
 * @brief sets callbacks to be invoked during the message window draw/update
 * @param draw (can be NULL) if set, message calls the function provided during draw
 * @param update (can be NULL) if set, message calls the function provided during update
 */
void eol_message_set_window_callbacks(void (*draw)(eolWindow *self),
                                      eolBool (*update)(eolWindow *self,GList *updates)
                                      );

/**
 * @brief Adds a new message to the message window using default settings
 * @param message the message to add
 */
void eol_message_new(eolLine message);

/**
 * @brief Adds a new message to the message window using provided settings
 * @param message the message to add
 * @param font (optional)font to use.  Pass "" for default font
 * @param fontSize which stock font to use
 * @param color the color of the text line
 * @param alpha the transparency of the text line added
 */
void eol_message_new_custom(eolLine message,
                            eolLine font,
                            eolUint fontSize,
                            eolVec3D color,
                            eolFloat alpha);


#endif
