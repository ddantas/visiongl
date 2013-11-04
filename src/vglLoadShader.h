
//GL
#include <GL/glew.h>
#include <GL/freeglut.h>

#define VGL_MAX_INFO_LOG_SIZE 1000

GLuint vglShaderTypeIsOk(GLuint type);
GLuint vglShaderCreateIsOk(GLuint shader);
GLuint vglShaderCompileIsOk(GLuint shader);
GLuint vglShaderLinkIsOk(GLuint program);
GLuint vglShaderValidateIsOk(GLuint program);
void vglShaderFileReadStatus(const GLchar* text, char* filename);
GLchar *vglTextFileRead(char *fn) ;
GLuint vglSingleShaderLoad(GLuint ShaderType, char* filename);
GLuint vglShaderLoad(GLuint ShaderType, char* filename);
GLuint vglProgramLoad(char* v_filename, char* f_filename);
