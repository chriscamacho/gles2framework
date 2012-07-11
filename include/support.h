//#include <linux/types.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <kazmath.h>

#include <stdbool.h>


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
int getDisplayWidth();
int getDisplayHeight();
void initSprite(int w, int h);
void drawSprite(float x, float y, float w, float h, float a, int tex);
void setMouseRelative(bool mode);

struct joystick_t {
	int fd;
	signed short axis[8]; // could have 2 analogue sticks with 2 digital hats
	long buttons;
};

struct joystick_t *getJoystick(int j);
void updateJoystick(struct joystick_t *js);

