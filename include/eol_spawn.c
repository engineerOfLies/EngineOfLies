#include "eol_spawn.h"
#include "eol_logger.h"
#include "glib/gstring.h"

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
  return spawn;
}

void eol_spawn_free(eolSpawn *spawn)
{
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  g_hash_table_destroy(spawn->keys);
  memset(spawn, 0, sizeof(eolSpawn));
}

void eol_spawn_add_key(eolSpawn *spawn,eolWord key,eolLine value)
{
  GString *keyString;
  GString *valueString;
  if (!spawn)return;
  if (spawn->keys == NULL)
  {
    spawn->keys = g_hash_table_new_full(g_str_hash,g_str_equal,g_free,g_free);
    if (spawn->keys == NULL)
    {
      eol_logger_message(
        EOL_LOG_ERROR,
        "eol_spawn: failed too allocated spawn key map\n");
    }
  }
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
