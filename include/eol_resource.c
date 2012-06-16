#include "eol_resource.h"
#include "eol_logger.h"


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
    void    (*data_delete)(void **data)
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
  manager->_data_size = dataSize;
  manager->_data_list = malloc(dataSize * max);
  manager->data_delete = data_delete;
  if (manager->_data_list == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_resource:unable to allocate resource list\n"
    );
    free(manager);
    return NULL;    
  }
  memset(manager->_data_list,0,dataSize * max);
  manager->_initialized = eolTrue;
  return manager;
}

void * eol_resource_get_next_element(eolResourceManager *manager,eolResource *element)
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
    element = (eolResource *)&manager->_data_list[0];
  }
  else
  {
    element = element + manager->_data_size;
  }
  while (element < (eolResource *)&manager->_data_list[(manager->_data_max * manager->_data_size)])
  {
    if (element->header.refCount > 0)
    {
      return element;
    }
    element = element + manager->_data_size;
  }
  return NULL;
}

void * eol_resource_find_element_by_filename(eolResourceManager *manager,char *filename)
{
  eolResource *element = NULL;
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
    element = (eolResource *)&manager->_data_list[(i * manager->_data_size)];
    if (strncmp(filename,element->header.filename,EOLLINELEN) == 0)
    {
      return element;
    }
  }
  return NULL;
}

void * eol_resource_new_element(eolResourceManager *manager)
{
  eolResource *element = NULL;
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
    element = (eolResource *)&manager->_data_list[(i * manager->_data_size)];
    if (element->header.refCount == 0)
    {
      memset(element,0,manager->_data_size);
      element->header.index = i;
      element->header.refCount = 1;
      return element;
    }
  }
  eol_logger_message(
    EOL_LOG_ERROR,
    "eol_resource: manager %s has no room left for new elements!\n",
    manager->name);
  return NULL;
}

/*eol@eof*/

