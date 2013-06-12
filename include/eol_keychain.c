#include "eol_keychain.h"
#include "eol_loader.h"
#include "eol_logger.h"
#include <glib/gstring.h>
#include <glib/glist.h>
#include <glib/ghash.h>
#include <glib.h>

/*local prototypes*/
void eol_keychain_print_link(eolKeychain *link,eolUint depth,eolBool listStart);

/*function definitions*/

void eol_g_string_free(char *string)
{
  if (!string)return;
  g_free(string);
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

void eol_keychain_list_clear(eolKeychain *list)
{
  GList *it = NULL;
  if (!list)return;
  if (list->keyType != eolKeychainList)return;
  for (it = list->keyValue;it != NULL;it = it->next)
  {
    eol_keychain_destroy(it->data);
  }
  g_list_free(list->keyValue);
  list->keyValue = NULL;
  list->itemCount = 0;
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

eolKeychain *eol_keychain_clone(eolKeychain *src)
{
  if (!src)return NULL;
  if (!src->keyClone)return NULL;
  return src->keyClone(src);
}

eolKeychain *eol_keychain_clone_string(eolKeychain *src)
{
  eolLine str;
  if (!eol_keychain_get_line(str,src))return NULL;
  return eol_keychain_new_string(str);
}

eolKeychain *eol_keychain_clone_list(eolKeychain *src)
{
  eolUint count,i;
  eolKeychain *item, *list = NULL;
  if (!src)return NULL;
  list = eol_keychain_new_list();
  if (!list)return NULL;
  count = eol_keychain_get_list_count(src);
  for (i = 0;i < count;i++)
  {
    item = eol_keychain_get_list_nth(src, i);
    if (!item)continue;
    eol_keychain_list_append(list,eol_keychain_clone(item));
  }
  return list;
}

eolKeychain *eol_keychain_clone_hash(eolKeychain *src)
{
  eolUint count,i;
  eolKeychain *hash;
  eolLine keystring;
  eolKeychain * value;
  if (!src)return NULL;
  hash = eol_keychain_new_hash();
  if (!hash)return NULL;
  count = eol_keychain_get_hash_count(src);
  for (i = 0;i < count;i++)
  {
    value = eol_keychain_get_hash_nth(keystring,src, i);
    if (!value)continue;
    eol_keychain_hash_insert(hash,keystring,eol_keychain_clone(value));
  }
  return hash;
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



eolKeychain *eol_keychain_new_int_scaler(eolInt value)
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

eolKeychain *eol_keychain_new_uint_scaler(eolUint value)
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

eolKeychain *eol_keychain_new_float_scaler(eolFloat value)
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

eolKeychain *eol_keychain_new_bool(eolBool n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%s",eol_string_from_bool(n));
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_int(eolInt n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%i",n);
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_uint(eolInt n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%u",n);
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_float(eolFloat n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%f",n);
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_vec2d(eolVec2D n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%f,%f",n.x,n.y);
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_vec3d(eolVec3D n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%f,%f,%f",n.x,n.y,n.z);
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_vec4d(eolVec4D n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%f,%f,%f,%f",n.x,n.y,n.z,n.w);
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_rect(eolRect n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%i,%i,%i,%i",n.x,n.y,n.w,n.h);
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_rectf(eolRectFloat n)
{
  eolLine text;
  snprintf(text,EOLLINELEN,"%f,%f,%f,%f",n.x,n.y,n.w,n.h);
  return eol_keychain_new_string(text);
}

eolKeychain *eol_keychain_new_orientation(eolOrientation ori)
{
  eolKeychain *oriHash;
  oriHash = eol_keychain_new_hash();
  if (!oriHash)return NULL;
  eol_keychain_hash_insert(oriHash,"position",eol_keychain_new_vec3d(ori.position));
  eol_keychain_hash_insert(oriHash,"scale",eol_keychain_new_vec3d(ori.scale));
  eol_keychain_hash_insert(oriHash,"rotation",eol_keychain_new_vec3d(ori.rotation));
  eol_keychain_hash_insert(oriHash,"color",eol_keychain_new_vec3d(ori.color));
  eol_keychain_hash_insert(oriHash,"alpha",eol_keychain_new_float(ori.alpha));
  return oriHash;
}

eolKeychain *eol_keychain_new_string(char *text)
{
  eolKeychain *link;
  link = eol_keychain_new();
  if (!link)return NULL;
  link->keyType = eolKeychainString;
  link->itemCount = strlen(text);
  link->keyFree = (eolKeychainFree)eol_g_string_free;
  link->keyClone = eol_keychain_clone_string;
  link->keyValue = g_strdup(text);
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
  link->keyClone = eol_keychain_clone_list;
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
  link->keyClone = eol_keychain_clone_hash;
  link->keyValue =
    g_hash_table_new_full(g_str_hash,
                          g_str_equal,
                          (GDestroyNotify)eol_g_string_free,
                          (GDestroyNotify)eol_keychain_destroy);
  return link;
}

eolBool eol_keychain_get_line(eolLine output,eolKeychain *key)
{
  if (!key)return eolFalse;
  if (key->keyType != eolKeychainString)return eolFalse;
  if (key->keyValue == NULL)return eolFalse;
  eol_line_cpy(output,key->keyValue);
  return eolTrue;
}

void eol_keychain_hash_remove(eolKeychain *hash,char *key)
{
  GHashTable*hashtable = NULL;
  if (!hash)return;
  if (hash->keyType != eolKeychainHash)return;
  if (hash->keyValue == NULL)return;
  hashtable = (GHashTable*)hash->keyValue;
  g_hash_table_remove(hashtable,key);
}

void eol_keychain_hash_insert(eolKeychain *hash,char *key,eolKeychain *value)
{
  GHashTable*hashtable = NULL;
  if (!hash)return;
  if (hash->keyType != eolKeychainHash)return;
  if (hash->keyValue == NULL)return;
  hashtable = (GHashTable*)hash->keyValue;
  if (g_hash_table_lookup(hashtable,key) != NULL)
  {
    g_hash_table_replace(hashtable,g_strdup(key),value);
  }
  else
  {
    g_hash_table_insert(hashtable,g_strdup(key),value);
    hash->itemCount++;
  }
}

void eol_keychain_list_append(eolKeychain *list,eolKeychain *item)
{
  if (!list)return;
  if (list->keyType != eolKeychainList)return;
  list->keyValue = g_list_append(list->keyValue,item);
  list->itemCount++;
}

eolKeychain *eol_keychain_get_hash_value(eolKeychain *hash,eolLine key)
{
  GHashTable*hashtable = NULL;
  if (!hash)
  {    
    return NULL;
  }
  if (hash->keyType != eolKeychainHash)
  {    
    return NULL;
  }
  if (hash->keyValue == NULL)
  {
    return NULL;
  }
  hashtable = (GHashTable*)hash->keyValue;
  return g_hash_table_lookup(hashtable,key);
}

eolUint eol_keychain_get_hash_count(eolKeychain *list)
{
  if (!list)return 0;
  if (list->keyType != eolKeychainHash)return 0;
  if (list->keyValue == NULL)return 0;
  return list->itemCount;
}

eolUint eol_keychain_get_list_count(eolKeychain *list)
{
  if (!list)return 0;
  if (list->keyType != eolKeychainList)return 0;
  if (list->keyValue == NULL)return 0;
  return list->itemCount;
}

eolKeychain *eol_keychain_get_hash_nth(eolLine key, eolKeychain *hash, eolUint n)
{
  GHashTable *hashtable = NULL;
  GList *keys = NULL, *values = NULL;
  if (!hash)return NULL;
  if (hash->keyType != eolKeychainHash)return NULL;
  if (hash->keyValue == NULL)return NULL;
  hashtable = (GHashTable*)hash->keyValue;
  keys = g_hash_table_get_keys(hashtable);
  values = g_hash_table_get_values(hashtable);
  if ((!keys)||(!values))return NULL;
  eol_line_cpy(key,g_list_nth_data(keys,n));
  return g_list_nth_data(values,n);
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

eolBool eol_keychain_get_hash_value_as_uint(eolUint *output, eolKeychain *hash, eolLine key)
{
  eolUint temp = 0;
  eolLine keyValue;
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(keyValue,chain->keyValue);
  if (sscanf(keyValue,"%ui",&temp) != 1)return eolFalse;
  *output = temp;
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_int(eolInt *output, eolKeychain *hash, eolLine key)
{
  eolInt temp = 0;
  eolLine keyValue;
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(keyValue,chain->keyValue);
  if (sscanf(keyValue,"%i",&temp) != 1)return eolFalse;
  *output = temp;
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_float(eolFloat *output, eolKeychain *hash, eolLine key)
{
  eolFloat temp = 0;
  eolLine keyValue;
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(keyValue,chain->keyValue);
  if (sscanf(keyValue,"%f",&temp) != 1)return eolFalse;
  *output = temp;
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_bool(eolBool *output, eolKeychain *hash, eolLine key)
{
  eolKeychain *chain;
  eolInt boo;
  if ((!hash) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)
  {
    return eolFalse;
  }
  if (chain->keyType != eolKeychainString)return eolFalse;
  boo = eol_bool_from_string(chain->keyValue);
  if (boo == -1)return eolFalse;/*tag was not boolean*/
  *output = boo;
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_line(eolLine output, eolKeychain *hash, eolLine key)
{
  eolKeychain *chain;
  if ((!hash) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)
  {
    return eolFalse;
  }
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(output,chain->keyValue);
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_vec4d(eolVec4D *output, eolKeychain *hash, eolLine key)
{
  eolVec4D temp = {0,0,0,0};
  eolLine keyValue;
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(keyValue,chain->keyValue);
  if (sscanf(keyValue,"%lf,%lf,%lf,%lf",&temp.x,&temp.y,&temp.z,&temp.w) != 4)return eolFalse;
  eol_vec4d_copy((*output),temp);
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_vec3d(eolVec3D *output, eolKeychain *hash, eolLine key)
{
  eolVec3D temp = {0,0,0};
  eolLine keyValue;
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(keyValue,chain->keyValue);
  if (sscanf(keyValue,"%lf,%lf,%lf",&temp.x,&temp.y,&temp.z) != 3)return eolFalse;
  eol_vec3d_copy((*output),temp);
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_vec2d(eolVec2D *output, eolKeychain *hash, eolLine key)
{
  eolVec2D temp = {0,0};
  eolLine keyValue;
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(keyValue,chain->keyValue);
  if (sscanf(keyValue,"%lf,%lf",&temp.x,&temp.y) != 2)return eolFalse;
  eol_vec2d_copy((*output),temp);
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_orientation(eolOrientation *output, eolKeychain *hash, eolLine key)
{
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainHash)return eolFalse;
  /*now parse the hash further*/
  eol_keychain_get_hash_value_as_vec3d(&output->position,chain,"position");
  eol_keychain_get_hash_value_as_vec3d(&output->rotation,chain,"rotation");
  eol_keychain_get_hash_value_as_vec3d(&output->scale,chain,"scale");
  eol_keychain_get_hash_value_as_vec3d(&output->scale,chain,"color");
  eol_keychain_get_hash_value_as_float(&output->alpha,chain,"alpha");
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_rect(eolRect *output, eolKeychain *hash, eolLine key)
{
  eolRect temp = {0,0,0,0};
  eolLine keyValue;
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(keyValue,chain->keyValue);
  if (sscanf(keyValue,"%hi,%hi,%hu,%hu",&temp.x,&temp.y,&temp.w,&temp.h) != 4)return eolFalse;
  eol_rect_copy(output,temp);
  return eolTrue;
}

eolBool eol_keychain_get_hash_value_as_rectfloat(eolRectFloat *output, eolKeychain *hash, eolLine key)
{
  eolRectFloat temp = {0,0,0,0};
  eolLine keyValue;
  eolKeychain *chain;
  if ((!hash) || (!output) || (strlen(key) == 0))return eolFalse;
  chain = eol_keychain_get_hash_value(hash,key);
  if (!chain)return eolFalse;
  if (chain->keyType != eolKeychainString)return eolFalse;
  eol_line_cpy(keyValue,chain->keyValue);
  if (sscanf(keyValue,"%lf,%lf,%lf,%lf",&temp.x,&temp.y,&temp.w,&temp.h) != 4)return eolFalse;
  eol_rectf_copy(output,temp);
  return eolTrue;
}

void eol_keychain_print_string(eolKeychain *link,eolUint depth)
{
  int i;
  if ((!link)||(!link->keyValue))return;
  for (i = 0; i < depth;i++){printf("  ");}
  printf("%s\n",(char *)link->keyValue);
}

void eol_keychain_print_hash(eolKeychain *link,eolUint depth,eolBool listStart)
{
  int i;
  GList *keys = NULL;
  GList *values = NULL;
  if (!link)return;
  if (link->keyType != eolKeychainHash)return;
  keys = g_hash_table_get_keys((GHashTable*)link->keyValue);
  values = g_hash_table_get_values((GHashTable*)link->keyValue);
  if (!listStart)printf("\n");
  for (;(keys != NULL) && (values != NULL); keys = keys->next,values = values->next)
  {
    for (i = 0; i < depth;i++){printf("  ");}
    printf("%s :",(char *)keys->data);
    eol_keychain_print_link((eolKeychain *)values->data,depth + 1,eolFalse);
    printf("\n");
  }
}

void eol_keychain_print_list(eolKeychain *link,eolUint depth,eolBool listStart)
{
  int i;
  GList *it;
  if (!link)return;
  if (link->keyType != eolKeychainList)return;
  printf("\n");
  for (it = (GList *)link->keyValue;it != NULL;it = it->next)
  {
    for (i = 0; i < (depth - 1);i++){printf("  ");}
    printf("- ");
    eol_keychain_print_link((eolKeychain *)it->data,depth + 1,eolTrue);
    printf("\n");
  }
}

void eol_keychain_print_link(eolKeychain *link,eolUint depth,eolBool listStart)
{
  if (!link)return;
  switch(link->keyType)
  {
    case eolKeychainString:
      eol_keychain_print_string(link,depth);
      break;
    case eolKeychainList:
      eol_keychain_print_list(link,depth,listStart);
      break;
    case eolKeychainHash:
      eol_keychain_print_hash(link,depth,listStart);
      break;
  }
}

void eol_keychain_print(eolKeychain *chain)
{
  eol_keychain_print_link(chain,0,eolFalse);
}

/*eol@eof*/
