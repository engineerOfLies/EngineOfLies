#ifndef __EOL_KEYCHAIN__
#define __EOL_KEYCHAIN__
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

enum eolKeychainTypes {
  eolKeychainVoid,   /**<custom data*/
  eolKeychainInt,    /**<eolInt*/
  eolKeychainUint,   /**<eolUint*/
  eolKeychainFloat,  /**<eolFloat*/
  eolKeychainString, /**<GString*/
  eolKeychainList,   /**<GList of something*/
  eolKeychainHash,   /**<GHashTable of something*/
  eolKeychainCustom0 /**<for user defined types.  EOL will not use Custom0 or after.*/
};

typedef void (*eolKeychainFree)(void *data);
/**
* @brief his structure wraps description information for a pointer to a container type
* it will be used in spawn and config types where we deal with pointers to unknown types.
*/
typedef struct
{
  eolUint keyType;
  eolUint itemCount;  /*in the case of list or hash*/
  eolKeychainFree keyFree;
  void *keyValue;
}eolKeychain;

/**
* @brief frees the eolKeychain and sets the passed in pointer to NULL
* @param a pointer to a pointer to a typed pointer.
*/
void eol_keychain_free(eolKeychain **point);

/**
* @brief destroys the typed pointer passed
* @param point the pointer to be destroyed
*/
void eol_keychain_destroy(eolKeychain *point);

/**
* @brief allocates and returns a pointer to an empty typed pointer
* @return NULL on allocation error or a zero'd out eolKeychain
*/
eolKeychain *eol_keychain_new();

/**
* @brief allocated and sets up a pointer to a GString filled with the default text
* @param text starting text.  May be empty
* @return NULL on allocation error or a set up eolKeychain of a GString
*/
eolKeychain *eol_keychain_new_string(char *text);



/**
* @brief allocated and sets up a pointer to an Empty GList.
* @return NULL on allocation error or a set up eolKeychain of a GList
*/
eolKeychain *eol_keychain_new_list();

/**
* @brief allocated and sets up a pointer to an Empty GHash.
* @return NULL on allocation error or a set up eolKeychain of a GHashTable
*/
eolKeychain *eol_keychain_new_hash();

/**
* @brief appends a list item to the eolKeychain list value.
* if its not a pointer to a list, it will return without doing anything.
* @param list the typed pointer of a glist
* @param item the eolKeychain to the item to be added to the list.
*/
void eol_keychain_list_append(eolKeychain *list,eolKeychain *item);

/**
* @brief Insert or replace a key in the eolKeychain of a hash.
* if it is not a pointer to a hash it will return without doing anything.
* @param hash the typed pointer of a ghash
* @param key the key to be inserted.  Limited to the size of eolWord.
* @param value the eolKeychain to the item to be added to the hash.
*/
void eol_keychain_hash_insert(eolKeychain *hash,eolWord key,eolKeychain *value);

/**
* @brief Removes a key from the eolKeychain of a hash.
* if it is not a pointer to a hash it will return without doing anything.
* if found, eol_keychain_destroy is called on the value.
* @param hash the typed pointer of a ghash
* @param key the key to be removed.  Limited to the size of eolWord.
*/
void eol_keychain_hash_remove(eolKeychain *hash,char *key);

/**
 * @brief looks up the key in the hash.
 * checks type before any operation
 * @param hash the typed pointer of a ghash
 * @param key the key to be found.  Limited to the size of eolWord.
 * @return NULL if not a hash, or not found. A typed pointer to the value if found.
 */
eolKeychain *eol_keychain_get_hash_value(eolKeychain *hash,eolWord key);

/**
* @brief looks up the nth item in the list
* checks type before any operation
* @param list the typed pointer of a glist
* @param n the index of the item to be found.
* @return NULL if not a list, or not found. A typed pointer to the value if found.
*/
eolKeychain *eol_keychain_get_list_nth(eolKeychain *list, eolUint n);

/**
 * @brief removed the nth item from the list
 * checks type before any operation
 * calls eol_keychain_destroy on the item found.
 * @param list the typed pointer of a glist
 * @param n the index of the item to be found and deleted
 */
void eol_keychain_list_remove_nth(eolKeychain *list, eolUint n);

/**
* @brief moves the nth item in the list to the bottom of the list
* checks type before any operation
* @param list the typed pointer of a glist
* @param n the index of the item to be moved
*/
void eol_keychain_list_move_nth_bottom(eolKeychain *list, eolUint n);

/**
* @brief moves the nth item in the list to the top of the list
* checks type before any operation
* @param list the typed pointer of a glist
* @param n the index of the item to be moved
*/
void eol_keychain_list_move_nth_top(eolKeychain *list, eolUint n);

/**
* @brief convenience function when working with GString types.
* @param string the string to free.
*/
void eol_g_string_free(GString *string);

#endif
