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
  PHYSFS_addToSearchPath("./", 1);
  PHYSFS_addToSearchPath("./system/pak0.pak", 1);
  _eol_loader_initialized = eolTrue;
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
  memset(file,0,sizeof(eolFile);
  
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

  memset(file,0,sizeof(eolFile);
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

/*eol@eof*/

