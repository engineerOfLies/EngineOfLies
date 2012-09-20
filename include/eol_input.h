#ifndef __EOL_CONTROLS__
#define __EOL_CONTROLS__
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
 * Definition of terms for eol_input:
 *
 *  - Down - state of the key when the input is active.
 *  - Up - state of the key when the input is inactive.
 *  - Pressed - state of a key or input when the last frame was not "down" and this
 * frame it is "down"
 *  - Released - state of an input wen the last frame was "down" and this frame is
 * "up"
 *  - Held - state of an input when this frame and the previous few frames have
 *  been "down".  Note that the threshold for this is configurable
 *  - Double Tap - state of an input when the key is rapidly tapped twice in
 *  succession.  Threshold is configurable.
 *
 */

/**
 * @brief structure for defining a control type.
 */
typedef struct
{
  eolLine  name;  /*name of the input*/
  eolWord  type;  /*input type: key, button, axis*/
  eolUint  id;      /*which key or axis or button*/
  eolFloat value;    /*current state of the input*/
  eolFloat oldvalue; /*last state of the input*/
  eolUint  timePress; /*this time index*/
  eolUint  lastPress; /*time index of the last time this input was pressed*/
}eolInput;

/**
 * @brief converts input config into sdl_key info
 *
 * @param type type of input: "mod" "key" "button", "+axis" "-axis"
 */
eolInt eol_input_parse(char *type, char *input);


/**
 * @brief initializes internal components of eol_input
 */
void eol_input_init();

/**
 * @brief returns the initialization status of eol_input
*
 * @return eolTrue if eol_input has been initialized, eolFalse otherwise.
 */
eolBool eol_input_is_initialized();

/**
 * @brief determines if the keyboard key was pressed this frame
 *
 * @param key the SDLKey value of the key of interest.  example: SDLK_a for the
 * a key.
 *
 * @return eolTrue if pressed, eolFalse otherwise
 */
eolBool eol_input_is_key_pressed(eolInt key);

/**
 * @brief determines if the keyboard key was released this frame
 *
 * @param key the SDLKey value of the key of interest.  example: SDLK_a for the
 * a key.
 *
 * @return eolTrue if released, eolFalse otherwise
 */
eolBool eol_input_is_key_released(eolInt key);

/**
 * @brief determines if the keyboard key is held during this frame
 *
 * @param key the SDLKey value of the key of interest.  example: SDLK_a for the
 * a key.
 *
 * @return eolTrue if held, eolFalse otherwise
 */
eolBool eol_input_is_key_held(eolInt key);

/**
 * @brief determines if the keyboard modifier key is held during this frame
 * Uses SDLMod enumerations, ex: KMOD_LSHIFT, KMOD_ALT, etc.
 * @param key the SDLMod value of the mod of interest.  
 *
 * @return eolTrue if held, eolFalse otherwise
 */
eolBool eol_input_is_mod_held(eolUint mod);

/**
 * @brief determines if the keyboard modifier key is down during this frame
 * Uses SDLMod enumerations, ex: KMOD_LSHIFT, KMOD_ALT, etc.
 * @param key the SDLMod value of the mod of interest.
 *
 * @return eolTrue if held, eolFalse otherwise
 */
eolBool eol_input_is_mod_down(eolUint mod);

/**
 * @brief determines if the keyboard modifier key is pressed during this frame
 * Uses SDLMod enumerations, ex: KMOD_LSHIFT, KMOD_ALT, etc.
 * @param key the SDLMod value of the mod of interest.  
 *
 * @return eolTrue if held, eolFalse otherwise
 */
eolBool eol_input_is_mod_pressed(eolUint mod);

/**
* @brief determines if the keyboard modifier key is released during this frame
* Uses SDLMod enumerations, ex: KMOD_LSHIFT, KMOD_ALT, etc.
* @param key the SDLMod value of the mod of interest.  
*
* @return eolTrue if held, eolFalse otherwise
*/
eolBool eol_input_is_mod_released(eolUint mod);

eolInput *eol_input_get_nth_input(char *name,eolInt n);

/**
 * @brief determines the if the named input is in a down state at the point of
 * this call.
 *
 * @param input the name of the input in question.  eg: "jump"
 *
 * @return the value of the input (if analog a float, if not 1 or 0) 
 */
eolFloat   eol_input_is_input_down(char *input);

/**
 * @brief determines the if the named input has been down continuously for set
 * period of time.  the held threshold is configureable.
 *
 * @param input the name of the input in question.  eg: "jump"
 *
 * @return the value of the input (if analog a float, if not 1) if it is held, 0 otherwise
 */
eolFloat   eol_input_is_input_held(char *input);

/**
 * @brief determines the if the named input was just pressed this frame
 *
 * @param input the name of the input in question.  eg: "jump"
 *
 * @return true if it is pressed
 */
eolBool   eol_input_is_input_pressed(char *input);

/**
 * @brief determines the if the named input was just released this frame
 *
 * @param input the name of the input in question.  eg: "jump"
 *
 * @return true if it is pressed
 */
eolBool   eol_input_is_input_released(char *input);

/**
 * @brief determines the if the named input was tapped twice in rapid
 * succession.  The threshold for "double tap" is configurable
 *
 * @param input the name of the input in question.  eg: "jump"
 *
 * @return true if it is pressed
 */
eolBool   eol_input_is_input_double_tapped(char *input);

/**
 * @brief updated internal input states.  This need to be called every frame or
 * as often as you need your inputs updated.
 */
void eol_input_update();

/**
 * @brief return the value of the input specified.  This is useful for analog
 * control types such as a joystick's X axis.
 */
eolFloat eol_get_input_value(char *input);

/**
 * @brief  load the input system configuration and input mappings from files.
 * If no files found, load defaults.
 */
void eol_input_load_config();

/**
 * @brief checks for any quit condition from the user.
 *
 * @return eolTrue if a quit condition has been found.
 */
eolBool eol_input_quit_check();

#endif
