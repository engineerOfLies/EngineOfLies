#include "eol_shader.h"
#include "eol_loader.h"
#include "eol_logger.h"

/*local global variables*/

/*local function prototypes*/
GLuint eol_shader_link(GLuint vertShader, GLuint fragShader);
GLuint eol_shader_compile(GLenum type, const GLchar *pszSource, GLint length);


GLuint eol_shader_load_program(char *filenameVertex, char *filenameFragment)
{
  GLuint program = 0;
  char *buffer1;
  char *buffer2;
  const GLchar *pSource = 0;
  long length = 0;
  GLuint vertShader = 0;
  GLuint fragShader = 0;
  FILE* file;

  if (filenameVertex != NULL)
  {
    file = fopen(filenameVertex, "r");
    if(!file)
    {
      eol_logger_message(
        EOL_LOG_WARN,
        "eol_shader:failed to open vert shader file %s\n", filenameVertex);
      return 0;
    }
    fseek (file , 0 , SEEK_END);
    length = ftell (file);
    rewind (file);
    buffer1= (char*) malloc(sizeof(char)*length);
    fread(buffer1,1,length,file);
    
    fclose (file);
    pSource = (const GLchar *)(buffer1);
    
    vertShader = eol_shader_compile(GL_VERTEX_SHADER, pSource, length);
  }
  
  if (filenameFragment != NULL)
  {
    file = fopen(filenameFragment, "r");
    if(!file)
    {
      eol_logger_message(
        EOL_LOG_WARN,
        "eol_shader:failed to open frag shader file %s\n", filenameFragment);
        /*TODO make this fail clean*/
      return 0;
    }
    fseek (file , 0 , SEEK_END);
    length = ftell(file);
    rewind(file);
    buffer2= (char*) malloc(sizeof(char)*length);
    fread(buffer2,1,length,file);
    
    fclose (file);
    pSource = (const GLchar *)(buffer2);
    fragShader = eol_shader_compile(GL_FRAGMENT_SHADER, pSource, length);
  }

  program = eol_shader_link(vertShader, fragShader);
  
  return program;
}

GLuint eol_shader_link(GLuint vertShader, GLuint fragShader)
{
  
  GLuint program;
  GLint linked = 0;
  if (glCreateProgram == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_shader:create program not supported.\n");
    return 0;
  }
  program = glCreateProgram();
  
  if (program)
  {
    
    if (vertShader)
      glAttachShader(program, vertShader);
    
    if (fragShader)
      glAttachShader(program, fragShader);
    
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
      eol_logger_message(
        EOL_LOG_WARN, "eol_shader:failed to link shader\n");
      return 0;
    }
    if (vertShader)
      glDeleteShader(vertShader);
    
    if (fragShader)
      glDeleteShader(fragShader);
  }
  
  return program;
}

GLuint eol_shader_compile(GLenum type, const GLchar *pszSource, GLint length)
{
  
  GLuint shader;
  GLint compiled = 0;
  if (glCreateShader == NULL)
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "eol_shader:create program not supported.\n");
    return 0;
  }
  shader = glCreateShader(type);
  
  if (shader)
  {
    glShaderSource(shader, 1, &pszSource, &length);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
      eol_logger_message(
        EOL_LOG_WARN,
        "eol_shader:failed to compile shader\n");
      return 0;
    }
  }
  
  return shader;
}

/*eol@eof*/
