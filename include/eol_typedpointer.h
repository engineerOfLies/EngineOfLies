#ifndef __EOL_TYPED_POINTER__
#define __EOL_TYPED_POINTER__
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
#include <glib/gstring.h>

/**
 * @brief the typed pointer is a pointer to a very limited subset of types.  It packs
 * type information in with the pointer so that destroying and copying can be done without
 * prior knowledge of its set up.
 */

enum eolTypedPointerTypes {
  eolTypedPointerVoid,   /**<custom data*/
  eolTypedPointerString, /**<GString*/
  eolTypedPointerList,   /**<GList of something*/
  eolTypedPointerHash,   /**<GHashTable of something*/
  eolTypedPointerCustom0 /**<for user defined types.  EOL will not use Custom0 or after.*/
};

typedef void (*eolTypedPointerFree)(void *data);
/**
* @brief his structure wraps description information for a pointer to a container type
* it will be used in spawn and config types where we deal with pointers to unknown types.
*/
typedef struct
{
  eolUint pointerType;
  eolUint itemCount;  /*in the case of list or hash*/
  eolTypedPointerFree pointerFree;
  void *pointerValue;
}eolTypedPointer;

/**
* @brief frees the eolTypedPointer and sets the passed in pointer to NULL
* @param a pointer to a pointer to a typed pointer.
*/
void eol_type_pointer_free(eolTypedPointer **point);

/**
* @brief destroys the typed pointer passed
* @param point the pointer to be destroyed
*/
void eol_type_destroy(eolTypedPointer *point);

/**
* @brief allocates and returns a pointer to an empty typed pointer
* @return NULL on allocation error or a zero'd out eolTypedPointer
*/
eolTypedPointer *eol_type_pointer_new();

/**
* @brief allocated and sets up a pointer to a GString filled with the default text
* @param text starting text.  May be empty
* @return NULL on allocation error or a set up eolTypedPointer of a GString
*/
eolTypedPointer *eol_type_pointer_new_string(char *text);

/**
* @brief allocated and sets up a pointer to an Empty GList.
* @return NULL on allocation error or a set up eolTypedPointer of a GList
*/
eolTypedPointer *eol_type_pointer_new_list();

/**
* @brief allocated and sets up a pointer to an Empty GHash.
* @return NULL on allocation error or a set up eolTypedPointer of a GHashTable
*/
eolTypedPointer *eol_type_pointer_new_hash();

/**
* @brief appends a list item to the eolTypedPointer list value.
* if its not a pointer to a list, it will return without doing anything.
* @param list the typed pointer of a glist
* @param item the eolTypedPointer to the item to be added to the list.
*/
void eol_type_pointer_list_append(eolTypedPointer *list,eolTypedPointer *item);

/**
* @brief Insert or replace a key in the eolTypedPointer of a hash.
* if it is not a pointer to a hash it will return without doing anything.
* @param hash the typed pointer of a ghash
* @param key the key to be inserted.  Limited to the size of eolWord.
* @param value the eolTypedPointer to the item to be added to the hash.
*/
void eol_type_pointer_hash_insert(eolTypedPointer *hash,eolWord key,eolTypedPointer *value);

/**
* @brief Removes a key from the eolTypedPointer of a hash.
* if it is not a pointer to a hash it will return without doing anything.
* if found, eol_type_destroy is called on the value.
* @param hash the typed pointer of a ghash
* @param key the key to be removed.  Limited to the size of eolWord.
*/
void eol_type_pointer_hash_remove(eolTypedPointer *hash,char *key);

/**
 * @brief looks up the key in the hash.
 * checks type before any operation
 * @param hash the typed pointer of a ghash
 * @param key the key to be found.  Limited to the size of eolWord.
 * @return NULL if not a hash, or not found. A typed pointer to the value if found.
 */
eolTypedPointer *eol_type_pointer_get_hash_value(eolTypedPointer *hash,eolWord key);

/**
* @brief looks up the nth item in the list
* checks type before any operation
* @param list the typed pointer of a glist
* @param n the index of the item to be found.
* @return NULL if not a list, or not found. A typed pointer to the value if found.
*/
eolTypedPointer *eol_type_pointer_get_list_nth(eolTypedPointer *list, eolUint n);

/**
 * @brief removed the nth item from the list
 * checks type before any operation
 * calls eol_type_destroy on the item found.
 * @param list the typed pointer of a glist
 * @param n the index of the item to be found and deleted
 */
void eol_type_pointer_list_remove_nth(eolTypedPointer *list, eolUint n);

/**
* @brief moves the nth item in the list to the bottom of the list
* checks type before any operation
* @param list the typed pointer of a glist
* @param n the index of the item to be moved
*/
void eol_type_pointer_list_move_nth_bottom(eolTypedPointer *list, eolUint n);

/**
* @brief moves the nth item in the list to the top of the list
* checks type before any operation
* @param list the typed pointer of a glist
* @param n the index of the item to be moved
*/
void eol_type_pointer_list_move_nth_top(eolTypedPointer *list, eolUint n);

/**
* @brief convenience function when working with GString types.
* @param string the string to free.
*/
void eol_g_string_free(GString *string);

#endif
