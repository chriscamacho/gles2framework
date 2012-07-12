#include <stdbool.h>
#include <stdio.h> // sprintf
#include <stdlib.h>  // malloc
#include <math.h>
#include <fcntl.h>  // open fcntl
#include <unistd.h> // read close 
#include <linux/joystick.h>

#ifndef __FOR_RPi_noX__

	#include  <X11/Xlib.h>
	#include  <X11/Xatom.h>
	#include  <X11/Xutil.h>

#endif //NOT  __FOR_RPi_noX__

#ifdef __FOR_RPi_noX__ 

	// sudo kbd_mode -a - on RPi if you have to kill it...

	#include "linux/kd.h"	// keyboard stuff...
	#include "termios.h"
	#include "sys/ioctl.h"

	static struct termios tty_attr_old;
	static int old_keyboard_mode;

	int __mouse_fd=-1;    

#endif

#include "input.h"



bool __keys[256];
int __mouse[3];
bool __rel_mouse;



void doEvents()
{

#ifndef __FOR_RPi_noX__
	
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
    
#endif // NOT  __FOR_RPi_noX__


#ifdef __FOR_RPi_noX__


    char buf[1];
    int res;

    res = read(0, &buf[0], 1);
    while (res >= 0) {
		if (buf[0] & 0x80) {
			//printf("key %i released\n",(buf[0]&~0x80));
			__keys[buf[0]&~0x80]=false;
		} else {
			//printf("key %i pressed\n",(buf[0]&~0x80));
			__keys[buf[0]&~0x80]=true;

		}
		res = read(0, &buf[0], 1);
    }
     
    if (__rel_mouse) {
    	__mouse[0]=0;__mouse[1]=0;	
    }
    
    if(__mouse_fd>0) {
    	signed char mbuf[3];
    	int mres;
    	mres=read(__mouse_fd,&mbuf[0],3);
    	while(mres>=0){
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


}

void setMouseRelative(bool mode) {
	__rel_mouse=mode;
}

int *getMouse()
{
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

    return &__mouse[0];
}

bool *getKeys()
{

#ifdef  __FOR_RPi_noX__

    struct termios tty_attr;
    int flags;

    /* make stdin non-blocking */
    flags = fcntl(0, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(0, F_SETFL, flags);

    /* save old keyboard mode */
    if (ioctl(0, KDGKBMODE, &old_keyboard_mode) < 0) {
	//return 0;
		printf("couldn't get the keyboard, are you running via ssh?\n");
		printf("keyboard routines do not work as expected over ssh...\n");
    }

    tcgetattr(0, &tty_attr_old);

    /* turn off buffering, echo and key processing */
    tty_attr = tty_attr_old;
    tty_attr.c_lflag &= ~(ICANON | ECHO | ISIG);
    tty_attr.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF);
    tcsetattr(0, TCSANOW, &tty_attr);

    ioctl(0, KDSKBMODE, K_RAW);
    
#endif  //__FOR_RPi_noX__


    return &__keys[0];
}

struct joystick_t *getJoystick(int j) {

	char devpath[20];
	sprintf(devpath,"/dev/input/js%i\0",j);
	
	struct joystick_t *js=malloc(sizeof(struct joystick_t));
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
}

void updateJoystick(struct joystick_t *js) {
	struct js_event jse;
	int jres;
	jres=read(js->fd,&jse,sizeof(struct js_event));
	while(jres>=0){
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
	
}

void releaseJoystick(struct joystick_t *js) {

	close(js->fd);
	free(js);
	
}

