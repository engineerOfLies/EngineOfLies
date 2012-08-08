#include "eol_loader.h"
#include "eol_logger.h"
#ifdef __APPLE__
#include "fmemopen.h"
#endif
eolBool _eol_loader_initialized = eolFalse;

eolBool eol_loader_initialized()
{
	return _eol_loader_initialized;
}

void eol_loader_init()
{
  PHYSFS_init(NULL);
  PHYSFS_mount("./", NULL, 1);
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
  file->_fileMode = eolLoaderWriteBinary;
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
  if (EOLFILEENDIAN == EOLFILELISTTLEENDIAN)
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
  if (EOLFILEENDIAN == EOLFILELISTTLEENDIAN)
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
    if (EOLFILEENDIAN == EOLFILELISTTLEENDIAN)
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
    if (EOLFILEENDIAN == EOLFILELISTTLEENDIAN)
    {
      PHYSFS_writeULE16(file->_PSfile, (PHYSFS_uint16) data[i]);
    }
    else
    {
      PHYSFS_writeUBE16(file->_PSfile, (PHYSFS_uint16) data[i]);
    }
  }
}

/*eol@eof*/

