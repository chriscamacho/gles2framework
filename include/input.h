
extern int __display_width,__display_height;

#ifdef __FOR_XORG__

	extern Window __win, __eventWin;
	extern Display *__x_display;

#endif

void doEvents();
int *getMouse();
bool *getKeys();
void setMouseRelative(bool mode);

struct joystick_t {
	int fd;
	signed short axis[8]; // could have 2 analogue sticks with 2 digital hats
	long buttons;
};

struct joystick_t *getJoystick(int j);
void updateJoystick(struct joystick_t *js);
void releaseJoystick(struct joystick_t *js);

