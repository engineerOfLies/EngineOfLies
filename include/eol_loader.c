#include "eol_loader.h"
#include "eol_logger.h"
#ifdef __APPLE__
#include "fmemopen.h"
#endif
#include <glib/ghash.h>

eolBool _eol_loader_initialized = eolFalse;

eolBool eol_loader_initialized()
{
	return _eol_loader_initialized;
}

void eol_loader_init()
{
  PHYSFS_init(NULL);
  PHYSFS_mount("./", NULL, 1);
  PHYSFS_setWriteDir("./");
  _eol_loader_initialized = eolTrue;
}

void eol_load_config()
{
  /*TODO: this*/
}

void eol_loader_add_search_path(char *filename,char *point)
{
  if (!filename)return;
  if (!_eol_loader_initialized)return;
  PHYSFS_mount(filename,point, 1);
}


eolFile *eol_loader_read_file_binary(char *filename)
{
  eolFile *file;
  if (!_eol_loader_initialized)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: not initialized\n"
      );
    return NULL;
  }
  if (filename == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: passed NULL data to load from object\n"
      );
    return NULL;
  }
  file = (eolFile *)malloc(sizeof(eolFile));
  if (file == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: unable to allocate eolFile handle\n"
      );
    return NULL;
  }
  memset(file,0,sizeof(eolFile));
  
  file->_PSfile = PHYSFS_openRead(filename);
  if(file->_PSfile == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader:Unable to open file %s!\n",
        filename);
    free(file);
    return NULL;
  }
  file->_fileMode = eolLoaderReadBinary;
  eol_line_cpy(file->filename,filename);
  eol_loader_read_uint_from_file((eolUint *)&file->_fileEndian,file);
  return file;
}

eolFile *eol_loader_write_file_binary(char *filename)
{
  eolFile *file;
  if (!_eol_loader_initialized)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: not initialized\n"
      );
    return NULL;
  }
  if (filename == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: passed NULL data to load from object\n"
      );
    return NULL;
  }
  file = (eolFile *)malloc(sizeof(eolFile));
  if (file == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: unable to allocate eolFile handle\n"
      );
    return NULL;
  }
  memset(file,0,sizeof(eolFile));

  file->_PSfile = PHYSFS_openWrite(filename);
  if(file->_PSfile == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader:Unable to open file %s!\n",
        filename);
    free(file);
    return NULL;
  }
  file->_fileMode = eolLoaderWriteBinary;
  eol_line_cpy(file->filename,filename);
  eol_loader_write_uint_to_file(file,EOLFILEENDIAN);
  file->_fileEndian = EOLFILEENDIAN;
  return file;
}

eolFile *eol_loader_read_file(char *filename)
{
	eolFile *file;
  if (!_eol_loader_initialized)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: not initialized\n"
      );
  	return NULL;
  }
  if (filename == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: passed NULL data to load from object\n"
      );
  	return NULL;
  }
  file = (eolFile *)malloc(sizeof(eolFile));
  if (file == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: unable to allocate eolFile handle\n"
      );
  	return NULL;
  }

  memset(file,0,sizeof(eolFile));
  file->_PSfile = PHYSFS_openRead(filename);
  if(file->_PSfile == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader:Unable to open file %s!\n",
        filename);
    free(file);
    return NULL;
  }
  file->size = PHYSFS_fileLength(file->_PSfile);
  file->_buffer = (char *)malloc(file->size);
  if(file->_buffer == NULL)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader:Unable to allocate space for %s file reading!\n",
        filename);
    PHYSFS_close(file->_PSfile);
    free(file);
    return NULL;
  }
  PHYSFS_read(file->_PSfile, file->_buffer, file->size, 1);
  file->file = fmemopen (file->_buffer, file->size, "r");

  if (!file->file)
  {
    eol_logger_message(
        EOL_LOG_ERROR,
        "eol_loader: failed to open file: %s\n",
        filename);
    PHYSFS_close(file->_PSfile);
    free(file->_buffer);
    free(file);
    return NULL;
  }
  file->_fileMode = eolLoaderRead;
  eol_line_cpy(file->filename,filename);
  return file;
}

void eol_loader_close_file(eolFile **file)
{
	if ((!file) ||
		 (!*file))
  {
  	return;
  }
  if ((*file)->file != NULL)
    fclose((*file)->file);
  if ((*file)->_PSfile != NULL)
    PHYSFS_close((*file)->_PSfile);
  if ((*file)->_buffer != NULL)
    free((*file)->_buffer);
	*file = NULL;
}

void eol_loader_write_uint_to_file(eolFile *file,eolUint data)
{
  if (!file)return;
  if (file->_fileMode != eolLoaderWriteBinary)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_loader:file not opened for writing\n");
    return;
  }
  if (file->_fileEndian == EOLFILELISTTLEENDIAN)
  {
    PHYSFS_writeULE32(file->_PSfile, (PHYSFS_uint32) data);
  }
  else
  {
    PHYSFS_writeUBE32(file->_PSfile, (PHYSFS_uint32) data);
  }
}

void eol_loader_write_int_to_file(eolFile *file,eolInt data)
{
  if (!file)return;
  if (file->_fileMode != eolLoaderWriteBinary)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_loader:file not opened for writing\n");
    return;
  }
  if (file->_fileEndian == EOLFILELISTTLEENDIAN)
  {
    PHYSFS_writeSLE32(file->_PSfile, (PHYSFS_sint32) data);
  }
  else
  {
    PHYSFS_writeSBE32(file->_PSfile, (PHYSFS_sint32) data);
  }
}

void eol_loader_write_line_to_file(eolFile *file,eolLine data)
{
  int i;
  if (!file)return;
  if (file->_fileMode != eolLoaderWriteBinary)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_loader:file not opened for writing\n");
    return;
  }
  for (i = 0;i < EOLLINELEN;i++)
  {
    if (file->_fileEndian == EOLFILELISTTLEENDIAN)
    {
      PHYSFS_writeULE16(file->_PSfile, (PHYSFS_uint16) data[i]);
    }
    else
    {
      PHYSFS_writeUBE16(file->_PSfile, (PHYSFS_uint16) data[i]);
    }
  }
}

void eol_loader_write_string_to_file(eolFile *file,char * data)
{
  int i;
  eolUint len;
  if ((!file) || (!data))return;
  if (file->_fileMode != eolLoaderWriteBinary)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_loader:file not opened for writing\n");
    return;
  }
  len = strlen(data);
  /*prepend string lenth*/
  eol_loader_write_uint_to_file(file,len);
  for (i = 0;i < len;i++)
  {
    if (file->_fileEndian == EOLFILELISTTLEENDIAN)
    {
      PHYSFS_writeULE16(file->_PSfile, (PHYSFS_uint16) data[i]);
    }
    else
    {
      PHYSFS_writeUBE16(file->_PSfile, (PHYSFS_uint16) data[i]);
    }
  }
}

void eol_loader_read_uint_from_file(eolUint * data, eolFile *file)
{
  if ((!file) || (!data))return;
  if (file->_fileMode != eolLoaderReadBinary)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_loader:file %s not opened for reading in binary\n",
      file->filename);
    return;
  }
  if (file->_fileEndian == EOLFILELISTTLEENDIAN)
  {
    PHYSFS_readULE32(file->_PSfile, (PHYSFS_uint32 *)data);
  }
  else
  {
    PHYSFS_readUBE32(file->_PSfile, (PHYSFS_uint32 *)data);
  }
}

void eol_loader_read_int_from_file(eolInt * data, eolFile *file)
{
  if ((!file) || (!data))return;
  if (file->_fileMode != eolLoaderReadBinary)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_loader:file %s not opened for reading in binary\n",
      file->filename);
     return;
  }
  if (file->_fileEndian == EOLFILELISTTLEENDIAN)
  {
    PHYSFS_readSLE32(file->_PSfile, (PHYSFS_sint32 *)data);
  }
  else
  {
    PHYSFS_readSBE32(file->_PSfile, (PHYSFS_sint32 *)data);
  }
}

void eol_loader_read_string_from_file(char ** data, eolFile *file)
{
  eolUint len = 0;
  int i;
  PHYSFS_uint16 temp;
  if ((!file) || (!data))return;
  if (file->_fileMode != eolLoaderReadBinary)
  {
    eol_logger_message(
      EOL_LOG_WARN,
      "eol_loader:file %s not opened for reading in binary\n",
      file->filename);
    return;
  }
  /*get prepend string lenth*/
  eol_loader_read_uint_from_file(&len, file);
  if (len == 0)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_loader:expected string from file: %s, has zero length.\n",
      file->filename);
    return;
  }
  if (*data == NULL)  /*otherwise assume its safe to write to*/
  {
    *data = (char*)malloc(sizeof(char)*(len + 1));
    if (*data == NULL)
    {
      eol_logger_message(
        EOL_LOG_INFO,
        "eol_loader:failed to allocate string buffer for reading from file %s.\n",
        file->filename);
      return;
    }
  }
  for (i = 0; i < len;i++)
  {
    if (file->_fileEndian == EOLFILELISTTLEENDIAN)
    {
      PHYSFS_readULE16(file->_PSfile, &temp);
    }
    else
    {
      PHYSFS_readUBE16(file->_PSfile, &temp);
    }
    (*data)[i] = (char)temp;
  }
  (*data)[len] = '\0';
}

/*saving and loading:*/

void eol_loader_write_keychain_hash(eolKeychain *link,eolFile *file)
{
  GList *keys = NULL;
  GList *values = NULL;
  if (!link)return;
  if (link->keyType != eolKeychainHash)return;
  /*
  type
  count
  key
  value - expand for keychain type
  key
  value
  ...
  */
  eol_loader_write_uint_to_file(file,link->keyType);
  eol_loader_write_uint_to_file(file,link->itemCount);
  keys = g_hash_table_get_keys((GHashTable*)link->keyValue);
  values = g_hash_table_get_values((GHashTable*)link->keyValue);
  for (;(keys != NULL) && (values != NULL); keys = keys->next,values = values->next)
  {
    eol_loader_write_string_to_file(file,(char *)keys->data);
    eol_loader_write_keychain_link((eolKeychain *)values->data,file);
  }
}

void eol_loader_write_keychain_list(eolKeychain *link,eolFile *file)
{
  GList *it;
  if (!link)return;
  if (link->keyType != eolKeychainList)return;
  eol_loader_write_uint_to_file(file,link->keyType);
  eol_loader_write_uint_to_file(file,link->itemCount);
  for (it = (GList *)link->keyValue;it != NULL;it = it->next)
  {
    eol_loader_write_keychain_link(it->data,file);
  }
}

void eol_loader_write_keychain_string(eolKeychain *link,eolFile *file)
{
  if (!link)return;
  if (link->keyType != eolKeychainString)return;
  /*
  type
  string (which is len,chars)
  */
  eol_loader_write_uint_to_file(file,link->keyType);
  eol_loader_write_string_to_file(file,link->keyValue);
}

void eol_loader_write_keychain_link(eolKeychain *link,eolFile *file)
{
  if ((!link) || (!file))return;
  switch(link->keyType)
  {
    case eolKeychainString:
      eol_loader_write_keychain_string(link,file);
      break;
    case eolKeychainList:
      eol_loader_write_keychain_list(link,file);
      break;
    case eolKeychainHash:
      eol_loader_write_keychain_hash(link,file);
      break;
    default:
      eol_logger_message(
        EOL_LOG_WARN,
        "eol_loader:unsupported keychain type for writing: %i\n",
        link->keyType);
  }
}

void eol_loader_keychain_save(eolKeychain *chain, char *filename)
{
  eolFile *file;
  if ((!chain) || (!filename))return;
  file = eol_loader_write_file_binary(filename);
  if (file != NULL)
  {
    eol_loader_write_keychain_link(chain,file);
    eol_loader_close_file(&file);
  }
}
/*
*** loading from file ***
*/

eolKeychain *eol_loader_read_keychain_hash(eolFile *file)
{
  int i;
  char * buffer = NULL;  /*all keys should be limited to an eolLine*/
  eolKeychain *link = NULL;
  eolUint itemCount = 0;
  /*
  count
  key
  value - expand for keychain type
  key
  value
  ...
  */
  eol_loader_read_uint_from_file(&itemCount,file);
  if (itemCount == 0)return NULL;
  link = eol_keychain_new_hash();
  if (link == NULL)return NULL;
  for (i= 0;i < itemCount; i++)
  {
    if (buffer != NULL)
    {
      free(buffer);
      buffer = NULL;
    }
    eol_loader_read_string_from_file(&buffer,file);
    if (strlen(buffer) == 0)
    {
      eol_logger_message(
        EOL_LOG_WARN,
        "eol_loader:missing hash key in file: %s\nexpected %i keys, failed on %i\n",
        file->filename,
        itemCount,
        i);
      return link;
    }
    eol_keychain_hash_insert(link,buffer,eol_loader_read_keychain_link(file));
  }
  return link;
}

eolKeychain *eol_loader_read_keychain_list(eolFile *file)
{
  int i;
  eolKeychain *link = NULL;
  eolUint itemCount = 0;
  eol_loader_read_uint_from_file(&itemCount,file);
  if (!itemCount)return NULL;
  link = eol_keychain_new_list();
  if (link == NULL)return NULL;
  for (i = 0;i < itemCount;i++)
  {
    eol_keychain_list_append(link,eol_loader_read_keychain_link(file));
  }
  return link;
}

eolKeychain *eol_loader_read_keychain_string(eolFile *file)
{
  eolKeychain * link = NULL;
  char *buffer = NULL;
  eol_loader_read_string_from_file((char **)&buffer,file);
  if ((buffer == NULL) || (strlen(buffer) == 0))return NULL;
  link = eol_keychain_new_string(buffer);
  free(buffer);
  return link;
}

eolKeychain *eol_loader_read_keychain_link(eolFile *file)
{
  eolUint newtype = 0;
  if (!file)return NULL;
  eol_loader_read_uint_from_file(&newtype,file);
  switch(newtype)
  {
    case eolKeychainString:
      return eol_loader_read_keychain_string(file);
    case eolKeychainHash:
      return eol_loader_read_keychain_hash(file);
    case eolKeychainList:
      return eol_loader_read_keychain_list(file);
    default:
      eol_logger_message(
        EOL_LOG_WARN,
        "eol_loader:unsupported keychain type for reading: %i\n",
        newtype);
  }
  return NULL;
}

void eol_loader_keychain_load_from_file(eolKeychain **chain, char *filename)
{
  eolFile *file;
  if ((!chain) || (!filename))return;
  file = eol_loader_read_file_binary(filename);
  if (file != NULL)
  {
    *chain = eol_loader_read_keychain_link(file);
    eol_loader_close_file(&file);
  }
}

/*eol@eof*/

