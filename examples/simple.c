
// turned into a lighting test as 6 planes (a cube) is not ideal for 
// checking frag lighting, there is also a sphere

// press Q & W
// and A & S to rotate the camera and light


#include "support.h"		// support routines
#include "keys.h"		// defines key indexes for key down boolean array
#include "obj.h"		// loading and displaying wavefront OBJ derived shapes
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>		// usleep

#include <kazmath.h>		// matrix manipulation routines


unsigned int cubeNumVerts = 36;

float cubeVerts[] = {
    // f 1/1/1 2/2/1 3/3/1
    0.5, -0.5, -0.5,	0.5, -0.5, 0.5,	-0.5, -0.5, 0.5,
    // f 1/1/1 3/3/1 4/4/1
    0.5, -0.5, -0.5,	-0.5, -0.5, 0.5,	-0.5, -0.5, -0.5,
    // f 5/5/2 8/6/2 7/7/2
    0.5, 0.5, -0.5,	-0.5, 0.5, -0.5,	-0.5, 0.5, 0.5,
    // f 5/5/2 7/7/2 6/8/2
    0.5, 0.5, -0.5,	-0.5, 0.5, 0.5,	0.5, 0.5, 0.5,
    // f 1/9/3 5/10/3 6/11/3
    0.5, -0.5, -0.5,	0.5, 0.5, -0.5,	0.5, 0.5, 0.5,
    // f 1/9/3 6/11/3 2/12/3
    0.5, -0.5, -0.5,	0.5, 0.5, 0.5,	0.5, -0.5, 0.5,
    // f 2/13/4 6/14/4 7/15/4
    0.5, -0.5, 0.5,	0.5, 0.5, 0.5,	-0.5, 0.5, 0.5,
    // f 2/13/4 7/15/4 3/16/4
    0.5, -0.5, 0.5,	-0.5, 0.5, 0.5,	-0.5, -0.5, 0.5,
    // f 3/17/5 7/18/5 8/19/5
    -0.5, -0.5, 0.5,	-0.5, 0.5, 0.5,	-0.5, 0.5, -0.5,
    // f 3/17/5 8/19/5 4/20/5
    -0.5, -0.5, 0.5,	-0.5, 0.5, -0.5,	-0.5, -0.5, -0.5,
    // f 5/21/6 1/22/6 4/23/6
    0.5, 0.5, -0.5,	0.5, -0.5, -0.5,	-0.5, -0.5, -0.5,
    // f 5/21/6 4/23/6 8/24/6
    0.5, 0.5, -0.5,	-0.5, -0.5, -0.5,	-0.5, 0.5, -0.5,
};

float cubeNormals[] = {
    // f 1/1/1 2/2/1 3/3/1
    0, -1, 0,    0, -1, 0,    0, -1, 0,
    // f 1/1/1 3/3/1 4/4/1
    0, -1, 0,    0, -1, 0,    0, -1, 0,
    // f 5/5/2 8/6/2 7/7/2
    0, 1, 0,    0, 1, 0,    0, 1, 0,
    // f 5/5/2 7/7/2 6/8/2
    0, 1, 0,    0, 1, 0,    0, 1, 0,
    // f 1/9/3 5/10/3 6/11/3
    1, 0, 0,    1, 0, 0,    1, 0, 0,
    // f 1/9/3 6/11/3 2/12/3
    1, 0, 0,    1, 0, 0,    1, 0, 0,
    // f 2/13/4 6/14/4 7/15/4
    0, 0, 1,    0, 0, 1,    0, 0, 1,
    // f 2/13/4 7/15/4 3/16/4
    0, 0, 1,    0, 0, 1,    0, 0, 1,
    // f 3/17/5 7/18/5 8/19/5
    -1, 0, 0,    -1, 0, 0,    -1, 0, 0,
    // f 3/17/5 8/19/5 4/20/5
    -1, 0, 0,    -1, 0, 0,    -1, 0, 0,
    // f 5/21/6 1/22/6 4/23/6
    0, 0, -1,    0, 0, -1,    0, 0, -1,
    // f 5/21/6 4/23/6 8/24/6
    0, 0, -1,    0, 0, -1,    0, 0, -1,
};

float cubeTexCoords[] = {
    // f 1/1/1 2/2/1 3/3/1
    0.338526, 0.661474,    0.663522, 0.661474,    0.663522, 0.336478,
    // f 1/1/1 3/3/1 4/4/1
    0.338526, 0.661474,    0.663522, 0.336478,    0.338526, 0.336478,
    // f 5/5/2 8/6/2 7/7/2
    0.005288, 0.994712,    0.329713, 0.994712,    0.329713, 0.670287,
    // f 5/5/2 7/7/2 6/8/2
    0.005288, 0.994712,    0.329713, 0.670287,    0.005288, 0.670287,
    // f 1/9/3 5/10/3 6/11/3
    0.993727, 0.337651,    0.670272, 0.337651,    0.670272, 0.661106,
    // f 1/9/3 6/11/3 2/12/3
    0.993727, 0.337651,    0.670272, 0.661106,    0.993727, 0.661106,
    // f 2/13/4 6/14/4 7/15/4
    0.337449, 0.995051,    0.663207, 0.995051,    0.663338, 0.669686,
    // f 2/13/4 7/15/4 3/16/4
    0.337449, 0.995051,    0.663338, 0.669686,    0.337711, 0.669162,
    // f 3/17/5 7/18/5 8/19/5
    0.004502, 0.663184,    0.331568, 0.663184,    0.331568, 0.336118,
    // f 3/17/5 8/19/5 4/20/5
    0.004502, 0.663184,    0.331568, 0.336118,    0.004408, 0.336118,
    // f 5/21/6 1/22/6 4/23/6
    0.338744, 0.329745,    0.664703, 0.329745,    0.664703, 0.004606,
    // f 5/21/6 4/23/6 8/24/6
    0.338744, 0.329745,    0.664703, 0.004606,    0.338744, 0.004606,
};




void render();			// func prototype

// obj shape textures
GLuint cubeTex,ballTex;

// structures holding various pointers and handles for obj shapes
struct obj_t cubeObj,ballObj;

// matrices and combo matrices
kmMat4 model, view, projection, mvp, vp, mv;

int frame = 0;
kmVec3 lightDir, viewDir; // vectors for shader lighting
kmVec3 pEye, pCenter, pUp;	// "camera" vectors
// from position , target position and up direction

float camAng,lightAng;

bool *keys;
int *mouse;
struct joystick_t *joy1;

font_t *font1,*font2;

int main()
{
	
	
    // creates a window and GLES context
    if (makeContext() != 0)
        exit(-1);

    // all the shaders have at least texture unit 0 active so
    // activate it now and leave it active
    glActiveTexture(GL_TEXTURE0);

    // The obj shapes and their textures are loaded
    // directly from embedded data in the executable
    cubeTex = loadPNG("resources/textures/dice.png");
    createObj(&cubeObj, cubeNumVerts, cubeVerts, cubeTexCoords, cubeNormals,
              "resources/shaders/textured.vert", "resources/shaders/textured.frag");

    // embedding data can waste precious ram, loading from disk is more efficient
    // no redundant data left when verts transfered to GPU
    loadObjCopyShader(&ballObj,"resources/models/sphere.gbo",&cubeObj);
    ballTex = loadPNG("resources/textures/jupiter.png");

    kmMat4Identity(&view);

    pEye.x = 0;
    pEye.y = 0;
    pEye.z = 5;
    pCenter.x = 0;
    pCenter.y = 0;
    pCenter.z = 0;
    pUp.x = 0;
    pUp.y = 1;
    pUp.z = 0;

    kmVec3Subtract(&viewDir,&pEye,&pCenter);
    kmVec3Normalize(&viewDir,&viewDir);

    kmMat4LookAt(&view, &pEye, &pCenter, &pUp);

    // these two matrices are pre combined for use with each model render
    // the view and projection
    kmMat4Assign(&vp, &projection);
    kmMat4Multiply(&vp, &vp, &view);

    // projection matrix, as distance increases
    // the way the model is drawn is effected
    kmMat4Identity(&projection);
    kmMat4PerspectiveProjection(&projection, 45,
                                (float)getDisplayWidth() / getDisplayHeight(), 0.1, 10);

    glViewport(0, 0, getDisplayWidth(), getDisplayHeight());

    // initialises glprint's matrix, shader and texture
    initGlPrint(getDisplayWidth(), getDisplayHeight());

	font1=createFont("resources/textures/font.png",0,256,16,16,16);
	font2=createFont("resources/textures/bigfont.png",32,512,9.5,32,48);
	
    // we don't want to draw the back of triangles
    // the blending is set up for glprint but disabled
    // while not in use
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0.5, 1, 1);

    // count each frame
    int num_frames = 0;

    // set to true to leave main loop
    bool quit = false;

    // get a pointer to the key down array
    keys = getKeys();
    mouse = getMouse();
    joy1=getJoystick(0);
    //setMouseRelative(true);

    while (!quit) {		// the main loop

        doEvents();	// update mouse and key arrays
        updateJoystick(joy1);

        if (keys[KEY_ESC])
            quit = true;	// exit if escape key pressed

        if (keys[KEY_A]) camAng=camAng+1;
        if (keys[KEY_S]) camAng=camAng-1;
        if (keys[KEY_W]) lightAng=lightAng+1;
        if (keys[KEY_Q]) lightAng=lightAng-1;

        render();	// the render loop

        usleep(16000);	// no need to run cpu/gpu full tilt

    }
    
    closeContext();		// tidy up
	releaseJoystick(joy1);

    return 0;
}

int rmx,rmy;

void render()
{

    float rad;		// radians of rotation based on frame counter
	kmMat4 tmpM;

    // clear the colour (drawing) and depth sort back (offscreen) buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // count the frame and base a rotation on it.
    frame++;
    rad = frame * (0.0175f);

    // rotate the light direction depending on lightAng
    lightDir.x=cos(lightAng/10.);
    lightDir.z=-1;
    lightDir.y=sin(lightAng/10.);
    kmVec3Normalize(&lightDir,&lightDir);


    pEye.x=cos(camAng/10.)*7.;
    pEye.z=sin(camAng/10.)*7.;
    pEye.y=1;

    // recalculate the view direction vector used by lighting
    kmVec3Subtract(&viewDir,&pEye,&pCenter);
    kmVec3Normalize(&viewDir,&viewDir);

    // update view matrix for new cam position
    kmMat4LookAt(&view, &pEye, &pCenter, &pUp);

    // these two matrices are pre combined for use with each model render
    // the view and projection
    kmMat4Assign(&vp, &projection);
    kmMat4Multiply(&vp, &vp, &view);


    // first set the model matrix with the models position (translation)
    // and rotation
    // as translation and rotation use different parts of the matrix
    // we can do both to the same matrix without having to multiply
    // (combine) two seperate matrices
    kmMat4Identity(&model);
    kmMat4Translation(&model, 1, 0, 0);
    //kmMat4RotationPitchYawRoll(&model, rad, rad * 1.5, rad * 2);
	kmMat4RotationYawPitchRoll(&tmpM, rad * 1.5, rad, rad * 2);
	kmMat4Multiply(&model,&model,&tmpM);

    // copy the combined view/projection matrix to the mvp matrix
    // to "reset" it
    // and then combine with the model matrix
    kmMat4Assign(&mvp, &vp);
    kmMat4Multiply(&mvp, &mvp, &model);	// model, view, projection combined matrix

    // combine the view and model matrices
    kmMat4Assign(&mv, &view);
    kmMat4Multiply(&mv, &mv, &model);	// view, model matrix for lighting

    glBindTexture(GL_TEXTURE_2D, cubeTex);
    drawObj(&cubeObj, &mvp, &mv,lightDir,viewDir);

//	----

    kmMat4Identity(&model);
    kmMat4Translation(&model, -1, 0, 0);
    //kmMat4RotationPitchYawRoll(&model, 0 , -rad, 0);
    kmMat4RotationYawPitchRoll(&tmpM, 0, -rad, 0);
    kmMat4Multiply(&model,&model,&tmpM);

    kmMat4Assign(&mvp, &vp);
    kmMat4Multiply(&mvp, &mvp, &model);	// model, view, projection combined matrix
    kmMat4Assign(&mv, &view);
    kmMat4Multiply(&mv, &mv, &model);	// view, model matrix for lighting

    glBindTexture(GL_TEXTURE_2D, ballTex);
    drawObj(&ballObj, &mvp, &mv,lightDir,viewDir);

//	----


    // see printf documentation for the formatting of variables...
    glPrintf(24, 24, font2, "frame=%i", frame);

    glPrintf(100, 260, font1, "mouse %i  %i   %i", mouse[0],mouse[1],mouse[2]);

	glPrintf(100, 280, font1, "joystick %i,%i  %i",joy1->axis[0],joy1->axis[1],joy1->buttons);
	
	
	glPrintf(100, 320, font2, "abcABCqrsQRS123"); 
    //rmx+=mouse[0];
    //rmy+=mouse[1];
    //glPrintf(100, 280, "%i  %i", rmx,rmy);

    // swap the front (visible) buffer for the back (offscreen) buffer
    swapBuffers();

}
