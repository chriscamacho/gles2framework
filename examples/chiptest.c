#include "support.h"		// support routines
#include <chipmunk.h>

/*
 *
 * demostration of using chipmunk physics with 2d sprites
 *
 */

void render();			// func prototype
void window_size_callback(GLFWwindow* window, int w, int h);

// textures
GLuint cloudTex,ballTex;
float centreX,centreY,cloudW,cloudH;


cpSpace *space;
cpShape *ground,*ground2,*ground3;


struct ball_t {
    cpShape *ballShape;
    cpBody *ballBody;
};

#define max_balls 80

struct ball_t balls[max_balls];



// matrices and combo matrices
kmMat4 model, view, projection, mvp, vp, mv;

int frame = 0;




struct cloud_t {
    float x,y,w,h,v;
};

#define max_clouds 20

struct cloud_t clouds[max_clouds];

font_t *font1;

GLFWwindow* window;
int width=640,height=480; // window width and height

struct timespec ts;  // frame timing


int main()
{

    // create a window and GLES context
	if (!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(width, height, "chipmunk physics test", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetWindowSizeCallback(window,window_size_callback);	
	glfwMakeContextCurrent(window);

    // all the shaders have at least texture unit 0 active so
    // activate it now and leave it active
    glActiveTexture(GL_TEXTURE0);

    cloudTex = loadPNG("resources/textures/cloud.png");
    ballTex = loadPNG("resources/textures/ball.png");


    glViewport(0, 0, width,height);

	font1=createFont("resources/textures/font.png",0,256,16,16,16);

    // initialises glprint's matrix, shader and texture
    initGlPrint(width,height);
    initSprite(width,height);

    centreX=((float)width)/2.0;
    centreY=((float)height)/2.0;
    cloudW=centreX/8.f;
    cloudH=centreY/8.f; // optional! TODO scale sprite to screen (in resize too)

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

	ground = cpSegmentShapeNew(space->staticBody, cpv(-100, centreY*2), cpv(100+centreX*2, centreY*2-64), 0);
	cpShapeSetFriction(ground, 1);
	cpSpaceAddShape(space, ground);

	ground2 = cpSegmentShapeNew(space->staticBody, cpv(-100, centreY), cpv(centreX, centreY+64), 0);
	cpShapeSetFriction(ground2, 1);
	cpSpaceAddShape(space, ground2);

	ground3 = cpSegmentShapeNew(space->staticBody, cpv(centreX, centreY/2.0+64), cpv(centreX*2, centreY/2.0), 0);
	cpShapeSetFriction(ground3, 1);
	cpSpaceAddShape(space, ground3);



	cpFloat moment = cpMomentForCircle(1, 0, 5, cpvzero);

    for (int i=0; i<max_balls; i++) {

		balls[i].ballBody = cpSpaceAddBody(space, cpBodyNew(1, moment));
		cpBodySetPos(balls[i].ballBody, cpv( rand_range(100,width-200), rand_range(-100,0)));
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


    while (!quit) {		// the main loop

        clock_gettime(0,&ts);  // note the time BEFORE we start to render the current frame
		glfwPollEvents();

        if (glfwGetKey(window,GLFW_KEY_ESC)==GLFW_PRESS || glfwWindowShouldClose(window))
            quit = true;	// exit if escape key pressed or window closed


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

        ts.tv_nsec+=20000000;  // 1000000000 / 50 = 50hz less time to render the frame
        thrd_sleep(&ts,NULL); // tinycthread

    }

	// we should really deallocate chipmonk stuff here
	// but we'll do the naughty and let the OS do it....

	glfwDestroyWindow(window);
	glfwTerminate();

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
    glfwSwapBuffers(window);

}

void window_size_callback(GLFWwindow* window, int w, int h)
{
	width=w;height=h;

    glViewport(0, 0, width, height);

    // projection matrix, as distance increases
    // the way the model is drawn is effected
    kmMat4Identity(&projection);
    kmMat4PerspectiveProjection(&projection, 45,
                                (float)width / height, 0.1, 10);

	reProjectGlPrint(width,height); // updates the projection matrix used by glPrint
	reProjectSprites(width,height); // updates the projection matrix used by the sprites

	centreX=((float)width)/2.0;
    centreY=((float)height)/2.0;

}
