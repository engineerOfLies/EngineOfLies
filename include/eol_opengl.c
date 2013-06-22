#include "eol_opengl.h"

GLboolean _eol_opengl_initialized = GL_FALSE;

#if defined(_WIN32)

/* OpenGL 1.2.*/
PFNGLCOPYTEXSUBIMAGE3DPROC              glCopyTexSubImage3D;
PFNGLDRAWRANGEELEMENTSPROC              glDrawRangeElements;
PFNGLTEXIMAGE3DPROC                     glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC                  glTexSubImage3D;

/* OpenGL 1.3.*/
PFNGLACTIVETEXTUREPROC                  glActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC            glClientActiveTexture;
PFNGLCOMPRESSEDTEXIMAGE1DPROC           glCompressedTexImage1D;
PFNGLCOMPRESSEDTEXIMAGE2DPROC           glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXIMAGE3DPROC           glCompressedTexImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC        glCompressedTexSubImage1D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC        glCompressedTexSubImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC        glCompressedTexSubImage3D;
PFNGLGETCOMPRESSEDTEXIMAGEPROC          glGetCompressedTexImage;
PFNGLLOADTRANSPOSEMATRIXDPROC           glLoadTransposeMatrixd;
PFNGLLOADTRANSPOSEMATRIXFPROC           glLoadTransposeMatrixf;
PFNGLMULTITEXCOORD1DPROC                glMultiTexCoord1d;
PFNGLMULTITEXCOORD1DVPROC               glMultiTexCoord1dv;
PFNGLMULTITEXCOORD1FPROC                glMultiTexCoord1f;
PFNGLMULTITEXCOORD1FVPROC               glMultiTexCoord1fv;
PFNGLMULTITEXCOORD1IPROC                glMultiTexCoord1i;
PFNGLMULTITEXCOORD1IVPROC               glMultiTexCoord1iv;
PFNGLMULTITEXCOORD1SPROC                glMultiTexCoord1s;
PFNGLMULTITEXCOORD1SVPROC               glMultiTexCoord1sv;
PFNGLMULTITEXCOORD2DPROC                glMultiTexCoord2d;
PFNGLMULTITEXCOORD2DVPROC               glMultiTexCoord2dv;
PFNGLMULTITEXCOORD2FPROC                glMultiTexCoord2f;
PFNGLMULTITEXCOORD2FVPROC               glMultiTexCoord2fv;
PFNGLMULTITEXCOORD2IPROC                glMultiTexCoord2i;
PFNGLMULTITEXCOORD2IVPROC               glMultiTexCoord2iv;
PFNGLMULTITEXCOORD2SPROC                glMultiTexCoord2s;
PFNGLMULTITEXCOORD2SVPROC               glMultiTexCoord2sv;
PFNGLMULTITEXCOORD3DPROC                glMultiTexCoord3d;
PFNGLMULTITEXCOORD3DVPROC               glMultiTexCoord3dv;
PFNGLMULTITEXCOORD3FPROC                glMultiTexCoord3f;
PFNGLMULTITEXCOORD3FVPROC               glMultiTexCoord3fv;
PFNGLMULTITEXCOORD3IPROC                glMultiTexCoord3i;
PFNGLMULTITEXCOORD3IVPROC               glMultiTexCoord3iv;
PFNGLMULTITEXCOORD3SPROC                glMultiTexCoord3s;
PFNGLMULTITEXCOORD3SVPROC               glMultiTexCoord3sv;
PFNGLMULTITEXCOORD4DPROC                glMultiTexCoord4d;
PFNGLMULTITEXCOORD4DVPROC               glMultiTexCoord4dv;
PFNGLMULTITEXCOORD4FPROC                glMultiTexCoord4f;
PFNGLMULTITEXCOORD4FVPROC               glMultiTexCoord4fv;
PFNGLMULTITEXCOORD4IPROC                glMultiTexCoord4i;
PFNGLMULTITEXCOORD4IVPROC               glMultiTexCoord4iv;
PFNGLMULTITEXCOORD4SPROC                glMultiTexCoord4s;
PFNGLMULTITEXCOORD4SVPROC               glMultiTexCoord4sv;
PFNGLMULTTRANSPOSEMATRIXDPROC           glMultTransposeMatrixd;
PFNGLMULTTRANSPOSEMATRIXFPROC           glMultTransposeMatrixf;
PFNGLSAMPLECOVERAGEPROC                 glSampleCoverage;

#endif

#ifndef __APPLE__
/* OpenGL 1.4.*/
PFNGLBLENDFUNCSEPARATEPROC              glBlendFuncSeparate;
PFNGLMULTIDRAWARRAYSPROC                glMultiDrawArrays;
PFNGLMULTIDRAWELEMENTSPROC              glMultiDrawElements;
PFNGLPOINTPARAMETERFPROC                glPointParameterf;
PFNGLPOINTPARAMETERFVPROC               glPointParameterfv;
PFNGLSECONDARYCOLOR3BPROC               glSecondaryColor3b;
PFNGLSECONDARYCOLOR3BVPROC              glSecondaryColor3bv;
PFNGLSECONDARYCOLOR3DPROC               glSecondaryColor3d;
PFNGLSECONDARYCOLOR3DVPROC              glSecondaryColor3dv;
PFNGLSECONDARYCOLOR3FPROC               glSecondaryColor3f;
PFNGLSECONDARYCOLOR3FVPROC              glSecondaryColor3fv;
PFNGLSECONDARYCOLOR3IPROC               glSecondaryColor3i;
PFNGLSECONDARYCOLOR3IVPROC              glSecondaryColor3iv;
PFNGLSECONDARYCOLOR3SPROC               glSecondaryColor3s;
PFNGLSECONDARYCOLOR3SVPROC              glSecondaryColor3sv;
PFNGLSECONDARYCOLOR3UBPROC              glSecondaryColor3ub;
PFNGLSECONDARYCOLOR3UBVPROC             glSecondaryColor3ubv;
PFNGLSECONDARYCOLOR3UIPROC              glSecondaryColor3ui;
PFNGLSECONDARYCOLOR3UIVPROC             glSecondaryColor3uiv;
PFNGLSECONDARYCOLOR3USPROC              glSecondaryColor3us;
PFNGLSECONDARYCOLOR3USVPROC             glSecondaryColor3usv;
PFNGLSECONDARYCOLORPOINTERPROC          glSecondaryColorPointer;
PFNGLWINDOWPOS2DPROC                    glWindowPos2d;
PFNGLWINDOWPOS2DVPROC                   glWindowPos2dv;
PFNGLWINDOWPOS2FPROC                    glWindowPos2f;
PFNGLWINDOWPOS2FVPROC                   glWindowPos2fv;
PFNGLWINDOWPOS2IPROC                    glWindowPos2i;
PFNGLWINDOWPOS2IVPROC                   glWindowPos2iv;
PFNGLWINDOWPOS2SPROC                    glWindowPos2s;
PFNGLWINDOWPOS2SVPROC                   glWindowPos2sv;
PFNGLWINDOWPOS3DPROC                    glWindowPos3d;
PFNGLWINDOWPOS3DVPROC                   glWindowPos3dv;
PFNGLWINDOWPOS3FPROC                    glWindowPos3f;
PFNGLWINDOWPOS3FVPROC                   glWindowPos3fv;
PFNGLWINDOWPOS3IPROC                    glWindowPos3i;
PFNGLWINDOWPOS3IVPROC                   glWindowPos3iv;
PFNGLWINDOWPOS3SPROC                    glWindowPos3s;
PFNGLWINDOWPOS3SVPROC                   glWindowPos3sv;

/* OpenGL 1.5*/
PFNGLGENQUERIESPROC                     glGenQueries;
PFNGLDELETEQUERIESPROC                  glDeleteQueries;
PFNGLISQUERYPROC                        glIsQuery;
PFNGLBEGINQUERYPROC                     glBeginQuery;
PFNGLENDQUERYPROC                       glEndQuery;
PFNGLGETQUERYIVPROC                     glGetQueryiv;
PFNGLGETQUERYOBJECTIVPROC               glGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC              glGetQueryObjectuiv;
PFNGLBINDBUFFERPROC                     glBindBuffer;
PFNGLDELETEBUFFERSPROC                  glDeleteBuffers;
PFNGLGENBUFFERSPROC                     glGenBuffers;
PFNGLISBUFFERPROC                       glIsBuffer;
PFNGLBUFFERDATAPROC                     glBufferData;
PFNGLBUFFERSUBDATAPROC                  glBufferSubData;
PFNGLGETBUFFERSUBDATAPROC               glGetBufferSubData;
PFNGLMAPBUFFERPROC                      glMapBuffer;
PFNGLUNMAPBUFFERPROC                    glUnmapBuffer;
PFNGLGETBUFFERPARAMETERIVPROC           glGetBufferParameteriv;
PFNGLGETBUFFERPOINTERVPROC              glGetBufferPointerv;

/* OpenGL 2.0*/
PFNGLBLENDEQUATIONSEPARATEPROC          glBlendEquationSeparate;
PFNGLDRAWBUFFERSPROC                    glDrawBuffers;
PFNGLSTENCILOPSEPARATEPROC              glStencilOpSeparate;
PFNGLSTENCILFUNCSEPARATEPROC            glStencilFuncSeparate;
PFNGLSTENCILMASKSEPARATEPROC            glStencilMaskSeparate;
PFNGLATTACHSHADERPROC                   glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC             glBindAttribLocation;
PFNGLCOMPILESHADERPROC                  glCompileShader;
PFNGLCREATEPROGRAMPROC                  glCreateProgram;
PFNGLCREATESHADERPROC                   glCreateShader;
PFNGLDELETEPROGRAMPROC                  glDeleteProgram;
PFNGLDELETESHADERPROC                   glDeleteShader;
PFNGLDETACHSHADERPROC                   glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC       glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC        glEnableVertexAttribArray;
PFNGLGETACTIVEATTRIBPROC                glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC               glGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC             glGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC              glGetAttribLocation;
PFNGLGETPROGRAMIVPROC                   glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC              glGetProgramInfoLog;
PFNGLGETSHADERIVPROC                    glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC               glGetShaderInfoLog;
PFNGLGETSHADERSOURCEPROC                glGetShaderSource;
PFNGLGETUNIFORMLOCATIONPROC             glGetUniformLocation;
PFNGLGETUNIFORMFVPROC                   glGetUniformfv;
PFNGLGETUNIFORMIVPROC                   glGetUniformiv;
PFNGLGETVERTEXATTRIBDVPROC              glGetVertexAttribdv;
PFNGLGETVERTEXATTRIBFVPROC              glGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC              glGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC        glGetVertexAttribPointerv;
PFNGLISPROGRAMPROC                      glIsProgram;
PFNGLISSHADERPROC                       glIsShader;
PFNGLLINKPROGRAMPROC                    glLinkProgram;
PFNGLSHADERSOURCEPROC                   glShaderSource;
PFNGLUSEPROGRAMPROC                     glUseProgram;
PFNGLUNIFORM1FPROC                      glUniform1f;
PFNGLUNIFORM2FPROC                      glUniform2f;
PFNGLUNIFORM3FPROC                      glUniform3f;
PFNGLUNIFORM4FPROC                      glUniform4f;
PFNGLUNIFORM1IPROC                      glUniform1i;
PFNGLUNIFORM2IPROC                      glUniform2i;
PFNGLUNIFORM3IPROC                      glUniform3i;
PFNGLUNIFORM4IPROC                      glUniform4i;
PFNGLUNIFORM1FVPROC                     glUniform1fv;
PFNGLUNIFORM2FVPROC                     glUniform2fv;
PFNGLUNIFORM3FVPROC                     glUniform3fv;
PFNGLUNIFORM4FVPROC                     glUniform4fv;
PFNGLUNIFORM1IVPROC                     glUniform1iv;
PFNGLUNIFORM2IVPROC                     glUniform2iv;
PFNGLUNIFORM3IVPROC                     glUniform3iv;
PFNGLUNIFORM4IVPROC                     glUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC               glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC               glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC               glUniformMatrix4fv;
PFNGLVALIDATEPROGRAMPROC                glValidateProgram;
PFNGLVERTEXATTRIB1DPROC                 glVertexAttrib1d;
PFNGLVERTEXATTRIB1DVPROC                glVertexAttrib1dv;
PFNGLVERTEXATTRIB1FPROC                 glVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC                glVertexAttrib1fv;
PFNGLVERTEXATTRIB1SPROC                 glVertexAttrib1s;
PFNGLVERTEXATTRIB1SVPROC                glVertexAttrib1sv;
PFNGLVERTEXATTRIB2DPROC                 glVertexAttrib2d;
PFNGLVERTEXATTRIB2DVPROC                glVertexAttrib2dv;
PFNGLVERTEXATTRIB2FPROC                 glVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC                glVertexAttrib2fv;
PFNGLVERTEXATTRIB2SPROC                 glVertexAttrib2s;
PFNGLVERTEXATTRIB2SVPROC                glVertexAttrib2sv;
PFNGLVERTEXATTRIB3DPROC                 glVertexAttrib3d;
PFNGLVERTEXATTRIB3DVPROC                glVertexAttrib3dv;
PFNGLVERTEXATTRIB3FPROC                 glVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC                glVertexAttrib3fv;
PFNGLVERTEXATTRIB3SPROC                 glVertexAttrib3s;
PFNGLVERTEXATTRIB3SVPROC                glVertexAttrib3sv;
PFNGLVERTEXATTRIB4NBVPROC               glVertexAttrib4Nbv;
PFNGLVERTEXATTRIB4NIVPROC               glVertexAttrib4Niv;
PFNGLVERTEXATTRIB4NSVPROC               glVertexAttrib4Nsv;
PFNGLVERTEXATTRIB4NUBPROC               glVertexAttrib4Nub;
PFNGLVERTEXATTRIB4NUBVPROC              glVertexAttrib4Nubv;
PFNGLVERTEXATTRIB4NUIVPROC              glVertexAttrib4Nuiv;
PFNGLVERTEXATTRIB4NUSVPROC              glVertexAttrib4Nusv;
PFNGLVERTEXATTRIB4BVPROC                glVertexAttrib4bv;
PFNGLVERTEXATTRIB4DPROC                 glVertexAttrib4d;
PFNGLVERTEXATTRIB4DVPROC                glVertexAttrib4dv;
PFNGLVERTEXATTRIB4FPROC                 glVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC                glVertexAttrib4fv;
PFNGLVERTEXATTRIB4IVPROC                glVertexAttrib4iv;
PFNGLVERTEXATTRIB4SPROC                 glVertexAttrib4s;
PFNGLVERTEXATTRIB4SVPROC                glVertexAttrib4sv;
PFNGLVERTEXATTRIB4UBVPROC               glVertexAttrib4ubv;
PFNGLVERTEXATTRIB4UIVPROC               glVertexAttrib4uiv;
PFNGLVERTEXATTRIB4USVPROC               glVertexAttrib4usv;
PFNGLVERTEXATTRIBPOINTERPROC            glVertexAttribPointer;

/* OpenGL 2.1*/
PFNGLUNIFORMMATRIX2X3FVPROC             glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC             glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC             glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC             glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC             glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC             glUniformMatrix4x3fv;
#endif

void eol_opengl_init()
{

#if defined(_WIN32)
#define GetProc(x) (*wglGetProcAddress)((const GLubyte*)x)
#endif
#ifndef _WIN32
#define GetProc(x) (*glXGetProcAddress)((const GLubyte*)x)
#endif

#if defined(_WIN32)
  /* OpenGL 1.2.*/
  glCopyTexSubImage3D         = (PFNGLCOPYTEXSUBIMAGE3DPROC)GetProc("glCopyTexSubImage3D");
  glDrawRangeElements         = (PFNGLDRAWRANGEELEMENTSPROC)(GetProc("glDrawRangeElements"));
  glTexImage3D                = (PFNGLTEXIMAGE3DPROC)(GetProc("glTexImage3D"));
  glTexSubImage3D             = (PFNGLTEXSUBIMAGE3DPROC)(GetProc("glTexSubImage3D"));

  /* OpenGL 1.3.*/
  glActiveTexture             = (PFNGLACTIVETEXTUREPROC)(GetProc("glActiveTexture"));
  glClientActiveTexture       = (PFNGLCLIENTACTIVETEXTUREPROC)(GetProc("glClientActiveTexture"));
  glMultiTexCoord1d           = (PFNGLMULTITEXCOORD1DPROC)(GetProc("glMultiTexCoord1d"));
  glMultiTexCoord1dv          = (PFNGLMULTITEXCOORD1DVPROC)(GetProc("glMultiTexCoord1dv"));
  glMultiTexCoord1f           = (PFNGLMULTITEXCOORD1FPROC)(GetProc("glMultiTexCoord1f"));
  glMultiTexCoord1fv          = (PFNGLMULTITEXCOORD1FVPROC)(GetProc("glMultiTexCoord1fv"));
  glMultiTexCoord1i           = (PFNGLMULTITEXCOORD1IPROC)(GetProc("glMultiTexCoord1i"));
  glMultiTexCoord1iv          = (PFNGLMULTITEXCOORD1IVPROC)(GetProc("glMultiTexCoord1iv"));
  glMultiTexCoord1s           = (PFNGLMULTITEXCOORD1SPROC)(GetProc("glMultiTexCoord1s"));
  glMultiTexCoord1sv          = (PFNGLMULTITEXCOORD1SVPROC)(GetProc("glMultiTexCoord1sv"));
  glMultiTexCoord2d           = (PFNGLMULTITEXCOORD2DPROC)(GetProc("glMultiTexCoord2d"));
  glMultiTexCoord2dv          = (PFNGLMULTITEXCOORD2DVPROC)(GetProc("glMultiTexCoord2dv"));
  glMultiTexCoord2f           = (PFNGLMULTITEXCOORD2FPROC)(GetProc("glMultiTexCoord2f"));
  glMultiTexCoord2fv          = (PFNGLMULTITEXCOORD2FVPROC)(GetProc("glMultiTexCoord2fv"));
  glMultiTexCoord2i           = (PFNGLMULTITEXCOORD2IPROC)(GetProc("glMultiTexCoord2i"));
  glMultiTexCoord2iv          = (PFNGLMULTITEXCOORD2IVPROC)(GetProc("glMultiTexCoord2iv"));
  glMultiTexCoord2s           = (PFNGLMULTITEXCOORD2SPROC)(GetProc("glMultiTexCoord2s"));
  glMultiTexCoord2sv          = (PFNGLMULTITEXCOORD2SVPROC)(GetProc("glMultiTexCoord2sv"));
  glMultiTexCoord3d           = (PFNGLMULTITEXCOORD3DPROC)(GetProc("glMultiTexCoord3d"));
  glMultiTexCoord3dv          = (PFNGLMULTITEXCOORD3DVPROC)(GetProc("glMultiTexCoord3dv"));
  glMultiTexCoord3f           = (PFNGLMULTITEXCOORD3FPROC)(GetProc("glMultiTexCoord3f"));
  glMultiTexCoord3fv          = (PFNGLMULTITEXCOORD3FVPROC)(GetProc("glMultiTexCoord3fv"));
  glMultiTexCoord3i           = (PFNGLMULTITEXCOORD3IPROC)(GetProc("glMultiTexCoord3i"));
  glMultiTexCoord3iv          = (PFNGLMULTITEXCOORD3IVPROC)(GetProc("glMultiTexCoord3iv"));
  glMultiTexCoord3s           = (PFNGLMULTITEXCOORD3SPROC)(GetProc("glMultiTexCoord3s"));
  glMultiTexCoord3sv          = (PFNGLMULTITEXCOORD3SVPROC)(GetProc("glMultiTexCoord3sv"));
  glMultiTexCoord4d           = (PFNGLMULTITEXCOORD4DPROC)(GetProc("glMultiTexCoord4d"));
  glMultiTexCoord4dv          = (PFNGLMULTITEXCOORD4DVPROC)(GetProc("glMultiTexCoord4dv"));
  glMultiTexCoord4f           = (PFNGLMULTITEXCOORD4FPROC)(GetProc("glMultiTexCoord4f"));
  glMultiTexCoord4fv          = (PFNGLMULTITEXCOORD4FVPROC)(GetProc("glMultiTexCoord4fv"));
  glMultiTexCoord4i           = (PFNGLMULTITEXCOORD4IPROC)(GetProc("glMultiTexCoord4i"));
  glMultiTexCoord4iv          = (PFNGLMULTITEXCOORD4IVPROC)(GetProc("glMultiTexCoord4iv"));
  glMultiTexCoord4s           = (PFNGLMULTITEXCOORD4SPROC)(GetProc("glMultiTexCoord4s"));
  glMultiTexCoord4sv          = (PFNGLMULTITEXCOORD4SVPROC)(GetProc("glMultiTexCoord4sv"));
  glLoadTransposeMatrixf      = (PFNGLLOADTRANSPOSEMATRIXFPROC)(GetProc("glLoadTransposeMatrixf"));
  glLoadTransposeMatrixd      = (PFNGLLOADTRANSPOSEMATRIXDPROC)(GetProc("glLoadTransposeMatrixd"));
  glMultTransposeMatrixf      = (PFNGLMULTTRANSPOSEMATRIXFPROC)(GetProc("glMultTransposeMatrixf"));
  glMultTransposeMatrixd      = (PFNGLMULTTRANSPOSEMATRIXDPROC)(GetProc("glMultTransposeMatrixd"));
  glSampleCoverage            = (PFNGLSAMPLECOVERAGEPROC)(GetProc("glSampleCoverage"));
  glCompressedTexImage3D      = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)(GetProc("glCompressedTexImage3D"));
  glCompressedTexImage2D      = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)(GetProc("glCompressedTexImage2D"));
  glCompressedTexImage1D      = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)(GetProc("glCompressedTexImage1D"));
  glCompressedTexSubImage3D   = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)(GetProc("glCompressedTexSubImage3D"));
  glCompressedTexSubImage2D   = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GetProc("glCompressedTexSubImage2D"));
  glCompressedTexSubImage1D   = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)(GetProc("glCompressedTexSubImage1D"));
  glGetCompressedTexImage     = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)(GetProc("glGetCompressedTexImage"));
#endif

#ifndef __APPLE__
  /* OpenGL 1.4.*/
  glMultiDrawArrays           = (PFNGLMULTIDRAWARRAYSPROC)(GetProc("glMultiDrawArrays"));
  glMultiDrawElements         = (PFNGLMULTIDRAWELEMENTSPROC)(GetProc("glMultiDrawElements"));
  glPointParameterf           = (PFNGLPOINTPARAMETERFPROC)(GetProc("glPointParameterf"));
  glPointParameterfv          = (PFNGLPOINTPARAMETERFVPROC)(GetProc("glPointParameterfv"));
  glSecondaryColor3b          = (PFNGLSECONDARYCOLOR3BPROC)(GetProc("glSecondaryColor3b"));
  glSecondaryColor3bv         = (PFNGLSECONDARYCOLOR3BVPROC)(GetProc("glSecondaryColor3bv"));
  glSecondaryColor3d          = (PFNGLSECONDARYCOLOR3DPROC)(GetProc("glSecondaryColor3d"));
  glSecondaryColor3dv         = (PFNGLSECONDARYCOLOR3DVPROC)(GetProc("glSecondaryColor3dv"));
  glSecondaryColor3f          = (PFNGLSECONDARYCOLOR3FPROC)(GetProc("glSecondaryColor3f"));
  glSecondaryColor3fv         = (PFNGLSECONDARYCOLOR3FVPROC)(GetProc("glSecondaryColor3fv"));
  glSecondaryColor3i          = (PFNGLSECONDARYCOLOR3IPROC)(GetProc("glSecondaryColor3i"));
  glSecondaryColor3iv         = (PFNGLSECONDARYCOLOR3IVPROC)(GetProc("glSecondaryColor3iv"));
  glSecondaryColor3s          = (PFNGLSECONDARYCOLOR3SPROC)(GetProc("glSecondaryColor3s"));
  glSecondaryColor3sv         = (PFNGLSECONDARYCOLOR3SVPROC)(GetProc("glSecondaryColor3sv"));
  glSecondaryColor3ub         = (PFNGLSECONDARYCOLOR3UBPROC)(GetProc("glSecondaryColor3ub"));
  glSecondaryColor3ubv        = (PFNGLSECONDARYCOLOR3UBVPROC)(GetProc("glSecondaryColor3ubv"));
  glSecondaryColor3ui         = (PFNGLSECONDARYCOLOR3UIPROC)(GetProc("glSecondaryColor3ui"));
  glSecondaryColor3uiv        = (PFNGLSECONDARYCOLOR3UIVPROC)(GetProc("glSecondaryColor3uiv"));
  glSecondaryColor3us         = (PFNGLSECONDARYCOLOR3USPROC)(GetProc("glSecondaryColor3us"));
  glSecondaryColor3usv        = (PFNGLSECONDARYCOLOR3USVPROC)(GetProc("glSecondaryColor3usv"));
  glSecondaryColorPointer     = (PFNGLSECONDARYCOLORPOINTERPROC)(GetProc("glSecondaryColorPointer"));
  glBlendFuncSeparate         = (PFNGLBLENDFUNCSEPARATEPROC)(GetProc("glBlendFuncSeparate"));
  glWindowPos2d               = (PFNGLWINDOWPOS2DPROC)(GetProc("glWindowPos2d"));
  glWindowPos2f               = (PFNGLWINDOWPOS2FPROC)(GetProc("glWindowPos2f"));
  glWindowPos2i               = (PFNGLWINDOWPOS2IPROC)(GetProc("glWindowPos2i"));
  glWindowPos2s               = (PFNGLWINDOWPOS2SPROC)(GetProc("glWindowPos2s"));
  glWindowPos2dv              = (PFNGLWINDOWPOS2DVPROC)(GetProc("glWindowPos2dv"));
  glWindowPos2fv              = (PFNGLWINDOWPOS2FVPROC)(GetProc("glWindowPos2fv"));
  glWindowPos2iv              = (PFNGLWINDOWPOS2IVPROC)(GetProc("glWindowPos2iv"));
  glWindowPos2sv              = (PFNGLWINDOWPOS2SVPROC)(GetProc("glWindowPos2sv"));
  glWindowPos3d               = (PFNGLWINDOWPOS3DPROC)(GetProc("glWindowPos3d"));
  glWindowPos3f               = (PFNGLWINDOWPOS3FPROC)(GetProc("glWindowPos3f"));
  glWindowPos3i               = (PFNGLWINDOWPOS3IPROC)(GetProc("glWindowPos3i"));
  glWindowPos3s               = (PFNGLWINDOWPOS3SPROC)(GetProc("glWindowPos3s"));
  glWindowPos3dv              = (PFNGLWINDOWPOS3DVPROC)(GetProc("glWindowPos3dv"));
  glWindowPos3fv              = (PFNGLWINDOWPOS3FVPROC)(GetProc("glWindowPos3fv"));
  glWindowPos3iv              = (PFNGLWINDOWPOS3IVPROC)(GetProc("glWindowPos3iv"));
  glWindowPos3sv              = (PFNGLWINDOWPOS3SVPROC)(GetProc("glWindowPos3sv"));

  /* OpenGL 1.5.*/
  glGenQueries                = (PFNGLGENQUERIESPROC)(GetProc("glGenQueries"));
  glDeleteQueries             = (PFNGLDELETEQUERIESPROC)(GetProc("glDeleteQueries"));
  glIsQuery                   = (PFNGLISQUERYPROC)(GetProc("glIsQuery"));
  glBeginQuery                = (PFNGLBEGINQUERYPROC)(GetProc("glBeginQuery"));
  glEndQuery                  = (PFNGLENDQUERYPROC)(GetProc("glEndQuery"));
  glGetQueryiv                = (PFNGLGETQUERYIVPROC)(GetProc("glGetQueryiv"));
  glGetQueryObjectiv          = (PFNGLGETQUERYOBJECTIVPROC)(GetProc("glGetQueryObjectiv"));
  glGetQueryObjectuiv         = (PFNGLGETQUERYOBJECTUIVPROC)(GetProc("glGetQueryObjectuiv"));
  glBindBuffer                = (PFNGLBINDBUFFERPROC)(GetProc("glBindBuffer"));
  glDeleteBuffers             = (PFNGLDELETEBUFFERSPROC)(GetProc("glDeleteBuffers"));
  glGenBuffers                = (PFNGLGENBUFFERSPROC)(GetProc("glGenBuffers"));
  glIsBuffer                  = (PFNGLISBUFFERPROC)(GetProc("glIsBuffer"));
  glBufferData                = (PFNGLBUFFERDATAPROC)(GetProc("glBufferData"));
  glBufferSubData             = (PFNGLBUFFERSUBDATAPROC)(GetProc("glBufferSubData"));
  glGetBufferSubData          = (PFNGLGETBUFFERSUBDATAPROC)(GetProc("glGetBufferSubData"));
  glMapBuffer                 = (PFNGLMAPBUFFERPROC)(GetProc("glMapBuffer"));
  glUnmapBuffer               = (PFNGLUNMAPBUFFERPROC)(GetProc("glUnmapBuffer"));
  glGetBufferParameteriv      = (PFNGLGETBUFFERPARAMETERIVPROC)(GetProc("glGetBufferParameteriv"));
  glGetBufferPointerv         = (PFNGLGETBUFFERPOINTERVPROC)(GetProc("glGetBufferPointerv"));

  /* OpenGL 2.0.*/
  glBlendEquationSeparate     = (PFNGLBLENDEQUATIONSEPARATEPROC)(GetProc("glBlendEquationSeparate"));
  glDrawBuffers               = (PFNGLDRAWBUFFERSPROC)(GetProc("glDrawBuffers"));
  glStencilOpSeparate         = (PFNGLSTENCILOPSEPARATEPROC)(GetProc("glStencilOpSeparate"));
  glStencilFuncSeparate       = (PFNGLSTENCILFUNCSEPARATEPROC)(GetProc("glStencilFuncSeparate"));
  glStencilMaskSeparate       = (PFNGLSTENCILMASKSEPARATEPROC)(GetProc("glStencilMaskSeparate"));
  glAttachShader              = (PFNGLATTACHSHADERPROC)(GetProc("glAttachShader"));
  glBindAttribLocation        = (PFNGLBINDATTRIBLOCATIONPROC)(GetProc("glBindAttribLocation"));
  glCompileShader             = (PFNGLCOMPILESHADERPROC)(GetProc("glCompileShader"));
  glCreateProgram             = (PFNGLCREATEPROGRAMPROC)(GetProc("glCreateProgram"));
  glCreateShader              = (PFNGLCREATESHADERPROC)(GetProc("glCreateShader"));
  glDeleteProgram             = (PFNGLDELETEPROGRAMPROC)(GetProc("glDeleteProgram"));
  glDeleteShader              = (PFNGLDELETESHADERPROC)(GetProc("glDeleteShader"));
  glDetachShader              = (PFNGLDETACHSHADERPROC)(GetProc("glDetachShader"));
  glDisableVertexAttribArray  = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GetProc("glDisableVertexAttribArray"));
  glEnableVertexAttribArray   = (PFNGLENABLEVERTEXATTRIBARRAYPROC)(GetProc("glEnableVertexAttribArray"));
  glGetActiveAttrib           = (PFNGLGETACTIVEATTRIBPROC)(GetProc("glGetActiveAttrib"));
  glGetActiveUniform          = (PFNGLGETACTIVEUNIFORMPROC)(GetProc("glGetActiveUniform"));
  glGetAttachedShaders        = (PFNGLGETATTACHEDSHADERSPROC)(GetProc("glGetAttachedShaders"));
  glGetAttribLocation         = (PFNGLGETATTRIBLOCATIONPROC)(GetProc("glGetAttribLocation"));
  glGetProgramiv              = (PFNGLGETPROGRAMIVPROC)(GetProc("glGetProgramiv"));
  glGetProgramInfoLog         = (PFNGLGETPROGRAMINFOLOGPROC)(GetProc("glGetProgramInfoLog"));
  glGetShaderiv               = (PFNGLGETSHADERIVPROC)(GetProc("glGetShaderiv"));
  glGetShaderInfoLog          = (PFNGLGETSHADERINFOLOGPROC)(GetProc("glGetShaderInfoLog"));
  glGetShaderSource           = (PFNGLGETSHADERSOURCEPROC)(GetProc("glGetShaderSource"));
  glGetUniformLocation        = (PFNGLGETUNIFORMLOCATIONPROC)(GetProc("glGetUniformLocation"));
  glGetUniformfv              = (PFNGLGETUNIFORMFVPROC)(GetProc("glGetUniformfv"));
  glGetUniformiv              = (PFNGLGETUNIFORMIVPROC)(GetProc("glGetUniformiv"));
  glGetVertexAttribdv         = (PFNGLGETVERTEXATTRIBDVPROC)(GetProc("glGetVertexAttribdv"));
  glGetVertexAttribfv         = (PFNGLGETVERTEXATTRIBFVPROC)(GetProc("glGetVertexAttribfv"));
  glGetVertexAttribiv         = (PFNGLGETVERTEXATTRIBIVPROC)(GetProc("glGetVertexAttribiv"));
  glGetVertexAttribPointerv   = (PFNGLGETVERTEXATTRIBPOINTERVPROC)(GetProc("glGetVertexAttribPointerv"));
  glIsProgram                 = (PFNGLISPROGRAMPROC)(GetProc("glIsProgram"));
  glIsShader                  = (PFNGLISSHADERPROC)(GetProc("glIsShader"));
  glLinkProgram               = (PFNGLLINKPROGRAMPROC)(GetProc("glLinkProgram"));
  glShaderSource              = (PFNGLSHADERSOURCEPROC)(GetProc("glShaderSource"));
  glUseProgram                = (PFNGLUSEPROGRAMPROC)(GetProc("glUseProgram"));
  glUniform1f                 = (PFNGLUNIFORM1FPROC)(GetProc("glUniform1f"));
  glUniform2f                 = (PFNGLUNIFORM2FPROC)(GetProc("glUniform2f"));
  glUniform3f                 = (PFNGLUNIFORM3FPROC)(GetProc("glUniform3f"));
  glUniform4f                 = (PFNGLUNIFORM4FPROC)(GetProc("glUniform4f"));
  glUniform1i                 = (PFNGLUNIFORM1IPROC)(GetProc("glUniform1i"));
  glUniform2i                 = (PFNGLUNIFORM2IPROC)(GetProc("glUniform2i"));
  glUniform3i                 = (PFNGLUNIFORM3IPROC)(GetProc("glUniform3i"));
  glUniform4i                 = (PFNGLUNIFORM4IPROC)(GetProc("glUniform4i"));
  glUniform1fv                = (PFNGLUNIFORM1FVPROC)(GetProc("glUniform1fv"));
  glUniform2fv                = (PFNGLUNIFORM2FVPROC)(GetProc("glUniform2fv"));
  glUniform3fv                = (PFNGLUNIFORM3FVPROC)(GetProc("glUniform3fv"));
  glUniform4fv                = (PFNGLUNIFORM4FVPROC)(GetProc("glUniform4fv"));
  glUniform1iv                = (PFNGLUNIFORM1IVPROC)(GetProc("glUniform1iv"));
  glUniform2iv                = (PFNGLUNIFORM2IVPROC)(GetProc("glUniform2iv"));
  glUniform3iv                = (PFNGLUNIFORM3IVPROC)(GetProc("glUniform3iv"));
  glUniform4iv                = (PFNGLUNIFORM4IVPROC)(GetProc("glUniform4iv"));
  glUniformMatrix2fv          = (PFNGLUNIFORMMATRIX2FVPROC)(GetProc("glUniformMatrix2fv"));
  glUniformMatrix3fv          = (PFNGLUNIFORMMATRIX3FVPROC)(GetProc("glUniformMatrix3fv"));
  glUniformMatrix4fv          = (PFNGLUNIFORMMATRIX4FVPROC)(GetProc("glUniformMatrix4fv"));
  glValidateProgram           = (PFNGLVALIDATEPROGRAMPROC)(GetProc("glValidateProgram"));
  glVertexAttrib1d            = (PFNGLVERTEXATTRIB1DPROC)(GetProc("glVertexAttrib1d"));
  glVertexAttrib1dv           = (PFNGLVERTEXATTRIB1DVPROC)(GetProc("glVertexAttrib1dv"));
  glVertexAttrib1f            = (PFNGLVERTEXATTRIB1FPROC)(GetProc("glVertexAttrib1f"));
  glVertexAttrib1fv           = (PFNGLVERTEXATTRIB1FVPROC)(GetProc("glVertexAttrib1fv"));
  glVertexAttrib1s            = (PFNGLVERTEXATTRIB1SPROC)(GetProc("glVertexAttrib1s"));
  glVertexAttrib1sv           = (PFNGLVERTEXATTRIB1SVPROC)(GetProc("glVertexAttrib1sv"));
  glVertexAttrib2d            = (PFNGLVERTEXATTRIB2DPROC)(GetProc("glVertexAttrib2d"));
  glVertexAttrib2dv           = (PFNGLVERTEXATTRIB2DVPROC)(GetProc("glVertexAttrib2dv"));
  glVertexAttrib2f            = (PFNGLVERTEXATTRIB2FPROC)(GetProc("glVertexAttrib2f"));
  glVertexAttrib2fv           = (PFNGLVERTEXATTRIB2FVPROC)(GetProc("glVertexAttrib2fv"));
  glVertexAttrib2s            = (PFNGLVERTEXATTRIB2SPROC)(GetProc("glVertexAttrib2s"));
  glVertexAttrib2sv           = (PFNGLVERTEXATTRIB2SVPROC)(GetProc("glVertexAttrib2sv"));
  glVertexAttrib3d            = (PFNGLVERTEXATTRIB3DPROC)(GetProc("glVertexAttrib3d"));
  glVertexAttrib3dv           = (PFNGLVERTEXATTRIB3DVPROC)(GetProc("glVertexAttrib3dv"));
  glVertexAttrib3f            = (PFNGLVERTEXATTRIB3FPROC)(GetProc("glVertexAttrib3f"));
  glVertexAttrib3fv           = (PFNGLVERTEXATTRIB3FVPROC)(GetProc("glVertexAttrib3fv"));
  glVertexAttrib3s            = (PFNGLVERTEXATTRIB3SPROC)(GetProc("glVertexAttrib3s"));
  glVertexAttrib3sv           = (PFNGLVERTEXATTRIB3SVPROC)(GetProc("glVertexAttrib3sv"));
  glVertexAttrib4Nbv          = (PFNGLVERTEXATTRIB4NBVPROC)(GetProc("glVertexAttrib4Nbv"));
  glVertexAttrib4Niv          = (PFNGLVERTEXATTRIB4NIVPROC)(GetProc("glVertexAttrib4Niv"));
  glVertexAttrib4Nsv          = (PFNGLVERTEXATTRIB4NSVPROC)(GetProc("glVertexAttrib4Nsv"));
  glVertexAttrib4Nub          = (PFNGLVERTEXATTRIB4NUBPROC)(GetProc("glVertexAttrib4Nub"));
  glVertexAttrib4Nubv         = (PFNGLVERTEXATTRIB4NUBVPROC)(GetProc("glVertexAttrib4Nubv"));
  glVertexAttrib4Nuiv         = (PFNGLVERTEXATTRIB4NUIVPROC)(GetProc("glVertexAttrib4Nuiv"));
  glVertexAttrib4Nusv         = (PFNGLVERTEXATTRIB4NUSVPROC)(GetProc("glVertexAttrib4Nusv"));
  glVertexAttrib4bv           = (PFNGLVERTEXATTRIB4BVPROC)(GetProc("glVertexAttrib4bv"));
  glVertexAttrib4d            = (PFNGLVERTEXATTRIB4DPROC)(GetProc("glVertexAttrib4d"));
  glVertexAttrib4dv           = (PFNGLVERTEXATTRIB4DVPROC)(GetProc("glVertexAttrib4dv"));
  glVertexAttrib4f            = (PFNGLVERTEXATTRIB4FPROC)(GetProc("glVertexAttrib4f"));
  glVertexAttrib4fv           = (PFNGLVERTEXATTRIB4FVPROC)(GetProc("glVertexAttrib4fv"));
  glVertexAttrib4iv           = (PFNGLVERTEXATTRIB4IVPROC)(GetProc("glVertexAttrib4iv"));
  glVertexAttrib4s            = (PFNGLVERTEXATTRIB4SPROC)(GetProc("glVertexAttrib4s"));
  glVertexAttrib4sv           = (PFNGLVERTEXATTRIB4SVPROC)(GetProc("glVertexAttrib4sv"));
  glVertexAttrib4ubv          = (PFNGLVERTEXATTRIB4UBVPROC)(GetProc("glVertexAttrib4ubv"));
  glVertexAttrib4uiv          = (PFNGLVERTEXATTRIB4UIVPROC)(GetProc("glVertexAttrib4uiv"));
  glVertexAttrib4usv          = (PFNGLVERTEXATTRIB4USVPROC)(GetProc("glVertexAttrib4usv"));
  glVertexAttribPointer       = (PFNGLVERTEXATTRIBPOINTERPROC)(GetProc("glVertexAttribPointer"));

  /* OpenGL 2.1.*/
  glUniformMatrix2x3fv        = (PFNGLUNIFORMMATRIX2X3FVPROC)(GetProc("glUniformMatrix2x3fv"));
  glUniformMatrix3x2fv        = (PFNGLUNIFORMMATRIX3X2FVPROC)(GetProc("glUniformMatrix3x2fv"));
  glUniformMatrix2x4fv        = (PFNGLUNIFORMMATRIX2X4FVPROC)(GetProc("glUniformMatrix2x4fv"));
  glUniformMatrix4x2fv        = (PFNGLUNIFORMMATRIX4X2FVPROC)(GetProc("glUniformMatrix4x2fv"));
  glUniformMatrix3x4fv        = (PFNGLUNIFORMMATRIX3X4FVPROC)(GetProc("glUniformMatrix3x4fv"));
  glUniformMatrix4x3fv        = (PFNGLUNIFORMMATRIX4X3FVPROC)(GetProc("glUniformMatrix4x3fv"));
#endif
    
#undef GetProc
  _eol_opengl_initialized = GL_TRUE;
}

GLboolean eol_opengl_is_initialized()
{
	return _eol_opengl_initialized;
}

void eol_opengl_get_screen_coordinate(
  GLdouble glx,
  GLdouble gly,
  GLdouble glz,
  const GLdouble * model,
  const GLdouble * proj,
  const GLint    * view,
  GLint    *x,
  GLint    *y
)
{
  GLdouble tx,ty,tz;
  gluProject(glx,gly,glz,model,proj,view,&tx,&ty,&tz);
  if (x)*x = (GLint)x;
  if (y)*y = (GLint)y;
}

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
  )
{  
  GLdouble scrx, scry;
  scrx = (GLdouble)x;
  scry = view[3] - (GLdouble)y;
  
  gluUnProject(scrx,scry,z,model,proj,view,glx,gly,glz);
}

GLint eol_opengl_power_of_two(GLint in)
{
  /*TODO: this does nothing*/
  return in;  
}
/*eol@eof*/

