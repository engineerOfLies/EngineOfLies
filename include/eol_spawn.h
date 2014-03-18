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
#include "eol_keychain.h"
#include "eol_actor.h"

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
  eolLine type;       /**<name of the spawn type, ie: play_start, exit, arrow_trap*/
  eolRectFloat bounds;/**<bounding rect for spawn placement*/
  eolUint id;         /**<idintifier inuque to the level*/
  eolOrientation ori; /**<orientation of the spawn*/
  eolLine actorFile;  /**<the file to use for the editor graphic*/
  eolActor *actor;    /**<the actor loaded during editing.  For live play, the entity will handle it*/
  eolKeychain *keys;  /**<definition of all keys for this spawn*/
}eolSpawn;

/**
 * @brief move a spawn to a new position
 * @param spawn the spawn to move
 * @param position the position to move to
 */
void eol_spawn_move(eolSpawn * spawn, eolVec3D position);

/**
 * @brief draws the spawn actor.  Used for editors, not live games
 * @param spawn the spawn to load, an actor must have been loaded
 */
void eol_spawn_draw(eolSpawn * spawn);

/**
 * @brief allocates and returns an eolSpawn.  Not necessary if using a declared spawn.
 * @return a pointer to an allocated and initialized spawn or NULL on error.
 */
eolSpawn * eol_spawn_new();

/**
 * @brief loads assets for a previously loaded spawn.  If the assets have already been loaded
 * this will be a no-op.
 * @param spawn a pointer to the address of the spawn to be set up
 */
void eol_spawn_setup(eolSpawn *spawn);

/**
* @brief deletes the spawn and sets the pointer passed to NULL.
* @param spawn a pointer tmodelFileo a pointer to the spawn to be deleted.  After call 
* the pointer will be set to NULL
*/
void eol_spawn_free(eolSpawn **spawn);

/**
 * @brief cleans up the spawn and then deletes it, the pointer will no longer be valid after this call.
 * @param spawn pointer to the spawn to be cleared.
 */
void eol_spawn_delete(eolSpawn *spawn);

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
 * @param n the nth item if the key maps to a list.  If not NULL key is assumed to be a list
 * @return eolTrue if the key was found and matched type eolUint, false otherwise.
 */
eolBool eol_spawn_get_key_uint(eolUint *out, eolSpawn *spawn,eolWord key,eolInt *n);
eolBool eol_spawn_get_key_int(eolInt *out, eolSpawn *spawn,eolWord key,eolInt *n);

eolBool eol_spawn_get_key_float(eolFloat *out, eolSpawn *spawn,eolWord key,eolInt *n);

eolBool eol_spawn_get_key_vec3D(eolVec3D *out, eolSpawn *spawn,eolWord key,eolInt *n);
eolBool eol_spawn_get_key_vec2D(eolVec2D *out, eolSpawn *spawn,eolWord key,eolInt *n);

eolBool eol_spawn_get_key_word(eolWord out, eolSpawn *spawn,eolWord key,eolInt *n);
eolBool eol_spawn_get_key_line(eolLine out, eolSpawn *spawn,eolWord key,eolInt *n);
eolBool eol_spawn_get_key_text(eolText out, eolSpawn *spawn,eolWord key,eolInt *n);

/**
 * @brief creates a keychain describing the spawn
 * used in saving the data.
 * @param spawn the spawn to convert to a keychain
 * @return NULL on error or a populated keychain for the spawn
 */
eolKeychain *eol_spawn_build_keychain(eolSpawn *spawn);

/**
 * @brief creates a new spawn based on config information in the keychain
 * @param conf configuration information that describes a spawn
 * @return NULL on error or a loaded and configed spawn
 */
eolSpawn *eol_spawn_create_from_keychain(eolKeychain *conf);

/**
 * @brief load a spawn or a list of spawn candidates from a file.
 * note: if the file contains "spawnList:" it will be treated as a list
 * @param filename the file to load
 * @return a GLIST of loaded spawns or NULL on error or empty
 */
GList *eol_spawn_load_from_file(eolLine filename);

/**
 * @brief copies the data from one spawn to another;
 * @param in the spawn that will copied.
 * @param id a unique id for this spawn
 * @return NULL on error or a duplicated spawn
 */
eolSpawn *eol_spawn_clone(eolSpawn *in,eolUint id);

#endif
