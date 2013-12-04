
//stderr, fprintf
#include <iostream>

//malloc
#include <malloc.h>

//strlen
#include <string.h>

//O_RDONLY
#include <fcntl.h>

#include "vglLoadShader.h"


GLuint vglShaderTypeIsOk(GLuint type){
  switch (type){
  case GL_VERTEX_SHADER:
  case GL_FRAGMENT_SHADER:
    fprintf(stderr, "Shader type OK.\n");
    return 1;
    break;
  }
  fprintf(stderr, "Error: invalid shader type.\n");
  return 0;
}

GLuint vglShaderCreateIsOk(GLuint shader){
  if (shader){
    fprintf(stderr, "Shader creation OK.\n");
  }
  else{
    GLchar infoLog[VGL_MAX_INFO_LOG_SIZE];
    fprintf(stderr, "Error in shader creation|\n");
  }
  return shader;
}

GLuint vglShaderCompileIsOk(GLuint shader){
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success){
    fprintf(stderr, "Shader compilation OK.\n");
  }
  else{
    GLchar infoLog[VGL_MAX_INFO_LOG_SIZE];
    glGetShaderInfoLog(shader, VGL_MAX_INFO_LOG_SIZE, NULL, infoLog);
    fprintf(stderr, "Error in shader compilation:\n");
    fprintf(stderr, "%s\n", infoLog);
  }
  return success;
}

GLuint vglShaderLinkIsOk(GLuint program){
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success){
    fprintf(stderr, "Link status OK.\n");
  }
  else{
    GLchar infoLog[VGL_MAX_INFO_LOG_SIZE];
    glGetProgramInfoLog(program, VGL_MAX_INFO_LOG_SIZE, NULL, infoLog);
    fprintf(stderr, "Error in program linking:\n");
    fprintf(stderr, "%s\n", infoLog);
  }
  return success;
}

GLuint vglShaderValidateIsOk(GLuint program){
  GLint success;
  glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
  if (success){
    fprintf(stderr, "Validate status OK.\n");
  }
  else{
    GLchar infoLog[VGL_MAX_INFO_LOG_SIZE];
    glGetProgramInfoLog(program, VGL_MAX_INFO_LOG_SIZE, NULL, infoLog);
    fprintf(stderr, "Error in program validation:\n");
    fprintf(stderr, "%s\n", infoLog);
  }
  return success;
}

void vglShaderFileReadStatus(const GLchar* text, char* filename){
  if (text){
    fprintf(stderr, "Shader source file read OK.\n");
  }
  else{
    fprintf(stderr, "Error reading shader source file:");
    if (filename){
      fprintf(stderr, " %s", filename);
    }
    else{
      fprintf(stderr, " filename is null");
    }
    fprintf(stderr, "\n");
  }
}

GLchar *vglTextFileRead(char *fn) {
  FILE *fp;
  GLchar *content = NULL;
  int f, count;

  if (fn != NULL) {
    fp = fopen(fn,"rt");
    if (fp != NULL) {

      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      fseek(fp, 0, SEEK_SET);

      if (count > 0) {
        content = (GLchar *)malloc(sizeof(GLchar) * (count+1));
        count = fread(content,sizeof(GLchar),count,fp);
        content[count] = '\0';
      }
      fclose(fp);
    }
  }
  return content;
} 

GLuint vglShaderLoad(GLuint ShaderType, char* filename){
  GLint retval = 0;

  GLuint s;
  GLuint success;
  GLint p;

  glGetIntegerv(GL_CURRENT_PROGRAM, &p);
  printf("current program = %d\n", p);

  if (vglShaderTypeIsOk(ShaderType)){
    s = glCreateShader(ShaderType);
  }

  if (vglShaderCreateIsOk (s)){
    const GLchar* src = vglTextFileRead(filename);

    vglShaderFileReadStatus(src, filename);

    printf("shader code = %s\n", src);
    if (src){
      const GLint len = strlen(src);
      printf("code length= %d\n", len);

      glShaderSource(s, 1, &src, &len);

      glCompileShader(s);
      success = vglShaderCompileIsOk(s);
      if (!success) return 0;

      if (p){
        printf("Will use existing program\n");
      }
      else{
        printf("Creating new program\n");
        p = glCreateProgram();
      }

      printf("Attaching shader\n");
      glAttachShader(p, s);

      glLinkProgram(p);
      success = vglShaderLinkIsOk(p);
      if (!success) return 0;
  
      glValidateProgram(p);
      success = vglShaderValidateIsOk(p);
      if (!success) return 0;

      glUseProgram(p);

      glGetIntegerv(GL_CURRENT_PROGRAM, &retval);
      printf("current program = %d\n", retval);

      glDeleteShader(s);
    }
    else{
      return 0;
    }
  }
  return p;
}

