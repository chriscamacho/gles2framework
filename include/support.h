//#include <linux/types.h>

#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#endif

#ifdef __FOR_GLFW__
#include <GL/glew.h>
#include <GL/glfw.h>
typedef unsigned int uint;
#endif

#include <kazmath.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif


enum shaderLocationType { shaderAttrib, shaderUniform };
GLuint getShaderLocation(int type, GLuint prog, const char *name);
char *file_read(const char *filename);
GLuint create_shader(const char *filename, GLenum type);
void print_log(GLuint object);
int loadPNG(const char *filename);
int makeContext();
void closeContext();
void swapBuffers();
int getDisplayWidth();
int getDisplayHeight();
void initSprite(int w, int h);
void drawSprite(float x, float y, float w, float h, float a, int tex);
float rand_range(float min,float max);



struct pointCloud_t {
	int totalPoints;
	float *pos;
	float *vel;
	int vertBuf;
	float tick;
};

void initPointClouds(const char* vertS, const char* fragS, float pntSize);
struct pointCloud_t* createPointCloud(int size);
void drawPointCloud(struct pointCloud_t* pntC,kmMat4* m);
void freePointCloud(struct pointCloud_t* pntC);


struct __fnt {
	uint tex;
	uint base;
	uint vertBuf;
	uint texBuf;
	float tHeight;
	float tLines;
	int fWidth;
	int fHeight;
};

typedef struct __fnt font_t;

font_t* createFont(const char* tpath,uint cbase,float tHeight,float tLines, int fWidth, int fHeight);
void initGlPrint(int w, int h);
void glPrintf(float x, float y, font_t* fnt, const char *fmt, ...);


