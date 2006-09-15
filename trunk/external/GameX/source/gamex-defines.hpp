//
// GameX - Definition Header - Constants used by GameX
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
// 

#ifndef GAMEX_DEFINES_INCLUDED

	#define GAMEX_DEFINES_INCLUDED

	#define DEGtoRAD (3.1415926538f/180.0f)

	// *** Constants for initializing GameX:

	// these are combinable flags to be used with GameX.Initialize()
	enum _GAMEX_INIT_FLAGS {
		// video flags:
		VIDEO_16BIT 			  = 0x00002000l, // run game in 16 bit color mode -- supported, currently fastest and most common mode
		VIDEO_FULLSCREEN		  = 0x00000001l, // run in fullscreen mode (at a resolution that best fits the requested viewport size)
		VIDEO_WINDOWED			  = 0x00000002l, // run in windowed mode -- if neither VIDEO_FULLSCREEN nor VIDEO_WINDOWED is specified, the user will be prompted as to whether to run in fullscreen mode when the game starts
		// other flags:
		RUN_USEESCAPEKEY		  = 0x01000000l, // allow the game to detect the escape key instead of escape automatically quitting
		RUN_AUTOSHOWINFO		  = 0x02000000l, // display debugging information (such as frame rate) in the title bar
		RUN_ALWAYSKEEPCURSOR	  = 0x04000000l, // prevent GameX from hiding the cursor in fullscreen mode -- note that a visible cursor over the game window can reduce the game's framerate considerably, it's better to draw your own cursor in the game if you need it
		RUN_ALWAYSHIDECURSOR	  = 0x08000000l, // allow GameX to always hide the cursor when it is over the game
		RUN_NOCONFIRMQUIT		  = 0x10000000l, // disallow GameX from asking whether the user is sure about quitting
		// video flags for advanced users:
		VIDEO_LOWRES			  = 0x00000004l, // Tells GameX to use low-resolution images to save memory/speed
		VIDEO_NORESIZE			  = 0x00000008l, // GameX automatically handles window resizing and maximizing if this flag is not specified.
		VIDEO_ALLOWREFRESHSYNC	  = 0x00000010l, // sync screen updates to monitor refresh rate -- attempts to prevent visual tearing artifacts at minimal cost of CPU time -- note that this limits the framerate to the current monitor's refresh rate (which is generally a good thing)
		VIDEO_SLOWDEBUGDRAWING	  = 0x00000100l, // use this to cause GameX to draw things REALLY slowly so you can see what's happening inside individual frames
		VIDEO_SOFTWAREONLY		  = 0x00000200l, // use this to force all D3D operations to use software instead of any available hardware -- usually a bad idea, unless using an outdated video card which doesn't fully support Direct3D
		VIDEO_24BIT 			  = 0x00004000l, // (NOT YET TESTED) run game in 24 bit color mode -- externally it's the same thing as 32 bit color mode -- untested for now
		VIDEO_32BIT 			  = 0x00008000l, // (MOSTLY IMPLEMENTED) run game in 32 bit color mode -- supported but may be slow depending on video card
		// audio flags for advanced users:
		AUDIO_DISABLE			  = 0x00010000l, // disable all audio support, so sounds and music will not play -- only possible use is to mute a game when debugging to make debugging easier or something
		// other flags for advanced users:
		RUN_NOMOUSEINPUT		  = 0x20000000l, // prevent GameX from keeping track of mouse movement and clicks
		RUN_BACKGROUND			  = 0x40000000l, // allow game to keep running even in the background by preventing GameX from automatically pausing the game -- possible uses are for networked games or cheat prevention, maybe
		RUN_NOALTENTERTOGGLE	  = 0x80000000l, // allow the player to hit alt-enter without toggling between fullscreen/windowed mode
	};
	typedef long GameXInitFlags;


	// *** Constants for drawing with GameX:

	// constants to be used alone or combined with | and used as flags whenever drawing images
	// for example, you can combine the DRAW_PLAIN and DRAWOP_CENTERED draw flags
	// like so:  GameX.DrawImage(DRAW_PLAIN | DRAWOP_CENTERED, &ship_img, x,y);
	enum _DRAW_FLAGS {
	// Draw Mode flags: (not to be combined with each other)
	// These determine the main way of drawing the image (copy the image directly, or add it onto the destination for a glow effect, etc.)
	// Exactly one Draw Mode flag must be specified whenever drawing an image.
		DRAW_PLAIN	       = 0x001l, // DEST = SOURCE -- draw image normally -- can still be combined with options and color/alpha blending (makes use of alpha layer if image has one)
		DRAW_NORMAL    = DRAW_PLAIN, // backward compatibility
		DRAW_ADD		   = 0x002l, // DEST = DEST + SOURCE -- draw additively using r,g,b intensity if given (multiplies by alpha layer if image has one, although alpha layer is NOT necessary for ADDED images and may slow things down) (alpha arg < 255 scales down intensity)
		DRAW_ADDED		 = DRAW_ADD, // backward compatibility
		DRAW_SUBTRACT	   = 0x004l, // DEST = DEST - SOURCE -- draw subtractively using r,g,b intensity if given (alpha arg < 255 scales down intensity) -- not supported on older video cards
		DRAW_GHOST		   = 0x008l, // DEST = DEST*(1-SOURCE) + SOURCE*SOURCE -- draw blended using color-valued alpha and r,g,b values if given (a < 255 scales down intensity/opacity) i.e. a sort of auto-alpha effect
		DRAW_INVERT 	   = 0x010l, // DEST = DEST + SOURCE - 2*SOURCE*DEST -- draw by inverting destination proportionally to source colors (alpha < 255 scales down intensity) -- if instead of doing this you just want to invert the source image when drawing, that's what DRAWOP_INVERTED is for
		DRAW_BURN		   = 0x020l, // DEST = SOURCE - SOURCE*SOURCE - DEST*SOURCE -- draw by inverting destination proportionally to greyness of source colors -- may not be supported on older video cards
		DRAW_INTENSIFY	   = 0x040l, // DEST = DEST + 2*SOURCE*DEST -- draw additively, but add less as the destination becomes darker -- not supported on older video cards
		DRAW_ADDSOFT	   = 0x080l, // DEST = DEST + SOURCE - SOURCE*DEST -- similar to DRAW_ADD, but adds less to brighter areas (prevents overexposure)
		DRAW_ADDSHARP	   = 0x100l, // DEST = DEST + 4*SOURCE*SOURCE -- similar to DRAW_ADD, but colors show more sharply (increases contrast)
		DRAW_MULTIPLY	   = 0x200l, // DEST = DEST * SOURCE -- multiply destination by source -- probably not useful in most situations -- not supported on older video cards
		DRAWMODESMASK	   = 0x3FFl, // DO NOT SPECIFY THIS FLAG! -- this is a mask for all of the draw modes and is only to be used internally
		// (Looking for DRAW_ALPHA or DRAW_MASKED? They no longer exist. Instead, load the image with LOAD_ALPHA or LOAD_MASKED and draw with DRAW_PLAIN to achieve these effects.)

	// Draw Option flags: (to be combined with a Draw Mode flag or also with each other)
	// These let you specify extra effects that can be combined with each other which can complement any drawing mode above.
	// Zero or more of these flags are allowed whenever drawing images/boxes/backgrounds 
		DRAWOP_BRIGHT		    = 0x1000l, // draw image with 2X brightness, so r,g,b of 128 act like 255, and r,g,b of 255 act like 510 (which is normally out of range) -- try combining with various draw modes for interesting effects
		DRAWOP_INVERTED 	    = 0x2000l, // temporarily invert the image before drawing it -- try combining with various draw modes for interesting effects -- do not confuse this flag with the very different DRAW_INVERT flag
		DRAWOP_NOFILTER 	    = 0x4000l, // render the image without doing bilinear filtering, i.e. turns off automatic image smoothing
										   // note that this does NOT increase drawing speed, but sometimes images look cleaner when not smoothed.
		DRAWOP_NODITHER 	    = 0x8000l, // dithering give the impression that there are more colors than really available (usually with diffusion-type pixel dithering),
										   // i.e. this flag turns off automatic color smoothing
		DRAWOP_CENTERED 	   = 0x10000l, // makes point coordinates specify center instead of top-left
										   // useful for many objects whose positions are defined in relation to their center
		DRAWOP_HMIRROR		   = 0x20000l, // horizontally flip the image when drawing -- the flip happens before any scaling/rotation/vertex blending
		DRAWOP_VMIRROR		   = 0x40000l, // vertically flip the image when drawing -- the flip happens before any scaling/rotation/vertex blending
		DRAWOP_NOCULL		   = 0x80000l, // don't skip drawing even if the scale is negative or it's facing the wrong way
		DRAWOP_NOBLENDALPHA   = 0x100000l, // treats alpha layer like a normal color layer to be transferred instead of using it to blend colors
		DRAWOP_ALPHAINTERSECT = 0x200000l, // if drawing an alpha image or texture in 3D, tells GameX to draw part of it twice to allow for 3D intersection with other 3D polygons (for advanced users)
		DRAWOP_KEEPSTATES	  = 0x400000l, // tells GameX to NOT reset draw states after calling the function this flag is given to.
		DRAWOP_DITHER		  =	     0x0l, // dithering is the default now -- this does nothing and is only here for backwards compatibility
		DRAWOPTIONSMASK 	  = 0x7FF000l  // DO NOT SPECIFY THIS FLAG! -- to be used internally
	}; 

	typedef long DrawFlags;

	#define VIEWPORT	(ImageX *)1 // dummy image that's really the viewport
									// to copy from the viewport, use this constant as the source image
	
	// defaults used internally:
	#define DEFAULT 	-99999 // reserved value that signifies either a default color or image rectangle full expansion (or rarely, non-scaled centered changed to DEFAULT2)
	#define DEFAULT2	-99998.0f // reserved value that signifies image rectangle non-scaled expansion
	#define FULLRECT	(*((RECT *)0)) // dummy value to mean a rectangle that fills up the entire available space (0,0,DEFAULT,DEFAULT)

	// different modes of 3D auto-sorting for use in Begin3DScene:
	enum _SCENE_SORT_MODE {
		SCENESORTZ = 0x1, // z-buffer byte
		SCENESORTD = 0x2, // depth-sort byte
		SCENESORTS = 0x4, // signed z byte
		SCENESORT2 = 0x8  // dual pass byte
	};

	typedef long SceneSortMode;


	// *** Constants for image usage (for advanced and internal use only):

	enum _IMAGE_USAGE_FLAGS {
	// (combinable)
	// These are no longer needed whenever loading or creating images, except internally by GameX.
	// The only way to use these flags externally now is with ImageX::ConvertUsageTo()
	// (their names remain unchanged to allow some backwards compatibility, compile errors->warnings instead)
		LOAD_ALPHA		= 0x02, // uses a transparency layer that's part of the image -- currently only affects DRAW_PLAIN and DRAW_ADD drawing of this image
		LOAD_MASKED 	= 0x04, // uses a mask to exclude 0-value pixels, or converts alpha layer to mask if LOAD_ALPHA is specified too
		LOAD_TARGETABLE = 0x08, // is capable of being drawn into -- automatically enabled as needed so not really necessary
		LOAD_NON3D		= 0x10, // tells GameX to load this image without support for 3D-accelerated drawing (from)
								// (this flag may not be fully supported and is for advanced users and testing only)
		LOAD_SCREENTARGET = (LOAD_TARGETABLE|LOAD_NON3D), // if copying from the screen, can copy into an image set with this for a speed boost
		LOADMODESMASK	= 0x1E,  // DO NOT SPECIFY THIS FLAG -- this is a mask for all of the load modes and is only to be used internally
	};

	typedef unsigned int ImageUsage;


	// *** Constants for playing sound with GameX:

	// combinable (but usually stand-alone) constants, only used for GameX.PlaySound() in the "how" parameter
	enum _SOUND_PLAY_MODE {
		PLAY_CONTINUE	  = 0x00, // continue playing sound if it's already playing (default)
		PLAY_REWIND 	  = 0x01, // start the sound over if it's already playing (otherwise the sound continues playing)
		PLAY_LOOP		  = 0x02, // set the sound to automatically rewind and play again each time it finishes
		PLAY_NOTIFLESSVOL = 0x04  // if the sound is currently playing, don't play now unless it's as loud or louder now
	};
	typedef int SoundPlayMode;

	// type definitions used internally for music
	#define MUSIC_TYPE_NONE -1
	#define MUSIC_TYPE_MIDI 0 
	#define MUSIC_TYPE_MP3 1
	#define WM_DIRECTSHOW_EVENT (WM_APP+1) // custom WindowFunc callback function for music events

	// *** Constants for getting input with GameX:

	// mouse button constants for GetMousePress()
	// note to self: these values must match the fields of di_mousestate.rgbButtons[]
	enum _MOUSE_BUTTON_ID {
		MOUSE_LEFT	  = (int)0, // left click
		MOUSE_RIGHT   = 1, // right click
		MOUSE_MIDDLE  = 2, // middle click (clicking on the scroll wheel, which not all mice have)
		MOUSE_MID	  = MOUSE_MIDDLE, // abbreviation
		MOUSE_EXTRA   = 3 // no idea what the fourth button on a mouse is, but might as well support it, if/when it exists
	};
	typedef int MouseButtonID;

	// prevent possible compiler complaining if the main GameX header wasn't included:
	#ifndef DIRECTINPUT_VERSION
		#pragma comment(lib,"dinput.lib")
		#define DIRECTINPUT_VERSION 0x0700
		#include "dinput.h"
	#endif

	enum _BUFFERED_KEY_ID {
		// things returned by GetBufferedKeyPress, besides regular characters:
		BUFFERED_KEY_NOTHING   = 0,
		BUFFERED_KEY_BACK      = VK_BACK,
		BUFFERED_KEY_BACKSPACE = VK_BACK,
		BUFFERED_KEY_DELETE    = VK_BACK,
		BUFFERED_KEY_RETURN = VK_RETURN,
		BUFFERED_KEY_ENTER  = VK_RETURN,
		BUFFERED_KEY_TAB = VK_TAB,
		BUFFERED_KEY_ESCAPE = VK_ESCAPE,
		BUFFERED_KEY_ESC    = VK_ESCAPE
	};

	// Key constants for all keyboard input functions (except GetKey/GetBufferedKeyPress which doesn't take or return a key id)
	// These correspond to physical keys on the keyboard that a game might want to respond to:
	// (many keys have multiple names, for instance KEY_RETURN and KEY_ENTER are exactly the same)
	enum _KEY_ID {
		// The four arrow keys:
		KEY_LEFT	  = DIK_LEFT, // <-
		KEY_LEFTARROW = DIK_LEFT,
		KEY_RIGHT	   = DIK_RIGHT, // ->
		KEY_RIGHTARROW = DIK_RIGHT,
		KEY_UP		= DIK_UP, // ^
		KEY_UPARROW = DIK_UP,
		KEY_DOWN	  = DIK_DOWN, // v
		KEY_DOWNARROW = DIK_DOWN,

		// Major keys around the main keyboard:
		KEY_ESCAPE = DIK_ESCAPE, // you must Initialize GameX with RUN_USEESCAPEKEY in order to use KEY_ESCAPE
		KEY_ESC    = DIK_ESCAPE,
		KEY_RETURN = DIK_RETURN, // the enter key
		KEY_ENTER  = DIK_RETURN,
		KEY_SPACE	 = DIK_SPACE, // the space bar
		KEY_SPACEBAR = DIK_SPACE,
		KEY_BACKSPACE = DIK_BACK,
		KEY_BACK	  = DIK_BACK, // the backspace key
		KEY_TAB = DIK_TAB, // the tab key
		KEY_SLASH		 = DIK_SLASH, // \|
		KEY_BAR 		 = DIK_SLASH,
		KEY_FORWARDSLASH = DIK_SLASH,

		// Central keyboard keys:
		KEY_TILDE = DIK_GRAVE, // `~
		KEY_GRAVE = DIK_GRAVE,
		KEY_1 = DIK_1, // 1! (the key ID of 1 on the main keyboard, NOT on the number pad)
		KEY_2 = DIK_2, // 2@
		KEY_3 = DIK_3, // 3#
		KEY_4 = DIK_4, // 4$
		KEY_5 = DIK_5, // 5%
		KEY_6 = DIK_6, // 6^
		KEY_7 = DIK_7, // 7&
		KEY_8 = DIK_8, // 8*
		KEY_9 = DIK_9, // 9(
		KEY_0 = DIK_0, // 0)
		KEY_MINUS = DIK_MINUS, // the -_ key on the main keyboard
		KEY_DASH  = DIK_MINUS,
		KEY_EQUALS = DIK_EQUALS, // the =+ key on the main keyboard
		KEY_PLUS   = DIK_EQUALS,
		KEY_Q = DIK_Q, // the letter keys
		KEY_W = DIK_W,
		KEY_E = DIK_E,
		KEY_R = DIK_R,
		KEY_T = DIK_T,
		KEY_Y = DIK_Y,
		KEY_U = DIK_U,
		KEY_I = DIK_I,
		KEY_O = DIK_O,
		KEY_P = DIK_P,
		KEY_LBRACKET	= DIK_LBRACKET, // [{
		KEY_LEFTBRACKET = DIK_LBRACKET,
		KEY_LBRACE		= DIK_LBRACKET,
		KEY_LEFTBRACE	= DIK_LBRACKET,
		KEY_RBRACKET	 = DIK_RBRACKET, // ]}
		KEY_RIGHTBRACKET = DIK_RBRACKET,
		KEY_RBRACE		 = DIK_RBRACKET,
		KEY_RIGHTBRACE	 = DIK_RBRACKET,
		KEY_A = DIK_A,
		KEY_S = DIK_S,
		KEY_D = DIK_D,
		KEY_F = DIK_F,
		KEY_G = DIK_G,
		KEY_H = DIK_H,
		KEY_J = DIK_J,
		KEY_K = DIK_K,
		KEY_L = DIK_L,
		KEY_SEMICOLON = DIK_SEMICOLON, // ;:
		KEY_COLON	  = DIK_SEMICOLON,
		KEY_APOSTROPHE = DIK_APOSTROPHE, // '"
		KEY_QUOTE	   = DIK_APOSTROPHE,
		KEY_Z = DIK_Z,
		KEY_X = DIK_X,
		KEY_C = DIK_C,
		KEY_V = DIK_V,
		KEY_B = DIK_B,
		KEY_N = DIK_N,
		KEY_M = DIK_M,
		KEY_COMMA = DIK_COMMA, // ,<
		KEY_LESS  = DIK_COMMA,
		KEY_PERIOD	= DIK_PERIOD, // .>
		KEY_GREATER = DIK_PERIOD,
		KEY_DOT 	= DIK_PERIOD,
		KEY_BACKSLASH = DIK_BACKSLASH, // /?
		KEY_QUESTION  = DIK_BACKSLASH,
		KEY_DIVIDE	  = DIK_BACKSLASH, // NOT on the numeric keypad, that's KEY_NUMDIVIDE

		// Keys on the number pad:
		// (note that not all keyboards have a number pad)
		KEY_NUM7 = DIK_NUMPAD7, // the key ID of 7 on the numeric keypad, NOT on the main keyboard
		KEY_NUM8 = DIK_NUMPAD8,
		KEY_NUM9 = DIK_NUMPAD9,
		KEY_NUM4 = DIK_NUMPAD4,
		KEY_NUM5 = DIK_NUMPAD5,
		KEY_NUM6 = DIK_NUMPAD6,
		KEY_NUM1 = DIK_NUMPAD1,
		KEY_NUM2 = DIK_NUMPAD2,
		KEY_NUM3 = DIK_NUMPAD3,
		KEY_NUM0 = DIK_NUMPAD0,
		KEY_NUMSTAR 	= DIK_NUMPADSTAR,
		KEY_NUMTIMES	= DIK_NUMPADSTAR,
		KEY_NUMMULTIPLY = DIK_NUMPADSTAR,
		KEY_NUMMINUS	= DIK_SUBTRACT,
		KEY_NUMSUBTRACT = DIK_SUBTRACT,
		KEY_NUMPLUS = DIK_ADD,
		KEY_NUMADD	= DIK_ADD,
		KEY_NUMDIVIDE = DIK_DIVIDE,
		KEY_NUMSLASH  = DIK_DIVIDE,
		KEY_NUMDECIMAL = DIK_DECIMAL,
		KEY_NUMPERIOD  = DIK_DECIMAL,
		KEY_NUMDOT	   = DIK_DECIMAL,

		// Function keys (most keyboards have F1 through F12 at the top) and keys nearby
		KEY_F1 = DIK_F1,
		KEY_F2 = DIK_F2,
		KEY_F3 = DIK_F3,
		KEY_F4 = DIK_F4,
		KEY_F5 = DIK_F5,
		KEY_F6 = DIK_F6,
		KEY_F7 = DIK_F7,
		KEY_F8 = DIK_F8,
		KEY_F9 = DIK_F9,
		KEY_F10 = DIK_F10,
		KEY_F11 = DIK_F11,
		KEY_F12 = DIK_F12,
		KEY_PRINT = DIK_SYSRQ,
		KEY_SYSRQ = DIK_SYSRQ,
		KEY_NUMLOCK = DIK_NUMLOCK, // NOTE: Use GameX.IsNumLockOn() instead to check whether num lock is on
		KEY_SCROLLOCK = DIK_SCROLL, // NOTE: Use GameX.IsScrollLockOn() instead to check whether scroll lock is on
		KEY_PAUSE = DIK_PAUSE,
		KEY_BREAK = DIK_PAUSE,
		KEY_INSERT = DIK_INSERT,
		KEY_HOME = DIK_HOME,
		KEY_PAGEUP = DIK_PRIOR,
		KEY_PGUP   = DIK_PRIOR,
		KEY_DELETE = DIK_DELETE,
		KEY_END = DIK_END,
		KEY_PAGEDOWN = DIK_NEXT,
		KEY_PAGEDN	 = DIK_NEXT,
		KEY_PGDN	 = DIK_NEXT,

		// Modifier keys:
		// Note: use IsAltDown(), IsShiftDown(), IsCtrlDown(), and IsWinDown() instead of these when possible
		KEY_LALT	= DIK_LMENU,
		KEY_LMENU	= DIK_LMENU,
		KEY_LEFTALT = DIK_LMENU,
		KEY_RALT	 = DIK_RMENU,
		KEY_RMENU	 = DIK_RMENU,
		KEY_RIGHTALT = DIK_RMENU,
		KEY_LSHIFT	  = DIK_LSHIFT,
		KEY_LEFTSHIFT = DIK_LSHIFT,
		KEY_RSHIFT	   = DIK_RSHIFT,
		KEY_RIGHTSHIFT = DIK_RSHIFT,
		KEY_LCTRL		= DIK_LCONTROL,
		KEY_LEFTCTRL	= DIK_LCONTROL,
		KEY_LCONTROL	= DIK_LCONTROL,
		KEY_LEFTCONTROL = DIK_LCONTROL,
		KEY_RCTRL		 = DIK_RCONTROL,
		KEY_RIGHTCTRL	 = DIK_RCONTROL,
		KEY_RCONTROL	 = DIK_RCONTROL,
		KEY_RIGHTCONTROL = DIK_RCONTROL,
		KEY_CAPS	 = DIK_CAPITAL, // Caps lock -- NOTE: Use GameX.IsCapsLockOn() instead to check whether caps lock is on.
		KEY_CAPSLOCK = DIK_CAPITAL, //					  These constants are for treating caps lock like a normal key, which it isn't.
		KEY_CAPITAL  = DIK_CAPITAL,
		KEY_LWIN = DIK_LWIN, // Left Windows key
		KEY_RWIN = DIK_RWIN, // Right Windows key (not all keyboards have it)
	};
	typedef int KeyID;

	// input byte codes used internally by GameX
	#define INPUT_DOUBLE	0x01 // custom bit code for double-pressed (for mouse double-click recognition)
	#define INPUT_HELD		0x20 // custom bit code for pressed and held, if turned on, should also turn off INPUT_PRESSED
	#define INPUT_PRESSED	0x80 // (must be 0x80 = 128, it's part of DirectInput functions for pressed states)

	#define WINDX_BUFSIZE	1024 // Keyboard Buffer Size, newest data is lost if buffer overflows


	enum _COLOR_SET_COND {
		COLOR_COND_NEGATIVE, // < 0
		COLOR_COND_POSITIVE, // > 0
		COLOR_COND_NEUTRAL, // -50 to 50
		COLOR_COND_ANYTHING, // anything
		COLOR_COND_GREATER, // > other
		COLOR_COND_LESS, // < other
		COLOR_COND_NEAR // other-50 to other+50
	};
	typedef int ColorSetCondition;

	enum _COLOR_SET_TYPE {
		COLOR_SET_CONSTANT, // 50
		COLOR_SET_A, // A
		COLOR_SET_B, // B
		COLOR_SET_ABS_A, // abs(A)
		COLOR_SET_ABS_B, // abs(B)
		COLOR_SET_MAX, // max(A,B)
		COLOR_SET_MIN, // min(A,B)
		COLOR_SET_A_PLUS_B, // A+B
		COLOR_SET_A_MINUS_B, // A-B
		COLOR_SET_ABS_A_PLUS_ABS_B, // abs(A)+abs(B)
		COLOR_SET_ABS_A_MINUS_ABS_B, // abs(A)-abs(B)
		COLOR_SET_L, // (float)(L*L*L)/(100.0f*100.0f)
		COLOR_SET_L_SMOOTH, // ((float)(L*L*L)/(100.0f*100.0f)-50)*2
		COLOR_SET_SIN_A, // sinf((float)A*((360.0f/100.0f)*DEGtoRAD))*100.0f
		COLOR_SET_SIN_B, // sinf((float)B*((360.0f/100.0f)*DEGtoRAD))*100.0f
		COLOR_SET_COS_A, // cosf((float)A*((360.0f/100.0f)*DEGtoRAD))*100.0f
		COLOR_SET_COS_B, // cosf((float)B*((360.0f/100.0f)*DEGtoRAD))*100.0f
		COLOR_SET_SIN_L, // sinf((float)(L*L*L)/(100.0f*100.0f)*((360.0f/100.0f)*DEGtoRAD))*100.0f
		COLOR_SET_COS_L // cosf((float)(L*L*L)/(100.0f*100.0f)*((360.0f/100.0f)*DEGtoRAD))*100.0f
	};
	typedef int ColorSetType;

	enum _COLOR_SWAP_TYPE {
		COLOR_SWAP_SAME=-1, // Reds->Reds, Greens->Greens, Blues->Blues
		COLOR_SWAP_ROTATE120=0, // Reds->Greens, Greens->Blues, Blues->Reds
		COLOR_SWAP_ROTATE240, // Reds->Blues, Greens->Reds, Blues->Greens
		COLOR_SWAP_REDGREEN,  // Reds <-> Greens
		COLOR_SWAP_REDBLUE,   // Blues <-> Reds
		COLOR_SWAP_GREENBLUE, // Greens <-> Blues
		COLOR_SWAP_BA,        // ROYGBIV -> RVIBGYO
		COLOR_SWAP_NBA,       // ROYGBIV -> GBIVROY
		COLOR_SWAP_BNA,       // ROYGBIV -> VROYGBI
		COLOR_SWAP_NBNA,      // ROYGBIV -> BGYORVI
		COLOR_SWAP_ANB,       // ROYGBIV -> VIBGYOR
		COLOR_SWAP_NAB,       // ROYGBIV -> GYORVIB
		COLOR_SWAP_NANB,      // ROYGBIV -> BIVROYG
		COLOR_SWAP_BB,        // ROYGBIV -> RRRRVIB
		COLOR_SWAP_NBB,       // ROYGBIV -> GGGGYIV
		COLOR_SWAP_BNB,       // ROYGBIV -> VVVVIYG
		COLOR_SWAP_NBNB,      // ROYGBIV -> BBBBVIR
		COLOR_SWAP_AA,        // ROYGBIV -> RVIBIVR
		COLOR_SWAP_NAA,       // ROYGBIV -> GYIVIYG
		COLOR_SWAP_ANA,       // ROYGBIV -> VIYGYIV
		COLOR_SWAP_NANA       // ROYGBIV -> BIVRVIB
	};
	typedef int ColorSwapType;

	// *** Internal Image-Handling Definitions:

	#define DRAW_UNDEF		0x0000		// can't be drawn or operated on

	#define IMG_OK			42 // chose 42 to make accidental setting to OK status more unlikely
	#define IMG_NOTREADY	1
	#define IMG_IGNORE		2
										// **** Image Pixel Channels
	enum _IMAGE_CHANNEL {
		CHAN_INDEXED=0,
		CHAN_RED=1,
		CHAN_GREEN=2,
		CHAN_BLUE=3,
		CHAN_ALPHA=4,
		CHAN_GRAY=5
	};
										// **** Image Pixel-Format Information
	enum _IMAGE_PIX_INFO {
		IMG_UNDEF			= 0x00, // Image information is undefined.
		IMG_ALPHA			= 0x01, // Image has an alpha channel (follows color data)
		IMG_MERGEALPHA		= 0x02, // Alpha channel is merged with data at pixel level (RGBA)	
		IMG_INDEXED 		= 0x04, // Image is indexed
		IMG_LOWCOLOR		= 0x08, // Image is low color.	(BPP=4+4+4+[A])
		IMG_HIGHCOLOR		= 0x10, // Image is high color. (BPP=5+6+5+[A])
		IMG_TRUECOLOR		= 0x20, // Image is true color. (BPP=8+8+8+[A])
		IMG_TRUECOLORX		= 0x40, // Image is true color. (BPP=8+8+8+8)
		IMG_MASKED			= 0x80, // Image uses a 0-mask (instead of alpha)
	};

	#define RED_LUMINANCE	  0.3086f // standard empirical luminance values of the primary colors,
	#define GREEN_LUMINANCE   0.6094f // used to convert color to grayscale while preserving
	#define BLUE_LUMINANCE	  0.0820f // the brightness the human eye perceives


	// *** Constants for initializing GameX:

	// current status of Gamex,
	// mostly used internally by GameX to signal errors
	enum _GAMEX_STATUS {
		GAMEX_NO_INIT = (int)0,
		GAMEX_INITCLASS_FAILED,
		GAMEX_INITCLASS_OK,
		GAMEX_INITWINTEMP_FAILED,
		GAMEX_INITWINTEMP_OK,
		GAMEX_INITWINREAL_FAILED,
		GAMEX_INITWINREAL_OK,
		GAMEX_SHOWWIN_FAILED,
		GAMEX_SHOWWIN_OK,
		GAMEX_DDSTART_FAILED,
		GAMEX_DDSTART_OK,
		GAMEX_DDSETLEVEL_FAILED,
		GAMEX_DDSETLEVEL_OK,
		GAMEX_SETMODE_FAILED,
		GAMEX_SETMODE_OK,
		GAMEX_FRONTSURFACE_FAILED,
		GAMEX_FRONTSURFACE_OK,
		GAMEX_BACKSURFACE_FAILED,
		GAMEX_BACKSURFACE_OK,
		GAMEX_READY, // important, status should be this during game
		GAMEX_REQSIZE_FAILED,
		GAMEX_BPP_FAILED,
		GAMEX_SOUNDINIT_FAILED,
		GAMEX_SOUNDCOOP_FAILED,
		GAMEX_SOUNDCREATE_FAILED,
		GAMEX_SOUNDMATCH_FAILED,
		GAMEX_SOUNDCAPS_FAILED,
		GAMEX_SOUNDBUFLEN_FAILED,
		GAMEX_SOUNDLOST_FAIL,
		GAMEX_SOUNDINVALID_FAIL,
		GAMEX_SOUNDPARAM_FAIL,
		GAMEX_SOUNDPRIOLEVEL_FAIL,
		GAMEX_IMAGEUSE_FAILED,
		GAMEX_CLIPPER_FAILED,
		GAMEX_SHUTTINGDOWN,
		GAMEX_D3DSTART_FAILED,
		GAMEX_UNINIT_IMG_DRAW_FAIL,
		GAMEX_INVALID_DRAW_TIME_FAIL,
		GAMEX_PIXEL_ACCESS_FAILURE,
		GAMEX_CREATING_TEST_SURFACE,
		GAMEX_TOTAL_STATUS_NUMBER
	};
	typedef int GameXStatus;


	#ifdef _DEBUG
		#define WINDX_DEBUG 					// Enable WindowsDX output to debugging file if in Debug build 
	#endif

#endif