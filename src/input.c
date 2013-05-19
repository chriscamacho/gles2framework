#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stdio.h> // sprintf
#include <stdlib.h>  // malloc
#include <math.h>

#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
#include <fcntl.h>  // open fcntl
#include <unistd.h> // read close 
#include <linux/joystick.h>

#ifdef __FOR_RPi__

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>
extern Display *__x_display;
extern Window __eventWin;

#endif // __FOR_RPi__

#include <dirent.h>  // scandir
#include <string.h> // strlen

#ifdef __FOR_RPi_noX__

// sudo kbd_mode -a - on RPi if you have to kill it...

#include "linux/kd.h"	// keyboard stuff...
#include "termios.h"
#include "sys/ioctl.h"

#endif

static struct termios tty_attr_old;
static int old_keyboard_mode;

int __mouse_fd=-1;

#endif

#ifdef __FOR_GLFW__
#include <GL/glfw.h>
#include <string.h>
#include "keys.h"
#endif

#include "input.h"


//glfw need more than 256 key for special keys
bool __keys[256+128];
int __mouse[3];
bool __rel_mouse;

int __key_fd=0; // defaults to 0 ie console

int __keyList[] = {
    KEY_ESC,
    KEY_ONE,
    KEY_TWO,
    KEY_THREE,
    KEY_FOUR,
    KEY_FIVE,
    KEY_SIX,
    KEY_SEVEN,
    KEY_EIGHT,
    KEY_NINE,
    KEY_ZERO,
    KEY_MINUS,
    KEY_TAB,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_LBRACKET,
    KEY_RBRACKET,
    KEY_RETURN,
    KEY_LCTRL,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_SEMICOLON,
    KEY_APOST,
    KEY_BACKTICK,
    KEY_LSHIFT,
    KEY_HASH,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_BSLASH,
    KEY_RSHIFT,
    KEY_NUMMULT,
    KEY_LALT,
    KEY_SPACE,
    KEY_CAPS,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_NUMLOCK,
    KEY_SCLOCK,
    KEY_NUM7,
    KEY_NUM8,
    KEY_NUM9,
    KEY_NUMMINUS,
    KEY_NUM4,
    KEY_NUM5,
    KEY_NUM6,
    KEY_NUMPLUS,
    KEY_NUM1,
    KEY_NUM2,
    KEY_NUM3,
    KEY_NUMZERO,
    KEY_NUMPERIOD,
    KEY_FSLASH,
    KEY_F11,
    KEY_F12,
    KEY_NUMENTER,
    KEY_RCTRL,
    KEY_NUMSLASH,
    KEY_SYSRQ,
    KEY_ALTGR,
    KEY_HOME,
    KEY_PGUP,
    KEY_END,
    KEY_PGDOWN,
    KEY_INSERT,
    KEY_DELETE,
    KEY_BREAK,
    KEY_LMETA,
    KEY_RMETA,
    KEY_MENU,
    KEY_CURSL,
    KEY_CURSR,
    KEY_CURSU,
    KEY_CURSD
};

#ifdef __FOR_RPi_noX__

void restoreKbd() {
    if (__key_fd==0) {
        tcsetattr(0, TCSAFLUSH, &tty_attr_old);
        //              ioctl(0, KDSKBMODE, old_keyboard_mode);
        ioctl(0, KDSKBMODE, K_XLATE);
    }
}

#endif


void doEvents()
{

#ifdef __FOR_RPi__

    XEvent event;

    if(__rel_mouse) {
        __mouse[0]=0;
        __mouse[1]=0;
    }

    while (XEventsQueued(__x_display, QueuedAfterReading)) {
        XNextEvent(__x_display, &event);
        switch (event.type) {

        case KeyPress:
            __keys[event.xkey.keycode & 0xff] = true;
            //printf("key=%i\n",event.xkey.keycode & 0xff);
            break;

        case KeyRelease:
            __keys[event.xkey.keycode & 0xff] = false;
            break;

        case MotionNotify:
            if (__rel_mouse) {
                __mouse[0] = event.xbutton.x-(__display_width/2.0);
                __mouse[1] = event.xbutton.y-(__display_height/2.0);
            } else {
                __mouse[0] = event.xbutton.x;
                __mouse[1] = event.xbutton.y;
            }
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

        if(__rel_mouse) {  // rel mode test here

            XSelectInput(__x_display, __eventWin, NoEventMask);
            XWarpPointer(__x_display, None, __eventWin, 0, 0, 0, 0, __display_width/2.0, __display_height/2.0);
            XFlush(__x_display);
            XSelectInput(__x_display, __eventWin, ExposureMask |
                         KeyPressMask | KeyReleaseMask |
                         ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
            XFlush(__x_display);

        }
    }

#endif // __FOR_RPi__


#ifdef __FOR_RPi_noX__

    if (__key_fd==0) {
        char buf[1];
        int res;

        res = read(__key_fd, &buf[0], 1);
        while (res >= 0) {
            //printf("keyboard %i\n",buf[0]);
            if (buf[0] & 0x80) {
                //printf("key %i released\n",(buf[0]&~0x80));
                __keys[buf[0]&~0x80]=false;
            } else {
                //printf("key %i pressed\n",(buf[0]&~0x80));
                __keys[buf[0]&~0x80]=true;

            }
            res = read(__key_fd, &buf[0], 1);
        }

    } else {

        struct input_event ev;
        int res;
        res = read(__key_fd, &ev,sizeof(struct input_event));
        while (res>=0) {
            //printf(" %i %i %i\n",ev.type,ev.code,ev.value);
            // should probably handle MSC and SYN as well - meh
            if (ev.type==EV_KEY) {
                if (ev.value==1) {
                    __keys[ev.code & 0xff]=true;
                } else if (ev.value==0) {
                    __keys[ev.code & 0xff]=false;
                }
            }
            res = read(__key_fd, &ev,sizeof(struct input_event));
        }


    }

    if (__rel_mouse) {
        __mouse[0]=0;
        __mouse[1]=0;
    }

    if(__mouse_fd>0) {
        signed char mbuf[3];
        int mres;
        mres=read(__mouse_fd,&mbuf[0],3);
        while(mres>=0) {
            //printf("%i %i %i\n",mbuf[0]&7,mbuf[1],mbuf[2]);
            __mouse[2]=mbuf[0]&7;
            if (__rel_mouse) {
                __mouse[0]=mbuf[1];
                __mouse[1]=-mbuf[2];
            } else {
                __mouse[0]=__mouse[0]+mbuf[1];
                __mouse[1]=__mouse[1]-mbuf[2];
                if (__mouse[0]<0) __mouse[0]=0;
                if (__mouse[1]<0) __mouse[1]=0;
                if (__mouse[0]>__display_width) __mouse[0]=__display_width;
                if (__mouse[1]>__display_height) __mouse[1]=__display_height;
            }

            mres=read(__mouse_fd,&mbuf[0],3);
        }

    }

#endif  // __FOR_RPi_noX__

#ifdef __FOR_GLFW__
    memset(__keys, GLFW_RELEASE, sizeof(__keys) / sizeof(__keys[0]));
    int keycount = sizeof(__keyList) / sizeof(__keyList[0]);
    for(int i = 0 ; i < keycount ; ++i)
    {
        int key = __keyList[i];
        if(glfwGetKey(key) == GLFW_PRESS)
        {
            __keys[key] = true;
        }
    }

    glfwGetMousePos(&__mouse[0], &__mouse[1]);
    __mouse[2] = 0;
    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        __mouse[2] |= 0x01;
    }
    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        __mouse[2] |= 0x02;
    }
    if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        __mouse[2] |= 0x04;
    }
#endif
}

void setMouseRelative(bool mode) {
    __rel_mouse=mode;
}

int *getMouse()
{
#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
    __rel_mouse=false;

#ifdef  __FOR_RPi_noX__
    __mouse_fd = open("/dev/input/mouse0", O_RDONLY);
    if (__mouse_fd < 0) {
        printf("open failed\n");
    } else {
        // make none blocking
        int flags = fcntl(__mouse_fd, F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(__mouse_fd, F_SETFL, flags);
    }

#endif //  __FOR_RPi_noX__
#endif

    return &__mouse[0];
}

static int __dsort (const struct dirent **a,const struct dirent **b) {
    return 1; // dummy sort
}

#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
static int __dfilter(const struct dirent *d) {
    if (d->d_type==DT_DIR) return 0;
    int i=0;
    i=strlen(d->d_name)-1;
    //printf ("%i %c %c %c \n",d->d_type,d->d_name[i-2],d->d_name[i-1],d->d_name[i]);
    // allegedly usb keyboard symlink *always* ends kbd
    if (d->d_name[i-2]=='k'  & d->d_name[i-1]=='b'  & d->d_name[i]=='d'  ) return 1;
    return 0;
}
#endif


bool *getKeys()
{
#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
    struct dirent **eps;
    int n;

    n = scandir ("/dev/input/by-path/", &eps, __dfilter, __dsort);
//    if (n >= 0)
    if(n >= 0 && eps != 0 && eps[0] != 0) {
        // only check 1st usb keyboard....
        char fn[256];
        sprintf(fn,"/dev/input/by-path/%s\0",eps[0]->d_name);
        __key_fd=open(fn, O_RDONLY);
        printf("%i  %s\n",__key_fd,fn);
    }

    if (__key_fd==-1) __key_fd=0; // on usb evdev failure default to console

    int flags;
    flags = fcntl(__key_fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(__key_fd, F_SETFL, flags);

#ifdef  __FOR_RPi_noX__



    if (__key_fd==0) {
        struct termios tty_attr;




        /* save old keyboard mode */
        if (ioctl(__key_fd, KDGKBMODE, &old_keyboard_mode) < 0) {
            //return 0;
            printf("couldn't get the keyboard, are you running via ssh\n");
            printf("or without setting evdev permissions? \n");
        }

        tcgetattr(__key_fd, &tty_attr_old);

        /* turn off buffering, echo and key processing */
        tty_attr = tty_attr_old;
        tty_attr.c_lflag &= ~(ICANON | ECHO | ISIG);
        tty_attr.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF);
        tcsetattr(__key_fd, TCSANOW, &tty_attr);

        ioctl(__key_fd, KDSKBMODE, K_RAW);
    }
#endif  //__FOR_RPi_noX__
#endif

    return &__keys[0];
}

struct joystick_t *getJoystick(int j) {
#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
    char devpath[20];
    sprintf(devpath,"/dev/input/js%i\0",j);

    struct joystick_t *js=(struct joystick_t*)malloc(sizeof(struct joystick_t));
    js->fd = open(devpath, O_RDONLY);
    if (js->fd < 0) {
        printf("joystick %i open failed\n",j);
    } else {
        // make none blocking
        int flags = fcntl(js->fd, F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(js->fd, F_SETFL, flags);
    }
    js->buttons=0;
    return js;
#endif

#ifdef __FOR_GLFW__
    //TODO
    return NULL;
#endif
}

void updateJoystick(struct joystick_t *js) {
#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
    struct js_event jse;
    int jres;
    jres=read(js->fd,&jse,sizeof(struct js_event));
    while(jres>=0) {
        jse.type &= ~JS_EVENT_INIT; // mask out synthetic event flag

        if (jse.type == JS_EVENT_AXIS) {
            if (jse.number<8) {
                js->axis[jse.number]=jse.value;
            }
        }

        if (jse.type == JS_EVENT_BUTTON) {

            if (jse.value==1) {
                js->buttons = js->buttons | 1<<jse.number;
            } else {
                js->buttons = js->buttons & ~(1<<jse.number);
            }
        }


        jres=read(js->fd,&jse,sizeof(struct js_event));
    }
#endif

#ifdef __FOR_GLFW__
    //TODO
#endif
}

void releaseJoystick(struct joystick_t *js) {
#if (defined(__FOR_RPi_noX__) || defined(__FOR_RPi__))
    close(js->fd);
    free(js);
#endif
}

