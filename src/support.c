#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>		// va_lists for glprint

#include  <GLES2/gl2.h>
#include  <EGL/egl.h>

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

Display *x_display;

#include <png.h>
#include <kazmath.h>
#include "support.h"

// TODO replace with params in initialisation call....


int display_width,display_height;

int getDisplayWidth() {
	return display_width;
}


int getDisplayHeight() {
	return display_height;
}


#ifndef __FOR_RPi__

#include  "bcm_host.h"
Window win, eventWin;

#else

EGLNativeWindowType win;
Window eventWin;

#endif

EGLDisplay egl_display;
EGLContext egl_context;
EGLSurface egl_surface;

// only used internally
void closeNativeWindow()
{

#ifdef __FOR_XORG__
	XDestroyWindow(x_display, win);	// on normal X (ie not pi) win and eventWin point to same window
	XCloseDisplay(x_display);
#endif				//__FOR_XORG__

#ifdef __FOR_RPi__
	XDestroyWindow(x_display, eventWin);	// on the pi win is dummy "context" window
	XCloseDisplay(x_display);
#endif				//__FOR_RPi__
}

// only used internally
void makeNativeWindow()
{

#ifdef __FOR_XORG__

	x_display = XOpenDisplay(NULL);	// open the standard display (the primary screen)
	if (x_display == NULL) {
		printf("cannot connect to X server\n");
	}

	Window root = DefaultRootWindow(x_display);	// get the root window (usually the whole screen)

	XSetWindowAttributes swa;
	swa.event_mask =
	    ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;

	display_width=640;display_height=480;  // xorg hard coded for now	
	int s = DefaultScreen(x_display);
	win = XCreateSimpleWindow(x_display, root,
				  10, 10, display_width, display_height, 1,
				  BlackPixel(x_display, s),
				  WhitePixel(x_display, s));
	XSelectInput(x_display, win, ExposureMask |
		     KeyPressMask | KeyReleaseMask |
		     ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	XSetWindowAttributes xattr;
	Atom atom;
	int one = 1;

	xattr.override_redirect = False;
	XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

/*
	atom = XInternAtom(x_display, "_NET_WM_STATE_FULLSCREEN", True);
	XChangeProperty(x_display, win,
			XInternAtom(x_display, "_NET_WM_STATE", True),
			XA_ATOM, 32, PropModeReplace, (unsigned char *)&atom,
			1);
*/

	XWMHints hints;
	hints.input = True;
	hints.flags = InputHint;
	XSetWMHints(x_display, win, &hints);

	XMapWindow(x_display, win);	// make the window visible on the screen
	XStoreName(x_display, win, "GLES2.0 framework");	// give the window a name

	// NB - RPi needs to use EGL_DEFAULT_DISPLAY that some X configs dont seem to like
	egl_display = eglGetDisplay((EGLNativeDisplayType) x_display);
	if (egl_display == EGL_NO_DISPLAY) {
		printf("Got no EGL display.\n");
	}

	eventWin = win;

#endif				//__FOR_XORG__

#ifdef __FOR_RPi__

	bcm_host_init();

	int32_t success = 0;


	// create an EGL window surface, passing context width/height
	success = graphics_get_display_size(0 /* LCD */ , &display_width,
					    &display_height);
	if (success < 0) {
		printf("unable to get display size\n");
		//return EGL_FALSE;
	}
	

	x_display = XOpenDisplay(NULL);	// open the standard display (the primary screen)
	if (x_display == NULL) {
		printf("cannot connect to X server\n");
	}

	Window root = DefaultRootWindow(x_display);	// get the root window (usually the whole screen)

	XSetWindowAttributes swa;
	swa.event_mask =
	    ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;

	int s = DefaultScreen(x_display);
	eventWin = XCreateSimpleWindow(x_display, root,
				       0, 0, display_width, display_height, 1,
				       BlackPixel(x_display, s),
				       WhitePixel(x_display, s));
	XSelectInput(x_display, eventWin, ExposureMask |
		     KeyPressMask | KeyReleaseMask |
		     ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	XSetWindowAttributes xattr;
	Atom atom;
	int one = 1;

	xattr.override_redirect = False;
	XChangeWindowAttributes(x_display, eventWin, CWOverrideRedirect,
				&xattr);

	XWMHints hints;
	hints.input = True;
	hints.flags = InputHint;
	XSetWMHints(x_display, eventWin, &hints);

	XMapWindow(x_display, eventWin);	// make the window visible on the screen
	XStoreName(x_display, eventWin, "Event trap");	// give the window a name

	// we have to be full screen to capture all mouse events
	// TODO consider using warp mouse to report relative motions
	// instead of absolute...

	XFlush(x_display);	// you have to flush or bcm seems to prevent window coming up? 

	Atom wmState = XInternAtom(x_display, "_NET_WM_STATE", False);
	Atom fullScreen = XInternAtom(x_display,
				      "_NET_WM_STATE_FULLSCREEN", False);
	XEvent xev;
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = True;
	xev.xclient.window = eventWin;
	xev.xclient.message_type = wmState;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;	//_NET_WM_STATE_ADD
	xev.xclient.data.l[1] = fullScreen;
	xev.xclient.data.l[2] = 0;
	XSendEvent(x_display, root, False,
		   SubstructureRedirectMask | SubstructureNotifyMask, &xev);

	XFlush(x_display);	// you have to flush or bcm seems to prevent window coming up? 
	
	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;



//	printf("display size %i,%i\n",display_width,display_height);

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = display_width;
	dst_rect.height = display_height;


	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = display_width << 16;
	src_rect.height = display_height << 16;

	dispman_display = vc_dispmanx_display_open(0 /* LCD */ );
	dispman_update = vc_dispmanx_update_start(0);

   VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FROM_SOURCE | DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,255,0 };

	dispman_element =
	    vc_dispmanx_element_add(dispman_update, dispman_display,
				    0 /*layer */ , &dst_rect, 0 /*src */ ,
				    &src_rect, DISPMANX_PROTECTION_NONE,
				    &alpha /*alpha */ , 0 /*clamp */ ,
				    0 /*transform */ );

	nativewindow.element = dispman_element;
	nativewindow.width = display_width;
	nativewindow.height = display_height;
	vc_dispmanx_update_submit_sync(dispman_update);

	win = &nativewindow;

	egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#endif				//__FOR_RPi__

}

int loadPNG(const char *filename)
{

	GLuint texture;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep *row_pointers = NULL;
	int bitDepth, colourType;

	FILE *pngFile = fopen(filename, "rb");

	if (!pngFile)
		return 0;

	png_byte sig[8];

	fread(&sig, 8, sizeof(png_byte), pngFile);
	rewind(pngFile);
	if (!png_check_sig(sig, 8)) {
		printf("png sig failure\n");
		return 0;
	}

	png_ptr =
	    png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		printf("png ptr not created\n");
		return 0;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("set jmp failed\n");
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		printf("cant get png info ptr\n");
		return 0;
	}

	png_init_io(png_ptr, pngFile);
	png_read_info(png_ptr, info_ptr);
	bitDepth = png_get_bit_depth(png_ptr, info_ptr);
	colourType = png_get_color_type(png_ptr, info_ptr);

	if (colourType == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);
	if (colourType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
		//png_set_gray_1_2_4_to_8(png_ptr);
		png_set_expand_gray_1_2_4_to_8(png_ptr);  // thanks to Jesse Jaara for bug fix for newer libpng...
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	if (bitDepth == 16)
		png_set_strip_16(png_ptr);
	else if (bitDepth < 8)
		png_set_packing(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	png_uint_32 width, height;
	png_get_IHDR(png_ptr, info_ptr, &width, &height,
		     &bitDepth, &colourType, NULL, NULL, NULL);

	int components;		// = GetTextureInfo(colourType);
	switch (colourType) {
	case PNG_COLOR_TYPE_GRAY:
		components = 1;
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		components = 2;
		break;
	case PNG_COLOR_TYPE_RGB:
		components = 3;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		components = 4;
		break;
	default:
		components = -1;
	}

	if (components == -1) {
		if (png_ptr)
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		printf("%s broken?\n", filename);
		return 0;
	}

	GLubyte *pixels =
	    (GLubyte *) malloc(sizeof(GLubyte) * (width * height * components));
	row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);

	for (int i = 0; i < height; ++i)
		row_pointers[i] =
		    (png_bytep) (pixels + (i * width * components));

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLuint glcolours;
	(components == 4) ? (glcolours = GL_RGBA) : (0);
	(components == 3) ? (glcolours = GL_RGB) : (0);
	(components == 2) ? (glcolours = GL_LUMINANCE_ALPHA) : (0);
	(components == 1) ? (glcolours = GL_LUMINANCE) : (0);

	printf("%s has %i colour components\n",filename,components);	
	//glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, glcolours, GL_UNSIGNED_BYTE, pixels);
	glTexImage2D(GL_TEXTURE_2D, 0, glcolours, width, height, 0, glcolours,
		     GL_UNSIGNED_BYTE, pixels);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(pngFile);
	free(row_pointers);
	free(pixels);

	return texture;

}

// only here to keep egl pointers out of frontend code
void swapBuffers()
{
	eglSwapBuffers(egl_display, egl_surface);	// get the rendered buffer to the screen
}

GLuint getShaderLocation(int type, GLuint prog, const char *name)
{
	GLuint ret;
	if (type == shaderAttrib)
		ret = glGetAttribLocation(prog, name);
	if (type == shaderUniform)
		ret = glGetUniformLocation(prog, name);
	if (ret == -1) {
		printf("Cound not bind shader location %s\n", name);
	}
	return ret;
}

/**
 * Store all the file's contents in memory, useful to pass shaders
 * source code to OpenGL
 */
char *file_read(const char *filename)
{
	FILE *in = fopen(filename, "rb");
	if (in == NULL)
		return NULL;

	int res_size = BUFSIZ;
	char *res = (char *)malloc(res_size);
	int nb_read_total = 0;

	while (!feof(in) && !ferror(in)) {
		if (nb_read_total + BUFSIZ > res_size) {
			if (res_size > 10 * 1024 * 1024)
				break;
			res_size = res_size * 2;
			res = (char *)realloc(res, res_size);
		}
		char *p_res = res + nb_read_total;
		nb_read_total += fread(p_res, 1, BUFSIZ, in);
	}

	fclose(in);
	res = (char *)realloc(res, nb_read_total + 1);
	res[nb_read_total] = '\0';
	return res;
}

/**
 * Display compilation errors from the OpenGL shader compiler
 */
void print_log(GLuint object)
{
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else {
		fprintf(stderr, "printlog: Not a shader or a program\n");
		return;
	}

	char *log = (char *)malloc(log_length);

	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);

	fprintf(stderr, "%s", log);
	free(log);
}

/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint create_shader(const char *filename, GLenum type)
{
	const GLchar *source = file_read(filename);
	if (source == NULL) {
		fprintf(stderr, "Error opening %s: ", filename);
		perror("");
		return 0;
	}
	GLuint res = glCreateShader(type);
	const GLchar *sources[] = {
		// Define GLSL version
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"
#else
		"#version 120\n"
#endif
		    ,
		// GLES2 precision specifiers
#ifdef GL_ES_VERSION_2_0
		// Define default float precision for fragment shaders:
		(type == GL_FRAGMENT_SHADER) ?
		    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
		    "precision highp float;           \n"
		    "#else                            \n"
		    "precision mediump float;         \n"
		    "#endif                           \n" : ""
		    // Note: OpenGL ES automatically defines this:
		    // #define GL_ES
#else
		// Ignore GLES 2 precision specifiers:
		"#define lowp   \n" "#define mediump\n" "#define highp  \n"
#endif
		    ,
		source
	};
	glShaderSource(res, 3, sources, NULL);
	free((void *)source);

	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		fprintf(stderr, "%s:", filename);
		print_log(res);
		glDeleteShader(res);
		return 0;
	}

	return res;
}

kmMat4 opm, otm, t;
GLuint printProg, opm_uniform;
GLuint fonttex, texture_uniform, cx_uniform, cy_uniform;
GLuint vert_attrib, uv_attrib;
GLuint quadvbo, texvbo;

const GLfloat quadVertices[] = {
	0, 0, 0,
	16, 16, 0,
	16, 0, 0,

	16, 16, 0,
	0, 0, 0,
	0, 16, 0
};

GLfloat texCoord[] = {
	0, 0,
	1. / 16, 1. / 16,
	1. / 16, 0,
	1. / 16, 1. / 16,
	0, 0,
	0, 1. / 16
};

void initGlPrint(int w, int h)
{

	kmMat4OrthographicProjection(&opm, 0, w, h, 0, -10, 10);

	GLuint vs, fs;
	vs = create_shader("resources/shaders/glprint.vert", GL_VERTEX_SHADER);
	fs = create_shader("resources/shaders/glprint.frag", GL_FRAGMENT_SHADER);

	printProg = glCreateProgram();
	glAttachShader(printProg, vs);
	glAttachShader(printProg, fs);
	glLinkProgram(printProg);
	int link_ok;
	glGetProgramiv(printProg, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		printf("glLinkProgram:");
		print_log(printProg);
		printf("\n");
	}

	cx_uniform = getShaderLocation(shaderUniform, printProg, "cx");
	cy_uniform = getShaderLocation(shaderUniform, printProg, "cy");
	opm_uniform =
	    getShaderLocation(shaderUniform, printProg, "opm_uniform");
	texture_uniform =
	    getShaderLocation(shaderUniform, printProg, "texture_uniform");

	vert_attrib = getShaderLocation(shaderAttrib, printProg, "vert_attrib");
	uv_attrib = getShaderLocation(shaderAttrib, printProg, "uv_attrib");

	fonttex = loadPNG("resources/textures/font.png");

	glGenBuffers(1, &quadvbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6, quadVertices,
		     GL_STATIC_DRAW);

	glGenBuffers(1, &texvbo);
	glBindBuffer(GL_ARRAY_BUFFER, texvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 6, texCoord,
		     GL_STATIC_DRAW);
}

void glPrintf(float x, float y, const char *fmt, ...)
{
	char text[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	glUseProgram(printProg);
	kmMat4Assign(&otm, &opm);
	kmMat4Translation(&t, x, y, -1);
	kmMat4Multiply(&otm, &otm, &t);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, fonttex);

	glEnableVertexAttribArray(vert_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
	glVertexAttribPointer(vert_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(uv_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, texvbo);
	glVertexAttribPointer(uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glUniform1i(texture_uniform, 0);

	for (int n = 0; n < strlen(text); n++) {

		int c = (int)text[n];
		float cx = c % 16;
		float cy = (int)(c / 16.0);
		cy = cy * (1. / 16);
		cx = cx * (1. / 16);

		glUniformMatrix4fv(opm_uniform, 1, GL_FALSE, (GLfloat *) & otm);
		glUniform1f(cx_uniform, cx);
		glUniform1f(cy_uniform, cy);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		kmMat4Translation(&t, 16, 0, 0);
		kmMat4Multiply(&otm, &otm, &t);
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisableVertexAttribArray(uv_attrib);
	glDisableVertexAttribArray(vert_attrib);

}

int makeContext()
{
	makeNativeWindow();	// sets global pointers for win and disp

	EGLint majorVersion;
	EGLint minorVersion;

	// most egl you can sends NULLS for maj/min but not RPi 
	if (!eglInitialize(egl_display, &majorVersion, &minorVersion)) {
		printf("Unable to initialize EGL\n");
		return 1;
	}

	EGLint attr[] = {	// some attributes to set up our egl-interface
		EGL_BUFFER_SIZE, 16,
		EGL_DEPTH_SIZE, 16,
		EGL_RENDERABLE_TYPE,
		EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLConfig ecfg;
	EGLint num_config;
	if (!eglChooseConfig(egl_display, attr, &ecfg, 1, &num_config)) {
		//cerr << "Failed to choose config (eglError: " << eglGetError() << ")" << endl;
		printf("failed to choose config eglerror:%i\n", eglGetError());	// todo change error number to text error
		return 1;
	}

	if (num_config != 1) {
		printf("Didn't get exactly one config, but %i\n", num_config);
		return 1;
	}

	egl_surface = eglCreateWindowSurface(egl_display, ecfg, win, NULL);

	if (egl_surface == EGL_NO_SURFACE) {
		//cerr << "Unable to create EGL surface (eglError: " << eglGetError() << ")" << endl;
		printf("failed create egl surface eglerror:%i\n",
		       eglGetError());
		return 1;
	}
	//// egl-contexts collect all state descriptions needed required for operation
	EGLint ctxattr[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	egl_context =
	    eglCreateContext(egl_display, ecfg, EGL_NO_CONTEXT, ctxattr);
	if (egl_context == EGL_NO_CONTEXT) {
		//cerr << "Unable to create EGL context (eglError: " << eglGetError() << ")" << endl;
		printf("unable to create EGL context eglerror:%i\n",
		       eglGetError());
		return 1;
	}
	//// associate the egl-context with the egl-surface
	eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

	return 0;
}

void closeContext()
{
	eglDestroyContext(egl_display, egl_context);
	eglDestroySurface(egl_display, egl_surface);
	eglTerminate(egl_display);

	closeNativeWindow();

}

bool __keys[256];
int __mouse[3];

void doEvents()
{

	XEvent event;

	while (XEventsQueued(x_display, QueuedAfterReading)) {
		XNextEvent(x_display, &event);
		switch (event.type) {

		case KeyPress:
			__keys[event.xkey.keycode & 0xff] = true;
			//printf("key=%i\n",event.xkey.keycode & 0xff);
			break;

		case KeyRelease:
			__keys[event.xkey.keycode & 0xff] = false;
			break;

		case MotionNotify:
			__mouse[0] = event.xbutton.x;
			__mouse[1] = event.xbutton.y;
			//printf("mouse %i,%i\n",__mouse[0],__mouse[1]);
			break;

		case ButtonPress:
			__mouse[2] =
			    __mouse[2] | (int)pow(2, event.xbutton.button - 1);
			break;
		case ButtonRelease:
			__mouse[2] =
			    __mouse[2] & (int)(255 -
					       pow(2,
						   event.xbutton.button - 1));
			break;

		}
	}
}

int *getMouse()
{
	return &__mouse[0];
}

bool *getKeys()
{
	return &__keys[0];
}
