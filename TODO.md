I would very much welcome people contacting me with ideas for this document.

# Features to be added

* shaders used by glprint etc should be released on exit (probably in
	closeContext) - we need to know what the user initialised 

* makeBestSellingFPSgame() function

## Features probably not going to be implemented

* simple height map terrain
  * probably better using precreated meshes


## Recently implemented

* Dropped specific handling of platforms 
  * no more horrid #ifdef mess
  * uses glfw for low level stuff


