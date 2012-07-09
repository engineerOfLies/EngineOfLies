#ifndef __EOL_SPAWN__
#define __EOL_SPAWN__
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
#include <glib/ghash.h>

/**
 * @brief the spawn type is used as a descriptor for the level for the entities that
 * get spawned for a level.  Its going to be populated by the level editor and used
 * by a game when a level is created
 * It is not a resource and is not managed like one.  It can ne be allocated with malloc
 * or declared and used as any other variable.
 * Note that deleting and adding keys is slow.  It should only be used in startup and editing.
 */
typedef struct
{
  eolWord type; /**<name of the spawn type, ie: play start*/
  eolUint id;   /**<idintifier inuque to the level*/
  GHashTable *keys;
}eolSpawn;

/**
 * @brief allocates and returns an eolSpawn.  Not necessary if using a declared spawn.
 * @return a pointer to an allocated and initialized spawn or NULL on error.
 */
eolSpawn * eol_spawn_new();

/**
 * @brief sets up a previously allocated eolSpawn.  Allocated internal memory.
 * @param spawn a pointer to the address of the spawn to be set up
 * @return eolTrue on success or eolFalse if it has already been set up or error.
 */
eolBool eol_spawn_setup(eolSpawn *spawn);

/**
* @brief frees up internally allocated memory, including the Spawn itself.
* @param spawn a pointer to a pointer to the spawn to be deleted.  After call 
* the pointer will be set to NULL
*/
void eol_spawn_delete(eolSpawn **spawn);

/**
 * @brief frees up internally allocated memory, but does not free the eolSpawn itself.
 * after a call to this, it can be freed with free();
 * @param spawn pointer to the spawn to be cleared.
 */
void eol_spawn_free(eolSpawn *spawn);

/**
 * @brief adds a key to the spawn keys.  If key already exists, it will overwrite.
 * @param spawn the spawn to add the key too.  If NULL, does nothing
 * @param key the key to add / replace
 * @param value the value to add / replace
 */
void eol_spawn_add_key(eolSpawn *spawn,eolWord key,eolLine value);

/**
 * @brief adds a key to the spawn keys where the key is a list.  If the key already
 * exists, it will add to the list instead of replacing the key.
 * @param spawn the spawn to add the key too.  If NULL, does nothing
 * @param key the key to add
 * @param value the value to add
 */
void eol_spawn_add_key_list(eolSpawn *spawn,eolWord key,eolLine value);

/**
 * @brief deletes the key from the spawn keys.
 * @param spawn the spawn to delete the key from
 * @param key the key to be deleted
 */
void eol_spawn_delete_key(eolSpawn *spawn,eolWord key);

/**
* @brief deletes the key from the spawn keys.
* @param spawn the spawn to delete the key from
* @param key the key to be deleted.  Must point to a list
*/
void eol_spawn_delete_key_list(eolSpawn *spawn,eolWord key);

/**
 * @brief checks to see if the key is present on spawn
 * @param spawn the spawn to checl, if NULL returns eolFalse
 * @param key the key to check for
 * @return eolTrue if found, false otherwise.
 */
eolBool eol_spawn_has_key(eolSpawn *spawn, eolWord key);

/**
 * @brief retrieves the specified spawn key as an eolUint.
 * @param out the output parameter.  If null function returns eolFalse immediately
 * @param spawn the spawn candidate to search
 * @param key the keyword to search for.  For instance "alpha"
 * @return eolTrue if the key was found and matched type eolUint, false otherwise.
 */
eolBool eol_spawn_get_key_uint(eolUint *out, eolSpawn *spawn,eolWord key);
eolBool eol_spawn_get_key_int(eolInt *out, eolSpawn *spawn,eolWord key);

eolBool eol_spawn_get_key_float(eolFloat *out, eolSpawn *spawn,eolWord key);

eolBool eol_spawn_get_key_vec3D(eolVec3D *out, eolSpawn *spawn,eolWord key);
eolBool eol_spawn_get_key_vec2D(eolVec2D *out, eolSpawn *spawn,eolWord key);

eolBool eol_spawn_get_key_word(eolWord out, eolSpawn *spawn,eolWord key);
eolBool eol_spawn_get_key_line(eolLine out, eolSpawn *spawn,eolWord key);
eolBool eol_spawn_get_key_text(eolText out, eolSpawn *spawn,eolWord key);

/**
 * @brief retrieves the specified spawn key as a glist.  Its up to you to parse the list.
 * @param out a pointer to a pointer to a GList so it can be pointed at the list.
 * @param spawn the spawn candidate to search
 * @param key the keyword to search for.  For instance "alpha"
 * @return eolTrue if the key was found and matched type eolUint, false otherwise.
 */
eolBool eol_spawn_get_key_list(GList **out, eolSpawn *spawn,eolWord key);



#endif
