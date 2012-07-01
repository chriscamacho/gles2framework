
#include <stdlib.h>
#include <stdbool.h>

#include <kazmath.h>		// matrix manipulation routines

#include "support.h"		// support routines
#include "keys.h"		// defines key indexes for key down boolean array
#include "obj.h"		// loading and displaying wavefront OBJ derived shapes

// OBJ vert data
#include "cube.h"
#include "ship.h"
#include "alien.h"
#include "shot.h"

#include <unistd.h>		// usleep
#include <sys/time.h>		// fps stuff



void render();			// func prototype

// obj shape textures
GLuint cubeTex, shipTex, alienTex, shotTex;

// structures holding various pointers and handles for obj shapes
struct obj_t cubeObj, shipObj, alienObj, shotObj;

// matrices and combo matrices
kmMat4 model, view, projection, mvp, vp, mv;

int frame = 0;
float lfps = 0;

int *mouse;
bool *keys;
kmVec3 pEye, pCenter, pUp;	// "camera" vectors

kmVec3 playerPos;
float playerPre_error, playerIntegral;

struct playerShot_t {
	kmVec3 pos;
	bool alive;
};

#define MAX_PLAYER_SHOTS 6
struct playerShot_t playerShots[MAX_PLAYER_SHOTS];

struct alien_t {
	kmVec3 pos;
	kmVec3 shot;
	bool alive;
	bool shotActive;

};

#define  MAX_ALIENS 12
struct alien_t aliens[MAX_ALIENS];

struct playerShot_t *getFreeShot()
{
	for (int n = 0; n < MAX_PLAYER_SHOTS; n++) {
		if (playerShots[n].alive == false)
			return &playerShots[n];
	}
	return 0;
}

void resetAliens()
{
	for (int n = 0; n < MAX_ALIENS; n++) {
		aliens[n].pos.x = -5 + n * 2;
		aliens[n].pos.z = -6;
		if (n > 5) {
			aliens[n].pos.x -= 12;
			aliens[n].pos.z = -8;
		}
		aliens[n].alive = true;

	}
}

//http://www.embeddedheaven.com/pid-control-algorithm-c-language.htm
//Define parameter
#define epsilon 0.01
#define dt 0.06			//0.01=100ms loop time
#define MAX 4			//For Current Saturation
#define MIN -4
#define Kp 0.1
#define Kd 0.01
#define Ki 0.005

float PIDcal(float setpoint, float actual_position, float *pre_error,
	     float *integral)
{
//      static float pre_error = 0;
//      static float integral = 0;
	float error;
	float derivative;
	float output;

	//Caculate P,I,D
	error = setpoint - actual_position;

	//In case of error too small then stop intergration
	if (abs(error) > epsilon) {
		integral[0] = integral[0] + error * dt;
	}
	derivative = (error - pre_error[0]) / dt;
	output = Kp * error + Ki * integral[0] + Kd * derivative;

	//Saturation Filter
	if (output > MAX) {
		output = MAX;
	} else if (output < MIN) {
		output = MIN;
	}
	//Update error
	pre_error[0] = error;

	return output;
}

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

	// just to prove that the vertices data in the arrays is no longer
	// used as its now in gpu land...
	for (int n = 0; n < cubeNumVerts; n++)
		cubeVerts[n] = 0;

	shipTex = loadPNG("resources/textures/shipv2.png");
	//createObjCopyShader(&shipObj, shipNumVerts, shipVerts, shipTexCoords,
	//		    shipNormals, &cubeObj);
	loadObjCopyShader(&shipObj,"resources/models/ship.gbo",&cubeObj);

	alienTex = loadPNG("resources/textures/alien.png");
	createObjCopyShader(&alienObj, alienNumVerts, alienVerts,
			    alienTexCoords, alienNormals, &cubeObj);

	shotTex = loadPNG("resources/textures/shot.png");
	createObjCopyShader(&shotObj, shotNumVerts, shotVerts,
			    shotTexCoords, shotNormals, &cubeObj);

	playerPos.x = 0;
	playerPos.y = 0;
	playerPos.z = 0;

	kmMat4Identity(&view);

	pEye.x = 0;
	pEye.y = 2;
	pEye.z = 4;
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
				    (float)getDisplayWidth() / getDisplayHeight(), 0.1, 100);

	glViewport(0, 0, getDisplayWidth(), getDisplayHeight());

	// these two matrices are pre combined for use with each model render
	kmMat4Assign(&vp, &projection);
	kmMat4Multiply(&vp, &vp, &view);

	// initialises glprint's matrix shader and texture
	initGlPrint(getDisplayWidth(), getDisplayHeight());

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);	// only used by glprintf
	glEnable(GL_DEPTH_TEST);

	struct timeval t, ta, t1, t2;	// fps stuff
	gettimeofday(&t1, NULL);
	int num_frames = 0;

	bool quit = false;

	mouse = getMouse();
	keys = getKeys();

	resetAliens();

	for (int n = 0; n < MAX_PLAYER_SHOTS; n++) {
		playerShots[n].alive = false;
	}

	while (!quit) {		// the main loop

		doEvents();	// update mouse and key arrays

		// mask of 4 is right mouse
		if (keys[KEY_ESC])
			quit = true;

		glClearColor(0, .5, 1, 1);

		// render between two gettimeofday calls so
		// we can sleep long enough to roughly sync
		// to ~60fps

		// TODO find something a tad more elegent

		long i;
		gettimeofday(&t, NULL);
		i = t.tv_sec * 1e6 + t.tv_usec;

		render();

		gettimeofday(&ta, NULL);
		long j = (ta.tv_sec * 1e6 + ta.tv_usec);

		i = j - i;
		if (i < 0)
			i = 1000000;	// pass through - slower that 60fps
		if (i < 16000)
			usleep(16000 - i);

		// every 10 frames average the time taken and store
		// fps value for later printing with glprintf
		if (++num_frames % 10 == 0) {
			gettimeofday(&t2, NULL);
			float dtf =
			    t2.tv_sec - t1.tv_sec + (t2.tv_usec -
						     t1.tv_usec) * 1e-6;
			lfps = num_frames / dtf;
			num_frames = 0;
			t1 = t2;
		}
	}

	closeContext();

	return 0;
}

float playerRoll, playerCroll;

int playerFireCount;

void render()
{

	float rad;		// radians rotation based on frame counter

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	frame++;
	rad = frame * (0.0175f * 2);

	kmMat4Identity(&model);
	kmMat4Translation(&model, playerPos.x, playerPos.y, playerPos.z);

	playerCroll +=
	    (PIDcal(playerRoll, playerCroll, &playerPre_error, &playerIntegral)
	     / 2);
	kmMat4RotationPitchYawRoll(&model, 0, 3.1416, playerCroll * 3);	//

	kmMat4Assign(&mvp, &vp);
	kmMat4Multiply(&mvp, &mvp, &model);

	kmMat4Assign(&mv, &view);
	kmMat4Multiply(&mv, &mv, &model);

	glBindTexture(GL_TEXTURE_2D, shipTex);
	drawObj(&shipObj, &mvp, &mv);

	glPrintf(100 + sinf(rad) * 16, 240 + cosf(rad) * 16,
		 "frame=%i fps=%3.2f", frame, lfps);

	kmVec3 tmp;

	playerFireCount--;

	if (keys[KEY_LCTRL] && playerFireCount < 0) {

		struct playerShot_t *freeShot;
		freeShot = getFreeShot();
		if (freeShot != 0) {
			playerFireCount = 15;
			freeShot->alive = true;
			kmVec3Assign(&freeShot->pos, &playerPos);
		}
	}

	for (int n = 0; n < MAX_PLAYER_SHOTS; n++) {
		if (playerShots[n].alive) {
			playerShots[n].pos.z -= .1;
			if (playerShots[n].pos.z < -10)
				playerShots[n].alive = false;

			kmMat4Identity(&model);
			kmMat4Translation(&model, playerShots[n].pos.x,
					  playerShots[n].pos.y,
					  playerShots[n].pos.z);
			kmMat4RotationPitchYawRoll(&model, rad * 4, 0,
						   -rad * 4);

			kmMat4Assign(&mvp, &vp);
			kmMat4Multiply(&mvp, &mvp, &model);

			kmMat4Assign(&mv, &view);
			kmMat4Multiply(&mv, &mv, &model);

			glBindTexture(GL_TEXTURE_2D, shotTex);
			drawObj(&shotObj, &mvp, &mv);
		}
	}

	playerRoll = 0;
	if (keys[KEY_CURSL] && playerPos.x > -10) {
		playerPos.x -= 0.1;
		playerRoll = .2;
	}
	if (keys[KEY_CURSR] && playerPos.x < 10) {
		playerPos.x += 0.1;
		playerRoll = -.2;
	}
	pEye.x = playerPos.x * 1.25;

	pCenter.x = playerPos.x;
	pCenter.y = playerPos.y + 1;
	pCenter.z = playerPos.z;

	int deadAliens;

	deadAliens = 0;

	for (int n = 0; n < MAX_ALIENS; n++) {
		if (aliens[n].alive == true) {

			kmMat4Identity(&model);
			kmMat4Translation(&model, aliens[n].pos.x,
					  aliens[n].pos.y, aliens[n].pos.z);
			kmMat4RotationPitchYawRoll(&model, -.4, 0, 0);

			kmMat4Assign(&mvp, &vp);
			kmMat4Multiply(&mvp, &mvp, &model);

			kmMat4Assign(&mv, &view);
			kmMat4Multiply(&mv, &mv, &model);

			glBindTexture(GL_TEXTURE_2D, alienTex);
			drawObj(&alienObj, &mvp, &mv);

			kmVec3 d;
			for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
				kmVec3Subtract(&d, &aliens[n].pos,
					       &playerShots[i].pos);
				if (kmVec3Length(&d) < .7
				    && playerShots[i].alive) {
					aliens[n].alive = false;
					playerShots[i].alive = false;
				}
			}
		} else {
			deadAliens++;
		}
	}

	if (deadAliens == MAX_ALIENS) {
		resetAliens();
	}

	kmMat4LookAt(&view, &pEye, &pCenter, &pUp);

	kmMat4Assign(&vp, &projection);
	kmMat4Multiply(&vp, &vp, &view);

	glPrintf(100, 280, "eye    %3.2f %3.2f %3.2f ", pEye.x, pEye.y, pEye.z);
	glPrintf(100, 296, "centre %3.2f %3.2f %3.2f ", pCenter.x, pCenter.y,
		 pCenter.z);
	glPrintf(100, 320, "mouse %i,%i %i ", mouse[0], mouse[1], mouse[2]);

	swapBuffers();

}
