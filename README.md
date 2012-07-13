# gles2framework

### a simple framework for OpenGLES 2.0 written in C

*** Brief API notes at the end of the readme ***

intended to help learning and tutorial writing

Currently supports XORG (linux) and the raspberry pi (none Xwindows proprietory EGL) on the pi it 
can be run with or without X

dependencies libpng, libEGL, libGLES (2.0), libode (0.12) for phystest, Chipmunk-6.1.1 for chiptest

pkg-config, build-esentials and development libraries must be installed to compile the framework

### Raspberry pi specifics

if you have not compiled GLES source code on your pi before you may need to make a file called

/etc/ld.so.conf.d/vc.conf

it should contain just the line

/opt/vc/lib

##### raw mouse on Raspberry pi

by default users don't have permissions to access the raw mouse device in order
to change these permissions a simple boot script can be made (TODO find out how
to do this properly!!! udev?)

(as root) make a script in /etc/init.d/rawmousefix

	#!/bin/sh
	if [ -c /dev/input/mouse0 ]
	then
	        echo "*** raw mouse fixup ***"
	        chgrp users /dev/input/mouse0
	        chmod 640 /dev/input/mouse0
	fi

check which runlevel you default to with runlevel and make the script run
at boot

	update-rc.d rawmousefix start 2

reboot and check you see the raw mouse fixup message in your boot sequence
also verify the group and permission with
	ls -l /dev/input/mouse0
	
it goes without saying (yet I will...) that your user account needs to be in
the users group - (it might not be depending on distro!)


	
	
### file structure for external libraries

some examples rely on external libraries they should be extracted and compiled in the same 
directory that you are working on the frame work like this:

	Chipmunk-6.1.1        ode-0.12            gles2framework 


### phystest (ode example)

the rather hacky ODE example is only really for advanced users... compile ode from source 
(version v0.12) release you can then 

You need it to use dSingle with trimesh support and you might want other things like custom cylinder
vs cylinder colliders etc you won't get from the repo version 

set drawstuff=none for quicker build (no demos etc)

run ./configure --help in the ode directory.


### chiptest (Chipmonk physics example)

a quick example showing some balls falling on some invisible slopes, niether the sprites or the 
position of the slopes are scaled depending on the display size, so the sample will look 
different on for example xorg or the pi (or even on the pi if using hdmi v's composite)
and is a good example of why you should use scaled sizes!

You can pass parameters to cmake or edit CMakeLists.txt so as to NOT compile the demos, 
you only need compile a static library

	option(BUILD_DEMOS "Build the demo applications" OFF)
	option(INSTALL_DEMOS "Install the demo applications" OFF)
	option(BUILD_SHARED "Build and install the shared library" OFF)
	option(BUILD_STATIC "Build as static library" ON)
	option(INSTALL_STATIC "Install the static library" OFF)


## Project structure

| |description|
|------------|--------------------------------------------------------------|
|-include|source code include files|
|-kazmath|source code and docs for the kazmath library|
|-lib|kazmath compiled as static lib goes here. Other libs may follow.  The framework may become a library|
|-o|somewhere to put intermediate binary objects|
|-src|source code for the framework|
|-obj2opengl|script used to turn wavefront OBJ models into source code also contains script to build GBO objects|
|-resources|holds textures, shaders and binary 3d models for the samples|
|-examples|example code showing use of the framework|
|Makefile|tells the compiler how to build the examples|
|README.md|this file!|
|TODO.md|aide memoire, ideas and inspiration for future development|

#### kazmath
there is no need to seperatly compile the kazmath library for your platform kazmath sources are now 
automatically compiled into a static library

All though the source is unchanged I have deleted everthing except the C source and the html 
documentation the full distribution of kazmath is available at https://github.com/Kazade/kazmath


#### obj2opengl

In order to create code from OBJ files use this script, ensure you set the output file name to 
something like shape.c

you will have to manually make shape.h it should look something like this

	extern float shapeVerts[];
	extern float shapeTexCoords[];
	extern float shapeNormals[];
	extern unsigned int shapeNumVerts;

in addition you will have to add a rule in the Makefile

this method is depricated you should use a GBO instead...


### makeGBO script

This is a binary file format, a kind of compiled OBJ file which can be used instead of embedding 
your objects in the executable (which can be wasteful in terms of ram) Once the object data is 
passed to the GPU the loaded data its based on is freed from memory...

To make a gbo (Gles Binary Object) file place your wavefront object into the same directory, if 
for example the shape is called alien.obj then execute ./makeGBO.sh alien - note the lack of the 
file extension it will output alien.gbo which you can then copy to your resources directory - 
see loadObj command detailed below.  makeGBO.sh relies on obj2opengl which must be in the same 
directory, it also needs at least the build esentials if going on another (artists) machine.

## support routines
_____

__int loadPNG(const char *filename);__

loads a specified png file returning a GLES texture handle

_____

__int makeContext();__

creates a native window and sets up a GLES context within it

_____

__void closeContext();__

closes GLES context and window

_____

__GLuint create\_shader(const char *filename, GLenum type);__

returns a GLES shader handle from a file you must specify what type of shader it is either 
GL\_VERTEX\_SHADER or GL\_FRAGMENT\_SHADER

_____

__GLuint getShaderLocation(int type, GLuint prog, const char *name);__

given a type of shaderAttrib or shaderUniform, a shader program handle and a name for the attrib 
or uniform it returns a location handle

_____

__void initGlPrint(int w, int h);__

This initialises the resources used by the glPrintf you must supply the windows width and height

_____

__void glPrintf(float x, float y, const char *fmt, ...);__

this behaves exactly like a normal printf except for the first two parameters which specify the starting coordinate

_____

__void swapBuffers();__

In order isolate egl and native window handles use this routine instead of eglSwapBuffers

_____

__void doEvents();__

this should be called once a frame to update the key down boolean array and the mouse information

_____

__int* getMouse();__

this returns a pointer to an array of 3 ints the first 2 are the x and y mouse position the 3rd int 
is a bit field reflecting the state of the mouse buttons

_____

__bool* getKeys();__

this is an array of 256 bools, while a key is held down the coresponding bool is true, key values are defined in keys.h

_____

*** deprecated may be removed in later version ***

__int createObj(struct obj\_t *obj, int numVerts, float verts[], float txVert[], float norms[], char *vertShader, char *fragShader);__

pass an empty obj_t struct, the number of vertices and arrays of verts, texture coordinates and 
normals, finally you need to specify the file names for the vert and frag shaders

_____

*** deprecated may be removed in later version ***

__int createObjCopyShader(struct obj\_t *obj, int numVerts, float verts[], float txVert[], float norms[], struct obj\_t *sdrobj);__

this allows you to initialise a obj shape but using an existing obj's shader.

_____

__void drawObj(struct obj\_t *obj, kmMat4 * combined, kmMat4 * mv, kmVec3 lightDir, kmVec3 viewDir);__

this draws an obj, you need to supply a combined model, view and projection matrices as well as a 
combined model, view matrix for the lighting, light and view direction vectors are also needed for 
lighting

_____

__int getDisplayWidth();__ 

__int getDisplayHeight();__

returns full screen width and height, for now when not on Raspberry PI the "screen" is fixed to a 
640x480 window

_____

__int loadObj(struct obj\_t *obj,const char *objFile, char *vert, char *frag);__

__int loadObjCopyShader(struct obj\_t *obj,const char *objFile, struct obj\_t *sdrobj);__

these are basically the same as their createObj counterparts except the OBJ is loaded from a 
compiled binary wavefront object instead of from embedded data.

_____

__void initSprite(int w, int h);__

__void drawSprite(float x, float y, float w, float h, float a, int tex);__

like glPrinf the sprite subsystem must be initialised before use, pass the dimensions of the screen.

when drawing a sprite you specify where you want it (x & y) the size of the sprite (w & h) the rotation (a) and which texture to use (tex)

_____

__void setMouseRelative(bool mode);__

if mode is true the mouse will report relative position changes only, this is handy for mouse 
look where you dont want the mouse constrained by the window. By default absolute mouse position 
is reported

_____

__struct joystick\_t *getJoystick(int j);__

__void updateJoystick(struct joystick\_t *js);__

__void releaseJoystick(struct joystick\_t *js);__

to get a pointer to a joystick call getJoystick with the index of the joystick 0-7
call this once only

once a frame call updateJoystick you will then have (in the joystick structure)

	js->axis[0..7]		upto 8 axes per joystick (signed short)
	js->buttons			long - each bit represents a button
	
when finished with a joystick you should call releaseJoystick to close its file
handle and free the structures memory.

_____


__void initPointClouds(const char* vertS, const char* fragS, float pntSize);__

__struct pointCloud\_t* createPointCloud(int size);__

__void drawPointCloud(struct pointCloud\_t* pntC,kmMat4* m);__

__void freePointCloud(struct pointCloud\_t* pntC);__


initPointClouds is used initialise the common shader used by the point clouds and set the size
of the individual points (this can be changed on the fly later by changing a shader uniform)

createPointCloud reserves space for the position and velocity components of each individual 
point in a cloud

when drawing a point cloud you must pass the combined model/view/projection matrix in
a similar manner to drawing obj shapes, note you must update the individual point positions
optionally using the supplied velocity value for each point

While it is ok to keep a point cloud around without drawing it for later use.
When the resources used by the cloud need to be released call freePoint cloud
this frees the point cloud structure itself and the associated points data

_____


