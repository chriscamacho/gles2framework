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
	int tex;
	int base;
	int vertBuf;
	int texBuf;
	float tHeight;
	float tLines;
	int fWidth;
	int fHeight;
};

typedef struct __fnt font_t;

font_t* createFont(const char* tpath,int cbase,float tHeight,float tLines, int fWidth, int fHeight);
void initGlPrint(int w, int h);
void glPrintf(float x, float y, font_t* fnt, const char *fmt, ...);
void reProjectSprites(int w, int h);
void reProjectGlPrint(int w, int h);
void resizePointCloudSprites(float s);
