#include "eol_typedpointer.h"
#include <glib/gstring.h>
#include <glib/glist.h>
#include <glib/ghash.h>

void eol_g_string_free(GString *string)
{
  g_string_free(string,eolTrue);
}

void eol_type_destroy(eolTypedPointer *point)
{
  eol_type_pointer_free(&point);
}

void eol_type_pointer_hash_free(eolTypedPointer *hash)
{
  if (!hash)return;
  if (hash->pointerType != eolTypedPointerHash)return;
  g_hash_table_destroy(hash->pointerValue);
  free(hash);
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
  point->pointerFree = (eolTypedPointerFree)eol_type_pointer_list_free;
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
  point->pointerFree = (eolTypedPointerFree)eol_type_pointer_hash_free;
  point->pointerValue =
  g_hash_table_new_full(g_str_hash,
                        g_str_equal,
                        (GDestroyNotify)eol_g_string_free,
                        (GDestroyNotify)eol_type_destroy);
  return point;
}

void eol_type_pointer_hash_insert(eolTypedPointer *hash,char *key,eolTypedPointer *value)
{
  GString *keyString;
  GHashTable*hashtable = NULL;
  if (!hash)return;
  if (hash->pointerType != eolTypedPointerHash)return;
  if (hash->pointerValue == NULL)return;
  hashtable = (GHashTable*)hash->pointerValue;
  keyString = g_string_new_len(key,EOLWORDLEN);
  if (g_hash_table_lookup(hashtable,keyString) != NULL)
  {
    g_hash_table_replace(hashtable,keyString,value);
  }
  else
  {
    g_hash_table_insert(hashtable,keyString,value);
    hash->itemCount++;
  }
  g_string_free(keyString,eolTrue);
}

void eol_type_pointer_list_append(eolTypedPointer *list,eolTypedPointer *item)
{
  if (!list)return;
  if (list->pointerType != eolTypedPointerList)return;
  list->pointerValue = g_list_append(list->pointerValue,item);
  list->itemCount++;
}

eolTypedPointer *eol_type_pointer_get_hash_value(eolTypedPointer *hash,eolWord key)
{
  eolTypedPointer *value;
  GString *keyString;
  GHashTable*hashtable = NULL;
  if (!hash)return NULL;
  if (hash->pointerType != eolTypedPointerHash)return NULL;
  if (hash->pointerValue == NULL)return NULL;
  hashtable = (GHashTable*)hash->pointerValue;
  keyString = g_string_new_len(key,EOLWORDLEN);
  value = g_hash_table_lookup(hashtable,keyString);
  g_string_free(keyString,eolTrue);
  return value;
}

eolTypedPointer *eol_type_pointer_get_list_nth(eolTypedPointer *list, eolUint n)
{
  if (!list)return NULL;
  if (list->pointerType != eolTypedPointerList)return NULL;
  if (list->pointerValue == NULL)return NULL;
  return g_list_nth_data((GList*)list->pointerValue,n);
}

void eol_type_pointer_list_remove_nth(eolTypedPointer *list, eolUint n)
{
  GList *link;
  if (!list)return;
  if (list->pointerType != eolTypedPointerList)return;
  if (list->pointerValue == NULL)return;
  link = g_list_nth_data((GList*)list->pointerValue,n);
  list->pointerValue = (GList*)g_list_remove_link(list->pointerValue,link);
  eol_type_pointer_free((eolTypedPointer**)&link->data);
  g_list_free(link);
}


/*eol@eof*/
