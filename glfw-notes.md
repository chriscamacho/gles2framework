GLFW3 works on a very wide range of platforms, its adoption allows the framework
to not be conserned with the minutia of the low level idiosyncrasies of individual
platforms, it alows massive simplification of the framework both for the end user
and also in implementing the framework itself.

This allows the framework to concentrate on providing a simple small set of useful
routines like model loading, gpPrint, sprites, 3d particles etc... 

Some none standard platforms might not support GLFW3 if that is the case, porting
GLFW3 to that plarform should be reasonably straightforward providing that making
a window for EGL is not too tiresome and that both EGL and GLES are complient... 


to build GLFW3 in Linux to use GLES2.0 use this command... 

cmake . -DGLFW_USE_EGL=true -DGLFW_CLIENT_LIBRARY=glesv2 

when compiling GLFW3 for gles2 although an opengl (2+) version should work just fine...

If you are developing on a PC but also targeting an embedded platform only occasional
testing should be required if you do compile GLFW3 for GLES2.0 ....

included tinycthreads, to allow cross platform yealding of cpu after each frame
(GLFW3 seems to embed it but doesn't export the functions...)

All samples have been updated to work with the new framework changes...

In general converting your old source to work with this update of the framework
should be both trivial and painless...


