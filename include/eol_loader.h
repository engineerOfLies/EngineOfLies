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
#include <stdio.h>
#include <physfs.h>

typedef struct
{
  FILE        * file;
  char        * _buffer;
  size_t        size;
  PHYSFS_File * _PSfile;
}eolFile;

void eol_loader_init();
eolBool eol_loader_initialized();
eolFile * eol_loader_read_file(char *filename);
void eol_loader_close_file(eolFile **file);

#endif

