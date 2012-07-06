gles2framework
==============

a simple framework for OpenGLES 2.0 written in C

*** Brief API notes at the end of the readme ***

intended to help learning and tutorial writing

Currently supports XORG (linux) and the raspberry pi
(none Xwindows proprietory EGL)
on the pi it uses X for input so you need to be running xwindows
when the executables run

dependencies libpng, libEGL, libGLES (2.0)

pkg-config, build-esentials and development libraries must be installed
to compile the framework

if you have not compiled GLES source code on your pi before you may need to make
a file called

/etc/ld.so.conf.d/vc.conf

it should contain just the line

/opt/vc/lib


the rather hacky ODE example is only really for advanced users...
compile ode from source (version v0.12) release you can then
cp ../ode-0.12/ode/src/.libs/libode.a .
from the gles2framework directory

You need it to use dSingle with trimesh support and you might want 
other things like custom cylinder vs cylinder colliders etc you won't 
get from the repo version set drawstuff=none for quicker build (no demos etc)

run ./configure --help in the ode directory.





Project structure
=================

-include________source code include files

-kazmath________source code and docs for the kazmath library

-o______________somewhere to put intermediate binary objects

-src____________source code for the framework

-obj2opengl_____script used to turn wavefront OBJ models into source code also contains script to build GBO objects

-resources______holds textures, shaders and binary 3d models for the samples

-examples_______example code showing use of the framework

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

this method is depricated you should use a GBO instead...

makeGBO script
==============

This is a binary file format, a kind of compiled OBJ file which can be used instead of
embedding your objects in the executable (which can be wasteful in terms of ram)
Once the object data is passed to the GPU the loaded data its based on is freed from
memory...
To make a gbo (Gles Binary Object) file place your wavefront object into the same directory
if for example the shape is called alien.obj then execute ./makeGBO alien - note the lack
of the file extension it will output alien.gbo which you can then copy to your resources
directory - see loadObj command detailed below.


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

*** deprecated may be removed in later version ***

int createObj(struct obj_t *obj, int numVerts, float verts[], float txVert[],
        float norms[], char *vertShader, char *fragShader);

pass an empty obj_t struct, the number of vertices and arrays of verts,
texture coordinates and normals, finally you need to specify the file names
for the vert and frag shaders 
________________________________________________________________________

*** deprecated may be removed in later version ***

int createObjCopyShader(struct obj_t *obj, int numVerts, float verts[],
			float txVert[], float norms[], struct obj_t *sdrobj);

this allows you to initialise a obj shape but using an existing obj's
shader.
________________________________________________________________________

void drawObj(struct obj_t *obj, kmMat4 * combined, kmMat4 * mv, kmVec3 lightDir, kmVec3 viewDir);

this draws an obj, you need to supply a combined model, view and projection
matrices as well as a combined model, view matrix for the lighting, 
light and view direction vectors are also needed for lighting

________________________________________________________________________

int getDisplayWidth();
int getDisplayHeight();

returns full screen width and height, for now when not on Raspberry PI 
the "screen" is fixed to a 640x480 window 

________________________________________________________________________

int loadObj(struct obj_t *obj,const char *objFile, char *vert, char *frag);

int loadObjCopyShader(struct obj_t *obj,const char *objFile, struct obj_t *sdrobj);


these are basically the same as their createObj counterparts except the
OBJ is loaded from a compiled binary wavefront object instead of from
embedded data.

________________________________________________________________________

void initSprite(int w, int h);

void drawSprite(float x, float y, float w, float h, float a, int tex);


like glPrinf the sprite subsystem must be initialised before use, pass the
dimensions of the screen.

when drawing a sprite you specify where you want it (x & y) the size
of the sprite (w & h) the rotation (a) and which texture to use (tex)

________________________________________________________________________

void setMouseRelative(bool mode);

if mode is true the mouse will report relative position changes only, this
is handy for mouse look where you dont want the mouse constrained by the
window.  By default absolute mouse position is reported

