

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>		// usleep

#include <kazmath.h>		// matrix manipulation routines

#include "support.h"		// support routines
#include "keys.h"		// defines key indexes for key down boolean array


/*
 * 
 * demostration of 2d sprites
 * 
 * 
 * (my apologies to Klaus ;) )
 */

void render();			// func prototype

// textures
GLuint cloudTex,biTex,triTex;
float centreX,centreY,cloudW,cloudH,planeW,planeH;


// matrices and combo matrices
kmMat4 model, view, projection, mvp, vp, mv;

int frame = 0;

bool *keys;


struct cloud_t {
	float x,y,w,h,v;
};

#define max_clouds 20

struct cloud_t clouds[max_clouds];


float rand_range(float min,float max) {
	return min + (max - min) * ((float)rand() / RAND_MAX) / 2.0;
}

int main()
{

	
    // creates a window and GLES context
    if (makeContext() != 0)
        exit(-1);

    // all the shaders have at least texture unit 0 active so
    // activate it now and leave it active
    glActiveTexture(GL_TEXTURE0);

    cloudTex = loadPNG("resources/textures/cloud.png");
    biTex = loadPNG("resources/textures/biplane.png");
    triTex = loadPNG("resources/textures/triplane.png");


    glViewport(0, 0, getDisplayWidth(), getDisplayHeight());

    // initialises glprint's matrix, shader and texture
    initGlPrint(getDisplayWidth(), getDisplayHeight());
    initSprite(getDisplayWidth(), getDisplayHeight());

	centreX=((float)getDisplayWidth())/2.0;
	centreY=((float)getDisplayHeight())/2.0;
	cloudW=centreX/5.f;
	cloudH=centreY/5.f; // optional! scale sprite to screen  
	planeW=centreX/6.;
	planeH=planeW*.75;

	for (int i=0;i<max_clouds;i++) {
		clouds[i].x=rand_range(0,centreX*4);
		clouds[i].y=rand_range(0,centreY*3);
		float size=rand_range(1,4);
		clouds[i].w=cloudW*size;
		clouds[i].h=cloudH*size;
		clouds[i].v=5-size;
	}


    // we don't want to draw the back of triangles
    // the blending is set up for glprint but disabled
    // while not in use
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0.5, 1, 1);

    // count each frame
    int num_frames = 0;

    // set to true to leave main loop
    bool quit = false;

    // get a pointer to the key down array
    keys = getKeys();

    while (!quit) {		// the main loop

        doEvents();	// update mouse and key arrays

        if (keys[KEY_ESC])
            quit = true;	// exit if escape key pressed


		for (int i=0;i<max_clouds;i++){

			clouds[i].x=clouds[i].x-clouds[i].v;
			if (clouds[i].x+clouds[i].w<0) {
				float size=rand_range(1.,4.);
				clouds[i].w=cloudW*size;
				clouds[i].h=cloudH*size;
				clouds[i].v=(5.-size);
				clouds[i].x=centreX*2.;
				clouds[i].y=rand_range(0,centreY*3.);
				
			}
		}
        
        render();	// the render loop

        usleep(16000);	// no need to run cpu/gpu full tilt

    }

    closeContext();		// tidy up

    return 0;
}

void render()
{

    float rad;		// radians of rotation based on frame counter

    // clear the colour (drawing) and depth sort back (offscreen) buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // count the frame and base a rotation on it.
    frame++;
    rad = frame * (0.0175f);

	for (int i=0;i<max_clouds;i++){
		drawSprite( clouds[i].x,clouds[i].y,clouds[i].w,clouds[i].h,0,cloudTex);
	}

	float r2=rad+.6f+(sin(frame*0.03)/6.);
	drawSprite((centreX-(planeW/2.))+cos(r2)*centreX*.75,
		centreY+sin(r2)*centreY*.75,
		planeW,planeH,r2+1.5708f,triTex);
	
	drawSprite((centreX-(planeW/2.))+cos(rad)*centreX*.75,
		centreY+sin(rad)*centreY*.75,
		planeW,planeH,rad+1.5708f,biTex);

	
    // see printf documentation for the formatting of variables...
    glPrintf(100, 240, "frame=%i", frame);

    // swap the front (visible) buffer for the back (offscreen) buffer
    swapBuffers();

}
