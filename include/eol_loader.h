#ifndef _EOL_LOADER_H_
#define _EOL_LOADER_H_
/*
    Copyright 2012 Engineer of Lies
    This file is part of the Engine of Lies game engine library

    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "eol_types.h"
#include "eol_matrix.h"
#include "eol_keychain.h"
#include <stdio.h>
#include <physfs.h>

/**
 * @brief To keep the assets made by eol consistent across platforms, all saving
 * to file will be a set Endian-nes, regardless of platform.
 */
#define EOLFILEBIGENDIAN 0
#define EOLFILELISTTLEENDIAN 1
#define EOLFILEENDIAN EOLFILELISTTLEENDIAN

enum eolLoaderMode {
  eolLoaderRead,
  eolLoaderReadBinary,
  eolLoaderWriteBinary
};

typedef struct
{
  FILE        * file;       /**<file pointer as std FILE type, not supported on all architectures*/
  eolLine       filename;   /**<for logging purposes*/
  char        * _buffer;    /**<the file data stored while the file is open*/
  size_t        size;       /**<the size of the file data stored while the file is open*/
  PHYSFS_File * _PSfile;    /**<pointer to the physfs file handle*/
  eolUint       _fileMode;  /**<*read or write, expects eolLoaderMode*/
  eolBool       _fileEndian;/**<*what reads or writes style should be used*/
}eolFile;

/**
 * @brief sets up the physFS internal system
 */
void eol_loader_init();

/**
 * @brief loads config for the loader.  Unlike other system config functions, this
 * MUST be called AFTR eol_loader_init() is called.
 */
void eol_load_config();

/**
 * @brief checks initialization status of the loader
 * @return eolTrue if initialized, eolFalse otherwise
 */
eolBool eol_loader_initialized();

/**
 * @brief adds a path to the search path for file loading.
 * @param path the directory path "./system/paks/" or archive "./system/pak0.pak"
 * @param point optional. defaults to "./" can be provided.
 */
void eol_loader_add_search_path(char *filename,char *point);

/**
 * @brief opens a file for reading from the filesystem or a pak file
 * @param filename the file to load.
 * @return a pointer to the opened eolFile handle or NULL on failure
 */
eolFile * eol_loader_read_file(char *filename);

/**
* @brief opens a file for writing
* @param filename the file to load.
* @return a pointer to the opened eolFile handle or NULL on failure
*/
eolFile * eol_loader_write_file_binary(char *filename);

/**
* @brief opens a file for reading from the filesystem or a pak file
* @param filename the file to load.
* @return a pointer to the opened eolFile handle or NULL on failure
*/
eolFile *eol_loader_read_file_binary(char *filename);

/**
 * @brief closes an eolFile handle and cleans up allocated memory
 * @param file a pointer to the file pointer to close.  Set to NULL on success.
 */
void eol_loader_close_file(eolFile **file);

/*
 *** Binary File Operations ***
 */

/**
 * @brief write data to an open eolFile buffer.
 * does nothing if the file handle is NULL or opened for reading.
 * @param file the file handle to write to
 * @param data the data to be written.
 */
void eol_loader_write_uint_to_file(eolFile *file,eolUint data);

/**
 * @brief read data from an open eolFile buffer.
 * does nothing if the file handle is NULL or opened for writing.
 * @param data output. Left alone on error or eof
 * @param file the file handle to read from
 * @param returns eolTrue if success or eolFalse on error
 */
eolBool eol_loader_read_uint_from_file(eolUint * data, eolFile *file);

/**
* @brief write data to an open eolFile buffer.
* does nothing if the file handle is NULL or opened for reading.
* @param file the file handle to write to
* @param data the data to be written.
*/
void eol_loader_write_int_to_file(eolFile *file,eolInt data);

/**
* @brief write data to an open eolFile buffer.
* does nothing if the file handle is NULL or opened for reading.
* @param file the file handle to write to
* @param data the data to be written.
*/
void eol_loader_write_line_to_file(eolFile *file,eolLine data);

/**
 * @brief write data to an open eolFile buffer.
 * does nothing if the file handle is NULL or opened for reading.
 * @param file the file handle to write to
 * @param data the data to be written.
 */
void eol_loader_write_string_to_file(eolFile *file,char * data);

/**
 * @brief read string data from an open eolFile buffer.
 * does nothing if the file handle is NULL or opened for writing.
 * @param data a pointer to a char pointer.  If the char pointer is not null, it will be used
 * otherwise a buffer will be allocated for the string in question and must be
 * freed separately.
 * @param file the file handle to read from
 */
void eol_loader_read_string_from_file(char ** data, eolFile *file);

/**
* @brief write data to an open eolFile buffer.
* does nothing if the file handle is NULL or opened for reading.
* @param file the file handle to write to
* @param data the data to be written.
*/
void eol_loader_write_float_to_file(eolFile *file,eolFloat data);

/**
* @brief write data to an open eolFile buffer.
* does nothing if the file handle is NULL or opened for reading.
* @param file the file handle to write to
* @param data the data to be written.
*/
void eol_loader_write_vec3D_to_file(eolFile *file, eolVec3D data);

/**
* @brief write data to an open eolFile buffer.
* does nothing if the file handle is NULL or opened for reading.
* @param file the file handle to write to
* @param data the data to be written.
*/
void eol_loader_write_rect_to_file(eolFile *file, eolRect data);

/**
* @brief write data to an open eolFile buffer.
* does nothing if the file handle is NULL or opened for reading.
* @param file the file handle to write to
* @param data the data to be written.
*/
void eol_loader_write_rectf_to_file(eolFile *file, eolRectFloat data);

/**
* @brief write data to an open eolFile buffer.
* does nothing if the file handle is NULL or opened for reading.
* @param file the file handle to write to
* @param data the data to be written.
*/
void eol_loader_write_matrix_to_file(eolFile *file, eolMat4 data);

/**
* @brief write data to an open eolFile buffer.
* does nothing if the file handle is NULL or opened for reading.
* @param file the file handle to write to
* @param data the data to be written.
*/
void eol_loader_write_orientation_to_file(eolFile *file, eolOrientation data);

/**
 * @brief Saves the entire keychain passed to the open file.
 * @param link the root of the keychain you wish to save
 * @param file the file to save it to.
 */
void eol_loader_write_keychain_to_file(eolKeychain *link,eolFile *file);

eolKeychain *eol_loader_read_keychain_from_file(eolFile *file);

void eol_loader_keychain_load(eolKeychain **chain, char *filename);
eolKeychain *eol_loader_read_keychain_link(eolFile *file);
void eol_loader_write_keychain_link(eolKeychain *link,eolFile *file);


#endif

