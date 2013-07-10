#include "eol_matrix.h"
#include "eol_logger.h"

void m_eol_mat4(
    eolMat4 m,
    float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34,
    float m41, float m42, float m43, float m44
  )
{
  m[0][0] = m11;
  m[0][1] = m12;
  m[0][2] = m13;
  m[0][3] = m14;
  m[1][0] = m21;
  m[1][1] = m22;
  m[1][2] = m23;
  m[1][3] = m24;
  m[2][0] = m31;
  m[2][1] = m32;
  m[2][2] = m33;
  m[2][3] = m34;
  m[3][0] = m41;
  m[3][1] = m42;
  m[3][2] = m43;
  m[3][3] = m44;
}

void eol_mat4_mult_vec4d(
  eolVec4D * out,
  eolMat4    mat,
  eolVec4D   vec
)
{
  eolFloat x,y,z,w;
  eolFloat ox,oy,oz,ow;
  if (!out)return;
  x=vec.x;
  y=vec.y;
  z=vec.z;
  w=vec.w;
  ox=x*mat[0][0] + y*mat[1][0] + mat[2][0]*z + mat[3][0]*w;
  oy=x*mat[0][1] + y*mat[1][1] + mat[2][1]*z + mat[3][1]*w;
  oz=x*mat[0][2] + y*mat[1][2] + mat[2][2]*z + mat[3][2]*w;
  ow=x*mat[0][3] + y*mat[1][3] + mat[2][3]*z + mat[3][3]*w;
  out->x = ox;
  out->y = oy;
  out->z = oz;
  out->w = ow;
}

void eol_mat4_from_mat16(
  eolMat4 *out,
  GLdouble in[16])
{
  if(!out)return;
  m_eol_mat4(*out,
             in[0], in[1], in[2], in[3],
             in[4], in[5], in[6], in[7],
             in[8], in[9], in[10],in[11],
             in[12],in[13],in[14],in[15]);
}

void eol_mat4_mult_vec3d(
  eolVec3D * out,
  eolMat4    mat,
  eolVec3D   vec3d
)
{
  eol_mat4_mult_vert(
    out,
    mat,
    vec3d
  );
}

void eol_mat4_mult_vert(
    eolVec3D * out,
    eolMat4    mat,
    eolVec3D   vec
  )
{
  eolFloat x,y,z;
  eolFloat ox,oy,oz;
  if (!out)return;
  x=vec.x;
  y=vec.y;
  z=vec.z;
  ox=x*mat[0][0] + y*mat[1][0] + mat[2][0]*z + mat[3][0];
  oy=x*mat[0][1] + y*mat[1][1] + mat[2][1]*z + mat[3][1];
  oz=x*mat[0][2] + y*mat[1][2] + mat[2][2]*z + mat[3][2];
  out->x = ox;
  out->y = oy;
  out->z = oz;
}

void eol_mat4_print(
    eolMat4 mat
  )
{
  int i,j;
  eol_logger_message(
    EOL_LOG_INFO,
    "matrix:\n");
  for (j = 0; j < 4; j++)
  {
    for (i = 0; i < 4; i++)
    {
      eol_logger_message(
        EOL_LOG_INFO,
        "\t%f",mat[j][i]);     
    }
    eol_logger_message(
      EOL_LOG_INFO,
      "\n");
  }
}

void eol_mat4_mult_vert_factor(
    eolVec3D * out,
    eolMat4    mat,
    eolVec3D   vec,
    float      f
  )
{
  eolFloat x,y,z;
  eolFloat ox,oy,oz;
  x=vec.x;
  y=vec.y;
  z=vec.z;
  ox=(x*mat[0][0] + y*mat[1][0] + mat[2][0]*z + mat[3][0]) *f;
  oy=(x*mat[0][1] + y*mat[1][1] + mat[2][1]*z + mat[3][1]) *f;
  oz=(x*mat[0][2] + y*mat[1][2] + mat[2][2]*z + mat[3][2]) *f;
  out->x = ox;
  out->y = oy;
  out->z = oz;
}

void eol_mat4_identity(eolMat4 m)
{
	memset(m,0,sizeof(eolMat4));
  m[0][0]= m[1][1]= m[2][2]= m[3][3]= 1.0;
}

void eol_mat4_mult_mat4(
    eolMat4 out,
    eolMat4 m2,
    eolMat4 m3
  )
{

  out[0][0] = m2[0][0]*m3[0][0] + m2[0][1]*m3[1][0] + m2[0][2]*m3[2][0] + m2[0][3]*m3[3][0];
  out[0][1] = m2[0][0]*m3[0][1] + m2[0][1]*m3[1][1] + m2[0][2]*m3[2][1] + m2[0][3]*m3[3][1];
  out[0][2] = m2[0][0]*m3[0][2] + m2[0][1]*m3[1][2] + m2[0][2]*m3[2][2] + m2[0][3]*m3[3][2];
  out[0][3] = m2[0][0]*m3[0][3] + m2[0][1]*m3[1][3] + m2[0][2]*m3[2][3] + m2[0][3]*m3[3][3];

  out[1][0] = m2[1][0]*m3[0][0] + m2[1][1]*m3[1][0] + m2[1][2]*m3[2][0] + m2[1][3]*m3[3][0];
  out[1][1] = m2[1][0]*m3[0][1] + m2[1][1]*m3[1][1] + m2[1][2]*m3[2][1] + m2[1][3]*m3[3][1];
  out[1][2] = m2[1][0]*m3[0][2] + m2[1][1]*m3[1][2] + m2[1][2]*m3[2][2] + m2[1][3]*m3[3][2];
  out[1][3] = m2[1][0]*m3[0][3] + m2[1][1]*m3[1][3] + m2[1][2]*m3[2][3] + m2[1][3]*m3[3][3];

  out[2][0] = m2[2][0]*m3[0][0] + m2[2][1]*m3[1][0] + m2[2][2]*m3[2][0] + m2[2][3]*m3[3][0];
  out[2][1] = m2[2][0]*m3[0][1] + m2[2][1]*m3[1][1] + m2[2][2]*m3[2][1] + m2[2][3]*m3[3][1];
  out[2][2] = m2[2][0]*m3[0][2] + m2[2][1]*m3[1][2] + m2[2][2]*m3[2][2] + m2[2][3]*m3[3][2];
  out[2][3] = m2[2][0]*m3[0][3] + m2[2][1]*m3[1][3] + m2[2][2]*m3[2][3] + m2[2][3]*m3[3][3];

  out[3][0] = m2[3][0]*m3[0][0] + m2[3][1]*m3[1][0] + m2[3][2]*m3[2][0] + m2[3][3]*m3[3][0];
  out[3][1] = m2[3][0]*m3[0][1] + m2[3][1]*m3[1][1] + m2[3][2]*m3[2][1] + m2[3][3]*m3[3][1];
  out[3][2] = m2[3][0]*m3[0][2] + m2[3][1]*m3[1][2] + m2[3][2]*m3[2][2] + m2[3][3]*m3[3][2];
  out[3][3] = m2[3][0]*m3[0][3] + m2[3][1]*m3[1][3] + m2[3][2]*m3[2][3] + m2[3][3]*m3[3][3];
}

void eol_mat4_copy(
    eolMat4 d,
    eolMat4 s
  )
{
	if (d == s)return;
  memcpy(d,s,sizeof(eolMat4));
}

void eol_matrix_rotate_vec3d(eolVec3D *out,eolVec3D rot,eolVec3D in)
{
  eolMat4 rotmatrix;
  if (!out)return;
  rotmatrix[0][0]=rot.x * EOL_DEGTORAD;
  rotmatrix[1][0]=1.0f;
  rotmatrix[2][0]=0.0f;
  rotmatrix[3][0]=0.0f;

  rotmatrix[0][1]=rot.y * EOL_DEGTORAD;
  rotmatrix[1][1]=0.0f;
  rotmatrix[2][1]=1.0f;
  rotmatrix[3][1]=0.0f;

  rotmatrix[0][2]=rot.z * EOL_DEGTORAD;
  rotmatrix[1][2]=0.0f;
  rotmatrix[2][2]=0.0f;
  rotmatrix[3][2]=1.0f;

  rotmatrix[0][3]=0.0f;
  rotmatrix[1][3]=0.0f;
  rotmatrix[2][3]=0.0f;
  rotmatrix[3][3]=0.0f;
  eol_mat4_mult_vert(
    out,
    rotmatrix,
    in
  );
}


eolBool eol_matrix_invert(eolMat4 d, eolMat4 s)
{
  eolMat4 inv;
  eolFloat det;
  eolInt i,j;
  
  inv[0][0] = s[1][1]  * s[2][2] * s[3][3] -
  s[1][1]  * s[2][3] * s[3][2] -
  s[2][1]  * s[1][2]  * s[3][3] +
  s[2][1]  * s[1][3]  * s[3][2] +
  s[3][1] * s[1][2]  * s[2][3] -
  s[3][1] * s[1][3]  * s[2][2];
  
  inv[1][0] = -s[1][0] * s[2][2] * s[3][3] +
  s[1][0] * s[2][3] * s[3][2] +
  s[2][0]  * s[1][2]  * s[3][3] -
  s[2][0]  * s[1][3]  * s[3][2] -
  s[3][0] * s[1][2]  * s[2][3] +
  s[3][0] * s[1][3]  * s[2][2];
  
  inv[2][0] = s[1][0] * s[2][1] * s[3][3] -
  s[1][0] * s[2][3] * s[3][1] -
  s[2][0]  * s[1][1] * s[3][3] +
  s[2][0]  * s[1][3] * s[3][1] +
  s[3][0] * s[1][1] * s[2][3] -
  s[3][0] * s[1][3] * s[2][1];
  
  inv[3][0] = -s[1][0] * s[2][1] * s[3][2] +
  s[1][0] * s[2][2] * s[3][1] +
  s[2][0]  * s[1][1] * s[3][2] -
  s[2][0]  * s[1][2] * s[3][1] -
  s[3][0] * s[1][1] * s[2][2] +
  s[3][0] * s[1][2] * s[2][1];
  
  inv[0][1] = -s[0][1] * s[2][2] * s[3][3] +
  s[0][1] * s[2][3] * s[3][2] +
  s[2][1]  * s[0][2]* s[3][3] -
  s[2][1]  * s[0][3]* s[3][2] -
  s[3][1] * s[0][2]* s[2][3] +
  s[3][1] * s[0][3]* s[2][2];
  
  inv[1][1] = s[0][0] * s[2][2] * s[3][3] -
  s[0][0] * s[2][3] * s[3][2] -
  s[2][0]  * s[0][2]* s[3][3] +
  s[2][0]  * s[0][3]* s[3][2] +
  s[3][0] * s[0][2]* s[2][3] -
  s[3][0] * s[0][3]* s[2][2];
  
  inv[2][1] = -s[0][0] * s[2][1] * s[3][3] +
  s[0][0] * s[2][3] * s[3][1] +
  s[2][0]  * s[0][1]* s[3][3] -
  s[2][0]  * s[0][3]* s[3][1] -
  s[3][0] * s[0][1]* s[2][3] +
  s[3][0] * s[0][3]* s[2][1];
  
  inv[3][1] = s[0][0] * s[2][1] * s[3][2] -
  s[0][0] * s[2][2] * s[3][1] -
  s[2][0]  * s[0][1]* s[3][2] +
  s[2][0]  * s[0][2]* s[3][1] +
  s[3][0] * s[0][1]* s[2][2] -
  s[3][0] * s[0][2]* s[2][1];
  
  inv[0][2] = s[0][1] * s[1][2] * s[3][3] -
  s[0][1] * s[1][3] * s[3][2] -
  s[1][1]  * s[0][2]* s[3][3] +
  s[1][1]  * s[0][3]* s[3][2] +
  s[3][1] * s[0][2]* s[1][3] -
  s[3][1] * s[0][3]* s[1][2];
  
  inv[1][2] = -s[0][0] * s[1][2] * s[3][3] +
  s[0][0] * s[1][3] * s[3][2] +
  s[1][0] * s[0][2]* s[3][3] -
  s[1][0] * s[0][3]* s[3][2] -
  s[3][0] * s[0][2]* s[1][3] +
  s[3][0] * s[0][3]* s[1][2];
  
  inv[2][2] = s[0][0] * s[1][1] * s[3][3] -
  s[0][0] * s[1][3] * s[3][1] -
  s[1][0] * s[0][1]* s[3][3] +
  s[1][0] * s[0][3]* s[3][1] +
  s[3][0] * s[0][1]* s[1][3] -
  s[3][0] * s[0][3]* s[1][1];
  
  inv[3][2] = -s[0][0] * s[1][1] * s[3][2] +
  s[0][0] * s[1][2] * s[3][1] +
  s[1][0] * s[0][1]* s[3][2] -
  s[1][0] * s[0][2]* s[3][1] -
  s[3][0] * s[0][1]* s[1][2] +
  s[3][0] * s[0][2]* s[1][1];
  
  inv[0][3] = -s[0][1]* s[1][2] * s[2][3] +
  s[0][1]* s[1][3] * s[2][2] +
  s[1][1] * s[0][2]* s[2][3] -
  s[1][1] * s[0][3]* s[2][2] -
  s[2][1] * s[0][2]* s[1][3] +
  s[2][1] * s[0][3]* s[1][2];
  
  inv[1][3] = s[0][0]* s[1][2] * s[2][3] -
  s[0][0]* s[1][3] * s[2][2] -
  s[1][0]* s[0][2]* s[2][3] +
  s[1][0]* s[0][3]* s[2][2] +
  s[2][0] * s[0][2]* s[1][3] -
  s[2][0] * s[0][3]* s[1][2];
  
  inv[2][3] = -s[0][0]* s[1][1] * s[2][3] +
  s[0][0]* s[1][3] * s[2][1] +
  s[1][0]* s[0][1]* s[2][3] -
  s[1][0]* s[0][3]* s[2][1] -
  s[2][0] * s[0][1]* s[1][3] +
  s[2][0] * s[0][3]* s[1][1];
  
  inv[3][3] = s[0][0]* s[1][1] * s[2][2] -
  s[0][0]* s[1][2] * s[2][1] -
  s[1][0]* s[0][1]* s[2][2] +
  s[1][0]* s[0][2]* s[2][1] +
  s[2][0] * s[0][1]* s[1][2] -
  s[2][0] * s[0][2]* s[1][1];
  
  det = s[0][0]* inv[0][0] + s[0][1]* inv[1][0] + s[0][2]* inv[2][0] + s[0][3]* inv[3][0];
  
  if (det == 0)
  {
    return eolFalse;
  }
  
  det = 1.0 / det;
  
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
      d[i][j] = inv[i][j] * det;
  }
  
  return eolTrue;
}

/*eol@eof*/

