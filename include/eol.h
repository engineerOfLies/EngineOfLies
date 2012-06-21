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
#ifndef _EOL_CORE_
#define _EOL_CORE_

#include "eol_types.h"

enum eol_components {
	EOL_GRAPHICS  = 1,
	EOL_FONT      = 2,
	EOL_SPRITE    = 4,
	EOL_MESH      = 8,
	EOL_ARMATURE  = 16,
	EOL_INPUT     = 32,
	EOL_MODEL     = 64,
	EOL_ALL       = 0xFFFF
};

/**
 * @brief Initializes the Engine of Lies and any specified subcomponents
 *
 * @param components a bitmask of desired components.  see the eol_components
 * enumerations for options.  EOL_ALL covers everything.
 */
void eol_init(eolUint components);

/**
* @brief returns the major and minor versions of eol*
*/
void eol_get_version(eolUint *major,eolUint *minor);
#endif

