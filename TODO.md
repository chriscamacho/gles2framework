I would very much welcome people contacting me with ideas for this document.

# Features to be added

* shaders used by glprint etc should be released on exit (probably in
	closeContext) - we need to know what the user initialised 

* makeBestSellingFPSgame() function

## Features probably not going to be implemented

* simple height map terrain
  * probably better using precreated meshes


## Recently implemented

* reimplement raw keyboard if xlib not available, so development can be done purely via ssh
  * will have to be done via makefile switch because of key defines 
  * added raw mouse support

* billboard 2.5D quads - implemended as point clouds

* joystick raw and xlib

