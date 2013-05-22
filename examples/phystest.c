/*

Probably not the worlds best example of using ODE!
cobbled together quickly in a few hours...

*/
#include <GL/glfw3.h>
#include "tinycthread.h" // lets us doze...

#include <stdlib.h>
#include <stdbool.h>

#include <kazmath.h>		// matrix manipulation routines

#include "support.h"		// support routines
#include "obj.h"		// loading and displaying wavefront OBJ derived shapes

#include <ode/ode.h>

#define numObj 128  // 64 boxes, 64 spheres
// TODO make a physics object struct with pointer to visual
// geom and body id...
dBodyID obj[numObj];
dGeomID geoms[numObj];

dWorldID world;
dSpaceID space;
dJointGroupID contactgroup;

static void
setTransform(const float pos[3], const float R[12], GLfloat * matrix)
{
    matrix[0] = R[0];
    matrix[1] = R[4];
    matrix[2] = R[8];
    matrix[3] = 0;
    matrix[4] = R[1];
    matrix[5] = R[5];
    matrix[6] = R[9];
    matrix[7] = 0;
    matrix[8] = R[2];
    matrix[9] = R[6];
    matrix[10] = R[10];
    matrix[11] = 0;
    matrix[12] = pos[0];
    matrix[13] = pos[1];
    matrix[14] = pos[2];
    matrix[15] = 1;
}

#define MAX_CONTACTS 8

static void nearCallback(void *data, dGeomID o1, dGeomID o2)
{
    int i;
    // if (o1->body && o2->body) return;

    // exit without doing anything if the two bodies are connected by a joint
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
        return;

    dContact contact[MAX_CONTACTS];	// up to MAX_CONTACTS contacts per box-box
    for (i = 0; i < MAX_CONTACTS; i++) {
        contact[i].surface.mode = dContactBounce | dContactSoftCFM;
        contact[i].surface.mu = dInfinity;
        contact[i].surface.mu2 = 0;
        contact[i].surface.bounce = 0.2;
        contact[i].surface.bounce_vel = 0.2;
        contact[i].surface.soft_cfm = 0.01;
    }
    int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom,
                        sizeof(dContact));
    if (numc) {
        dMatrix3 RI;
        dRSetIdentity(RI);
        for (i = 0; i < numc; i++) {
            dJointID c =
                dJointCreateContact(world, contactgroup, contact + i);
            dJointAttach(c, b1, b2);
        }
    }

}

struct coll_t {
    int numVerts;
    float *verts;
};

struct coll_t loadCollisionObj(const char *objFile)
{
    FILE *pFile;
    struct coll_t r;
    r.numVerts = 0;
    r.verts = NULL;
    pFile = fopen(objFile, "rb");
    if (pFile == NULL) {
        printf("Cant find open model - %s\n", objFile);
        return r;
    }
    unsigned int magic;
    int NumVerts;

    fread(&magic, 1, sizeof(unsigned int), pFile);
    if (magic != 0x614f4247) {
        printf("Does not appear to be a version 'a' GBO file\n");
        return r;
    }
    fread(&NumVerts, 1, sizeof(unsigned int), pFile);

    float *Verts = malloc(sizeof(float) * 3 * NumVerts);
    fread(Verts, 1, sizeof(float) * 3 * NumVerts, pFile);

    r.numVerts = NumVerts;
    r.verts = Verts;
    return r;
}



void render();			// func prototype
void window_size_callback(GLFWwindow* window, int w, int h);

// obj shape textures
GLuint cubeTex, groundTex,ballTex;
font_t *font1;

// structures holding various pointers and handles for obj shapes
struct obj_t cubeObj, groundObj,ballObj;
struct coll_t groundColl;
dGeomID groundGeom;
dTriMeshDataID triData;

// matrices and combo matrices
kmMat4 model, view, projection, mvp, vp, mv;
kmVec3 lightDir;
kmVec3 pEye, pCenter, pUp;	// "camera" vectors
// from position , target position and up direction

int frame = 0;
float *pos, *rot;

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
	
    lightDir.x=.25;
    lightDir.y=.75;
    lightDir.z=-.25;
    kmVec3Normalize(&lightDir,&lightDir);






    // all the shaders have at least texture unit 0 active so
    // activate it now and leave it active
    glActiveTexture(GL_TEXTURE0);

    // The obj shapes and their textures are loaded
    cubeTex = loadPNG("resources/textures/dice.png");

    loadObj(&cubeObj,"resources/models/cube.gbo",
            "resources/shaders/textured.vert",
            "resources/shaders/textured.frag");

    groundTex = loadPNG("resources/textures/raspbian.png");
    loadObjCopyShader(&groundObj, "resources/models/ground.gbo",
                      &cubeObj);

    ballTex = loadPNG("resources/textures/jupiter.png");
    loadObjCopyShader(&ballObj, "resources/models/sphere.gbo",
                      &cubeObj);


    kmMat4Identity(&view);

    pEye.x = 0;
    pEye.y = 14;
    pEye.z = 30;
    pCenter.x = 0;
    pCenter.y = 5;
    pCenter.z = 0;
    pUp.x = 0;
    pUp.y = 1;
    pUp.z = 0;

    kmMat4LookAt(&view, &pEye, &pCenter, &pUp);

    // projection matrix, as distance increases
    // the way the model is drawn is effected
    kmMat4Identity(&projection);
    kmMat4PerspectiveProjection(&projection, 45,
                                (float) width / height, 0.1, 1000);

    glViewport(0, 0, width,height);

    // these two matrices are pre combined for use with each model render
    // the view and projection
    kmMat4Assign(&vp, &projection);
    kmMat4Multiply(&vp, &vp, &view);

    // initialises glprint's matrix, shader and texture
    initGlPrint(width,height);
	font1=createFont("resources/textures/font.png",0,256,16,16,16);
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



    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    dWorldSetGravity(world, 0, -9.8, 0);
    dWorldSetCFM(world, 1e-5);
    //dCreatePlane (space,0,1,0,0);

    dMatrix3 R;
    dMass m;
    dMassSetZero(&m);
    dMassSetBox(&m, 20, 0.5, 0.5, 0.5);
    for (int i = 0; i < numObj; i++) {
        obj[i] = dBodyCreate(world);
        if (i<numObj/2) {
            geoms[i] = dCreateBox(space, 1,1,1);
            dBodySetPosition(obj[i],
                             6+dRandReal() * 2 - 1, i + 1, dRandReal() * 2 - 1);
        } else {
            geoms[i] = dCreateSphere(space,1);
            dBodySetPosition(obj[i],
                             -6+dRandReal() * 2 - 1, i + 1 - (numObj/2), dRandReal() * 2 - 1);

        }
        dRFromAxisAndAngle(R, dRandReal() * 2.0 - 1.0,
                           dRandReal() * 2.0 - 1.0,
                           dRandReal() * 2.0 - 1.0,
                           dRandReal() * 10.0 - 5.0);
        dBodySetRotation(obj[i], R);
        dBodySetMass(obj[i], &m);
        dGeomSetBody(geoms[i], obj[i]);

    }

    groundColl = loadCollisionObj("resources/models/ground.gbo");
    int *groundInd = malloc(groundColl.numVerts * sizeof(int));
    for (int i = 0; i < groundColl.numVerts; i++)
        groundInd[i] = i;	// decidedly sub optimal !
    triData = dGeomTriMeshDataCreate();

    dGeomTriMeshDataBuildSingle(triData, &groundColl.verts[0],
                                3 * sizeof(float), groundColl.numVerts,
                                groundInd, groundColl.numVerts,
                                3 * sizeof(int));
    groundGeom = dCreateTriMesh(space, triData, NULL, NULL, NULL);


    while (!quit) {		// the main loop

        clock_gettime(0,&ts);  // note the time BEFORE we start to render the current frame
		glfwPollEvents();

        if (glfwGetKey(window,GLFW_KEY_ESC)==GLFW_PRESS || glfwWindowShouldClose(window))
            quit = true;	// exit if escape key pressed or window closed
            

        if (glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS ) {
            for (int i = 0; i < numObj; i++) {
                dBodyAddForce(obj[i], dRandReal() * 2.0 - 1.0,
                              1 + dRandReal(), dRandReal() * 2.0 - 1.0);
            }
        }

        for (int i=0; i<numObj; i++) {

            pos=(float*)dBodyGetPosition(obj[i]);
            if (pos[1]<-5) {
                if (i<numObj/2) {
                    dBodySetPosition(obj[i],
                                     6+dRandReal() * 2 - 1, 5+i, dRandReal() * 2 - 1);
                } else {
                    dBodySetPosition(obj[i],
                                     -6+dRandReal() * 2 - 1, 5+i-(numObj/2), dRandReal() * 2 - 1);
                }
                dBodySetLinearVel(obj[i],0,0,0);
            }
        }

        dSpaceCollide(space, 0, &nearCallback);
        dWorldQuickStep(world, 1. / 50);
        dJointGroupEmpty(contactgroup);

        pEye.x = sin(frame/100.)*18;
        pEye.z = cos(frame/100.)*18;

        kmMat4LookAt(&view, &pEye, &pCenter, &pUp);
        kmMat4Assign(&vp, &projection);
        kmMat4Multiply(&vp, &vp, &view);

        render();		// the render loop

        ts.tv_nsec+=20000000;  // 1000000000 / 50 = 50hz less time to render the frame
        thrd_sleep(&ts,NULL); // tinycthread

    }

    // TODO although OS will throw stuff away OK
    // need to double check here for completness

    dJointGroupDestroy(contactgroup);
    dSpaceDestroy(space);
    dWorldDestroy(world);
    dCloseODE();
    
	glfwDestroyWindow(window);
	glfwTerminate();

    return 0;
}

kmVec3 viewDir;

void render()
{

    float rad;			// radians of rotation based on frame counter

    // clear the colour (drawing) and depth sort back (offscreen) buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // count the frame and base a rotation on it.
    frame++;

    // recalculate the view direction vector used by lighting
    kmVec3Subtract(&viewDir,&pEye,&pCenter);
    kmVec3Normalize(&viewDir,&viewDir);

    glBindTexture(GL_TEXTURE_2D, groundTex);
    kmMat4Identity(&model);
    kmMat4Assign(&mvp, &vp);
    kmMat4Multiply(&mvp, &mvp, &model);	// model, view, projection combined matrix

    kmMat4Assign(&mv, &view);
    kmMat4Multiply(&mv, &mv, &model);	// view, model matrix for lighting

    drawObj(&groundObj, &mvp, &mv,lightDir,viewDir);

    glBindTexture(GL_TEXTURE_2D, cubeTex);
    for (int i = 0; i < numObj; i++) {

        pos = (float *) dBodyGetPosition(obj[i]);
        rot = (float *) dBodyGetRotation(obj[i]);
        setTransform(pos, rot, &model.mat[0]);

        kmMat4Assign(&mvp, &vp);
        kmMat4Multiply(&mvp, &mvp, &model);	// model, view, projection combined matrix

        kmMat4Assign(&mv, &view);
        kmMat4Multiply(&mv, &mv, &model);	// view, model matrix for lighting

        if (i==numObj/2) {
            glBindTexture(GL_TEXTURE_2D, ballTex);
        }

        if (i<(numObj/2)) {
            drawObj(&cubeObj, &mvp, &mv,lightDir,viewDir);
        } else {
            drawObj(&ballObj, &mvp, &mv,lightDir,viewDir);
        }
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
                                (float)width / height, 0.1, 1000);

	reProjectGlPrint(width,height); // updates the projection matrix used by glPrint

}
