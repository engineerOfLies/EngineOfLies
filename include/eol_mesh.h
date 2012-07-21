#ifndef _EOL_MESH_H_
#define _EOL_MESH_H_
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
#include "eol_sprite.h"

enum eolMeshDrawModes {
  eolMeshDrawWireframe = 1,
  eolMeshDrawMesh      = 2,
  eolMeshDrawSmooth    = 3,
  eolMeshDrawTextured  = 4,
  eolMeshDrawLighting  = 5,
  eolMeshDrawShaded    = 6
};

/**
 * vertex in 2D space.  Used in texture space.
 */
typedef eolFloat eolVert2D[2];

/**
 * face structure for mesh data.
 */
typedef struct
{
  eolInt vertices[3];  /**<index of vertex*/
  eolInt normals[3];  /**<index of vertex*/
  eolInt texels[3];  /**<index of vertex*/
}eolFace;

typedef struct
{
	eolLine  name;      /**<name of the bone*/
	eolInt   numVerts;  /**<how many vertices are associated with this bone*/
	eolInt   *vertices; /**<indices of vertices for this bone */
	eolFloat *weights;  /**<weight of each virtice for this bone.*/
	eolInt   _assignedV;
	eolInt   _assignedW;
}eolGroup;

typedef struct
{
	eolInt  _refCount;    /**<internal reference count for this mesh*/
  
  eolLine filename;     /**<filename of loaded mesh*/
  eolVec3D center;      /**<relative position of the mesh's center*/
  eolVec3D bounds;      /**<width, height and depth of mesh*/
  
  eolUint _numVertices;  /**<number of vertices that make up this mesh*/
  eolUint _numNormals;   /**<number of normals calculated for this mesh*/
  eolUint _numTexels;    /**<number of texture coordinates for this mesh*/
  eolUint _numFaces;     /**<number of faces that make up this mesh*/
  eolUint _numGroups;    /**<number of bones associated with this mesh*/
  
  eolVec3D *_rest;      /**<pointer to allocated vertex array at rest position*/
  eolVec3D *_normals;   /**<pointer to allocated normal array*/
  eolVert2D *_texels;    /**<pointer to allocated texture vertex array*/
  eolFace   *_faces;     /**<pointer to allocated triangle array*/
  eolGroup  *_groups;    /**<pointer to allocated bone array*/
  eolVec3D *_vertices;  /**<pointer to allocated vertex array*/
  eolInt    _curFrame;  /**<frame that is currently in the _vertices list*/
  
  eolBool _usesBuffer;   /**<if this mesh is using frame buffered arrays*/
  eolUint _vertexArray;  /**<index of frame buffered vertex array.  0 if not used*/
  eolUint _normalArray;  /**<index of frame buffered normal array.  0 if not used*/
  eolUint _texelArray;   /**<index of frame buffered texel array.  0 if not used*/
}eolMesh;

/**
 * @brief initializes internal structures and sets up eol_mesh for use.
 */
void eol_mesh_init();

/**
 * @brief frees all internal data and marks eol_mesh as uninitialized.
 */
void eol_mesh_deinit();

/**
 * @brief queries eol_mesh initialization status.
 *
 * @return eol_mesh initialization state
 */
eolBool eol_mesh_initialized();

/**
 * @brief loads the eol_mesh configuration from file or loads defaults if no
 * file found.
 */
void eol_mesh_load_config();

/**
 * @brief returns an pointer to usable internal mesh data.  Data is zero set and
 * reference count set to one.
 *
 * @return a new useful mesh data structure or NULL if out of memory or other
 * error.
 */
eolMesh * eol_mesh_new();

/**
 * @brief frees the mesh data provided.  Sets the pointer to NULL to prevent
 * accidental access.  Note that it merely decriments reference count for the
 * mesh, as others may be using the same data.  Actual freeing can be deferred.
 */
void eol_mesh_free(eolMesh **mesh);

/**
 * @brief loads mesh data from file.  It will try to get the data from binary
 * format first,  if it is not available it will load from .obj.  If not found,
 * out of memory or other error it will return NULL.
 *
 * @param filename the path included filename, without the extension.  It will
 * automattically check for the .emesh or .obj files.
 * @return the loaded mesh data or NULL on error.
 */
eolMesh * eol_mesh_load(char *filename);

/**
 * @brief draws the provided mesh to the active draw buffer.
 *
 * @param mesh the mesh to draw
 * @param position the x,y,z position to draw it at
 * @param rotation the x,y,z rotation
 * @param scale    the x,y,z scaling factor (1,1,1 is no scaling)
 * @param color    the r,g,b color shifting factor.  (1,1,1 is no color shift)
 * @param alpha    the transparency to draw it with (1 is opaque, 0 is no
 * drawing)
 * @param skin     the loaded sprite to use as the texture.
 */
void eol_mesh_draw(
    eolMesh *mesh,
    eolVec3D position,
    eolVec3D rotation,
    eolVec3D scale,
    eolVec3D color,
    eolFloat alpha,
    eolSprite *skin
  );

/**
 * @brief searches the mesh provided for the vertex group by the name.
 *
 * @param name the name to search for.
 * @param mesh the mesh to search
 *
 * @return NULL if not found or error.  a pointer to the vertex group in the
 * mesh otherwise.
 */
eolGroup * eol_mesh_get_group_by_name(char *name,eolMesh *mesh);

#endif

