#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <kazmath.h>

#include <stdbool.h>

#ifdef __FOR_RPI__

#include  "bcm_host.h"

#endif

enum shaderLocationType { shaderAttrib, shaderUniform };
GLuint getShaderLocation(int type, GLuint prog, const char *name);

char *file_read(const char *filename);
GLuint create_shader(const char *filename, GLenum type);
void print_log(GLuint object);
int loadPNG(const char *filename);
int makeContext();
void closeContext();
void initGlPrint(int w, int h);
void glPrintf(float x, float y, const char *fmt, ...);
void swapBuffers();
void doEvents();
int *getMouse();
bool *getKeys();
