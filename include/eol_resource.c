#include "eol_resource.h"
#include "eol_logger.h"
#include <assert.h>

/*TODO make loading a resource a threaded option.  Ie: for non-critical resources
  it can be loaded in a separate thread*/

/*local variables*/

/*local function prototypes*/
void eol_resource_delete_element(eolResourceManager *manager,eolResourceHeader *element);
eolResourceHeader * eol_resource_find_element_by_filename(eolResourceManager *manager,char *filename);
void *eol_resource_get_data_by_header(eolResourceHeader *resource);
eolResourceHeader * eol_resource_get_next_element(eolResourceManager *manager,eolResourceHeader *element);


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
      element->refCount++;
      return eol_resource_get_data_by_header(element);
    }
  }
  element = eol_resource_new_element(manager);
  if (element == NULL)
  {
    return NULL;
  }
  if (!manager->data_load(filename,eol_resource_get_data_by_header(element)))
  {
    eol_resource_delete_element(manager,element);
    return NULL;
  }
  strncpy(element->filename,filename,EOLLINELEN);
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
      element = (eolResourceHeader *)&manager->_data_list[(i * manager->_data_size)];
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
  eolResourceManager *manager = NULL;
  manager = eol_resource_manager_new();
  if (manager == NULL)
  {
    return NULL;
  }
  strncpy(manager->name,managerName,EOLLINELEN);
  manager->_data_count = 0;
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
  manager->_initialized = eolTrue;
  return manager;
}

void * eol_resource_get_next_data(eolResourceManager *manager,void *data)
{
  eolResourceHeader *header = NULL;
  if (data != NULL)
  {
    header = (eolResourceHeader *)data;
    header--;
  }
  return (void *)eol_resource_get_next_element(manager,header);
}

eolResourceHeader * eol_resource_get_next_element(eolResourceManager *manager,eolResourceHeader *element)
{
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
    element = (eolResourceHeader *)&manager->_data_list[0];
  }
  else
  {
    element = element + manager->_data_size;
  }
  while (element < (eolResourceHeader *)&manager->_data_list[(manager->_data_max * manager->_data_size)])
  {
    if (element->refCount > 0)
    {
      return element;
    }
    element = element + manager->_data_size;
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
    element = (eolResourceHeader *)&manager->_data_list[(i * manager->_data_size)];
    if (strncmp(filename,element->filename,EOLLINELEN) == 0)
    {
      return element;
    }
  }
  return NULL;
}

void * eol_resource_new_element(eolResourceManager *manager)
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
  /*TODO: be smarter about resource reclaiming*/
  for (i = 0 ; i < manager->_data_max;i++)
  {
    element = (eolResourceHeader *)&manager->_data_list[(i * manager->_data_size)];
    if (element->refCount == 0)
    {
      eol_resource_delete_element(manager,element);
      memset(element,0,manager->_data_size);
      element->index = i;
      element->refCount = 1;
      element++;/*the data, not the header*/
      return element;
    }
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
  element = (eolResourceHeader *)(*data);
  element--;
  element->refCount--;
  if (manager->_data_unique)
  {
    if (manager->data_delete != NULL)
    {
      manager->data_delete(*data);
    }
  }
  *data = NULL;
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
    element = (eolResourceHeader *)&manager->_data_list[(i * manager->_data_size)];
    if (element->refCount == 0)
    {
      manager->data_delete(&element[1]);
    }
  }
}

void eol_resource_delete_element(eolResourceManager *manager,eolResourceHeader *element)
{
  if (!manager)return;
  if (!element)return;
  if (manager->data_delete == NULL)
  {
    manager->data_delete(&element[1]);
  }
  memset(element,0,manager->_data_size);
}

void *eol_resource_get_data_by_header(eolResourceHeader *resource)
{
  if (!resource)return NULL;
  return (void *)&resource[1];
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
  header = (eolResourceHeader *)element;
  header--;
  /*range verification*/
  if (header < (eolResourceHeader *)&manager->_data_list[0])
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_resource:under seeking in data list\n");
    assert(0);
    return -1;
  }
  if (header >= (eolResourceHeader *)&manager->_data_list[manager->_data_max])
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_resource:over seeking in data list\n");
    return -1;
  }
  return header->index;
}

/*eol@eof*/

