#ifndef __EOL_FONT__
#define __EOL_FONT__
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
 * @purpose this resource manager will handle all of the boilerplate code
 *          that goes into loading and managing a resource
 */

typedef struct
{
  eolBool   _initialized;
  eolLine   name;
  eolUint   _data_count;
  eolUint   _data_max;
  eolUint   _data_size;
  eolBool   _data_unique;
  char *    _data_list;   /**<character buffer of data*/
  void (*data_delete)(void *data);
  eolBool (*data_load)(char *filename,void *data);
}eolResourceManager;

/*All resources managed by this system must contain this header structure.*/
typedef struct
{
  eolLine filename;
  eolUint refCount;
  eolUint index;
}eolResourceHeader;

eolResourceManager *eol_resource_manager_new();

eolResourceManager * eol_resource_manager_init(
    eolLine managerName,
    eolUint max,
    eolUint dataSize,
    eolBool dataUnique,
    void    (*data_delete)(void *data),
    eolBool (*data_load)(char *filename,void *data)
    );

/**
 * @brief returns a pointer to a new allocated element or NULL on error
 *
 * @param manager the resource manager to get an element from
 *
 * @return a pointer to the data or NULL on error
 */
void * eol_resource_new_element(eolResourceManager *manager);

void eol_resource_free_element(eolResourceManager *manager,void **data);
    
void eol_resource_manager_free(eolResourceManager **manager);

/**
 * @brief delete any unreferenced data from the resource manager
 *        a delete function must be set.
 *
 * @param manager the resource manager to be cleaned.
 */
void eol_resource_manager_clean(eolResourceManager *manager);

void *eol_resource_manager_load_resource(eolResourceManager *manager,char *filename);

#endif

