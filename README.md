# gles2framework

### a simple framework for OpenGLES 2.0 written in C

*** Brief API notes at the end of the readme ***

intended to help learning and tutorial writing

Currently supports XORG (linux) Desktop using mesa GLES2.0 libs or various embedded 
platforms such as Allwinner A20

dependencies, libEGL, libGLES (2.0), libode (0.15.2) for phystest, Chipmunk-7.0.2 for chiptest

pkg-config, build-esentials and development libraries must be installed to compile the framework



##### raw mouse, keyboard and joystick on some embedded platforms

When not using xwindows (ie via ssh) input including keyboard is now done entirely via
the kernel evdev interface.

you (might) need some udev rules

make a file called /etc/udev/rules.d/99-evdev.rules (as root) it should contain the following

	KERNEL=="event*", NAME="input/%k", MODE="0640", GROUP="evdev"
	KERNEL=="mouse*", NAME="input/%k", MODE="0640", GROUP="evdev"
	KERNEL=="js*", NAME="input/%k", MODE="0640", GROUP="evdev"

you need to add an new group and add your user account to the group (as root)

	groupadd evdev
	usermod -a -G evdev your_user_name

you'll need to log out and back in again

You can now run your programs from ssh and it will only use the targets attached
usb keyboard rather than being confused with the ssh console

editing files via ssh (sftp enabled editor) and compiling with a ssh console is the recommended
way of developing with this framework if running with a small LCD and you have old tired eyes...

	
### file structure for external libraries

some examples rely on external libraries they should be extracted and compiled in the same 
directory that you are working on the frame work like this:

	Chipmunk-7.0.2            gles2framework		ode-0.16
	


### phystest (ode example)

the rather hacky ODE example is only really for advanced users...  
(install Debian package version v0.11sp-dev (single precision package) 

If you want some of the extra bits of libODE (trimesh vs cylinder collisions - and other luxuries) then 
you can always compile ODE from source, see the ODE website for details

suggested compile configuration for ODE

./configure --enable-libccd --with-box-cylinder=libccd --with-drawstuff=none --disable-demos 

this should by default provide trimesh collider etc as a static library


### chiptest (Chipmonk physics example)

a quick example showing some balls falling on some invisible slopes, niether the sprites or the 
position of the slopes are scaled depending on the display size, so the sample will look 
different on different platforms and is a good example of why you should use scaled sizes!

You only need compile a static library (the demos still use a *very* out of date
version of GLFW (version 2 not actually avalible in a number of distros))

easiest way to bully cmake into working is just to remame the cmake file in the demo sub directory

mv demo/CMakeLists.txt demo/CMakeLists.txt.xxx

Once you have done this you can run make so the chipmunk library is available for the chiptest example


## Project structure

| |description|
|------------|--------------------------------------------------------------|
|-include|source code include files|
|-kazmath|source code and docs for the kazmath library|
|-lib|kazmath compiled as static lib goes here. Other libs may follow.  The framework may become a library|
|-o|somewhere to put intermediate binary objects|
|-src|source code for the framework|
|-tools|a tool to package up 3d shapes and one to make 2d bitmap fonts|
|-resources|holds textures, shaders and binary 3d models for the samples|
|-examples|example code showing use of the framework|
|Makefile|tells the compiler how to build the examples|
|README.md|this file!|
|TODO.md|aide memoire, ideas and inspiration for future development|

#### kazmath
there is no need to seperatly compile the kazmath library for your platform kazmath sources are now 
automatically compiled into a static library

All though the source is unchanged I have deleted everthing except the C source
the full distribution of kazmath is available at https://github.com/Kazade/kazmath


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
__int makeContextXY(int x, int y);__

creates a native window and sets up a GLES context within it
makeContextXY takes dimesions for the window, giving -1,-1 will
put the context in fullscreen mode, you can the use getDisplayWidth() /
getDisplayHeight() to resize things according to this resolution if
needed.

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

__font_t* createFont(const char* tpath,uint cbase,float tHeight,float tLines, int fWidth, int fHeight);__

tpath is the full path and of the texture for this font, cbase is the ascii code of the first character 
tHeight is the height in pixels of the texture, tLines specifies the number of lines in total the texture 
contains (I think there is a bug with this!) fWidth and fHeight are the width and height a character

TODO freeFont to release a fonts resources...
_____

__void glPrintf(float x, float y, font_t fnt, const char *fmt, ...);__

this behaves exactly like a normal printf except for the first two parameters which specify the starting coordinate 
you must specify a previously created font structure to print with

_____

__void swapBuffers();__

In order isolate egl and native window handles use this routine instead of eglSwapBuffers
_____

__int setSwapInterval(int);__

The number of frames EGL should wait before swapBuffers actually swaps the buffer, this freqently does
nothing on many EGL implementations however...
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

__void reProjectGlPrint(int w, int h)__
__void reProjectSprites(int w, int h)__

When glfw calls your window resize callback you should run these functions
if you are using the apropriate facilities to inform them of the screen
changes
___

__void resizePointCloudSprites(float s)__

This allows you to change the size of the point sprite's used - you would
usually do this in the screen resize callback.

