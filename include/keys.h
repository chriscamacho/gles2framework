#ifdef __FOR_RPi__

#define KEY_ESC			9
#define KEY_ONE			10
#define KEY_TWO			11
#define KEY_THREE		12
#define KEY_FOUR		13
#define KEY_FIVE		14
#define KEY_SIX			15
#define KEY_SEVEN		16
#define KEY_EIGHT		17
#define KEY_NINE		18
#define KEY_ZERO		19
#define KEY_MINUS		20

#define KEY_TAB 		23
#define KEY_Q			24
#define KEY_W			25
#define KEY_E			26
#define KEY_R			27
#define KEY_T			28
#define KEY_Y			29
#define KEY_U			30
#define KEY_I			31
#define KEY_O			32
#define KEY_P			33
#define KEY_LBRACKET	34
#define KEY_RBRACKET	35
#define KEY_RETURN		36
#define KEY_LCTRL		37
#define KEY_A			38
#define KEY_S			39
#define KEY_D			40
#define KEY_F			41
#define KEY_G			42
#define KEY_H			43
#define KEY_J			44
#define KEY_K			45
#define KEY_L			46
#define KEY_SEMICOLON	47
#define KEY_APOST		48
#define KEY_BACKTICK	49
#define KEY_LSHIFT		50
#define KEY_HASH		51
#define KEY_Z			52
#define KEY_X			53
#define KEY_C			54
#define KEY_V			55
#define KEY_B			56
#define KEY_N			57
#define KEY_M			58
#define KEY_COMMA		59
#define KEY_PERIOD		60
#define KEY_BSLASH		61
#define KEY_RSHIFT		62
#define KEY_NUMMULT		63
#define KEY_LALT		64
#define KEY_SPACE		65
#define KEY_CAPS		66
#define KEY_F1			67
#define KEY_F2			68
#define KEY_F3			69
#define KEY_F4			70
#define KEY_F5			71
#define KEY_F6			72
#define KEY_F7			73
#define KEY_F8			74
#define KEY_F9			75
#define KEY_F10			76
#define KEY_NUMLOCK		77
#define KEY_SCLOCK		78
#define KEY_NUM7		79
#define KEY_NUM8		80
#define KEY_NUM9		81
#define KEY_NUMMINUS	82
#define KEY_NUM4		83
#define KEY_NUM5		84
#define KEY_NUM6		85
#define KEY_NUMPLUS		86
#define KEY_NUM1		87
#define KEY_NUM2		88
#define KEY_NUM3		89
#define KEY_NUMZERO		90
#define KEY_NUMPERIOD	91

#define KEY_FSLASH		94
#define KEY_F11			95
#define KEY_F12			96

#define KEY_NUMENTER	104
#define KEY_RCTRL		105
#define KEY_NUMSLASH	106
#define KEY_SYSRQ		107
#define KEY_ALTGR		108

#define KEY_HOME		110

#define KEY_PGUP		112

#define KEY_END			115

#define KEY_PGDOWN		117
#define KEY_INSERT		118
#define KEY_DELETE		119

#define KEY_BREAK		127

#define KEY_LMETA		133
#define KEY_RMETA		134
#define KEY_MENU		135

#define KEY_CURSL		113
#define KEY_CURSR		114
#define KEY_CURSU		111
#define KEY_CURSD		116

#endif

#ifdef __FOR_RPi_noX__

#define KEY_ESC			1
#define KEY_ONE			2
#define KEY_TWO			3
#define KEY_THREE		4
#define KEY_FOUR		5
#define KEY_FIVE		6
#define KEY_SIX			7
#define KEY_SEVEN		8
#define KEY_EIGHT		9
#define KEY_NINE		10
#define KEY_ZERO		11
#define KEY_MINUS		12

#define KEY_TAB 		15
#define KEY_Q			16
#define KEY_W			17
#define KEY_E			18
#define KEY_R			19
#define KEY_T			20
#define KEY_Y			21
#define KEY_U			22
#define KEY_I			23
#define KEY_O			24
#define KEY_P			25
#define KEY_LBRACKET	26
#define KEY_RBRACKET	27
#define KEY_RETURN		28
#define KEY_LCTRL		29
#define KEY_A			30
#define KEY_S			31
#define KEY_D			32
#define KEY_F			33
#define KEY_G			34
#define KEY_H			35
#define KEY_J			36
#define KEY_K			37
#define KEY_L			38
#define KEY_SEMICOLON	39
#define KEY_APOST		40
#define KEY_BACKTICK	41
#define KEY_LSHIFT		42
#define KEY_HASH		43
#define KEY_Z			44
#define KEY_X			45
#define KEY_C			46
#define KEY_V			47
#define KEY_B			48
#define KEY_N			49
#define KEY_M			50
#define KEY_COMMA		51
#define KEY_PERIOD		52
#define KEY_BSLASH		52
#define KEY_RSHIFT		54
#define KEY_NUMMULT		
#define KEY_LALT		56
#define KEY_SPACE		57
#define KEY_CAPS		58
#define KEY_F1			59
#define KEY_F2			60
#define KEY_F3			61
#define KEY_F4			62
#define KEY_F5			63
#define KEY_F6			64
#define KEY_F7			65
#define KEY_F8			66
#define KEY_F9			67
#define KEY_F10			68
#define KEY_NUMLOCK		69
#define KEY_SCLOCK		70
#define KEY_NUM7		// TODO my mini keyboard don't have these!
#define KEY_NUM8		
#define KEY_NUM9		
#define KEY_NUMMINUS	
#define KEY_NUM4		
#define KEY_NUM5		
#define KEY_NUM6		
#define KEY_NUMPLUS		
#define KEY_NUM1		
#define KEY_NUM2		
#define KEY_NUM3		
#define KEY_NUMZERO		
#define KEY_NUMPERIOD	

#define KEY_FSLASH		86
#define KEY_F11			87
#define KEY_F12			88

#define KEY_NUMENTER	
#define KEY_RCTRL		
#define KEY_NUMSLASH	
#define KEY_SYSRQ		99
#define KEY_ALTGR		100

#define KEY_HOME		

#define KEY_PGUP		

#define KEY_END			

#define KEY_PGDOWN		
#define KEY_INSERT		
#define KEY_DELETE		111

#define KEY_BREAK		119

#define KEY_LMETA		125
#define KEY_RMETA		
#define KEY_MENU		127

#define KEY_CURSL		105
#define KEY_CURSR		106
#define KEY_CURSU		103
#define KEY_CURSD		108



#endif


#ifdef __FOR_GLFW__

#include <GL/glfw.h>

#define KEY_ESC			GLFW_KEY_ESC
#define KEY_ONE			'1'
#define KEY_TWO			'2'
#define KEY_THREE		'3'
#define KEY_FOUR		'4'
#define KEY_FIVE		'5'
#define KEY_SIX			'6'
#define KEY_SEVEN		'7'
#define KEY_EIGHT		'8'
#define KEY_NINE		'9'
#define KEY_ZERO		'0'
#define KEY_MINUS		'-'

#define KEY_TAB 		GLFW_KEY_TAB
#define KEY_Q			'Q'
#define KEY_W			'W'
#define KEY_E			'E'
#define KEY_R			'R'
#define KEY_T			'T'
#define KEY_Y			'Y'
#define KEY_U			'U'
#define KEY_I			'I'
#define KEY_O			'O'
#define KEY_P			'P'
#define KEY_LBRACKET	'['
#define KEY_RBRACKET	']'
#define KEY_RETURN		GLFW_KEY_ENTER
#define KEY_LCTRL		GLFW_KEY_LCTRL
#define KEY_A			'A'
#define KEY_S			'S'
#define KEY_D			'D'
#define KEY_F			'F'
#define KEY_G			'G'
#define KEY_H			'H'
#define KEY_J			'J'
#define KEY_K			'K'
#define KEY_L			'L'
#define KEY_SEMICOLON	';'
#define KEY_APOST		'\''
#define KEY_BACKTICK	'`'
#define KEY_LSHIFT		GLFW_KEY_LSHIFT
#define KEY_HASH		GLFW_KEY_LAST   /*correct?*/
#define KEY_Z			'Z'
#define KEY_X			'X'
#define KEY_C			'C'
#define KEY_V			'V'
#define KEY_B			'B'
#define KEY_N			'N'
#define KEY_M			'M'
#define KEY_COMMA		','
#define KEY_PERIOD		'.'
#define KEY_BSLASH		'\\'
#define KEY_RSHIFT		GLFW_KEY_RSHIFT
#define KEY_NUMMULT		GLFW_KEY_KP_MULTIPLY
#define KEY_LALT		GLFW_KEY_LALT
#define KEY_SPACE		GLFW_KEY_SPACE
#define KEY_CAPS		GLFW_KEY_CAPS_LOCK
#define KEY_F1			GLFW_KEY_F1
#define KEY_F2			GLFW_KEY_F2
#define KEY_F3			GLFW_KEY_F3
#define KEY_F4			GLFW_KEY_F4
#define KEY_F5			GLFW_KEY_F5
#define KEY_F6			GLFW_KEY_F6
#define KEY_F7			GLFW_KEY_F7
#define KEY_F8			GLFW_KEY_F8
#define KEY_F9			GLFW_KEY_F9
#define KEY_F10			GLFW_KEY_F10
#define KEY_NUMLOCK		GLFW_KEY_KP_NUM_LOCK
#define KEY_SCLOCK		GLFW_KEY_SCROLL_LOCK
#define KEY_NUM7		GLFW_KEY_KP_7
#define KEY_NUM8		GLFW_KEY_KP_8
#define KEY_NUM9		GLFW_KEY_KP_9
#define KEY_NUMMINUS	GLFW_KEY_KP_SUBTRACT
#define KEY_NUM4		GLFW_KEY_KP_4
#define KEY_NUM5		GLFW_KEY_KP_5
#define KEY_NUM6		GLFW_KEY_KP_6
#define KEY_NUMPLUS		GLFW_KEY_KP_ADD
#define KEY_NUM1		GLFW_KEY_KP_1
#define KEY_NUM2		GLFW_KEY_KP_2
#define KEY_NUM3		GLFW_KEY_KP_3
#define KEY_NUMZERO		GLFW_KEY_KP_0
#define KEY_NUMPERIOD	GLFW_KEY_KP_DECIMAL

#define KEY_FSLASH		'/'
#define KEY_F11			GLFW_KEY_F11
#define KEY_F12			GLFW_KEY_F12

#define KEY_NUMENTER	GLFW_KEY_KP_ENTER
#define KEY_RCTRL		GLFW_KEY_RCTRL
#define KEY_NUMSLASH	GLFW_KEY_KP_DIVIDE
#define KEY_SYSRQ		GLFW_KEY_UNKNOWN    /*correct?*/
#define KEY_ALTGR		GLFW_KEY_SPECIAL    /*correct?*/

#define KEY_HOME		GLFW_KEY_HOME

#define KEY_PGUP		GLFW_KEY_PAGEUP

#define KEY_END			GLFW_KEY_END

#define KEY_PGDOWN		GLFW_KEY_PAGEDOWN
#define KEY_INSERT		GLFW_KEY_INSERT
#define KEY_DELETE		GLFW_KEY_DEL

#define KEY_BREAK		GLFW_KEY_PAUSE

#define KEY_LMETA		GLFW_KEY_LSUPER   /*correct?*/
#define KEY_RMETA		GLFW_KEY_RSUPER   /*correct?*/
#define KEY_MENU		GLFW_KEY_MENU

#define KEY_CURSL		GLFW_KEY_LEFT
#define KEY_CURSR		GLFW_KEY_RIGHT
#define KEY_CURSU		GLFW_KEY_UP
#define KEY_CURSD		GLFW_KEY_DOWN

#endif