#include "support.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>		// va_lists for glprint


#include  <GLES2/gl2.h>
#include  <EGL/egl.h>

#ifdef __FOR_RPi_noX__
extern void restoreKbd();
extern int __mouse_fd;
#endif

#ifndef __FOR_RPi_noX__

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

Display *__x_display;

#endif //NOT  __FOR_RPi_noX__

#include <png.h>
#include <kazmath.h>


int __display_width,__display_height;

int getDisplayWidth() {
    return __display_width;
}


int getDisplayHeight() {
    return __display_height;
}


#ifdef __FOR_XORG__

Window __win, __eventWin;

#endif


#ifndef __FOR_XORG__ // ie one of pi options

EGLNativeWindowType __win;

#ifndef __FOR_RPi_noX__
Window __eventWin;
#endif

#endif

EGLDisplay __egl_display;
EGLContext __egl_context;
EGLSurface __egl_surface;

// only used internally
void closeNativeWindow()
{

#ifdef __FOR_XORG__
    XDestroyWindow(__x_display, __win);	// on normal X (ie not pi) win and __eventWin point to same window
    XCloseDisplay(__x_display);
#endif				//__FOR_XORG__

#ifdef __FOR_RPi__
    XDestroyWindow(__x_display, __eventWin);	// on the pi win is dummy "context" window
    XCloseDisplay(__x_display);
#endif				//__FOR_RPi__

#ifdef __FOR_RPi_noX__
    restoreKbd();
#endif

}

// only used internally
void makeNativeWindow()
{

#ifdef __FOR_XORG__

    __x_display = XOpenDisplay(NULL);	// open the standard display (the primary screen)
    if (__x_display == NULL) {
        printf("cannot connect to X server\n");
    }

    Window root = DefaultRootWindow(__x_display);	// get the root window (usually the whole screen)


    XSetWindowAttributes swa;
    swa.event_mask =
        ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;

    __display_width=640;
    __display_height=480;  // xorg hard coded for now
    int s = DefaultScreen(__x_display);
    __win = XCreateSimpleWindow(__x_display, root,
                                10, 10, __display_width, __display_height, 1,
                                BlackPixel(__x_display, s),
                                WhitePixel(__x_display, s));
    XSelectInput(__x_display, __win, ExposureMask |
                 KeyPressMask | KeyReleaseMask |
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

    XSetWindowAttributes xattr;
    Atom atom;
    int one = 1;

    xattr.override_redirect = False;
    XChangeWindowAttributes(__x_display, __win, CWOverrideRedirect, &xattr);

    /*
    	atom = XInternAtom(__x_display, "_NET_WM_STATE_FULLSCREEN", True);
    	XChangeProperty(__x_display, win,
    			XInternAtom(__x_display, "_NET_WM_STATE", True),
    			XA_ATOM, 32, PropModeReplace, (unsigned char *)&atom,
    			1);
    */

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(__x_display, __win, &hints);

    XMapWindow(__x_display, __win);	// make the window visible on the screen
    XStoreName(__x_display, __win, "GLES2.0 framework");	// give the window a name

    // NB - RPi needs to use EGL_DEFAULT_DISPLAY that some X configs dont seem to like
    __egl_display = eglGetDisplay((EGLNativeDisplayType) __x_display);
    if (__egl_display == EGL_NO_DISPLAY) {
        printf("Got no EGL display.\n");
    }

    __eventWin = __win;




    Cursor invisibleCursor;
    Pixmap bitmapNoData;
    XColor black;
    static char noData[] = { 0,0,0,0,0,0,0,0 };
    black.red = black.green = black.blue = 0;

    bitmapNoData = XCreateBitmapFromData(__x_display, __win, noData, 8, 8);
    invisibleCursor = XCreatePixmapCursor(__x_display, bitmapNoData, bitmapNoData,
                                          &black, &black, 0, 0);
    XDefineCursor(__x_display,__win, invisibleCursor);
    XFreeCursor(__x_display, invisibleCursor);


#endif				//__FOR_XORG__

#ifdef __FOR_RPi__

    bcm_host_init();

    int32_t success = 0;


    // create an EGL window surface, passing context width/height
    success = graphics_get_display_size(0 /* LCD */ , &__display_width,
                                        &__display_height);
    if (success < 0) {
        printf("unable to get display size\n");
        //return EGL_FALSE;
    }


    __x_display = XOpenDisplay(NULL);	// open the standard display (the primary screen)
    if (__x_display == NULL) {
        printf("cannot connect to X server\n");
    }

    Window root = DefaultRootWindow(__x_display);	// get the root window (usually the whole screen)

    XSetWindowAttributes swa;
    swa.event_mask =
        ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;

    int s = DefaultScreen(__x_display);
    __eventWin = XCreateSimpleWindow(__x_display, root,
                                     0, 0, __display_width, __display_height, 1,
                                     BlackPixel(__x_display, s),
                                     WhitePixel(__x_display, s));
    XSelectInput(__x_display, __eventWin, ExposureMask |
                 KeyPressMask | KeyReleaseMask |
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

    XSetWindowAttributes xattr;
    Atom atom;
    int one = 1;

    xattr.override_redirect = False;
    XChangeWindowAttributes(__x_display, __eventWin, CWOverrideRedirect,
                            &xattr);

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(__x_display, __eventWin, &hints);

    XMapWindow(__x_display, __eventWin);	// make the window visible on the screen
    XStoreName(__x_display, __eventWin, "Event trap");	// give the window a name

    // we have to be full screen to capture all mouse events
    // TODO consider using warp mouse to report relative motions
    // instead of absolute...

    XFlush(__x_display);	// you have to flush or bcm seems to prevent window coming up?

    Atom wmState = XInternAtom(__x_display, "_NET_WM_STATE", False);
    Atom fullScreen = XInternAtom(__x_display,
                                  "_NET_WM_STATE_FULLSCREEN", False);
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.serial = 0;
    xev.xclient.send_event = True;
    xev.xclient.window = __eventWin;
    xev.xclient.message_type = wmState;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;	//_NET_WM_STATE_ADD
    xev.xclient.data.l[1] = fullScreen;
    xev.xclient.data.l[2] = 0;
    XSendEvent(__x_display, root, False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);

    XFlush(__x_display);	// you have to flush or bcm seems to prevent window coming up?

    static EGL_DISPMANX_WINDOW_T nativewindow;

    DISPMANX_ELEMENT_HANDLE_T dispman_element;
    DISPMANX_DISPLAY_HANDLE_T dispman_display;
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;



//	printf("display size %i,%i\n",__display_width,__display_height);


    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = __display_width;
    dst_rect.height = __display_height;



    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = __display_width << 16;
    src_rect.height = __display_height << 16;

    dispman_display = vc_dispmanx_display_open(0 /* LCD */ );
    dispman_update = vc_dispmanx_update_start(0);

    //VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FROM_SOURCE | DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,255,0 };
    VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,255,0 };
    dispman_element =
        vc_dispmanx_element_add(dispman_update, dispman_display,
                                0 /*layer */ , &dst_rect, 0 /*src */ ,
                                &src_rect, DISPMANX_PROTECTION_NONE,
                                &alpha /*alpha */ , 0 /*clamp */ ,
                                0 /*transform */ );

    nativewindow.element = dispman_element;
    nativewindow.width = __display_width;
    nativewindow.height = __display_height;
    vc_dispmanx_update_submit_sync(dispman_update);

    __win = &nativewindow;

    __egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#endif				//__FOR_RPi__

#ifdef __FOR_RPi_noX__

    bcm_host_init();

    int32_t success = 0;

    success = graphics_get_display_size(0 /* LCD */ , &__display_width,
                                        &__display_height);
    if (success < 0) {
        printf("unable to get display size\n");
        //return EGL_FALSE;
    }


    static EGL_DISPMANX_WINDOW_T nativewindow;

    DISPMANX_ELEMENT_HANDLE_T dispman_element;
    DISPMANX_DISPLAY_HANDLE_T dispman_display;
    DISPMANX_UPDATE_HANDLE_T dispman_update;
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;


    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.width = __display_width;
    dst_rect.height = __display_height;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = __display_width << 16;
    src_rect.height = __display_height << 16;

    dispman_display = vc_dispmanx_display_open(0 /* LCD */ );
    dispman_update = vc_dispmanx_update_start(0);

    VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,255,0 };
    dispman_element =
        vc_dispmanx_element_add(dispman_update, dispman_display,
                                0 /*layer */ , &dst_rect, 0 /*src */ ,
                                &src_rect, DISPMANX_PROTECTION_NONE,
                                &alpha /*alpha */ , 0 /*clamp */ ,
                                0 /*transform */ );

    nativewindow.element = dispman_element;
    nativewindow.width = __display_width;
    nativewindow.height = __display_height;
    vc_dispmanx_update_submit_sync(dispman_update);

    __win = &nativewindow;

    __egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (__egl_display == EGL_NO_DISPLAY) {
        printf("Got no EGL display.\n");
    }

#endif //__FOR_RPi_noX__


}


float rand_range(float start,float range) {
    return start + range * ((float)rand() / RAND_MAX) ;

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

    //printf("%s has %i colour components\n",filename,components);
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
    eglSwapBuffers(__egl_display, __egl_surface);	// get the rendered buffer to the screen
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


struct {  // blob of globals for the glPrint routine
    kmMat4 opm, otm, t;
    GLuint printProg, opm_uniform;
    GLuint fonttex, texture_uniform, cx_uniform, cy_uniform;
    GLuint vert_attrib, uv_attrib;
    GLuint quadvbo, texvbo;
} __glp;

void initGlPrint(int w, int h)
{
    const GLfloat quadVertices[] = {
        0,	0,	0,
        16, 16, 0,
        16,	0,	0,

        16, 16, 0,
        0,	0,	0,
        0,	16,	0
    };

    const GLfloat texCoord[] = {
        0,			0,
        1. / 16,	1. / 16,
        1. / 16,	0,
        1. / 16,	1. / 16,
        0,			0,
        0,			1. / 16
    };


    kmMat4OrthographicProjection(&__glp.opm, 0, w, h, 0, -10, 10);

    GLuint vs, fs;
    vs = create_shader("resources/shaders/glprint.vert", GL_VERTEX_SHADER);
    fs = create_shader("resources/shaders/glprint.frag", GL_FRAGMENT_SHADER);

    __glp.printProg = glCreateProgram();
    glAttachShader(__glp.printProg, vs);
    glAttachShader(__glp.printProg, fs);
    glLinkProgram(__glp.printProg);
    int link_ok;
    glGetProgramiv(__glp.printProg, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        printf("glLinkProgram:");
        print_log(__glp.printProg);
        printf("\n");
    }

    __glp.cx_uniform = getShaderLocation(shaderUniform, __glp.printProg, "cx");
    __glp.cy_uniform = getShaderLocation(shaderUniform, __glp.printProg, "cy");
    __glp.opm_uniform =
        getShaderLocation(shaderUniform, __glp.printProg, "opm_uniform");
    __glp.texture_uniform =
        getShaderLocation(shaderUniform, __glp.printProg, "texture_uniform");

    __glp.vert_attrib = getShaderLocation(shaderAttrib, __glp.printProg, "vert_attrib");
    __glp.uv_attrib = getShaderLocation(shaderAttrib, __glp.printProg, "uv_attrib");

    __glp.fonttex = loadPNG("resources/textures/font.png");

    glGenBuffers(1, &__glp.quadvbo);
    glBindBuffer(GL_ARRAY_BUFFER, __glp.quadvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6, quadVertices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &__glp.texvbo);
    glBindBuffer(GL_ARRAY_BUFFER, __glp.texvbo);
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

    glUseProgram(__glp.printProg);
    kmMat4Assign(&__glp.otm, &__glp.opm);
    kmMat4Translation(&__glp.t, x, y, -1);
    kmMat4Multiply(&__glp.otm, &__glp.otm, &__glp.t);

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, __glp.fonttex);

    glEnableVertexAttribArray(__glp.vert_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, __glp.quadvbo);
    glVertexAttribPointer(__glp.vert_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(__glp.uv_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, __glp.texvbo);
    glVertexAttribPointer(__glp.uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUniform1i(__glp.texture_uniform, 0);

    for (int n = 0; n < strlen(text); n++) {

        int c = (int)text[n];
        float cx = c % 16;
        float cy = (int)(c / 16.0);
        cy = cy * (1. / 16);
        cx = cx * (1. / 16);

        glUniformMatrix4fv(__glp.opm_uniform, 1, GL_FALSE, (GLfloat *) & __glp.otm);
        glUniform1f(__glp.cx_uniform, cx);
        glUniform1f(__glp.cy_uniform, cy);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        kmMat4Translation(&__glp.t, 16, 0, 0);
        kmMat4Multiply(&__glp.otm, &__glp.otm, &__glp.t);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisableVertexAttribArray(__glp.uv_attrib);
    glDisableVertexAttribArray(__glp.vert_attrib);

}


struct {  // blob of globals for the sprite routine
    kmMat4 opm, otm, t, r;
    GLuint spriteProg, opm_uniform;
    GLuint texture_uniform, cx_uniform, cy_uniform, u_size;
    GLuint vert_attrib, uv_attrib;
    GLuint quadvbo, texvbo;
} __spr;



void initSprite(int w, int h)
{
    const GLfloat quadVertices[] = {
        -.5,	-.5,	0,
        .5,  	 .5,	0,
        .5,	-.5,	0,

        .5,  	 .5,	0,
        -.5,	-.5,	0,
        -.5,	 .5,	0
    };

    const GLfloat texCoord[] = {
        0,		0,
        1.,		1,
        1.,		0,
        1.,		1.,
        0,		0,
        0,		1.
    };


    kmMat4OrthographicProjection(&__spr.opm, 0, w, h, 0, -10, 10); // support layers ?

    GLuint vs, fs;
    vs = create_shader("resources/shaders/sprite.vert", GL_VERTEX_SHADER);
    fs = create_shader("resources/shaders/sprite.frag", GL_FRAGMENT_SHADER);

    __spr.spriteProg = glCreateProgram();
    glAttachShader(__spr.spriteProg, vs);
    glAttachShader(__spr.spriteProg, fs);
    glLinkProgram(__spr.spriteProg);
    int link_ok;
    glGetProgramiv(__spr.spriteProg, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        printf("glLinkProgram:");
        print_log(__spr.spriteProg);
        printf("\n");
    }

    __spr.u_size = getShaderLocation(shaderUniform, __spr.spriteProg, "u_size");
    __spr.opm_uniform =
        getShaderLocation(shaderUniform, __spr.spriteProg, "opm_uniform");
    __spr.texture_uniform =
        getShaderLocation(shaderUniform, __spr.spriteProg, "texture_uniform");

    __spr.vert_attrib = getShaderLocation(shaderAttrib, __spr.spriteProg, "vert_attrib");
    __spr.uv_attrib = getShaderLocation(shaderAttrib, __spr.spriteProg, "uv_attrib");


    glGenBuffers(1, &__spr.quadvbo);
    glBindBuffer(GL_ARRAY_BUFFER, __spr.quadvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6, quadVertices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &__spr.texvbo);
    glBindBuffer(GL_ARRAY_BUFFER, __spr.texvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 6, texCoord,
                 GL_STATIC_DRAW);

}

void drawSprite(float x, float y, float w, float h, float a, int tex)
{


    glUseProgram(__spr.spriteProg);
    kmMat4Assign(&__spr.otm, &__spr.opm);
    kmMat4Translation(&__spr.t, x, y, -1); // support z layers?
    kmMat4RotationZ(&__spr.r,a);
    kmMat4Multiply(&__spr.t,&__spr.t,&__spr.r);
    kmMat4Multiply(&__spr.otm, &__spr.otm, &__spr.t);

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, tex);

    glEnableVertexAttribArray(__spr.vert_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, __spr.quadvbo);
    glVertexAttribPointer(__spr.vert_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(__spr.uv_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, __spr.texvbo);
    glVertexAttribPointer(__spr.uv_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUniform1i(__spr.texture_uniform, 0);
    glUniform2f(__spr.u_size, w,h);


    glUniformMatrix4fv(__spr.opm_uniform, 1, GL_FALSE, (GLfloat *) & __spr.otm);

    glDrawArrays(GL_TRIANGLES, 0, 6);


    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisableVertexAttribArray(__glp.uv_attrib);
    glDisableVertexAttribArray(__glp.vert_attrib);

}





int makeContext()
{
    makeNativeWindow();	// sets global pointers for win and disp

    EGLint majorVersion;
    EGLint minorVersion;

    // most egl you can sends NULLS for maj/min but not RPi
    if (!eglInitialize(__egl_display, &majorVersion, &minorVersion)) {
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
    if (!eglChooseConfig(__egl_display, attr, &ecfg, 1, &num_config)) {
        //cerr << "Failed to choose config (eglError: " << eglGetError() << ")" << endl;
        printf("failed to choose config eglerror:%i\n", eglGetError());	// todo change error number to text error
        return 1;
    }

    if (num_config != 1) {
        printf("Didn't get exactly one config, but %i\n", num_config);
        return 1;
    }

    __egl_surface = eglCreateWindowSurface(__egl_display, ecfg, __win, NULL);

    if (__egl_surface == EGL_NO_SURFACE) {
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
    __egl_context =
        eglCreateContext(__egl_display, ecfg, EGL_NO_CONTEXT, ctxattr);
    if (__egl_context == EGL_NO_CONTEXT) {
        //cerr << "Unable to create EGL context (eglError: " << eglGetError() << ")" << endl;
        printf("unable to create EGL context eglerror:%i\n",
               eglGetError());
        return 1;
    }
    //// associate the egl-context with the egl-surface
    eglMakeCurrent(__egl_display, __egl_surface, __egl_surface, __egl_context);

    return 0;
}


extern int __key_fd;

void closeContext()
{
    eglDestroyContext(__egl_display, __egl_context);
    eglDestroySurface(__egl_display, __egl_surface);
    eglTerminate(__egl_display);

    closeNativeWindow();

#ifdef __FOR_RPi_noX__
    if (__mouse_fd!=-1) {
        close(__mouse_fd);
    }
#endif

    close(__key_fd);


}





struct __pointGlobs {
    int Partprogram,part_mvp_uniform,part_tex_attrib;
    int part_tex_uniform,part_vert_attrib;
    int part_size_uniform;
} __pg;


void drawPointCloud(struct pointCloud_t* pntC, kmMat4* mat) {

    glUseProgram(__pg.Partprogram);
    glUniformMatrix4fv(__pg.part_mvp_uniform, 1, GL_FALSE, (GLfloat *) mat);
    glUniform1i(__pg.part_tex_uniform, 0);

// least sucky depth fudge!
    glEnable(GL_POINTS);
//    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glEnableVertexAttribArray(__pg.part_vert_attrib);
    glBindBuffer(GL_ARRAY_BUFFER, pntC->vertBuf);
    glBufferSubData(GL_ARRAY_BUFFER, 0,sizeof(float)*3*pntC->totalPoints, pntC->pos);
    glVertexAttribPointer(__pg.part_vert_attrib,3,GL_FLOAT,GL_FALSE,0,0);
    glDrawArrays(GL_POINTS,0,pntC->totalPoints);
    glDisableVertexAttribArray(pntC->vertBuf);
    glDisable(GL_POINTS);
    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);



}


struct pointCloud_t* createPointCloud(int size) {

    struct pointCloud_t* pntC=malloc(sizeof(struct pointCloud_t));
    pntC->totalPoints=size;
    pntC->pos=malloc(size*sizeof(float)*3);
    pntC->vel=malloc(size*sizeof(float)*3);


    glGenBuffers(1, &pntC->vertBuf);
    glBindBuffer(GL_ARRAY_BUFFER, pntC->vertBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * pntC->totalPoints, pntC->pos,
                 GL_DYNAMIC_DRAW);

    return pntC;
}

void freePointCloud(struct pointCloud_t* pntC) {
    free(pntC->pos);
    free(pntC->vel);
    free(pntC);
}

void initPointClouds(const char* vertS, const char* fragS, float pntSize) {

    GLint link_ok = GL_FALSE;

    GLuint vs, fs;
    vs = create_shader(vertS, GL_VERTEX_SHADER);
    fs = create_shader(fragS, GL_FRAGMENT_SHADER);
    //printf("vs=%i fs=%i\n",vs,fs);
    __pg.Partprogram = glCreateProgram();
    glAttachShader(__pg.Partprogram, vs);
    glAttachShader(__pg.Partprogram, fs);
    glLinkProgram(__pg.Partprogram);
    glGetProgramiv(__pg.Partprogram, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        printf("particle glLinkProgram error \n");
        print_log(__pg.Partprogram);
        //    return 0;
    }

    __pg.part_vert_attrib =
        getShaderLocation(shaderAttrib, __pg.Partprogram, "vertex_attrib");
    __pg.part_mvp_uniform =
        getShaderLocation(shaderUniform, __pg.Partprogram, "mvp_uniform");
    __pg.part_tex_uniform =
        getShaderLocation(shaderUniform, __pg.Partprogram, "u_texture");

    __pg.part_size_uniform =
        getShaderLocation(shaderUniform, __pg.Partprogram, "u_point_size");
    glUseProgram(__pg.Partprogram);
    glUniform1f(__pg.part_size_uniform, pntSize);

}
