

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>		// usleep

#include <kazmath.h>		// matrix manipulation routines

#include "support.h"		// support routines
#include "keys.h"		// defines key indexes for key down boolean array
#include "input.h"

#include <chipmunk.h>

/*
 *
 * demostration of using chipmunk physics with 2d sprites
 *
 */

void render();			// func prototype

// textures
GLuint cloudTex,ballTex;
float centreX,centreY,cloudW,cloudH;


cpSpace *space;
cpShape *ground,*ground2,*ground3;


struct ball_t {
    cpShape *ballShape;
    cpBody *ballBody;
};

#define max_balls 120

struct ball_t balls[max_balls];



// matrices and combo matrices
kmMat4 model, view, projection, mvp, vp, mv;

int frame = 0;

bool *keys;


struct cloud_t {
    float x,y,w,h,v;
};

#define max_clouds 20

struct cloud_t clouds[max_clouds];

font_t *font1;

int main()
{


    // creates a window and GLES context
    if (makeContext() != 0)
        exit(-1);

    // all the shaders have at least texture unit 0 active so
    // activate it now and leave it active
    glActiveTexture(GL_TEXTURE0);

    cloudTex = loadPNG("resources/textures/cloud.png");
    ballTex = loadPNG("resources/textures/ball.png");


    glViewport(0, 0, getDisplayWidth(), getDisplayHeight());

    // initialises glprint's matrix, shader and texture
    initGlPrint(getDisplayWidth(), getDisplayHeight());
	font1=createFont("resources/textures/font.png",0,256,16,16,16);
	
	initSprite(getDisplayWidth(), getDisplayHeight());

    centreX=((float)getDisplayWidth())/2.0;
    centreY=((float)getDisplayHeight())/2.0;
    cloudW=centreX/8.f;
    cloudH=centreY/8.f; // optional! scale sprite to screen

    for (int i=0; i<max_clouds; i++) {
        clouds[i].x=rand_range(0,centreX*2);
        clouds[i].y=rand_range(0,centreY*2);
        float size=rand_range(1,4);
        clouds[i].w=cloudW*size;
        clouds[i].h=cloudH*size;
        clouds[i].v=(5-size)*2;
    }

	space = cpSpaceNew();
	cpSpaceSetGravity(space, cpv(0, 100));

	ground = cpSegmentShapeNew(space->staticBody, cpv(-100, centreY*2), cpv(100+centreX*2, centreY*2-128), 0);
	cpShapeSetFriction(ground, 1);
	cpSpaceAddShape(space, ground);

	ground2 = cpSegmentShapeNew(space->staticBody, cpv(-100, centreY), cpv(centreX, centreY+128), 0);
	cpShapeSetFriction(ground2, 1);
	cpSpaceAddShape(space, ground2);

	ground3 = cpSegmentShapeNew(space->staticBody, cpv(centreX, centreY/2.0+128), cpv(centreX*2, centreY/2.0), 0);
	cpShapeSetFriction(ground3, 1);
	cpSpaceAddShape(space, ground3);



	cpFloat moment = cpMomentForCircle(1, 0, 5, cpvzero);

    for (int i=0; i<max_balls; i++) {

		balls[i].ballBody = cpSpaceAddBody(space, cpBodyNew(1, moment));
		cpBodySetPos(balls[i].ballBody, cpv( rand_range(100,centreX*2-100), 0));
		balls[i].ballShape = cpSpaceAddShape(space, cpCircleShapeNew(balls[i].ballBody, 16, cpvzero));
		cpShapeSetFriction(balls[i].ballShape, 0.7);
	}



    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0.5, 1, 1);

    // set to true to leave main loop
    bool quit = false;

    // get a pointer to the key down array
    keys = getKeys();

    while (!quit) {		// the main loop

        doEvents();	// update mouse and key arrays

        if (keys[KEY_ESC])
            quit = true;	// exit if escape key pressed


        for (int i=0; i<max_clouds; i++) {

            clouds[i].x=clouds[i].x-clouds[i].v;
            if (clouds[i].x+clouds[i].w<0) {
                float size=rand_range(1.,4.);
                clouds[i].w=cloudW*size;
                clouds[i].h=cloudH*size;
                clouds[i].v=(5.-size)*2.;
                clouds[i].x=clouds[i].w+centreX*2.;
                clouds[i].y=rand_range(0,centreY*2.);

            }
        }
        
        cpSpaceStep(space, 1.0/30.0);
        
        for (int i=0; i<max_balls; i++) {
			
			cpVect pos = cpBodyGetPos(balls[i].ballBody);
			if(pos.y>centreY*2){
				cpFloat x = rand_range(100,centreX*4); // ??
				cpBodySetPos(balls[i].ballBody, cpv(x, 0));
				cpBodyResetForces(balls[i].ballBody);
				cpBodySetVel(balls[i].ballBody, cpv(0,0));
			}
		}

        render();	// the render loop

        usleep(16000);	// no need to run cpu/gpu full tilt

    }

	// we should really deallocate chipmonk stuff here
	// but we'll do the naughty and let the OS do it....

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

    for (int i=0; i<max_clouds; i++) {
        drawSprite( clouds[i].x,clouds[i].y,clouds[i].w,clouds[i].h,0,cloudTex);
    }

    for (int i=0; i<max_balls; i++) {
		cpVect pos = cpBodyGetPos(balls[i].ballBody);
		float r = cpBodyGetAngle(balls[i].ballBody);
        drawSprite( pos.x,pos.y,32,32,r,ballTex);
    }

    // see printf documentation for the formatting of variables...
    glPrintf(100, 240, font1,"frame=%i", frame);

    // swap the front (visible) buffer for the back (offscreen) buffer
    swapBuffers();

}
