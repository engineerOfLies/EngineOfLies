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
  eolUint   _data_id_pool;/**<increments with every allocated resource.*/
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
  eolUint id;       /**<unique identifier.  In case of a memory re-use it can be
                        compared with expected for validity*/
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

/**
 * @brief reduces the reference count to the element passed in.  if data in the
 *        manager is unique. it will invoke delete on it.
 *
 * @param manager the resource manager where the element belongs
 * @param data a pointer to the element pointer.  The element pointer will be
 *             set to NULL upon completion.
 *
 */
void eol_resource_free_element(eolResourceManager *manager,void **data);

void eol_resource_manager_free(eolResourceManager **manager);

/**
 * @brief delete any unreferenced data from the resource manager
 *        a delete function must be set.
 *
 * @param manager the resource manager to be cleaned.
 */
void eol_resource_manager_clean(eolResourceManager *manager);

/**
* @brief delete ALL allocated resources.
*        a delete function must be set.
*
* @param manager the resource manager to be cleared.
*/
void eol_resource_manager_clear(eolResourceManager *manager);

/**
 * @brief Allocated and loads a resource from file.  Calls the manager's data load
 * function pointer.
 * @param manager to load a resource for
 * @param filename the file to load the resource from
 * @return a pointer to the allocated and loaded resource or NULL on error
 */
void *eol_resource_manager_load_resource(eolResourceManager *manager,char *filename);

/**
 * @brief returns the index of the element passed.
 * @param manager the resource manager to check
 * @param data the resource data pointer to check
 * @return the unsigned integer index of the element, or -1 on error
 */
eolInt eol_resource_element_get_index(eolResourceManager *manager,void *element);

/**
* @brief returns the index of the element passed.
* @param manager the resource manager to check
* @param data the resource data pointer to check
* @return the unsigned integer id of the element, or -1 on error
*/
eolInt eol_resource_element_get_id(eolResourceManager *manager,void *element);

/**
 * @brief iterates through the resource list
 *
 * @param manager the resource manager to iterate through
 * @param data the position to iterate from.  If passed NULL, it will
 * iterate through the list from the beginning.
 * 
 * @return a pointer to the next element in the resource manager or NULL if
 * there are no more elements.
 */
void * eol_resource_get_next_data(eolResourceManager *manager,void *data);

/**
 * @brief confirms that the resource pointed at by element is the one expected based
 * on the id.
 * @param manager the resource manager for which this element is a member
 * @param element the element in question
 * @param id the expected id of the element
 * @return eolTrue if the id's match or eolFalse on error or mismatch.
 */
eolBool eol_resource_element_id_valid(eolResourceManager *manager,void *element,eolUint id);

#endif

