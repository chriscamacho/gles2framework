#include "support.h"		// support routines

//#include <GL/glfw3.h>
//#include "tinycthread.h" // lets us doze...

//#include <stdlib.h>
//#include <stdbool.h>
//#include <unistd.h>		// usleep

//#include <kazmath.h>		// matrix manipulation routines



/*
 *
 * demostration of 2d sprites
 *
 *
 * (my apologies to Klaus ;) )
 */

void render();			// func prototype
// this gets called if the window is resized
void window_size_callback(GLFWwindow* window, int w, int h);


// textures
GLuint cloudTex,biTex,triTex;
float centreX,centreY,cloudW,cloudH,planeW,planeH;

font_t *font1;

// matrices and combo matrices
kmMat4 model, view, projection, mvp, vp, mv;

int frame = 0;

bool *keys;


struct cloud_t {
    float x,y,w,h,v;
};

#define max_clouds 20

struct cloud_t clouds[max_clouds];

GLFWwindow* window;
int width=640,height=480; // window width and height

struct timespec ts;  // frame timing


int main()
{

    // create a window and GLES context
	if (!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(width, height, "sprite test", NULL, NULL);
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
    biTex = loadPNG("resources/textures/biplane.png");
    triTex = loadPNG("resources/textures/triplane.png");

    // initialises glprint's matrix, shader and texture
    initGlPrint(width,height);
    font1=createFont("resources/textures/font.png",0,256,16,16,16);
    initSprite(width,height);

	centreX=((float)width)/2.0;
    centreY=((float)height)/2.0;
    
    cloudW=centreX/8.f;
    cloudH=centreY/8.f; // optional! scale sprite to screen
    planeW=centreX/6.;
    planeH=planeW*.75;

    for (int i=0; i<max_clouds; i++) {
        clouds[i].x=rand_range(0,centreX*2);
        clouds[i].y=rand_range(0,centreY*2);
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
                clouds[i].v=(5.-size);
                clouds[i].x=centreX*2.+cloudW*2;
                clouds[i].y=rand_range(0,centreY*2.);

            }
        }

        render();	// the render loop

        ts.tv_nsec+=20000000;  // 1000000000 / 50 = 50hz less time to render the frame
        thrd_sleep(&ts,NULL); // tinycthread

    }

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

    float r2=rad+.6f+(sin(frame*0.03)/6.);
    drawSprite((centreX-(planeW/2.))+cos(r2)*(centreX*.75),
               centreY+sin(r2)*(centreY*.75),
               planeW,planeH,r2+1.5708f,triTex);

    drawSprite((centreX-(planeW/2.))+cos(rad)*(centreX*.75),
               centreY+sin(rad)*(centreY*.75),
               planeW,planeH,rad+1.5708f,biTex);


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
