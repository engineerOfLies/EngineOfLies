#include "eol_typedpointer.h"
#include <glib/gstring.h>
#include <glib/glist.h>

void eol_g_string_free(GString *string)
{
  g_string_free(string,eolTrue);
}

void eol_type_destroy(eolTypedPointer *point)
{
  eol_type_pointer_free(&point);
}

void eol_type_pointer_list_free(eolTypedPointer *list)
{
  GList *it = NULL;
  if (!list)return;
  if (list->pointerType != eolTypedPointerList)return;
  for (it = list->pointerValue;it != NULL;it = it->next)
  {
    eol_type_destroy(it->data);
  }
  free(list);
}

void eol_type_pointer_free(eolTypedPointer **point)
{
  if (!point)return;
  if (!*point)return;
  if ((*point)->pointerValue != NULL)
  {
    if ((*point)->pointerFree != NULL)
    {
      (*point)->pointerFree((*point)->pointerValue);
    }
    else
    {
      free((*point)->pointerValue);
    }
  }
  free(*point);
  *point = NULL;
}

eolTypedPointer *eol_type_pointer_new()
{
  eolTypedPointer *point = NULL;
  point = (eolTypedPointer *)malloc(sizeof(eolTypedPointer));
  if (point == NULL)
  {
    return NULL;
  }
  memset(point,0,sizeof(eolTypedPointer));
  return point;
}

eolTypedPointer *eol_type_pointer_new_string(char *text)
{
  eolTypedPointer *point;
  point = eol_type_pointer_new();
  if (!point)return NULL;
  point->pointerType = eolTypedPointerString;
  point->itemCount = strlen(text);
  point->pointerFree = (eolTypedPointerFree)eol_g_string_free;
  point->pointerValue = g_string_new(text);
  return point;
}

eolTypedPointer *eol_type_pointer_new_list()
{
  eolTypedPointer *point;
  point = eol_type_pointer_new();
  if (!point)return NULL;
  point->pointerType = eolTypedPointerList;
  point->itemCount = 0;
  point->pointerFree = eol_type_pointer_list_free;
  point->pointerValue = NULL;
  return point;
}

eolTypedPointer *eol_type_pointer_new_hash()
{
  eolTypedPointer *point;
  point = eol_type_pointer_new();
  if (!point)return NULL;
  point->pointerType = eolTypedPointerHash;
  point->itemCount = 0;
  point->pointerFree = NULL;
  point->pointerValue = NULL;
  return point;
}

void eol_type_pointer_list_append(eolTypedPointer *list,eolTypedPointer *item)
{
  if (!list)return;
  if (list->pointerType != eolTypedPointerList)return;
  list->pointerValue = g_list_append(list->pointerValue,item);
  list->itemCount++;
}


/*eol@eof*/
