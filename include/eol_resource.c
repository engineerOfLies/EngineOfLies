#include "eol_resource.h"
#include "eol_logger.h"
#include "eol_graphics.h"
#include <assert.h>

/*TODO make loading a resource a threaded option.  Ie: for non-critical resources
  it can be loaded in a separate thread*/

/*local variables*/

/*local function prototypes*/
void eol_resource_delete_element(eolResourceManager *manager,eolResourceHeader *element);
eolResourceHeader * eol_resource_find_element_by_filename(eolResourceManager *manager,char *filename);
void *eol_resource_get_data_by_header(eolResourceHeader *resource);
eolResourceHeader *eol_resource_get_header_by_data(void *data);
eolResourceHeader * eol_resource_get_next_element(eolResourceManager *manager,eolResourceHeader *element);
eolBool eol_resource_validate_header_range(eolResourceManager *manager,eolResourceHeader *element);

eolResourceHeader *eol_resource_get_header_by_index(eolResourceManager *manager,eolUint i)
{
  if (!manager)return NULL;
  if (i >= manager->_data_max)return NULL;
  return (eolResourceHeader *)&manager->_data_list[(i * manager->_data_size)];
}

void *eol_resource_manager_load_resource(eolResourceManager *manager,char *filename)
{
  eolResourceHeader * element = NULL;
  if (!filename)return NULL;
  if (manager->data_load == NULL)
  {
    return NULL;
  }
  if (manager->_data_unique == eolFalse)
  {
    element = eol_resource_find_element_by_filename(manager,filename);
    if (element != NULL)
    {
      if (element->refCount == 0)
      {
        manager->_data_count++;
      }
      element->refCount++;
      return eol_resource_get_data_by_header(element);
    }
  }
  element = eol_resource_get_header_by_data(eol_resource_new_element(manager));
  if (element == NULL)
  {
    return NULL;
  }
  
  if (manager->data_load != NULL)
  {
    if (!manager->data_load(filename,eol_resource_get_data_by_header(element)))
    {
      eol_resource_delete_element(manager,element);
      return NULL;
    }
  }
  eol_line_cpy(element->filename,filename);
  return eol_resource_get_data_by_header(element);
}

void eol_resource_manager_free(eolResourceManager **manager_pp)
{
  int i;
  eolResourceManager *manager = NULL;
  eolResourceHeader *element = NULL;
  if (!manager_pp)return;
  if (!*manager_pp)return;
  manager = *manager_pp;
  if (manager->data_delete != NULL)
  {
    for (i = 0 ; i < manager->_data_max;i++)
    {
      element = eol_resource_get_header_by_index(manager,i);
      manager->data_delete(eol_resource_get_data_by_header(element));
    }
  }
  free(manager->_data_list);
  free(manager);
  *manager_pp = NULL;
}

eolResourceManager *eol_resource_manager_new()
{
  eolResourceManager *manager = NULL;
  manager = (eolResourceManager *)malloc(sizeof(eolResourceManager));
  if (manager == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_resource: unable to allocate resource manager!\n");
    return NULL;
  }
  memset(manager,0,sizeof(eolResourceManager));
  return manager;
}

eolResourceManager * eol_resource_manager_init(
    eolLine managerName,
    eolUint max,
    eolUint dataSize,
    eolBool dataUnique,
    void    (*data_delete)(void *data),
    eolBool (*data_load)(char *filename,void *data)
  )
{
  int i;
  eolResourceHeader * element = NULL;
  eolResourceManager *manager = NULL;
  manager = eol_resource_manager_new();
  if (manager == NULL)
  {
    return NULL;
  }
  strncpy(manager->name,managerName,EOLLINELEN);
  manager->_data_count = 0;
  manager->_data_id_pool = 0;
  manager->_data_max = max;
  manager->_data_unique = dataUnique;
  manager->_data_size = dataSize + sizeof(eolResourceHeader);
  manager->_data_list = malloc(manager->_data_size * max);
  manager->data_delete = data_delete;
  manager->data_load = data_load;
  if (manager->_data_list == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_resource:unable to allocate resource list\n"
    );
    free(manager);
    return NULL;    
  }
  memset(manager->_data_list,0,manager->_data_size * max);
  for (i = 0 ; i < manager->_data_max;i++)
  {
    element = eol_resource_get_header_by_index(manager,i);
    element->index = i;
    element->refCount = 0;
    element->timeFree = 0;
  }
  manager->_initialized = eolTrue;
  return manager;
}

eolUint eol_resource_manager_get_element_count(eolResourceManager *manager)
{
  if (!manager)return 0;
  return manager->_data_count;
}

void * eol_resource_get_next_data(eolResourceManager *manager,void *data)
{
  eolResourceHeader *header = NULL;
  if (data != NULL)
  {
    header = (eolResourceHeader *)data;
    header--;
  }
  return eol_resource_get_data_by_header(eol_resource_get_next_element(manager,header));
}

eolResourceHeader * eol_resource_get_next_element(eolResourceManager *manager,eolResourceHeader *element)
{
  int index = 0;
  if (manager == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:passed a NULL manager\n");
    return NULL;
  }
  if (manager->_initialized == eolFalse)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:manager %s uninitialized\n",
      manager->name);
    return NULL;
  }
  if (element == NULL)
  {
    index = 0;
  }
  else
  {
    index = element->index + 1;
  }
  for ( ; index < manager->_data_max;index++)
  {
    element = (eolResourceHeader *)&manager->_data_list[(index * manager->_data_size)];
    if (element->refCount > 0)
    {
      return element;
    }
  }
  return NULL;
}

eolResourceHeader * eol_resource_find_element_by_filename(eolResourceManager *manager,char *filename)
{
  eolResourceHeader *element = NULL;
  int i = 0;
  if (manager == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:passed a NULL manager\n");
    return NULL;
  }
  if (manager->_initialized == eolFalse)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:manager %s uninitialized\n",
      manager->name);
    return NULL;
  }
  for (i = 0 ; i < manager->_data_max;i++)
  {
    element = eol_resource_get_header_by_index(manager,i);
    if (strncmp(filename,element->filename,EOLLINELEN) == 0)
    {
      return element;
    }
  }
  return NULL;
}

void * eol_resource_new_element(eolResourceManager *manager)
{
  int i = 0;
  eolResourceHeader *element = NULL;
  eolResourceHeader *oldest = NULL;
  eolUint            oldestIndex = 0;
  eolUint            oldestTime = eol_graphics_get_now() + 1;
  if (manager == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:passed a NULL manager\n");
    return NULL;
  }
  if (manager->_initialized == eolFalse)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:manager %s uninitialized\n",
      manager->name);
    return NULL;
  }

  for (i = 0 ; i < manager->_data_max;i++)
  {
    element = eol_resource_get_header_by_index(manager,i);
    if (element->refCount == 0)
    {
      if (element->timeFree <= oldestTime)
      {
        oldest = element;
        oldestTime = element->timeFree;
        oldestIndex = i;
        if (element->timeFree == 0)
        {
          /*cant get older than never used*/
          break;
        }
      }
    }
  }
  if (oldest != NULL)
  {
    element = oldest;
    eol_resource_delete_element(manager,element);
    element->id = manager->_data_id_pool++;
    element->refCount = 1;
    manager->_data_count++;
    return eol_resource_get_data_by_header(element);
  }

  eol_logger_message(
    EOL_LOG_ERROR,
    "eol_resource: manager %s has no room left for new elements!\n",
    manager->name);
  return NULL;
}

void eol_resource_free_element(eolResourceManager *manager,void **data)
{
  eolResourceHeader *element;
  if (!manager)return;
  if (!data)return;
  if (!*data)return;
  element = eol_resource_get_header_by_data(*data);
  if (manager->_data_unique)
  {
    if (manager->data_delete != NULL)
    {
      manager->data_delete(*data);
    }
    eol_resource_delete_element(manager,element);
    element->timeFree = 0;
    manager->_data_count--;
    element->refCount = 0;
  }
  else
  {
    element->refCount--;
    if (element->refCount == 0)
    {
      manager->_data_count--;
    }
    element->timeFree = eol_graphics_get_now();
  }
  *data = NULL;
}

void eol_resource_manager_clear(eolResourceManager *manager)
{
  eolResourceHeader *element = NULL;
  int i = 0;
  if (manager == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:passed a NULL manager\n");
    return;
  }
  if (manager->_initialized == eolFalse)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:manager %s uninitialized\n",
      manager->name);
    return;
  }
  if (manager->data_delete == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:manager %s had no delete function\n",
      manager->name);
    return;
  }
  for (i = 0 ; i < manager->_data_max;i++)
  {
    element = eol_resource_get_header_by_index(manager,i);
    manager->data_delete(&element[1]);
  }
  manager->_data_count = 0;
}

void eol_resource_manager_clean(eolResourceManager *manager)
{
  eolResourceHeader *element = NULL;
  int i = 0;
  if (manager == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:passed a NULL manager\n");
    return;
  }
  if (manager->_initialized == eolFalse)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:manager %s uninitialized\n",
      manager->name);
    return;
  }
  if (manager->data_delete == NULL)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_resource:manager %s had no delete function\n",
      manager->name);
    return;
  }
  for (i = 0 ; i < manager->_data_max;i++)
  {
    element = eol_resource_get_header_by_index(manager,i);
    if (element->refCount == 0)
    {
      manager->data_delete(&element[1]);
    }
  }
}

void eol_resource_delete_element(eolResourceManager *manager,eolResourceHeader *element)
{
  eolUint index;
  if (!manager)return;
  if (!element)return;
  if (manager->data_delete != NULL)
  {
    manager->data_delete(&element[1]);
  }
  index = element->index;
  memset(element,0,manager->_data_size);
  element->index = index;
}

eolResourceHeader *eol_resource_get_header_by_data(void *data)
{
  eolResourceHeader *header;
  if (!data)return NULL;
  header = data;
  header--;
  return header;
}

void *eol_resource_get_data_by_header(eolResourceHeader *resource)
{
  if (!resource)return NULL;
  return (void *)((char *)resource + sizeof(eolResourceHeader));
}

eolBool eol_resource_element_id_valid(eolResourceManager *manager,void *element,eolUint id)
{
  if (eol_resource_element_get_id(manager,element) == id)return eolTrue;
  return eolFalse;
}

eolInt eol_resource_element_get_id(eolResourceManager *manager,void *element)
{
  eolResourceHeader *header;
  if (!manager)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_resource:passed a NULL manager\n");

    return -1;
  }
  if (!element)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_resource:passed a NULL element\n");
    return -1;
  }
  header = eol_resource_get_header_by_data(element);
  /*range verification*/
  if (!eol_resource_validate_header_range(manager,header))
  {
    return -1;
  }
  return header->id;
}

eolUint eol_resource_element_get_refcount(eolResourceManager *manager,void *element)
{
  eolResourceHeader *header;
  if (!manager)
  {
    eol_logger_message(
    EOL_LOG_INFO,
                       "eol_resource:passed a NULL manager\n");
                       
                       return 0;
  }
  if (!element)
  {
    eol_logger_message(
    EOL_LOG_INFO,
                       "eol_resource:passed a NULL element\n");
                       return 0;
  }
  header = eol_resource_get_header_by_data(element);
  /*range verification*/
  if (!eol_resource_validate_header_range(manager,header))
  {
    return 0;
  }
  return header->refCount;
}

void eol_resource_element_get_filename(eolLine filename, eolResourceManager *manager,void *element)
{
  eolResourceHeader *header;
  if (!manager)
  {
    eol_logger_message(
    EOL_LOG_INFO,
                       "eol_resource:passed a NULL manager\n");
                       
                       return ;
  }
  if (!element)
  {
    eol_logger_message(
    EOL_LOG_INFO,
                       "eol_resource:passed a NULL element\n");
                       return ;
  }
  header = eol_resource_get_header_by_data(element);
  /*range verification*/
  if (!eol_resource_validate_header_range(manager,header))
  {
    return;
  }
  eol_line_cpy(filename,header->filename);
}

eolInt eol_resource_element_get_index(eolResourceManager *manager,void *element)
{
  eolResourceHeader *header;
  if (!manager)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_resource:passed a NULL manager\n");

    return -1;
  }
  if (!element)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_resource:passed a NULL element\n");
    return -1;
  }
  header = eol_resource_get_header_by_data(element);
  /*range verification*/
  if (!eol_resource_validate_header_range(manager,header))
  {
    return -1;
  }
  return header->index;
}

eolBool eol_resource_validate_header_range(eolResourceManager *manager,eolResourceHeader *element)
{
  if (!manager)return eolFalse;
  if (!element)return eolFalse;
  if ((void *)element < (void *)manager->_data_list)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_resource: underseeking in data list.\n");
    return eolFalse;
  }
  if ((void *)element > (void *)&manager->_data_list[manager->_data_max * manager->_data_size])
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_resource: overseeking in data list.\n");
    return eolFalse;
  }
  return eolTrue;
}
/*eol@eof*/

