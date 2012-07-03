
#include <stdlib.h>
#include <stdbool.h>

#include <kazmath.h>		// matrix manipulation routines

#include "support.h"		// support routines
#include "keys.h"		// defines key indexes for key down boolean array
#include "obj.h"		// loading and displaying wavefront OBJ derived shapes

// OBJ vert data
#include "cube.h"

#include <unistd.h>		// usleep


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

int main()
{


	// creates a window and GLES context
	if (makeContext() != 0)
		exit(-1);

	// all the shaders have at least texture unit 0 active so
	// activate it now and leave it active
	glActiveTexture(GL_TEXTURE0);

	// The obj shapes and their textures are loaded
	cubeTex = loadPNG("resources/textures/dice.png");
	createObj(&cubeObj, cubeNumVerts, cubeVerts, cubeTexCoords, cubeNormals,
		  "resources/shaders/textured.vert", "resources/shaders/textured.frag");

	loadObjCopyShader(&ballObj,"resources/models/sphere.gbo",&cubeObj);
	ballTex = loadPNG("resources/textures/jupiter.png");

	// "camera" is static so just set up once here
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

	while (!quit) {		// the main loop

		doEvents();	// update mouse and key arrays

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

	lightDir.x=cos(lightAng/10.);
	lightDir.y=0;
	lightDir.z=sin(lightAng/10.);


	pEye.x=cos(camAng/10.)*7.;
	pEye.z=sin(camAng/10.)*7.;
	pEye.y=1;

	kmVec3Subtract(&viewDir,&pEye,&pCenter);
	kmVec3Normalize(&viewDir,&viewDir);

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
	kmMat4RotationPitchYawRoll(&model, rad, rad * 1.5, rad * 2);

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


	kmMat4Identity(&model);
	kmMat4Translation(&model, -1, 0, 0);
	kmMat4RotationPitchYawRoll(&model, 0 , -rad, 0);

	kmMat4Assign(&mvp, &vp);
	kmMat4Multiply(&mvp, &mvp, &model);	// model, view, projection combined matrix
	kmMat4Assign(&mv, &view);
	kmMat4Multiply(&mv, &mv, &model);	// view, model matrix for lighting

	glBindTexture(GL_TEXTURE_2D, ballTex);
	drawObj(&ballObj, &mvp, &mv,lightDir,viewDir);





	// see printf documentation for the formatting of variables...
	glPrintf(100, 240, "frame=%i", frame);

	// swap the front (visible) buffer for the back (offscreen) buffer
	swapBuffers();

}
