gles2framework
==============

a simple framework for OpenGLES 2.0 written in C

intended to help learning and tutorial writing

Currently supports XORG (linux) and soon to support the raspberry pi
(none Xwindows proprietory EGL)

dependencies libpng, libEGL, libGLES (2.0) 

Brief API notes at the end of the readme


Project structure
=================

-include________source code include files

-kazmath________source code and included for the kazmath library

-o______________somewhere to put intermediate binary objects

-src____________source code for the framework

-src-models_____sorce code for model data

-shaders________fragment and vertex shaders used by the framework

-textures_______images used to texture models

-obj2opengl_____script used to turn wavefront OBJ models into source code

-models_________source models used by obj2opengl

simple.c________the simplist example of using the framework

main.c__________a more complex "gamelet" example of using the framework

Makefile________tells the compiler how to build the examples

README.md_______this file!



kazmath
=======

there is no need to seperatly compile the kazmath library for your platform
kazmath sources are now automatically included into the project by
the Makefile

All though the source is unchanged I have deleted everthing except the C source and
the html documentation the full distribution of kazmath is available at 
https://github.com/Kazade/kazmath


obj2opengl
==========

In order to create code from OBJ files use this script, ensure you set 
the output file name to something like shape.c

you will have to manually make shape.h it should look something like this

extern float shapeVerts[];
extern float shapeTexCoords[];
extern float shapeNormals[];
extern unsigned int shapeNumVerts;

in addition you will have to add a rule in the Makefile




support routines
================

int loadPNG(const char *filename);

loads a specified png file returning a GLES texture handle
________________________________________________________________________

int makeContext();

creates a native window and sets up a GLES context within it
________________________________________________________________________

void closeContext();

closes GLES context and window
________________________________________________________________________

GLuint create_shader(const char *filename, GLenum type);

returns a GLES shader handle from a file you must specify what type 
of shader it is either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
________________________________________________________________________

GLuint getShaderLocation(int type, GLuint prog, const char *name);

given a type of shaderAttrib or shaderUniform, a shader program handle
and a name for the attrib or uniform it returns a location handle 
________________________________________________________________________

void initGlPrint(int w, int h);

This initialises the resources used by the glPrintf you must supply
the windows width and height
________________________________________________________________________

void glPrintf(float x, float y, const char *fmt, ...);

this behaves exactly like a normal printf except for the first two
parameters which specify the starting coordinate
________________________________________________________________________

void swapBuffers();

In order isolate egl and native window handles use this routine instead
of eglSwapBuffers
________________________________________________________________________

void doEvents();

this should be called once a frame to update the key down boolean array
and the mouse information
________________________________________________________________________

int* getMouse();

this returns a pointer to an array of 3 ints the first 2 are the x and y
mouse position the 3rd int is a bit field reflecting the state of the
mouse buttons
________________________________________________________________________

bool* getKeys();

this is an array of 256 bools, while a key is held down the coresponding
bool is true, key values are defined in keys.h
________________________________________________________________________

int createObj(struct obj_t *obj, int numVerts, float verts[], float txVert[],
        float norms[], char *vertShader, char *fragShader);

pass an empty obj_t struct, the number of vertices and arrays of verts,
texture coordinates and normals, finally you need to specify the file names
for the vert and frag shaders 
________________________________________________________________________

int createObjCopyShader(struct obj_t *obj, int numVerts, float verts[],
			float txVert[], float norms[], struct obj_t *sdrobj);

this allows you to initialise a obj shape but using an existing obj's
shader.
________________________________________________________________________

void drawObj(struct obj_t *obj, kmMat4 * combined, kmMat4 * mv);

this draws an obj, you need to supply a combined model, view and projection
matrices as well as a combined model, view matrix for the lighting

-------------------------------------------------------------------------

int getDisplayWidth();
int getDisplayHeight();

returns full screen width and height, for now when not on Raspberry PI 
the "screen" is fixed to a 640x480 window 


