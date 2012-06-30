
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
GLuint cubeTex;

// structures holding various pointers and handles for obj shapes
struct obj_t cubeObj;

// matrices and combo matrices
kmMat4 model, view, projection, mvp, vp, mv;

int frame = 0;

bool *keys;

int main()
{
	kmVec3 pEye, pCenter, pUp;	// "camera" vectors
	// from position , target position and up direction

	// creates a window and GLES context
	if (makeContext() != 0)
		exit(-1);

	// all the shaders have at least texture unit 0 active so
	// activate it now and leave it active
	glActiveTexture(GL_TEXTURE0);

	// The obj shapes and their textures are loaded
	cubeTex = loadPNG("textures/dice.png");
	createObj(&cubeObj, cubeNumVerts, cubeVerts, cubeTexCoords, cubeNormals,
		  "shaders/textured.vert", "shaders/textured.frag");

	// "camera" is static so just set up once here
	kmMat4Identity(&view);

	pEye.x = 0;
	pEye.y = 0;
	pEye.z = 0;
	pCenter.x = 0;
	pCenter.y = 0;
	pCenter.z = -5;
	pUp.x = 0;
	pUp.y = 1;
	pUp.z = 0;

	kmMat4LookAt(&view, &pEye, &pCenter, &pUp);

	// projection matrix, as distance increases
	// the way the model is drawn is effected
	kmMat4Identity(&projection);
	kmMat4PerspectiveProjection(&projection, 45,
				    (float)getDisplayWidth() / getDisplayHeight(), 0.1, 10);

	glViewport(0, 0, getDisplayWidth(), getDisplayHeight());

	// these two matrices are pre combined for use with each model render
	// the view and projection
	kmMat4Assign(&vp, &projection);
	kmMat4Multiply(&vp, &vp, &view);

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
	rad = frame * (0.0175f * 4);

	//

	// first set the model matrix with the models position (translation)
	// and rotation
	// as translation and rotation use different parts of the matrix
	// we can do both to the same matrix without having to multiply
	// (combine) two seperate matrices 
	kmMat4Identity(&model);
	kmMat4Translation(&model, 0, 0, -5);
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
	drawObj(&cubeObj, &mvp, &mv);

	// see printf documentation for the formatting of variables...
	glPrintf(100, 240, "frame=%i", frame);

	// swap the front (visible) buffer for the back (offscreen) buffer
	swapBuffers();

}
