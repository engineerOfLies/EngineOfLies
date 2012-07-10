#include "eol_spawn.h"
#include "eol_logger.h"
#include <glib/gstring.h>

void eol_g_string_free(GString *string)
{
  g_string_free(string,eolTrue);
}
/*
TODO:  need to actually store type information in the hash before this can work
void eol_spawn_copy(eolSpawn *out, eolSpawn in)
{
  GHashTableIter *iter = NULL;
  GString *key,*value;
  if (!out)return eolFalse;
  if (out->keys != NULL)
  {
    eol_spawn_free(out);
  }
  if (out->keys == NULL)
  {
    if (eol_spawn_setup(out) == eolFalse)return;
  }
  g_hash_table_iter_init(iter,in.keys);
  while(g_hash_table_iter_next(iter,key,value))
  {
    g_hash_table_insert(spawn->keys,g_strdup(key),g_strdub(value));
  }
}*/

eolBool eol_spawn_setup(eolSpawn *spawn)
{
  if (!spawn)return eolFalse;
  if (spawn->keys != NULL)return eolFalse;
  spawn->keys =
  g_hash_table_new_full(g_str_hash,
                        g_str_equal,
                        (GDestroyNotify)eol_g_string_free,
                        (GDestroyNotify)eol_g_string_free);
  if (spawn->keys == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_spawn: failed too allocated spawn key map\n");
    return eolFalse;
  }
  return eolTrue;
}

eolSpawn * eol_spawn_new()
{
  eolSpawn *spawn = NULL;
  spawn = (eolSpawn *)malloc(sizeof(eolSpawn));
  if (spawn == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_spawn:failed to allocated a new eolSpawn\n");
    return NULL;
  }
  memset(spawn,0,sizeof(eolSpawn));
  if (eol_spawn_setup(spawn) == eolFalse)
  {
    free(spawn);
    return NULL;
  }
  return spawn;
}

void eol_spawn_delete(eolSpawn **spawn)
{
  if ((!spawn) || (!*spawn))return;
  eol_spawn_free(*spawn);
  free(*spawn);
  *spawn = NULL;
}

void eol_spawn_free(eolSpawn *spawn)
{
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  g_hash_table_destroy(spawn->keys);
  memset(spawn, 0, sizeof(eolSpawn));
}

void eol_spawn_delete_key(eolSpawn *spawn,eolWord key)
{
  GString *keyString;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  keyString = g_string_new_len(key,EOLWORDLEN);
  g_hash_table_remove(spawn->keys,keyString);
  g_string_free(keyString,eolTrue);
}

void eol_spawn_key_list_move_up(eolSpawn *spawn,eolWord key,eolUint n)
{
  GString *keyString = NULL;
  GList *list = NULL;
  GList *mover;
  GList *prev;
  GString *data;
  if (n == 0)return;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  keyString = g_string_new_len(key,EOLWORDLEN);
  list = g_hash_table_lookup(spawn->keys,keyString);
  if (list)
  {
    mover = g_list_nth(list,n);
    if (mover != NULL)
    {
      prev = mover->prev;
      data = mover->data;
      mover = g_list_delete_link(list,mover);
      if (mover)
      {
        list = g_list_insert_before(list,prev,data);
      }
    }
  }
  g_string_free(keyString,eolTrue);
}

void eol_spawn_delete_key_list_item(eolSpawn *spawn,eolWord key,eolUint n)
{
  GString *keyString = NULL;
  GList *list = NULL;
  GList *dead;
  GString *lData = NULL;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  keyString = g_string_new_len(key,EOLWORDLEN);
  list = g_hash_table_lookup(spawn->keys,keyString);
  if (list)
  {
    lData = g_list_nth_data(list,n);
    if (lData != NULL)
    {
      dead = g_list_remove(list,lData);
      g_string_free(lData,eolTrue);
      if (dead)g_list_free(dead);
    }
  }
  g_string_free(keyString,eolTrue);
}

void eol_spawn_delete_key_list(eolSpawn *spawn,eolWord key)
{
  GString *keyString = NULL;
  GList *list = NULL;
  GList *l = NULL;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  keyString = g_string_new_len(key,EOLWORDLEN);
  list = g_hash_table_lookup(spawn->keys,keyString);
  if (list)
  {
    for (l = list;l != NULL; l = l->next)
    {
      g_string_free(l->data,eolTrue);
    }
    g_list_free(list);
  }
  g_string_free(keyString,eolTrue);
}

void eol_spawn_add_key(eolSpawn *spawn,eolWord key,eolLine value)
{
  GString *keyString;
  GString *valueString;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  keyString = g_string_new_len(key,EOLWORDLEN);
  valueString = g_string_new_len(key,EOLLINELEN);
  if (g_hash_table_lookup(spawn->keys,keyString) != NULL)
  {
    g_hash_table_replace(spawn->keys,keyString,valueString);
  }
  else
  {
    g_hash_table_insert(spawn->keys,keyString,valueString);
  }
}

void eol_spawn_add_key_list(eolSpawn *spawn,eolWord key,eolLine value)
{
  GString *keyString;
  GString *valueString;
  GList *list;
  if (!spawn)return;
  if (!spawn->keys)return;
  keyString = g_string_new_len(key,EOLWORDLEN);
  valueString = g_string_new_len(key,EOLLINELEN);
  list = g_hash_table_lookup(spawn->keys,keyString);
  if (list == NULL)
  {
    list = g_list_append(list,valueString);
    g_hash_table_insert(spawn->keys,keyString,list);
  }
  else
  {
    list = g_list_append(list,valueString);
  }
}

GString * eol_spawn_get_key(eolSpawn *spawn, eolWord key)
{
  GString *value = NULL;
  GString *keyString = NULL;
  if ((!spawn) || (!spawn->keys))return NULL;
  keyString = g_string_new_len(key,EOLWORDLEN);
  value = g_hash_table_lookup(spawn->keys,keyString);
  g_string_free(keyString,eolTrue);
  return value;
}

eolBool eol_spawn_has_key(eolSpawn *spawn, eolWord key)
{
  if (eol_spawn_get_key(spawn,key) == NULL)return eolFalse;
  return eolTrue;
}

/*eol@eof*/
