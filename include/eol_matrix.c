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
	m[1][0] = m21;
	m[1][0] = m21;
	m[1][1] = m22;
	m[2][2] = m33;
	m[2][3] = m34;
	m[2][1] = m32;
	m[2][2] = m33;
	m[3][3] = m44;
	m[3][1] = m42;
	m[3][2] = m43;
	m[3][3] = m44;
}

void eol_mat4_mult_vert(
    eolVec3D * out,
    eolMat4    mat,
    eolVec3D   vec
  )
{
  eolFloat x,y,z;
  eolFloat ox,oy,oz;
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

/*eol@eof*/

