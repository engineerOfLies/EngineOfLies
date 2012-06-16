#ifndef _EOL_ARMATURE_H_
#define _EOL_ARMATURE_H_
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
#include "eol_mesh.h"
#include "eol_types.h"
#include "eol_matrix.h"

/**
 * @brief this composes the skeletal structure of the armature at rest.
 */
typedef struct S_Bone
{
  eolLine    name;         /**<name of the bone*/
  eolInt     index;        /**<the index of this bone*/
  eolGroup * meshGroup;    /**<mesh group for this bone*/
  eolBool    hasParent;    /**<true if the bone has a parent*/
  eolInt     parentIndex;  /**<index of the bone's parent*/
  eolInt     numChildren;  /**<how many child bones of this one*/
  eolInt   * children;     /**<allocated array of child bone indices*/
  eolMat4    matrix;       /**<a bone's at rest postion,rotation and size*/
}eolBone;

typedef struct
{
  eolLine    name;
  eolUint    boneIndex;
  eolMat4    matrix;
  eolVec3D   scale;
}eolPoseBone;

typedef struct
{
	eolPoseBone *bones;
}eolPose;

typedef struct
{
	eolInt   _refCount;  /**< internal reference count for this armature*/
  eolLine   filename;  /**< name of the file loaded */
  eolLine   name;      /**< name of the armature*/

  eolMat4   matrix;    /**< Armature matrix*/

  eolInt   boneCount;  /**< number of bones in the armature */
  eolInt   frameCount; /**< number of positions the bones are posed in */

  eolBone * root;      /**< pointer to the root of the skeleton */ 
  eolBone * _bones;    /**< allocated list of bones */
  eolPose * _poses;    /**< allocated list of bone positions */
  eolBool   _isLinked; /**< if this armature has been linked to a mesh*/
}eolArmature;

/**
 * @brief loads up defaults for armatures.
 */
void eol_armature_load_config();

/**
 * @brief initializes internal structures for armature system
 */
void eol_armature_init();

/**
 * @brief checks initialization status for armature system
 *
 * @return eolTrue if initialized, eolFalse otherwise
 */
eolBool eol_armature_initialized();

/**
 * @brief loads an armature structure from file
 *
 * @param filename the name of the armature file to load.
 * @return a pointer to loaded armature date or NULL on error.
 */
eolArmature * eol_armature_load(char *filename);

/**
 * @brief returns the armature information to the system.  It will only be freed
 * from memory when it is no longer needed.  For safety, it sets the pointer
 * passed to NULL.
 *
 * @param arm a pointer to the armature pointer that will be returned.  Your
 * pointer will be set to NULL when done.
 */
void eol_armature_free(eolArmature **arm);

/**
 * @brief delete any unreferenced but allocated armature data.  This will be
 * needed to run between levels and on demand.
 */
void eol_armature_cleanup();

/**
 * @brief frees all allocated memory and closes downthe armature system.
 */
void eol_armature_close();

/**
 * @brief deforms the drawing vertices of the model based on the bone pose for 
 * the specified frame
 * 
 * @param arm the armature to use for deforming
 * @param mesh the mesh to deform
 * @param frame which frame to deform for.  Note that if the frame it outside
 * the range of frames for arm, no change will occur.
 */
void
eol_armature_deform_mesh(
    eolArmature *arm,
    eolMesh     *mesh,
    eolUint      frame
);

/**
 * @brief set up references between mesh and armature.
 *
 * @param arm the armature to be linked
 * @param mesh the mesh to be linked
 */
void
eol_armature_link_mesh(
    eolArmature   *arm,
    eolMesh       *mesh
);

/**
 * @brief prints out to log the bone structure
 */
void
eol_armature_print_bone_hierarchy(
  eolArmature *arm
);

#endif

