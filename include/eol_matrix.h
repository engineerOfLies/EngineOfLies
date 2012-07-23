#ifndef __EOL_MATRIX__
#define __EOL_MATRIX__
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
#include "eol_mesh.h"

/**
 * @brief a 4x4 double matrix useful in a lot of 3D programming.
 */
typedef eolDouble eolMat4[4][4];

/**
 * @brief make a matrix with the values provided.
 */
void m_eol_mat4(
    eolMat4 m,
    float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34,
    float m41, float m42, float m43, float m44
  );

/**
 * @brief multiply the vertex by the matrix
 *
 * @param out the output transformed vertex
 * @param mat the matrix to apply to the vertex
 * @param ver the vertex to multiply by the matrix
 */
void eol_mat4_mult_vert(
    eolVec3D * out,
    eolMat4    mat,
    eolVec3D   ver
  );
  
void eol_mat4_mult_vert_factor(
    eolVec3D * out,
    eolMat4    mat,
    eolVec3D   vec,
    float      f
  );


/**
 * @brief creates an identity matrix
 *
 * @param m the matrix that will be set to an identity
 */
void eol_mat4_identity(eolMat4 m);

/**
 * @brief multiplay one matrix by another
 *
 * @param out the result is stored here
 * @param m2 one of the multiplicants
 * @param m3 one of the multiplicants
 */
void eol_mat4_mult_mat4(
    eolMat4 out,
    eolMat4 m2,
    eolMat4 m3
  );

/**
 * @brief copy one matrix onto another
 *
 * @param d the destination for the copy
 * @param s the source for the copy
 */
void eol_mat4_copy(
    eolMat4 d,
    eolMat4 s
  );

/**
 * @brief prints out the information in a matrix to logger info
 *
 * @param mat the matrix to print out
 */
void eol_mat4_print(
    eolMat4 mat
  );

/**
 * @brief Uses matrix multiplication to rotate the input vector about the rotation.
 *
 * @param out output.  Pointer to where the result will be stored.
 * @param rot the rotation vector
 * @param in  the input vector, the vector to be rotated.
 */
void eol_matrix_rotate_vec3d(eolVec3D *out,eolVec3D rot,eolVec3D in);

#endif

