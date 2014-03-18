#ifndef _EOL_OPENGL_
#define _EOL_OPENGL_

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

/**
  @purpose This file is intended to define openGL constants if they are not
  already available with the installed opengl libraries.
*/

/*
The Z value of the 2D plane, a range of 0 to 1
0 = the near Z
1 = the far Z
*/
#define EOL_2DPLANE_    0.99f

#if defined(_WIN32)
#include <windows.h>
#endif
#if defined(__APPLE__)
#include <OpenGL/gl.h>    
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>    
#include <GL/glu.h>
#endif

#if defined(_WIN32)
#include <glext.h>
#elif defined(__APPLE__)
#include <OpenGL/glext.h>
#else
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

#include <math.h>
#if defined(_WIN32)

/*
   OpenGL 1.2
*/

#define GL_ALIASED_LINE_WIDTH_RANGE               0x846E
#define GL_ALIASED_POINT_SIZE_RANGE               0x846D
#define GL_BGR                                    0x80E0
#define GL_BGRA                                   0x80E1
#define GL_CLAMP_TO_EDGE                          0x812F
#define GL_LIGHT_MODEL_COLOR_CONTROL              0x81F8
#define GL_MAX_3D_TEXTURE_SIZE                    0x8073
#define GL_MAX_ELEMENTS_INDICES                   0x80E9
#define GL_MAX_ELEMENTS_VERTICES                  0x80E8
#define GL_PACK_IMAGE_HEIGHT                      0x806C
#define GL_PACK_SKIP_IMAGES                       0x806B
#define GL_PROXY_TEXTURE_3D                       0x8070
#define GL_RESCALE_NORMAL                         0x803A
#define GL_SEPARATE_SPECULAR_COLOR                0x81FA
#define GL_SINGLE_COLOR                           0x81F9
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY          0x0B23
#define GL_SMOOTH_LINE_WIDTH_RANGE                0x0B22
#define GL_SMOOTH_POINT_SIZE_GRANULARITY          0x0B13
#define GL_SMOOTH_POINT_SIZE_RANGE                0x0B12
#define GL_TEXTURE_3D                             0x806F
#define GL_TEXTURE_BASE_LEVEL                     0x813C
#define GL_TEXTURE_BINDING_3D                     0x806A
#define GL_TEXTURE_DEPTH                          0x8071
#define GL_TEXTURE_MAX_LEVEL                      0x813D
#define GL_TEXTURE_MAX_LOD                        0x813B
#define GL_TEXTURE_MIN_LOD                        0x813A
#define GL_TEXTURE_WRAP_R                         0x8072
#define GL_UNPACK_IMAGE_HEIGHT                    0x806E
#define GL_UNPACK_SKIP_IMAGES                     0x806D
#define GL_UNSIGNED_BYTE_2_3_3_REV                0x8362
#define GL_UNSIGNED_BYTE_3_3_2                    0x8032
#define GL_UNSIGNED_INT_10_10_10_2                0x8036
#define GL_UNSIGNED_INT_2_10_10_10_REV            0x8368
#define GL_UNSIGNED_INT_8_8_8_8                   0x8035
#define GL_UNSIGNED_INT_8_8_8_8_REV               0x8367
#define GL_UNSIGNED_SHORT_1_5_5_5_REV             0x8366
#define GL_UNSIGNED_SHORT_4_4_4_4                 0x8033
#define GL_UNSIGNED_SHORT_4_4_4_4_REV             0x8365
#define GL_UNSIGNED_SHORT_5_5_5_1                 0x8034
#define GL_UNSIGNED_SHORT_5_6_5                   0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV               0x8364

typedef void (APIENTRY * PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRY * PFNGLDRAWRANGEELEMENTSPROC) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
typedef void (APIENTRY * PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRY * PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);

extern PFNGLCOPYTEXSUBIMAGE3DPROC                 glCopyTexSubImage3D;
extern PFNGLDRAWRANGEELEMENTSPROC                 glDrawRangeElements;
extern PFNGLTEXIMAGE3DPROC                        glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC                     glTexSubImage3D;

/*
   OpenGL 1.3
*/

#define GL_ACTIVE_TEXTURE                         0x84E0
#define GL_ADD_SIGNED                             0x8574
#define GL_CLAMP_TO_BORDER                        0x812D
#define GL_CLAMP_TO_BORDER_SGIS                   0x812D
#define GL_CLIENT_ACTIVE_TEXTURE                  0x84E1
#define GL_COMBINE                                0x8570
#define GL_COMBINE_ALPHA                          0x8572
#define GL_COMBINE_RGB                            0x8571
#define GL_COMPRESSED_ALPHA                       0x84E9
#define GL_COMPRESSED_INTENSITY                   0x84EC
#define GL_COMPRESSED_LUMINANCE                   0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA             0x84EB
#define GL_COMPRESSED_RGB                         0x84ED
#define GL_COMPRESSED_RGBA                        0x84EE
#define GL_COMPRESSED_TEXTURE_FORMATS             0x86A3
#define GL_CONSTANT                               0x8576
#define GL_DOT3_RGB                               0x86AE
#define GL_DOT3_RGBA                              0x86AF
#define GL_INTERPOLATE                            0x8575
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE              0x851C
#define GL_MAX_TEXTURE_UNITS                      0x84E2
#define GL_MULTISAMPLE                            0x809D
#define GL_MULTISAMPLE_BIT                        0x20000000
#define GL_NORMAL_MAP                             0x8511
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS         0x86A2
#define GL_OPERAND0_ALPHA                         0x8598
#define GL_OPERAND0_RGB                           0x8590
#define GL_OPERAND1_ALPHA                         0x8599
#define GL_OPERAND1_RGB                           0x8591
#define GL_OPERAND2_ALPHA                         0x859A
#define GL_OPERAND2_RGB                           0x8592
#define GL_PREVIOUS                               0x8578
#define GL_PRIMARY_COLOR                          0x8577
#define GL_PROXY_TEXTURE_CUBE_MAP                 0x851B
#define GL_REFLECTION_MAP                         0x8512
#define GL_RGB_SCALE                              0x8573
#define GL_SAMPLES                                0x80A9
#define GL_SAMPLE_ALPHA_TO_COVERAGE               0x809E
#define GL_SAMPLE_ALPHA_TO_ONE                    0x809F
#define GL_SAMPLE_BUFFERS                         0x80A8
#define GL_SAMPLE_COVERAGE                        0x80A0
#define GL_SAMPLE_COVERAGE_INVERT                 0x80AB
#define GL_SAMPLE_COVERAGE_VALUE                  0x80AA
#define GL_SOURCE0_ALPHA                          0x8588
#define GL_SOURCE0_RGB                            0x8580
#define GL_SOURCE1_ALPHA                          0x8589
#define GL_SOURCE1_RGB                            0x8581
#define GL_SOURCE2_ALPHA                          0x858A
#define GL_SOURCE2_RGB                            0x8582
#define GL_SUBTRACT                               0x84E7
#define GL_TEXTURE0                               0x84C0
#define GL_TEXTURE1                               0x84C1
#define GL_TEXTURE10                              0x84CA
#define GL_TEXTURE11                              0x84CB
#define GL_TEXTURE12                              0x84CC
#define GL_TEXTURE13                              0x84CD
#define GL_TEXTURE14                              0x84CE
#define GL_TEXTURE15                              0x84CF
#define GL_TEXTURE16                              0x84D0
#define GL_TEXTURE17                              0x84D1
#define GL_TEXTURE18                              0x84D2
#define GL_TEXTURE19                              0x84D3
#define GL_TEXTURE2                               0x84C2
#define GL_TEXTURE20                              0x84D4
#define GL_TEXTURE21                              0x84D5
#define GL_TEXTURE22                              0x84D6
#define GL_TEXTURE23                              0x84D7
#define GL_TEXTURE24                              0x84D8
#define GL_TEXTURE25                              0x84D9
#define GL_TEXTURE26                              0x84DA
#define GL_TEXTURE27                              0x84DB
#define GL_TEXTURE28                              0x84DC
#define GL_TEXTURE29                              0x84DD
#define GL_TEXTURE3                               0x84C3
#define GL_TEXTURE30                              0x84DE
#define GL_TEXTURE31                              0x84DF
#define GL_TEXTURE4                               0x84C4
#define GL_TEXTURE5                               0x84C5
#define GL_TEXTURE6                               0x84C6
#define GL_TEXTURE7                               0x84C7
#define GL_TEXTURE8                               0x84C8
#define GL_TEXTURE9                               0x84C9
#define GL_TEXTURE_BINDING_CUBE_MAP               0x8514
#define GL_TEXTURE_COMPRESSED                     0x86A1
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE          0x86A0
#define GL_TEXTURE_COMPRESSION_HINT               0x84EF
#define GL_TEXTURE_CUBE_MAP                       0x8513
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X            0x8516
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y            0x8518
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z            0x851A
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X            0x8515
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y            0x8517
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z            0x8519
#define GL_TRANSPOSE_COLOR_MATRIX                 0x84E6
#define GL_TRANSPOSE_MODELVIEW_MATRIX             0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX            0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX               0x84E5

typedef void (APIENTRY * PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEPROC) (GLenum target, GLint level, void *img);
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDPROC) (const GLdouble *m);
typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFPROC) (const GLfloat *m);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DPROC) (GLenum target, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FPROC) (GLenum target, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IPROC) (GLenum target, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SPROC) (GLenum target, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DPROC) (GLenum target, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IPROC) (GLenum target, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SPROC) (GLenum target, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IPROC) (GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDPROC) (const GLdouble *m);
typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFPROC) (const GLfloat *m);
typedef void (APIENTRY * PFNGLSAMPLECOVERAGEPROC) (GLclampf value, GLboolean invert);

extern PFNGLACTIVETEXTUREPROC                     glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC               glClientActiveTexture;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC              glCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC              glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC              glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC           glCompressedTexSubImage1D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC           glCompressedTexSubImage2D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC           glCompressedTexSubImage3D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC             glGetCompressedTexImage;
extern PFNGLLOADTRANSPOSEMATRIXDPROC              glLoadTransposeMatrixd;
extern PFNGLLOADTRANSPOSEMATRIXFPROC              glLoadTransposeMatrixf;
extern PFNGLMULTITEXCOORD1DPROC                   glMultiTexCoord1d;
extern PFNGLMULTITEXCOORD1DVPROC                  glMultiTexCoord1dv;
extern PFNGLMULTITEXCOORD1FPROC                   glMultiTexCoord1f;
extern PFNGLMULTITEXCOORD1FVPROC                  glMultiTexCoord1fv;
extern PFNGLMULTITEXCOORD1IPROC                   glMultiTexCoord1i;
extern PFNGLMULTITEXCOORD1IVPROC                  glMultiTexCoord1iv;
extern PFNGLMULTITEXCOORD1SPROC                   glMultiTexCoord1s;
extern PFNGLMULTITEXCOORD1SVPROC                  glMultiTexCoord1sv;
extern PFNGLMULTITEXCOORD2DPROC                   glMultiTexCoord2d;
extern PFNGLMULTITEXCOORD2DVPROC                  glMultiTexCoord2dv;
extern PFNGLMULTITEXCOORD2FPROC                   glMultiTexCoord2f;
extern PFNGLMULTITEXCOORD2FVPROC                  glMultiTexCoord2fv;
extern PFNGLMULTITEXCOORD2IPROC                   glMultiTexCoord2i;
extern PFNGLMULTITEXCOORD2IVPROC                  glMultiTexCoord2iv;
extern PFNGLMULTITEXCOORD2SPROC                   glMultiTexCoord2s;
extern PFNGLMULTITEXCOORD2SVPROC                  glMultiTexCoord2sv;
extern PFNGLMULTITEXCOORD3DPROC                   glMultiTexCoord3d;
extern PFNGLMULTITEXCOORD3DVPROC                  glMultiTexCoord3dv;
extern PFNGLMULTITEXCOORD3FPROC                   glMultiTexCoord3f;
extern PFNGLMULTITEXCOORD3FVPROC                  glMultiTexCoord3fv;
extern PFNGLMULTITEXCOORD3IPROC                   glMultiTexCoord3i;
extern PFNGLMULTITEXCOORD3IVPROC                  glMultiTexCoord3iv;
extern PFNGLMULTITEXCOORD3SPROC                   glMultiTexCoord3s;
extern PFNGLMULTITEXCOORD3SVPROC                  glMultiTexCoord3sv;
extern PFNGLMULTITEXCOORD4DPROC                   glMultiTexCoord4d;
extern PFNGLMULTITEXCOORD4DVPROC                  glMultiTexCoord4dv;
extern PFNGLMULTITEXCOORD4FPROC                   glMultiTexCoord4f;
extern PFNGLMULTITEXCOORD4FVPROC                  glMultiTexCoord4fv;
extern PFNGLMULTITEXCOORD4IPROC                   glMultiTexCoord4i;
extern PFNGLMULTITEXCOORD4IVPROC                  glMultiTexCoord4iv;
extern PFNGLMULTITEXCOORD4SPROC                   glMultiTexCoord4s;
extern PFNGLMULTITEXCOORD4SVPROC                  glMultiTexCoord4sv;
extern PFNGLMULTTRANSPOSEMATRIXDPROC              glMultTransposeMatrixd;
extern PFNGLMULTTRANSPOSEMATRIXFPROC              glMultTransposeMatrixf;
extern PFNGLSAMPLECOVERAGEPROC                    glSampleCoverage;


/*
   OpenGL 1.4
*/

#define GL_BLEND_DST_ALPHA                        0x80CA
#define GL_BLEND_DST_RGB                          0x80C8
#define GL_BLEND_SRC_ALPHA                        0x80CB
#define GL_BLEND_SRC_RGB                          0x80C9
#define GL_COLOR_SUM                              0x8458
#define GL_COMPARE_R_TO_TEXTURE                   0x884E
#define GL_CURRENT_FOG_COORDINATE                 0x8453
#define GL_CURRENT_SECONDARY_COLOR                0x8459
#define GL_DECR_WRAP                              0x8508
#define GL_DEPTH_COMPONENT16                      0x81A5
#define GL_DEPTH_COMPONENT24                      0x81A6
#define GL_DEPTH_COMPONENT32                      0x81A7
#define GL_DEPTH_TEXTURE_MODE                     0x884B
#define GL_FOG_COORDINATE                         0x8451
#define GL_FOG_COORDINATE_ARRAY                   0x8457
#define GL_FOG_COORDINATE_ARRAY_POINTER           0x8456
#define GL_FOG_COORDINATE_ARRAY_STRIDE            0x8455
#define GL_FOG_COORDINATE_ARRAY_TYPE              0x8454
#define GL_FOG_COORDINATE_SOURCE                  0x8450
#define GL_FRAGMENT_DEPTH                         0x8452
#define GL_GENERATE_MIPMAP                        0x8191
#define GL_GENERATE_MIPMAP_HINT                   0x8192
#define GL_INCR_WRAP                              0x8507
#define GL_MAX_TEXTURE_LOD_BIAS                   0x84FD
#define GL_MIRRORED_REPEAT                        0x8370
#define GL_POINT_DISTANCE_ATTENUATION             0x8129
#define GL_POINT_FADE_THRESHOLD_SIZE              0x8128
#define GL_POINT_SIZE_MAX                         0x8127
#define GL_POINT_SIZE_MIN                         0x8126
#define GL_SECONDARY_COLOR_ARRAY                  0x845E
#define GL_SECONDARY_COLOR_ARRAY_POINTER          0x845D
#define GL_SECONDARY_COLOR_ARRAY_SIZE             0x845A
#define GL_SECONDARY_COLOR_ARRAY_STRIDE           0x845C
#define GL_SECONDARY_COLOR_ARRAY_TYPE             0x845B
#define GL_TEXTURE_COMPARE_FUNC                   0x884D
#define GL_TEXTURE_COMPARE_MODE                   0x884C
#define GL_TEXTURE_DEPTH_SIZE                     0x884A
#define GL_TEXTURE_FILTER_CONTROL                 0x8500
#define GL_TEXTURE_LOD_BIAS                       0x8501

typedef void (APIENTRY * PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRY * PFNGLFOGCOORDDPROC) (GLdouble coord);
typedef void (APIENTRY * PFNGLFOGCOORDDVPROC) (const GLdouble *coord);
typedef void (APIENTRY * PFNGLFOGCOORDFPROC) (GLfloat coord);
typedef void (APIENTRY * PFNGLFOGCOORDFVPROC) (const GLfloat *coord);
typedef void (APIENTRY * PFNGLFOGCOORDPOINTERPROC) (GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRY * PFNGLMULTIDRAWARRAYSPROC) (GLenum mode, GLint *first, GLsizei *count, GLsizei primcount);
typedef void (APIENTRY * PFNGLMULTIDRAWELEMENTSPROC) (GLenum mode, const GLsizei *count, GLenum type, const void* *indices, GLsizei primcount);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRY * PFNGLPOINTPARAMETERIPROC) (GLenum pname, GLint param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERIVPROC) (GLenum pname, const GLint *params);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BPROC) (GLbyte red, GLbyte green, GLbyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BVPROC) (const GLbyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DPROC) (GLdouble red, GLdouble green, GLdouble blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DVPROC) (const GLdouble *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FPROC) (GLfloat red, GLfloat green, GLfloat blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FVPROC) (const GLfloat *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IPROC) (GLint red, GLint green, GLint blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IVPROC) (const GLint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SPROC) (GLshort red, GLshort green, GLshort blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SVPROC) (const GLshort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBPROC) (GLubyte red, GLubyte green, GLubyte blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBVPROC) (const GLubyte *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIPROC) (GLuint red, GLuint green, GLuint blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIVPROC) (const GLuint *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USPROC) (GLushort red, GLushort green, GLushort blue);
typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USVPROC) (const GLushort *v);
typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTERPROC) (GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (APIENTRY * PFNGLWINDOWPOS2DPROC) (GLdouble x, GLdouble y);
typedef void (APIENTRY * PFNGLWINDOWPOS2DVPROC) (const GLdouble *v);
typedef void (APIENTRY * PFNGLWINDOWPOS2FPROC) (GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLWINDOWPOS2FVPROC) (const GLfloat *v);
typedef void (APIENTRY * PFNGLWINDOWPOS2IPROC) (GLint x, GLint y);
typedef void (APIENTRY * PFNGLWINDOWPOS2IVPROC) (const GLint *v);
typedef void (APIENTRY * PFNGLWINDOWPOS2SPROC) (GLshort x, GLshort y);
typedef void (APIENTRY * PFNGLWINDOWPOS2SVPROC) (const GLshort *v);
typedef void (APIENTRY * PFNGLWINDOWPOS3DPROC) (GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLWINDOWPOS3DVPROC) (const GLdouble *v);
typedef void (APIENTRY * PFNGLWINDOWPOS3FPROC) (GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLWINDOWPOS3FVPROC) (const GLfloat *v);
typedef void (APIENTRY * PFNGLWINDOWPOS3IPROC) (GLint x, GLint y, GLint z);
typedef void (APIENTRY * PFNGLWINDOWPOS3IVPROC) (const GLint *v);
typedef void (APIENTRY * PFNGLWINDOWPOS3SPROC) (GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLWINDOWPOS3SVPROC) (const GLshort *v);

extern PFNGLBLENDFUNCSEPARATEPROC                 glBlendFuncSeparate;
extern PFNGLMULTIDRAWARRAYSPROC                   glMultiDrawArrays;
extern PFNGLMULTIDRAWELEMENTSPROC                 glMultiDrawElements;
extern PFNGLPOINTPARAMETERFPROC                   glPointParameterf;
extern PFNGLPOINTPARAMETERFVPROC                  glPointParameterfv;
extern PFNGLSECONDARYCOLOR3BPROC                  glSecondaryColor3b;
extern PFNGLSECONDARYCOLOR3BVPROC                 glSecondaryColor3bv;
extern PFNGLSECONDARYCOLOR3DPROC                  glSecondaryColor3d;
extern PFNGLSECONDARYCOLOR3DVPROC                 glSecondaryColor3dv;
extern PFNGLSECONDARYCOLOR3FPROC                  glSecondaryColor3f;
extern PFNGLSECONDARYCOLOR3FVPROC                 glSecondaryColor3fv;
extern PFNGLSECONDARYCOLOR3IPROC                  glSecondaryColor3i;
extern PFNGLSECONDARYCOLOR3IVPROC                 glSecondaryColor3iv;
extern PFNGLSECONDARYCOLOR3SPROC                  glSecondaryColor3s;
extern PFNGLSECONDARYCOLOR3SVPROC                 glSecondaryColor3sv;
extern PFNGLSECONDARYCOLOR3UBPROC                 glSecondaryColor3ub;
extern PFNGLSECONDARYCOLOR3UBVPROC                glSecondaryColor3ubv;
extern PFNGLSECONDARYCOLOR3UIPROC                 glSecondaryColor3ui;
extern PFNGLSECONDARYCOLOR3UIVPROC                glSecondaryColor3uiv;
extern PFNGLSECONDARYCOLOR3USPROC                 glSecondaryColor3us;
extern PFNGLSECONDARYCOLOR3USVPROC                glSecondaryColor3usv;
extern PFNGLSECONDARYCOLORPOINTERPROC             glSecondaryColorPointer;
extern PFNGLWINDOWPOS2DPROC                       glWindowPos2d;
extern PFNGLWINDOWPOS2DVPROC                      glWindowPos2dv;
extern PFNGLWINDOWPOS2FPROC                       glWindowPos2f;
extern PFNGLWINDOWPOS2FVPROC                      glWindowPos2fv;
extern PFNGLWINDOWPOS2IPROC                       glWindowPos2i;
extern PFNGLWINDOWPOS2IVPROC                      glWindowPos2iv;
extern PFNGLWINDOWPOS2SPROC                       glWindowPos2s;
extern PFNGLWINDOWPOS2SVPROC                      glWindowPos2sv;
extern PFNGLWINDOWPOS3DPROC                       glWindowPos3d;
extern PFNGLWINDOWPOS3DVPROC                      glWindowPos3dv;
extern PFNGLWINDOWPOS3FPROC                       glWindowPos3f;
extern PFNGLWINDOWPOS3FVPROC                      glWindowPos3fv;
extern PFNGLWINDOWPOS3IPROC                       glWindowPos3i;
extern PFNGLWINDOWPOS3IVPROC                      glWindowPos3iv;
extern PFNGLWINDOWPOS3SPROC                       glWindowPos3s;
extern PFNGLWINDOWPOS3SVPROC                      glWindowPos3sv;

#endif

/*
   OpenGL 1.5
*/

#if 0

typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#define GL_BUFFER_SIZE                            0x8764
#define GL_BUFFER_USAGE                           0x8765
#define GL_QUERY_COUNTER_BITS                     0x8864
#define GL_CURRENT_QUERY                          0x8865
#define GL_QUERY_RESULT                           0x8866
#define GL_QUERY_RESULT_AVAILABLE                 0x8867
#define GL_ARRAY_BUFFER                           0x8892
#define GL_ELEMENT_ARRAY_BUFFER                   0x8893
#define GL_ARRAY_BUFFER_BINDING                   0x8894
#define GL_ELEMENT_ARRAY_BUFFER_BINDING           0x8895
#define GL_VERTEX_ARRAY_BUFFER_BINDING            0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING            0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING             0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING             0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING     0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING         0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING   0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING    0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING            0x889E
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING     0x889F
#define GL_READ_ONLY                              0x88B8
#define GL_WRITE_ONLY                             0x88B9
#define GL_READ_WRITE                             0x88BA
#define GL_BUFFER_ACCESS                          0x88BB
#define GL_BUFFER_MAPPED                          0x88BC
#define GL_BUFFER_MAP_POINTER                     0x88BD
#define GL_STREAM_DRAW                            0x88E0
#define GL_STREAM_READ                            0x88E1
#define GL_STREAM_COPY                            0x88E2
#define GL_STATIC_DRAW                            0x88E4
#define GL_STATIC_READ                            0x88E5
#define GL_STATIC_COPY                            0x88E6
#define GL_DYNAMIC_DRAW                           0x88E8
#define GL_DYNAMIC_READ                           0x88E9
#define GL_DYNAMIC_COPY                           0x88EA
#define GL_SAMPLES_PASSED                         0x8914
#define GL_FOG_COORD_SOURCE                       GL_FOG_COORDINATE_SOURCE
#define GL_FOG_COORD                              GL_FOG_COORDINATE
#define GL_CURRENT_FOG_COORD                      GL_CURRENT_FOG_COORDINATE
#define GL_FOG_COORD_ARRAY_TYPE                   GL_FOG_COORDINATE_ARRAY_TYPE
#define GL_FOG_COORD_ARRAY_STRIDE                 GL_FOG_COORDINATE_ARRAY_STRIDE
#define GL_FOG_COORD_ARRAY_POINTER                GL_FOG_COORDINATE_ARRAY_POINTER
#define GL_FOG_COORD_ARRAY                        GL_FOG_COORDINATE_ARRAY
#define GL_FOG_COORD_ARRAY_BUFFER_BINDING         GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING
#define GL_SRC0_RGB                               GL_SOURCE0_RGB
#define GL_SRC1_RGB                               GL_SOURCE1_RGB
#define GL_SRC2_RGB                               GL_SOURCE2_RGB
#define GL_SRC0_ALPHA                             GL_SOURCE0_ALPHA
#define GL_SRC1_ALPHA                             GL_SOURCE1_ALPHA
#define GL_SRC2_ALPHA                             GL_SOURCE2_ALPHA

#endif

#ifndef __APPLE__
typedef void (APIENTRY * PFNGLGENQUERIESPROC) (GLsizei n, GLuint *ids);
typedef void (APIENTRY * PFNGLDELETEQUERIESPROC) (GLsizei n, const GLuint *ids);
typedef GLboolean (APIENTRY * PFNGLISQUERYPROC) (GLuint id);
typedef void (APIENTRY * PFNGLBEGINQUERYPROC) (GLenum target, GLuint id);
typedef void (APIENTRY * PFNGLENDQUERYPROC) (GLenum target);
typedef void (APIENTRY * PFNGLGETQUERYIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETQUERYOBJECTIVPROC) (GLuint id, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETQUERYOBJECTUIVPROC) (GLuint id, GLenum pname, GLuint *params);
typedef void (APIENTRY * PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef GLboolean (APIENTRY * PFNGLISBUFFERPROC) (GLuint buffer);
typedef void (APIENTRY * PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY * PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRY * PFNGLGETBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, void *data);
typedef void* (APIENTRY * PFNGLMAPBUFFERPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRY * PFNGLUNMAPBUFFERPROC) (GLenum target);
typedef void (APIENTRY * PFNGLGETBUFFERPARAMETERIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETBUFFERPOINTERVPROC) (GLenum target, GLenum pname, void* *params);

extern PFNGLGENQUERIESPROC                        glGenQueries;
extern PFNGLDELETEQUERIESPROC                     glDeleteQueries;
extern PFNGLISQUERYPROC                           glIsQuery;
extern PFNGLBEGINQUERYPROC                        glBeginQuery;
extern PFNGLENDQUERYPROC                          glEndQuery;
extern PFNGLGETQUERYIVPROC                        glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC                  glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC                 glGetQueryObjectuiv;
extern PFNGLBINDBUFFERPROC                        glBindBuffer;
extern PFNGLDELETEBUFFERSPROC                     glDeleteBuffers;
extern PFNGLGENBUFFERSPROC                        glGenBuffers;
extern PFNGLISBUFFERPROC                          glIsBuffer;
extern PFNGLBUFFERDATAPROC                        glBufferData;
extern PFNGLBUFFERSUBDATAPROC                     glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC                  glGetBufferSubData;
extern PFNGLMAPBUFFERPROC                         glMapBuffer;
extern PFNGLUNMAPBUFFERPROC                       glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC              glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC                 glGetBufferPointerv;
#endif
/*
  OpenGL 2.0
*/

#if 0

typedef char GLchar;

#define GL_BLEND_EQUATION_RGB                     GL_BLEND_EQUATION
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED            0x8622
#define GL_VERTEX_ATTRIB_ARRAY_SIZE               0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE             0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE               0x8625
#define GL_CURRENT_VERTEX_ATTRIB                  0x8626
#define GL_VERTEX_PROGRAM_POINT_SIZE              0x8642
#define GL_VERTEX_PROGRAM_TWO_SIDE                0x8643
#define GL_VERTEX_ATTRIB_ARRAY_POINTER            0x8645
#define GL_STENCIL_BACK_FUNC                      0x8800
#define GL_STENCIL_BACK_FAIL                      0x8801
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL           0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS           0x8803
#define GL_MAX_DRAW_BUFFERS                       0x8824
#define GL_DRAW_BUFFER0                           0x8825
#define GL_DRAW_BUFFER1                           0x8826
#define GL_DRAW_BUFFER2                           0x8827
#define GL_DRAW_BUFFER3                           0x8828
#define GL_DRAW_BUFFER4                           0x8829
#define GL_DRAW_BUFFER5                           0x882A
#define GL_DRAW_BUFFER6                           0x882B
#define GL_DRAW_BUFFER7                           0x882C
#define GL_DRAW_BUFFER8                           0x882D
#define GL_DRAW_BUFFER9                           0x882E
#define GL_DRAW_BUFFER10                          0x882F
#define GL_DRAW_BUFFER11                          0x8830
#define GL_DRAW_BUFFER12                          0x8831
#define GL_DRAW_BUFFER13                          0x8832
#define GL_DRAW_BUFFER14                          0x8833
#define GL_DRAW_BUFFER15                          0x8834
#define GL_BLEND_EQUATION_ALPHA                   0x883D
#define GL_POINT_SPRITE                           0x8861
#define GL_COORD_REPLACE                          0x8862
#define GL_MAX_VERTEX_ATTRIBS                     0x8869
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED         0x886A
#define GL_MAX_TEXTURE_COORDS                     0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS                0x8872
#define GL_FRAGMENT_SHADER                        0x8B30
#define GL_VERTEX_SHADER                          0x8B31
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS        0x8B49
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS          0x8B4A
#define GL_MAX_VARYING_FLOATS                     0x8B4B
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS         0x8B4C
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS       0x8B4D
#define GL_SHADER_TYPE                            0x8B4F
#define GL_FLOAT_VEC2                             0x8B50
#define GL_FLOAT_VEC3                             0x8B51
#define GL_FLOAT_VEC4                             0x8B52
#define GL_INT_VEC2                               0x8B53
#define GL_INT_VEC3                               0x8B54
#define GL_INT_VEC4                               0x8B55
#define GL_BOOL                                   0x8B56
#define GL_BOOL_VEC2                              0x8B57
#define GL_BOOL_VEC3                              0x8B58
#define GL_BOOL_VEC4                              0x8B59
#define GL_FLOAT_MAT2                             0x8B5A
#define GL_FLOAT_MAT3                             0x8B5B
#define GL_FLOAT_MAT4                             0x8B5C
#define GL_SAMPLER_1D                             0x8B5D
#define GL_SAMPLER_2D                             0x8B5E
#define GL_SAMPLER_3D                             0x8B5F
#define GL_SAMPLER_CUBE                           0x8B60
#define GL_SAMPLER_1D_SHADOW                      0x8B61
#define GL_SAMPLER_2D_SHADOW                      0x8B62
#define GL_DELETE_STATUS                          0x8B80
#define GL_COMPILE_STATUS                         0x8B81
#define GL_LINK_STATUS                            0x8B82
#define GL_VALIDATE_STATUS                        0x8B83
#define GL_INFO_LOG_LENGTH                        0x8B84
#define GL_ATTACHED_SHADERS                       0x8B85
#define GL_ACTIVE_UNIFORMS                        0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH              0x8B87
#define GL_SHADER_SOURCE_LENGTH                   0x8B88
#define GL_ACTIVE_ATTRIBUTES                      0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH            0x8B8A
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT        0x8B8B
#define GL_SHADING_LANGUAGE_VERSION               0x8B8C
#define GL_CURRENT_PROGRAM                        0x8B8D
#define GL_POINT_SPRITE_COORD_ORIGIN              0x8CA0
#define GL_LOWER_LEFT                             0x8CA1
#define GL_UPPER_LEFT                             0x8CA2
#define GL_STENCIL_BACK_REF                       0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK                0x8CA4
#define GL_STENCIL_BACK_WRITEMASK                 0x8CA5

#endif

#ifndef __APPLE__
typedef void (APIENTRY * PFNGLBLENDEQUATIONSEPARATEPROC) (GLenum modeRGB, GLenum modeAlpha);
typedef void (APIENTRY * PFNGLDRAWBUFFERSPROC) (GLsizei n, const GLenum *bufs);
typedef void (APIENTRY * PFNGLSTENCILOPSEPARATEPROC) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (APIENTRY * PFNGLSTENCILFUNCSEPARATEPROC) (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
typedef void (APIENTRY * PFNGLSTENCILMASKSEPARATEPROC) (GLenum face, GLuint mask);
typedef void (APIENTRY * PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY * PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar *name);
typedef void (APIENTRY * PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRY * PFNGLCREATEPROGRAMPROC) (void);
typedef GLuint (APIENTRY * PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY * PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRY * PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY * PFNGLGETACTIVEATTRIBPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRY * PFNGLGETACTIVEUNIFORMPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
typedef void (APIENTRY * PFNGLGETATTACHEDSHADERSPROC) (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *obj);
typedef GLint (APIENTRY * PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRY * PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY * PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY * PFNGLGETSHADERSOURCEPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
typedef GLint (APIENTRY * PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRY * PFNGLGETUNIFORMFVPROC) (GLuint program, GLint location, GLfloat *params);
typedef void (APIENTRY * PFNGLGETUNIFORMIVPROC) (GLuint program, GLint location, GLint *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBDVPROC) (GLuint index, GLenum pname, GLdouble *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBFVPROC) (GLuint index, GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBIVPROC) (GLuint index, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVPROC) (GLuint index, GLenum pname, void* *pointer);
typedef GLboolean (APIENTRY * PFNGLISPROGRAMPROC) (GLuint program);
typedef GLboolean (APIENTRY * PFNGLISSHADERPROC) (GLuint shader);
typedef void (APIENTRY * PFNGLLINKPROGRAMPROC) (GLuint program);
/* Newer GLEXT hack */
#if GL_GLEXT_VERSION!=85
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length);
#endif
typedef void (APIENTRY * PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (APIENTRY * PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRY * PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY * PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRY * PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef void (APIENTRY * PFNGLUNIFORM2IPROC) (GLint location, GLint v0, GLint v1);
typedef void (APIENTRY * PFNGLUNIFORM3IPROC) (GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRY * PFNGLUNIFORM4IPROC) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRY * PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRY * PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRY * PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRY * PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLVALIDATEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1DPROC) (GLuint index, GLdouble x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FPROC) (GLuint index, GLfloat x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1SPROC) (GLuint index, GLshort x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2DPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FPROC) (GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2SPROC) (GLuint index, GLshort x, GLshort y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3SPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NBVPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NIVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NSVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUBVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUSVPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4BVPROC) (GLuint index, const GLbyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4DVPROC) (GLuint index, const GLdouble *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FVPROC) (GLuint index, const GLfloat *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4IVPROC) (GLuint index, const GLint *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4SPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4SVPROC) (GLuint index, const GLshort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4UBVPROC) (GLuint index, const GLubyte *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4UIVPROC) (GLuint index, const GLuint *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4USVPROC) (GLuint index, const GLushort *v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

extern PFNGLBLENDEQUATIONSEPARATEPROC             glBlendEquationSeparate;
extern PFNGLDRAWBUFFERSPROC                       glDrawBuffers;
extern PFNGLSTENCILOPSEPARATEPROC                 glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC               glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC               glStencilMaskSeparate;
extern PFNGLATTACHSHADERPROC                      glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC                glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC                     glCompileShader;
extern PFNGLCREATEPROGRAMPROC                     glCreateProgram;
extern PFNGLCREATESHADERPROC                      glCreateShader;
extern PFNGLDELETEPROGRAMPROC                     glDeleteProgram;
extern PFNGLDELETESHADERPROC                      glDeleteShader;
extern PFNGLDETACHSHADERPROC                      glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC          glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC           glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC                   glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC                  glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC                glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC                 glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC                      glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC                 glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC                       glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC                  glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC                   glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC                glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC                      glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC                      glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC                 glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC                 glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC                 glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC           glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC                         glIsProgram;
extern PFNGLISSHADERPROC                          glIsShader;
extern PFNGLLINKPROGRAMPROC                       glLinkProgram;
extern PFNGLSHADERSOURCEPROC                      glShaderSource;
extern PFNGLUSEPROGRAMPROC                        glUseProgram;
extern PFNGLUNIFORM1FPROC                         glUniform1f;
extern PFNGLUNIFORM2FPROC                         glUniform2f;
extern PFNGLUNIFORM3FPROC                         glUniform3f;
extern PFNGLUNIFORM4FPROC                         glUniform4f;
extern PFNGLUNIFORM1IPROC                         glUniform1i;
extern PFNGLUNIFORM2IPROC                         glUniform2i;
extern PFNGLUNIFORM3IPROC                         glUniform3i;
extern PFNGLUNIFORM4IPROC                         glUniform4i;
extern PFNGLUNIFORM1FVPROC                        glUniform1fv;
extern PFNGLUNIFORM2FVPROC                        glUniform2fv;
extern PFNGLUNIFORM3FVPROC                        glUniform3fv;
extern PFNGLUNIFORM4FVPROC                        glUniform4fv;
extern PFNGLUNIFORM1IVPROC                        glUniform1iv;
extern PFNGLUNIFORM2IVPROC                        glUniform2iv;
extern PFNGLUNIFORM3IVPROC                        glUniform3iv;
extern PFNGLUNIFORM4IVPROC                        glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC                  glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC                  glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC                  glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC                   glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC                    glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC                   glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC                    glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC                   glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC                    glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC                   glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC                    glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC                   glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC                    glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC                   glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC                    glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC                   glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC                    glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC                   glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC                    glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC                   glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC                    glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC                   glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC                  glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NIVPROC                  glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NSVPROC                  glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NUBPROC                  glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB4NUBVPROC                 glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC                 glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC                 glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4BVPROC                   glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC                    glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC                   glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC                    glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC                   glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC                   glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC                    glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC                   glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC                  glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC                  glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC                  glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC               glVertexAttribPointer;
#endif
/*
   OpenGL 2.1
*/

#define GL_COMPRESSED_SLUMINANCE                  0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA            0x8C4B
#define GL_COMPRESSED_SRGB                        0x8C48
#define GL_COMPRESSED_SRGB_ALPHA                  0x8C49
#define GL_CURRENT_RASTER_SECONDARY_COLOR         0x845F
#define GL_FLOAT_MAT2x3                           0x8B65
#define GL_FLOAT_MAT2x4                           0x8B66
#define GL_FLOAT_MAT3x2                           0x8B67
#define GL_FLOAT_MAT3x4                           0x8B68
#define GL_FLOAT_MAT4x2                           0x8B69
#define GL_FLOAT_MAT4x3                           0x8B6A
#define GL_PIXEL_PACK_BUFFER                      0x88EB
#define GL_PIXEL_PACK_BUFFER_BINDING              0x88ED
#define GL_PIXEL_UNPACK_BUFFER                    0x88EC
#define GL_PIXEL_UNPACK_BUFFER_BINDING            0x88EF
#define GL_SLUMINANCE                             0x8C46
#define GL_SLUMINANCE8                            0x8C47
#define GL_SLUMINANCE8_ALPHA8                     0x8C45
#define GL_SLUMINANCE_ALPHA                       0x8C44
#define GL_SRGB                                   0x8C40
#define GL_SRGB8                                  0x8C41
#define GL_SRGB8_ALPHA8                           0x8C43
#define GL_SRGB_ALPHA                             0x8C42

#ifndef __APPLE__
typedef void (APIENTRY * PFNGLUNIFORMMATRIX2X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX3X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX2X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX4X2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX3X4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX4X3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

extern PFNGLUNIFORMMATRIX2X3FVPROC                glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC                glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC                glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC                glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC                glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC                glUniformMatrix4x3fv;
#endif

/**
 * @brief sets up opengl for use on this system.
 */
void eol_opengl_init();

/**
 * @brief returns the initialization status of opengl
 *
 * @return true if opengl has been initialized, false otherwise
 */
GLboolean eol_opengl_is_initialized();

/**
 * @brief calculate the 3D position for the given 2D position at the depth
 * provided
 *
 * @param x the 2D space x position
 * @param y the 2D space y position
 * @param z the depth into 3D space to calculate the projection
 * @param model the current gl model
 * @param proj the current gl projection
 * @param view the current gl view
 * @param glx the output projected x position in 3D
 * @param gly the output projected y position in 3D
 * @param glz the output projected z position in 3D
 */
void eol_opengl_get_gl_coordinate(
  GLint    x,
  GLint    y,
  GLdouble z,
  const GLdouble * model,
  const GLdouble * proj,
  const GLint    * view,
  GLdouble * glx,
  GLdouble * gly,
  GLdouble * glz
  );

/**
 * @brief calculate the 2D screen coordinates for the given 3D position
 * @param glx x position in 3D
 * @param gly y position in 3D
 * @param glz z position in 3D
 * @param model the current gl model
 * @param proj the current gl projection
 * @param view the current gl view
 * @param x output the 2D space x position
 * @param y output he 2D space y position
 */
void eol_opengl_get_screen_coordinate(
  GLdouble glx,
  GLdouble gly,
  GLdouble glz,
  const GLdouble * model,
  const GLdouble * proj,
  const GLint    * view,
  GLint    *x,
  GLint    *y
);

/**
  * @brief returns the smallest bounding power of two
  *
  * @param in the number to be bound by a power of two
  *
  * @return the smallest power of two greater than in
 */
GLint eol_opengl_power_of_two(GLint in);

#endif
