#include "eol_keychain.h"
#include <glib/gstring.h>
#include <glib/glist.h>
#include <glib/ghash.h>

void eol_g_string_free(GString *string)
{
  g_string_free(string,eolTrue);
}

void eol_keychain_scalar_free(eolKeychain *scalar)
{
  if (!scalar)return;
  if (scalar->keyType != eolKeychainList)return;
  if (scalar->keyValue == NULL)return;
  free(scalar->keyValue);
  scalar->keyValue = NULL;
}
void eol_keychain_destroy(eolKeychain *link)
{
  eol_keychain_free(&link);
}

void eol_keychain_hash_free(eolKeychain *hash)
{
  if (!hash)return;
  if (hash->keyType != eolKeychainHash)return;
  g_hash_table_destroy(hash->keyValue);
  free(hash);
}

void eol_keychain_list_free(eolKeychain *list)
{
  GList *it = NULL;
  if (!list)return;
  if (list->keyType != eolKeychainList)return;
  for (it = list->keyValue;it != NULL;it = it->next)
  {
    eol_keychain_destroy(it->data);
  }
  free(list);
}

void eol_keychain_free(eolKeychain **link)
{
  if (!link)return;
  if (!*link)return;
  if ((*link)->keyValue != NULL)
  {
    if ((*link)->keyFree != NULL)
    {
      (*link)->keyFree((*link)->keyValue);
    }
    else
    {
      free((*link)->keyValue);
    }
  }
  free(*link);
  *link = NULL;
}

eolKeychain *eol_keychain_new()
{
  eolKeychain *link = NULL;
  link = (eolKeychain *)malloc(sizeof(eolKeychain));
  if (link == NULL)
  {
    return NULL;
  }
  memset(link,0,sizeof(eolKeychain));
  return link;
}

eolKeychain *eol_keychain_new_int(eolInt value)
{
  eolInt *cast;
  eolKeychain *link;
  link = eol_keychain_new();
  if (!link)return NULL;
  link->keyType = eolKeychainInt;
  link->keyFree = (eolKeychainFree)eol_keychain_scalar_free;
  link->keyValue = malloc(sizeof(eolInt));
  if (link->keyValue == NULL)
  {
    free (link);
    return NULL;
  }
  cast = (eolInt *)link->keyValue;
  *cast = value;
  return link;
}

eolKeychain *eol_keychain_new_uint(eolUint value)
{
  eolUint *cast;
  eolKeychain *link;
  link = eol_keychain_new();
  if (!link)return NULL;
  link->keyType = eolKeychainUint;
  link->keyFree = (eolKeychainFree)eol_keychain_scalar_free;
  link->keyValue = malloc(sizeof(eolUint));
  if (link->keyValue == NULL)
  {
    free (link);
    return NULL;
  }
  cast = (eolUint *)link->keyValue;
  *cast = value;
  return link;
}

eolKeychain *eol_keychain_new_float(eolFloat value)
{
  eolFloat *cast;
  eolKeychain *link;
  link = eol_keychain_new();
  if (!link)return NULL;
  link->keyType = eolKeychainFloat;
  link->keyFree = (eolKeychainFree)eol_keychain_scalar_free;
  link->keyValue = malloc(sizeof(eolFloat));
  if (link->keyValue == NULL)
  {
    free (link);
    return NULL;
  }
  cast = (eolFloat *)link->keyValue;
  *cast = value;
  return link;
}

eolKeychain *eol_keychain_new_string(char *text)
{
  eolKeychain *link;
  link = eol_keychain_new();
  if (!link)return NULL;
  link->keyType = eolKeychainString;
  link->itemCount = strlen(text);
  link->keyFree = (eolKeychainFree)eol_g_string_free;
  link->keyValue = g_string_new(text);
  return link;
}

eolKeychain *eol_keychain_new_list()
{
  eolKeychain *link;
  link = eol_keychain_new();
  if (!link)return NULL;
  link->keyType = eolKeychainList;
  link->itemCount = 0;
  link->keyFree = (eolKeychainFree)eol_keychain_list_free;
  link->keyValue = NULL;
  return link;
}

eolKeychain *eol_keychain_new_hash()
{
  eolKeychain *link;
  link = eol_keychain_new();
  if (!link)return NULL;
  link->keyType = eolKeychainHash;
  link->itemCount = 0;
  link->keyFree = (eolKeychainFree)eol_keychain_hash_free;
  link->keyValue =
  g_hash_table_new_full(g_str_hash,
                        g_str_equal,
                        (GDestroyNotify)eol_g_string_free,
                        (GDestroyNotify)eol_keychain_destroy);
  return link;
}

void eol_keychain_hash_remove(eolKeychain *hash,char *key)
{
  GString *keyString;
  GHashTable*hashtable = NULL;
  if (!hash)return;
  if (hash->keyType != eolKeychainHash)return;
  if (hash->keyValue == NULL)return;
  hashtable = (GHashTable*)hash->keyValue;
  keyString = g_string_new_len(key,EOLWORDLEN);
  g_hash_table_remove(hashtable,keyString);
  g_string_free(keyString,eolTrue);
}

void eol_keychain_hash_insert(eolKeychain *hash,char *key,eolKeychain *value)
{
  GString *keyString;
  GHashTable*hashtable = NULL;
  if (!hash)return;
  if (hash->keyType != eolKeychainHash)return;
  if (hash->keyValue == NULL)return;
  hashtable = (GHashTable*)hash->keyValue;
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

void eol_keychain_list_append(eolKeychain *list,eolKeychain *item)
{
  if (!list)return;
  if (list->keyType != eolKeychainList)return;
  list->keyValue = g_list_append(list->keyValue,item);
  list->itemCount++;
}

eolKeychain *eol_keychain_get_hash_value(eolKeychain *hash,eolWord key)
{
  eolKeychain *value;
  GString *keyString;
  GHashTable*hashtable = NULL;
  if (!hash)return NULL;
  if (hash->keyType != eolKeychainHash)return NULL;
  if (hash->keyValue == NULL)return NULL;
  hashtable = (GHashTable*)hash->keyValue;
  keyString = g_string_new_len(key,EOLWORDLEN);
  value = g_hash_table_lookup(hashtable,keyString);
  g_string_free(keyString,eolTrue);
  return value;
}

eolKeychain *eol_keychain_get_list_nth(eolKeychain *list, eolUint n)
{
  if (!list)return NULL;
  if (list->keyType != eolKeychainList)return NULL;
  if (list->keyValue == NULL)return NULL;
  return g_list_nth_data((GList*)list->keyValue,n);
}

void eol_keychain_list_remove_nth(eolKeychain *list, eolUint n)
{
  GList *link;
  if (!list)return;
  if (list->keyType != eolKeychainList)return;
  if (list->keyValue == NULL)return;
  link = g_list_nth((GList*)list->keyValue,n);
  if (link == NULL)return;
  list->keyValue = g_list_remove_link(list->keyValue,link);
  eol_keychain_free((eolKeychain**)&link->data);
  g_list_free(link);
}

void eol_keychain_list_move_nth_top(eolKeychain *list, eolUint n)
{
  GList *link;
  if (!list)return;
  if (list->keyType != eolKeychainList)return;
  if (list->keyValue == NULL)return;
  link = g_list_nth((GList*)list->keyValue,n);
  if (link == NULL)return;
  list->keyValue = g_list_remove_link(list->keyValue,link);
  list->keyValue = g_list_concat(link,list->keyValue);
  g_list_free(link);
}

void eol_keychain_list_move_nth_bottom(eolKeychain *list, eolUint n)
{
  GList *link;
  if (!list)return;
  if (list->keyType != eolKeychainList)return;
  if (list->keyValue == NULL)return;
  link = g_list_nth((GList*)list->keyValue,n);
  if (link == NULL)return;
  list->keyValue = g_list_remove_link(list->keyValue,link);
  list->keyValue = g_list_concat(list->keyValue,link);
  g_list_free(link);
}

/*eol@eof*/
