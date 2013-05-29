#include "eol_spawn.h"
#include "eol_logger.h"

eolSpawn *eol_spawn_clone(eolSpawn *in)
{
  eolSpawn *out;
  if (!in)return NULL;
  out = eol_spawn_new();
  if (!out)return NULL;
  eol_line_cpy(out->type,in->type);
  out->id = in->id;
  eol_orientation_copy(&out->ori,in->ori);
  out->keys = eol_keychain_clone(in->keys);
  return out;
}

eolKeychain *eol_spawn_build_keychain(eolSpawn *spawn)
{
  eolKeychain * spawnKey = NULL;
  if (!spawn)return NULL;
  spawnKey = eol_keychain_new_hash();
  if (!spawnKey)return NULL;
  eol_keychain_hash_insert(spawnKey,"type",eol_keychain_new_string(spawn->type));
  eol_keychain_hash_insert(spawnKey,"id",eol_keychain_new_uint(spawn->id));
  eol_keychain_hash_insert(spawnKey,"ori",eol_keychain_new_orientation(spawn->ori));
  eol_keychain_hash_insert(spawnKey,"keys",eol_keychain_clone(spawn->keys));
  return spawnKey;
}

eolBool eol_spawn_setup(eolSpawn *spawn)
{
  if (!spawn)return eolFalse;
  if (spawn->keys != NULL)return eolFalse;
  spawn->keys = eol_keychain_new_hash();
  if (spawn->keys == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "eol_spawn: failed too allocated spawn key map\n");
    return eolFalse;
  }
  return eolTrue;
}

eolSpawn *eol_spawn_create_from_keychain(eolKeychain *conf)
{
  eolSpawn *spawn;
  if (!conf)return NULL;
  spawn = eol_spawn_new();
  if (!spawn)return NULL;
  spawn->keys = eol_keychain_get_hash_value(conf,"keys");
  eol_keychain_get_hash_value_as_orientation(&spawn->ori, conf, "ori");
  eol_keychain_get_hash_value_as_uint(&spawn->id, conf, "id");
  eol_keychain_get_hash_value_as_line(spawn->type, conf, "type");
  return spawn;
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
  eol_keychain_destroy(spawn->keys);
  memset(spawn, 0, sizeof(eolSpawn));
}

void eol_spawn_delete_key(eolSpawn *spawn,eolWord key)
{
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  eol_keychain_hash_remove(spawn->keys,key);
}

void eol_spawn_key_list_move_to_top(eolSpawn *spawn,eolWord key,eolUint n)
{
  eolKeychain *list;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  list = eol_keychain_get_hash_value(spawn->keys,key);
  eol_keychain_list_move_nth_top(list, n);
}

void eol_spawn_delete_key_list_item(eolSpawn *spawn,eolWord key,eolUint n)
{
  eolKeychain *list;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  list = eol_keychain_get_hash_value(spawn->keys,key);
  eol_keychain_list_remove_nth(list,n);
}

void eol_spawn_add_key(eolSpawn *spawn,eolWord key,eolLine value)
{
  eolKeychain *valuePointer;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  valuePointer = eol_keychain_new_string(value);
  eol_keychain_hash_insert(spawn->keys,key,valuePointer);
}

void eol_spawn_add_key_list(eolSpawn *spawn,eolWord key,eolLine value)
{
  eolKeychain *valuePointer;
  eolKeychain *list;
  if (!spawn)return;
  if (spawn->keys == NULL)return;
  valuePointer = eol_keychain_new_string(value);
  list = eol_keychain_get_hash_value(spawn->keys,key);
  if (!list)
  {
    list = eol_keychain_new_list();
    eol_keychain_hash_insert(spawn->keys,key,list);
  }
  eol_keychain_list_append(list,valuePointer);
}

GString * eol_spawn_get_key(eolSpawn *spawn, eolWord key)
{
  eolKeychain *value;
  if (!spawn)return NULL;
  value = eol_keychain_get_hash_value(spawn->keys,key);
  if (value == NULL)return NULL;
  if (value->keyType != eolKeychainString)return NULL;
  return (GString *)value->keyValue;
}

GString * eol_spawn_get_key_list_item(eolSpawn *spawn, eolWord key,eolUint n)
{
  eolKeychain *value;
  eolKeychain *list;
  if (!spawn)return NULL;
  list = eol_keychain_get_hash_value(spawn->keys,key);
  value = eol_keychain_get_list_nth(list,n);
  if (value == NULL)return NULL;
  if (value->keyType != eolKeychainString)return NULL;
  return (GString *)value->keyValue;
}

eolBool eol_spawn_has_key(eolSpawn *spawn, eolWord key)
{
  if (!spawn)return eolFalse;
  if (eol_keychain_get_hash_value(spawn->keys,key) == NULL)return eolFalse;
  return eolTrue;
}

eolBool eol_spawn_get_key_uint(eolUint *out, eolSpawn *spawn,eolWord key,eolInt *n)
{
  eolUint temp;
  GString *string;
  eolKeychain *value;
  eolKeychain *list;
  if (!spawn)return eolFalse;
  if (!out)return eolFalse;
  if (n != NULL)
  {
    list = eol_keychain_get_hash_value(spawn->keys,key);
    value = eol_keychain_get_list_nth(list,*n);
  }
  else
  {
    value = eol_keychain_get_hash_value(spawn->keys,key);
  }
  if (value == NULL)return eolFalse;
  if (value->keyType != eolKeychainString)return eolFalse;
  string = (GString *)value->keyValue;
  if (sscanf(string->str,"%ui",&temp) == 0)return eolFalse;
  *out = temp;
  return eolTrue;
}
/*eol@eof*/
