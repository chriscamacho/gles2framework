
// turned into a lighting test as 6 planes (a cube) is not idea for 
// checking frag lighting, there is also a sphere

// press Q & W
// and A & S to rotate the camera and light

#include "support.h"		// support routines
#include "obj.h"		// loading and displaying wavefront OBJ derived shapes

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
// this gets called if the window is resized
void window_size_callback(GLFWwindow* window, int w, int h);

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

GLFWwindow* window;
int width,height;

font_t *font1,*font2;

struct timespec ts;

int main()
{
	
    // create a window and GLES context
	if (!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(640, 480, "Simple test", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetWindowSizeCallback(window,window_size_callback);	
	glfwMakeContextCurrent(window);



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

    pEye.x = 0;		pEye.y = 0;		pEye.z = 5;
    pCenter.x = 0;	pCenter.y = 0;	pCenter.z = 0;
    pUp.x = 0;		pUp.y = 1;		pUp.z = 0;

    kmVec3Subtract(&viewDir,&pEye,&pCenter);
    kmVec3Normalize(&viewDir,&viewDir);

    kmMat4LookAt(&view, &pEye, &pCenter, &pUp);

    // these two matrices are pre combined for use with each model render
    // the view and projection
    kmMat4Assign(&vp, &projection);
    kmMat4Multiply(&vp, &vp, &view);


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



    while (!quit) {		// the main "logic" loop - stuff to do each frame that isn't directly rendering
		
        clock_gettime(0,&ts);  // note the time BEFORE we start to render the current frame

		glfwPollEvents();

        if (glfwGetKey(window,GLFW_KEY_ESC)==GLFW_PRESS || glfwWindowShouldClose(window))
            quit = true;	// exit if escape key pressed or window closed

        if (glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS) camAng=camAng+1;
        if (glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS) camAng=camAng-1;
        if (glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS) lightAng=lightAng+1;
        if (glfwGetKey(window,GLFW_KEY_Q)==GLFW_PRESS) lightAng=lightAng-1;


        render();	// the render loop
        
        // it's good practice not to use 100% of the CPU time
        // cpu / gpu is physically cooler on embedded systems 
        // and also gives the OS chance to do other things without 
        // making the framerate look intermittent ....
        
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

    initGlPrint(width, height);

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

//	----

    kmMat4Identity(&model);
    kmMat4Translation(&model, -1, 0, 0);
    kmMat4RotationPitchYawRoll(&model, 0 , -rad, 0);

    kmMat4Assign(&mvp, &vp);
    kmMat4Multiply(&mvp, &mvp, &model);	// model, view, projection combined matrix
    kmMat4Assign(&mv, &view);
    kmMat4Multiply(&mv, &mv, &model);	// view, model matrix for lighting

    glBindTexture(GL_TEXTURE_2D, ballTex);
    drawObj(&ballObj, &mvp, &mv,lightDir,viewDir);

//	----


    // see printf documentation for the formatting of variables...
    glPrintf(24, 24, font2, "frame=%i", frame);

	double mx,my;
	glfwGetCursorPos (window,&mx,&my);
    glPrintf(100, 260, font1, "mouse %i  %i ",(int)mx,(int)my);
	
	glPrintf(100, 320, font2, "abcABCqrsQRS123"); 


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

	reProjectGlPrint(width,height);
}
