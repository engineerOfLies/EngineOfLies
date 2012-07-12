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
* @brief frees the eolTypedPointer
*/
void eol_type_pointer_free(eolTypedPointer **point);

/**
* @brief allocates and returns a pointer to an empty typed pointer
* @return NULL on allocation error or a zero'd out eolTypedPointer
*/
eolTypedPointer *eol_type_pointer_new();

/**
* @brief allocated and sets up a pointer to a GString filled with the default text
* @param text starting text.  May be empty
* @return NULL on allocation error or a set up eolTypedPointer to a GString
*/
eolTypedPointer *eol_type_pointer_new_string(char *text);

/**
* @brief allocated and sets up a pointer to an Empty GList.  A destroy function will not
* be set.  You must assign one to the returned typedPointer.
* @return NULL on allocation error or a set up eolTypedPointer to a GString
*/
eolTypedPointer *eol_type_pointer_new_list();

eolTypedPointer *eol_type_pointer_new_hash();


void eol_type_pointer_list_append(eolTypedPointer *list,eolTypedPointer *item);

void eol_type_destroy(eolTypedPointer *point);

/**
* @brief convenience function when working with GString types.
* @param string the string to free.
*/
void eol_g_string_free(GString *string);

#endif
