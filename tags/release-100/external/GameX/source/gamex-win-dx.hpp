//
// GameX - WindowsDX Class Header 
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
// 
#ifndef WINDX_DEF
	#define WINDX_DEF

	// Windows
	#pragma comment(lib,"kernel32.lib") // auto-link with required windows libraries
	#pragma comment(lib,"user32.lib") // (just in case the linker settings are missing these)
	#pragma comment(lib,"gdi32.lib")
	#include <windows.h>
	#include <stdio.h>
	#include <stdarg.h>
	#include <time.h>

	// DirectDraw
	#pragma comment(lib,"ddraw.lib") // auto-link DDRAW.LIB
	#define DIRECTDRAW_VERSION 0x0700 // specify version 7.0 of DirectDraw
	#include <ddraw.h>

	// Direct3D
	#define DIRECT3D_VERSION 0x0700 // specify version 7.0 of Direct3D -- the last version that's compatible with DirectDraw
	#include <d3d.h>				// note: for newer versions of D3D we would need to 
	#include <d3dtypes.h>			//		 include different files, like d3d8.h

	// DirectInput
	#pragma comment(lib,"dinput.lib") // auto-link DINPUT.LIB
	#define DIRECTINPUT_VERSION 0x0700 // specify version 7.0 of DirectInput
	#include <dinput.h>

	// DirectSound
	#pragma comment(lib,"dsound.lib") // auto-link DSOUND.LIB
	#define DIRECTSOUND_VERSION 0x0700 // specify version 7.0 of DirectSound
	#include <dsound.h>
	
	// DirectMusic
	#pragma comment(lib,"dxguid.lib") // auto-link DXGUID.LIB
	#include <dmksctrl.h>
	#include <dmusici.h>
	#include <dmusicc.h>
	#include <dmusicf.h>

	// DirectShow
	#pragma comment(lib,"strmiids.lib") // auto-link STRMIIDS.LIB
	#include <strmif.h>
	#include <control.h>
	#include <uuids.h>
	#include <evcode.h>

	// headers of other components of GameX we need:
	#include "gamex-debug.hpp" // allows calling of debug.Output("something") to output to the "debug.txt" file
	#include "gamex-image.hpp" // support for loading and converting graphics
	#include "gamex-sound.hpp" // support for loading sounds and music
	#include "gamex-camera.hpp" // 3D camera support
	#include "gamex-vector.hpp" // vector support
	#include "gamex-matrix.hpp" // matrix support
	#include "gamex-defines.hpp" // defines important GameX constants
	#include "gamex-utilities.hpp" // general classes for GameX functions

	class WindowsDX {
	public:
		WindowsDX (void);
		~WindowsDX (void);

		//**** Starting or Exiting GameX

		// Initializes GameX, must be called before using any graphical or audio parts of GameX.
		// Usage Example: GameX.Initialize("My Game", VIDEO_16BIT, 800, 600);
		bool Initialize (char* name, GameXInitFlags options, int xres, int yres, int speed = 60);

		// Exits program, asks for user confirmation first unless RUN_NOCONFIRMQUIT
		// is part of the Initialize options or unrecoverable is set to true.
		void Quit (bool unrecoverable=false);

		//**** Debugging Functions

		// Adds a log to the debug.txt standard debugging output file
		inline void DebugOutput(char* message) {debug.Output(message,"");}

		// Automatically called by GameX if RUN_AUTOSHOWINFO is part of the Initialize option flags.
		// Draws the game cps,fps,idle status to the corner of the screen.
		void DrawStatus(bool continuously=false);

		// Functions that allow you to easily display various types of dialog boxes
		void InfoDialog(char* message); // gives info, user hits OK
		void WarningDialog(char* message); // gives warning info, user hits OK
		void ErrorDialog(char* message); // gives error info, user hits OK
		int ChoiceDialog(char* message, bool three_choices = false, int default_choice = 1); // gives info, user hits YES, NO, or (if three_choices is true) CANCEL -- returns 0 for NO, 1 for YES, or -1 for CANCEL
		int ErrorChoiceDialog(char* message, int default_choice = -1); // gives error info, user hits ABORT, RETRY, or IGNORE -- returns -1 for ABORT, 0 for IGNORE, or 1 for RETRY

		//**** Random Number Functions

		inline int GetRandomInt(int min, int max) {if(min>max){int temp = max; max=min; min=temp;} return min+(rand()%(max-min+1));}
		inline float GetRandomFloat(float min, float max) {return min+((float)rand()*(max-min)/(float)RAND_MAX);}
		void ResetRandomSeed(void); // called automatically when game starts
		void SetRandomSeed(int seed); // for advanced purposes involving reproduction of random number series


		//**** Timing Functions

		// Gets a value that increases linearly with time
		// (the amount of game time that has gone by so far, in number of distinct game states.)
		// You can keep track of a variable for this on your own; this method is just for convenience.
		inline int GetGlobalCounter() {return win_global_counter;}

		// Gets a value that oscillates sinusoidally between a min and max value at a given speed.
		// Usage Example: float brightness = GameX.GetOscillatingValue(0.0f,255.0f,4.0f);
		// Note: phase is in DEGREES. so (for example) phase of 90 makes cosinusoidal oscillation
		inline float GetOscillatingValue(float min, float max, float speed, float phase=0) {return min + (max-min)/2.0f * (1.0f + sinf( (phase+(float)win_global_counter*speed)*DEGtoRAD) );}

		// Convert between a number of game cycles and the number of seconds it should take
		inline float CyclesToSeconds(int cycles) {return (float)cycles/(float)win_cps;}
		inline int SecondsToCycles(float seconds) {return (int)(seconds*(float)win_cps);}


		//**** Performance Checking Functions

		// Gets the approximate percentage of CPU power that the game is currently NOT using.
		// Higher values mean the game is putting less strain on the CPU (which is good).
		inline int GetCurrentIdlePercent (void) {return p_idle;}

		// Gets the main loop/cycle rate the game is currently achieving, (calls/second of GameRun())
		// Higher values returned means the game is running faster.
		inline int GetCurrentCycleRate (void)	{return c_rate;}
		inline int GetTargetCycleRate (void)	{return win_cps;}

		// Gets the frame rate the game is currently achieving, (calls/second of GameDraw())
		// Higher values are better and mean the game is running smoother.
		inline int GetCurrentFrameRate (void)	{return f_rate;}
		inline int GetTargetFrameRate (void)	{return min(win_fps,win_cps);}
 

		//**** Altering Timing (optional functions to change game's auto-timing while it's running)

	#ifndef GAMEX_RUN_UNMANAGED
		void SetGameSpeed (int cps); // for dynamically changing the game speed that's initially set in GameX.Initialize()
		void SetLagLimit (int max_cpf); // most lagged frames allowed per instant, or 0 to mean infinite (for networked games)
		void SetForcedFrameRate (int fps); // tries to force a certain framerate, for debugging use only
		void ResetForcedFrameRate (void); // go back to targeting the best possible framerate that's visible
		inline int GetGameSpeed (void) {return GetCurrentCycleRate();} // alternate name for GetCurrentCycleRate, to be used with SetGameSpeed
	#endif
		

		//**** Game Screen Functions

		// Sets the viewport range, i.e. changes where GameX thinks the window is, for clipping
		// Games can use this for things like split-screen mode, along with CameraX::SetWindow()
		void SetView (int xLeft, int yTop, int xRight, int yBottom);
		void GetView (int& xLeft, int& yTop, int& xRight, int& yBottom);
		void ResetView (void); // resets view to entire window

		// Gets the current game screen width or height. It's better to use these than to hard-code.
		inline int GetWidth (void) {return win_width;}
		inline int GetHeight (void) {return win_height;}

		// Returns true if GameX is currently in full-screen mode, false otherwise
		inline bool IsFullScreen (void) {return win_fullscreen;}

		inline int GetBPP (void) {return win_screenbpp;}

		
		//**** Drawing Functions: (for drawing sprites, shapes, polygons, backgrounds, etc.)
		// You can create a ColorX "on the fly" like so: GameX.FillScreen(ColorX(255,0,0));
		// But ImageX objects must be prepared first with ImageX::Create() or ImageX::Load()

		// 2D drawing functions:
		void DrawPoint	 (ColorX& clr, int x, int y);
		void DrawLine	 (ColorX& clr, int x1,int y1, int x2,int y2);
		void DrawRect	 (ColorX& clr, int xLeft, int yTop, int xRight, int yBottom);
		void DrawPolygon (ColorX& clr, int x1,int y1, int x2,int y2, int x3,int y3, int x4,int y4);

		// 2D image drawing functions:
		void DrawImage			 (ImageX* img, int x, int y);
		void DrawImage			 (ImageX* img, int x, int y, int width, int height );
		void DrawImage			 (ImageX* img, int x, int y, float angle, float scale); // shorthand for calling SetDrawAngle, SetDrawScale before drawing DrawImage
		void DrawLineImage		 (ImageX* img, int x1,int y1, int x2,int y2, bool pixelUnitScale=false);
		void DrawTexturedRect	 (ImageX* img, int xLeft, int yTop, int xRight, int yBottom);
		void DrawTexturedPolygon (ImageX* img, int x1,int y1, int x2,int y2,  int x3,int y3,  int x4,int y4);

		// 3D drawing functions:
		void DrawPoint3D   (ColorX& clr, Vector3DF& v);
		void DrawLine3D    (ColorX& clr, Vector3DF& v1, Vector3DF& v2);
		void DrawPolygon3D (ColorX& clr, Vector3DF& v1, Vector3DF& v2, Vector3DF& v3, Vector3DF& v4);

		// 3D image drawing functions:
		void DrawImage3D		   (ImageX* img, Vector3DF& v, bool worldUnitScale=true);
		void DrawLineImage3D	   (ImageX* img, Vector3DF& v1, Vector3DF& v2, bool worldUnitScale=false);
		void DrawTexturedPolygon3D (ImageX* img, Vector3DF& v1, Vector3DF& v2, Vector3DF& v3, Vector3DF& v4);

		// Screen-effect drawing functions:
		void ClearScreen (void);
		void FillScreen  (ColorX& clr);
		void DrawTiled	 (ImageX* img, float tx=0, float ty=0, float rx=DEFAULT, float ry=DEFAULT); // draws image tiled over screen, good for backgrounds or foregrounds
 
		// Text Drawing Functions (very slow; it's faster and looks better to draw text graphically):
		void DrawText (int x, int y, char* msg, int r=255, int g=255, int b=255);
		void DrawOutlinedText (int x, int y, char* msg, int r=255, int g=255, int b=255, int ro=0, int go=0, int bo=0);

		// Special blurring functions (very slow, and may cause mosaic effect without graphic acceleration):
		void BlurScreen  (float amount); // use sparingly
		void BlurRect	 (float amount, int xLeft, int yTop, int xRight, int yBottom);


		void SpecialPauseFade(float blur1, float blur2, int r1, int r2, int g1, int g2, int b1, int b2, int a);
		bool CopyToClipboard(ImageX* img); // copies given ImageX (or VIEWPORT) to the Windows clipboard, returns false if failed, true otherwise

		bool ToggleFullscreen (void);
		bool ChangeGameSize(int xres, int yres);

		// optional states that can be set before calling the above drawing functions:

		inline void SetDrawCam			 (CameraX* cam) /* sets the 3D projection camera */ {draw_state->cam = cam;}
		inline void SetDrawMode 		 (DrawFlags mode)  /* sets the drawing mode */ {draw_state->flags = mode;}
		inline void SetDrawPart 		 (int xLeft, int yTop,	int xRight, int yBottom) /* sets to draw only a rectangular part of an image */ {draw_state->src_rect.left = xLeft; draw_state->src_rect.top = yTop; draw_state->src_rect.right = xRight; draw_state->src_rect.bottom = yBottom;}
		inline void SetDrawAngle		 (float angle)	/* sets drawing rotation angle in degrees counter-clockwise */ {draw_state->angle = angle;}
//		inline void SetDrawAngle		 (float angleX, float angleY, float angleZ) /* automatic 3D rotation of sprites not yet implemented */
		inline void SetDrawScale		 (float scale) /* sets drawing scale, can be a factor, # of pixels, or # of 3D world units, whichever makes sense */ {draw_state->scalex = scale; draw_state->scaley = scale;}
		inline void SetDrawScale		 (float scaleX, float scaleY) /* sets drawing scale separately for width and height */ {draw_state->scalex = scaleX; draw_state->scaley = scaleY;}
		inline void SetDrawTranslate	 (float transX, float transY) /* sets after-projection translation values, in pixels */ {draw_state->transx = transX; draw_state->transy = transY;}
		inline void SetDrawShading(ColorX color) /* set the image shading/transparency */ {draw_state->colors = color;}
			   void SetDrawShadingEffect(DrawFlags effect);
		inline void SetDrawWarp 		 (float w0, float w1, float w2, float w3) {draw_state->warp_mode = 1; draw_state->warp[0] = w0; draw_state->warp[1] = w1; draw_state->warp[2] = w2; draw_state->warp[3] = w3;}
		inline void SetDrawDestination	 (ImageX* dest) {draw_state->dst = dest;}
		inline void SetDrawDepth   (float depth) {draw_state->depth2D = depth + draw_state->cam->GetNear();} // only affects 2D drawing into 3D scenes! -- is the distance in world units from the screen, 0.0 means as close as possible, 10.0f means further away, etc.
		inline void ResetDrawStates (void) {draw_state->flags = DRAW_PLAIN; SetDrawPart(0,0,DEFAULT,DEFAULT); draw_state->angle = 0.0f; draw_state->scalex = draw_state->scaley = 1.0f; draw_state->transx = draw_state->transy = 0.0f; draw_state->colors = ColorX(); SetDrawShadingEffect(DRAW_MULTIPLY); draw_state->warp_mode = 0; draw_state->dst = VIEWPORT;}



		//**** Advanced Drawing Functions:

		// Advanced Image Drawing Functions (uses states set by the Draw State Setting Functions below):
		// Take drawing flags, the image to draw, and optionally the image to draw into if not drawing to the viewport.
		// all other settings taken from states set by functions like SetDrawShading() above.

		// Getter method for certain states, for convenience
		inline DrawFlags GetDrawMode() {return draw_state->flags;}
		inline float GetDrawAngle() {return draw_state->angle;}
		inline float GetDrawScaleX() {return draw_state->scalex;}
		inline float GetDrawScaleY() {return draw_state->scaley;}
		inline float GetDrawTransX() {return draw_state->transx;}
		inline float GetDrawTransY() {return draw_state->transy;}
		inline CameraX* GetDrawCam() {return draw_state->cam;}
		inline ColorX& GetDrawShading() {return draw_state->colors;}
		inline DrawFlags GetDrawShadingEffect() {return draw_state->color_effect;}
		inline ImageX* GetDrawDest() {return draw_state->dst;}

		//**** 3D Scene Functions

		// Tells GameX that you are about to draw a 3D scene to the viewport
		// so GameX will automagically draw the scene in the right order for you.
		// NOTE: A 3D scene means any bunch of things drawn with SetDrawToPointSprite, SetDrawToPolygon, DrawMaster, and/or DrawRectEx
		void Begin3DScene (void);

		// Tells GameX that you're done drawing a 3D scene.
		// Must be called immediately after drawing the scene or GameX may draw it wrong.
		void End3DScene (void);


		//**** Individual-Pixel-Drawing Functions:
		// WARNING: Not affected by clipping; game may crash if you draw a pixel out of bounds.

		// Tells GameX that you are about to read or write pixels
		// and if you pass it an ImageX it lets you set pixels in an image.
		// You MUST call this function before using DrawPixel or ReadPixel or they'll fail to work.
		void AccessPixels (ImageX* destination = VIEWPORT);

		// Draws a pixel (must call AccessPixels first!) -- r,g,b range from 0 to 255
		inline void DrawPixel (int x,int y,int r,int g,int b,int a) {(*FuncDrawPixel) (x,y,r,g,b,a);}
		inline void DrawPixel (int x,int y,int r,int g,int b) {(*FuncDrawPixel) (x,y,r,g,b,255);}

		// Reads a pixel's values (must call AccessPixels first!) -- get r,g,b ranging from 0 to 255
		inline void ReadPixel (int x,int y, int &r, int &g, int &b, int &a) {(*FuncReadPixel) (x,y,r,g,b,a);}
		inline void ReadPixel (int x,int y, int &r, int &g, int &b) {int a; (*FuncReadPixel) (x,y,r,g,b,a);}

		// Tells GameX that you're done accessing individual pixels.
		// You MUST call this function before you can do any non-pixel drawing after calling AccessPixels.
		void EndPixelAccess (void);


		//**** Sound Functions:
		// (must load a SoundX object with sound.LoadWav("soundfile.wav"); before using the following)

		// Plays a sound at specified volume (0.0f to 1.0f), stereo panning (-1.0f to 0.0f to 1.0f -- left to middle to right)
		// and using a given frequency multiplier (1.0f for normal, 2.0f for double speed and pitch, 0.5f for half speed and pitch)
		void PlaySound (SoundX* snd, SoundPlayMode mode=PLAY_CONTINUE, float vol=1.0f, float pan=0.0f, float freq=1.0f);

		// Stops a sound from playing if it was playing/looping
		void StopSound (SoundX* snd);

		// Sets the sound to play from its beginning
		void RewindSound (SoundX* snd);

		// Returns true if the given sound is still playing, looping, or paused, false otherwise
		bool IsSoundPlaying (SoundX* snd);


		//**** Music Functions:
		// (must load a MusicX object with music.Load(music_filename); before using the following)

		// Plays a MusicX object, and immediately stops playing any other music that may be playing
		// times of 0 means infinite loop, 1 means play once, 2 means play twice, etc.
		// speed_factor of 1.0 means normal speed, 2.0 means double speed, etc.
		void PlayMusic (MusicX* music, int times = 0, float volume = 1.0f, float fade_in_seconds = 1.0f, float tempo = 1.0f, float pitch = 1.0f);

		// Fades the volume to 0 over given seconds if seconds>0, then stops the music from playing
		void StopMusic (MusicX* music, float seconds = 2.0f);

		// Fades the music up or down to the given volume factor, over the given time
		// seconds is how many seconds to alter volume over
		// target_factor of 0.0f is fade to silence, 0.5f is fade to half master volume, 1.0f is no change, 1.27f is fade to max amplified
		void VolumeFadeMusic(MusicX* music, float volume_factor = 0.0f, float seconds = 0.0f);

		// Bends the pitch of the mucis up or down to the given pitch factor, over the given time
		// seconds is how many seconds to alter pitch over. This method does not change speed of play
		// pitch_factor of 0.5f is half pitch (low frequency), 2.0f is double pitch (high frequency), 1.0f is no change, etc.
		void PitchBendMusic(MusicX* music, float pitch_factor = 0.5f, float seconds = 0.0f);

		// Sets the tempo, or speed of play, of all music -- does not change the pitch
		// 0.5f is half speed, 1.0f is normal speed, 2.0f is double speed, etc.
		// Can be called before or after you start playing the song
		// note: there may be a small time delay after calling this function before the change takes effect
		void SetMusicTempo(float tempo_factor);

		// Returns whatever tempo was last set by GetMusicTempo, or the default of 1.0f
		float GetMusicTempo(void) {return master_music_tempo;}

		// Sets the volume of all music
		// Can be called before or after you start playing a song
		// If an individual music song has its volume faded to 50% and the master volume is 50%
		// that will result in the music being played at 25% volume
		void SetMasterMusicVolume(float volume_factor);

		// Returns whatever volume was last set by SetMasterMusicVolume, or the default of 1.0f
		float GetMasterMusicVolume(void) {return master_music_volume;}

		// returns true if the given song is still playing, looping, or paused, false otherwise
		bool IsMusicPlaying(MusicX* music);

		// pauses whatever MIDI song is currently playing (has only been tested internally to GameX)
		void PauseMusic (void);

		// resumes whatever MIDI song is currently playing (has only been tested internally to GameX)
		// due to the way it is paused, a stray note might play for a while after resuming
		void ResumeMusic (void);
		

		//**** Keyboard Input Functions:
		// NOTE: See gamex-defines.hpp for the key constants, like KEY_RETURN and KEY_SPACE

		// Old input functions, for backward compatibility and for developers who only need a simple interface:
		inline bool GetKeyDown (KeyID k)   {return IsKeyDown (k);}
		inline char GetKey (void)		   {return GetBufferedKeyPress();}

		// Returns the next char of buffered key input (like 'A' or 'b' or '3'), returns 0 if there is no key input left
		// Use this to get typed data input from the user (such as a name at a name entry screen)
		inline char GetBufferedKeyPress (void)	{return (win_keys>0) ? win_keybuf[--win_keys] : 0;}

		// Returns the next char of buffered key input without advancing past it, returns 0 if there is no key input left
		// Use this if you want to see what GetBufferedKeyPress would return without actually taking it out of the buffer
		inline char PeekBufferedKeyPress (void) {return (win_keys>0) ? win_keybuf[win_keys-1] : 0;}

		// Completely clears the key buffer
		// Use this, for instance, before asking for input to clear unwanted data from earlier
		inline void ClearKeyBuffer (void)		{win_keys = 0;}

		// Returns true if the given key is down, false otherwise
		// Use this if you only want to know if a given key is down or not (like the trigger of a machine gun)
		inline bool IsKeyDown (KeyID k) 	 {return (di_keystate[k] & INPUT_PRESSED) ? true : false;}

		// Returns true if the given key is down and was up at least once since the last time this function was called on it, false otherwise
		// Use this if you want to see if a key was just pressed (not just being held down, so you only respond once)
		inline bool GetKeyPress (KeyID k)		  {return (di_keypressed[k] & INPUT_PRESSED) ? true : false;}

		// Returns true if the given key is down and was not held down since the last time GetKeyPress was called on it, false otherwise
		// Use this if you want to see if a key was just pressed but don't want to also mark it as being held down yet
		inline bool PeekKeyPress (KeyID k)		  {return (di_keypressed[k] & INPUT_PRESSED) ? true : false;}
		
		// Returns true if any key on the keyboard is down and was up at least once since GetKeyPress was called on it, false otherwise
		// Use this at "Press Any Key" prompts. Note that this automatically excludes certain keys, such as volume control keys that a game should not respond to.
		bool AnyKeyPress (void);
		bool PeekAnyKeyPress (void);

		// Convenience methods for checking for modifier keys being held down:
		inline bool IsShiftDown (void)	{return IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT);}
		inline bool IsAltDown (void)	{return IsKeyDown(KEY_LALT)   || IsKeyDown(KEY_RALT);}
		inline bool IsCtrlDown (void)	{return IsKeyDown(KEY_LCTRL)  || IsKeyDown(KEY_RCTRL);}
		inline bool IsWinDown (void)	{return IsKeyDown(KEY_LWIN)   || IsKeyDown(KEY_RWIN);}

		// To check whether caps/num/scroll lock is on:
		inline bool IsCapsLockOn (void)   {return (GetKeyState(VK_CAPITAL) != 0);}
		inline bool IsNumLockOn (void)	   {return (GetKeyState(VK_NUMLOCK) != 0);}
		inline bool IsScrollLockOn (void)	{return (GetKeyState(VK_SCROLL) != 0);}


		//**** Mouse Input Functions:
		// NOTE: See gamex-defines.hpp for the mouse constants, like MOUSE_LEFT and MOUSE_RIGHT 

		inline int GetMouseX  (void) {return mouse_x*win_width/win_realx;} // mouse x position, right is positive (scaled to the virtual screen so the game can use it without worrying about size variations that it can't control)
		inline int GetMouseY  (void) {return mouse_y*win_height/win_realy;} // mouse y position, down is positive (scaled to the virtual screen)
		inline int GetMouseDX (void) {return di_mousestate.lX;} // mouse x movement, right is positive
		inline int GetMouseDY (void) {return di_mousestate.lY;} // mouse y movement, down is positive
		inline int GetMouseDZ (void) {return di_mousestate.lZ;} // mouse wheel movement, up is positive

		// sets the mouse position in terms of game pixels, (0,0) being top-left corner of the game view
		// valid (x,y) are between (0,0) and (win_width-1,win_height-1) 
		void SetMousePosition (int x, int y);

		// returns true if the given mouse button is down, false otherwise
		inline bool IsMouseDown(MouseButtonID b)	 {return (di_mousestate.rgbButtons[b] & INPUT_PRESSED) ? true : false;}

		// returns true if the given mouse button is down and was up at least once since the last time this function was called on it, false otherwise
		inline bool GetMouseClick(MouseButtonID b)		  {return (di_mousepressed[b] & INPUT_PRESSED) ? true : false;}

		// returns true if the given mouse button is down and was not held down since the last time GetMouseClick was called on it, false otherwise
		// (i.e. returns true for as long as the mouse button is held down after a click, can be used for click-drags)
		inline bool PeekMouseClick(MouseButtonID b) 	  {return (di_mousepressed[b] & INPUT_PRESSED) ? true : false;}

		// returns true if any mouse button is down and was up at least once since GetMouseClick was called on it, false otherwise
		inline bool AnyMouseClick(void) 		{for(MouseButtonID b=0; b<4; b++) if(GetMouseClick(b)) return true; return false;}
		inline bool PeekAnyMouseClick(void) 	{for(MouseButtonID b=0; b<4; b++) if(PeekMouseClick(b)) return true; return false;}

		// returns true if the given mouse button is double-clicked and was up at least once since the last time this function was called on it, false otherwise
		// use this if you want to respond once to a double-click on a certain mouse button
		inline bool GetMouseDoubleClick(MouseButtonID b)  {if(di_mousepressed[b] & INPUT_DOUBLE) {di_mousepressed[b] ^= (INPUT_HELD|INPUT_DOUBLE); return true;} else return false;}

		// returns true if the given mouse button is double-clicked and was not held down since the last time GetMouseDoubleClick was called on it, false otherwise
		// (i.e. returns true for as long as the mouse button is held down after a double-click, can be used for double-click-drags)
		inline bool PeekMouseDoubleClick(MouseButtonID b) {return (di_mousepressed[b] & INPUT_DOUBLE) ? true : false;}

		//**** Internal Functions (most of these are to be made private soon) 


		inline HWND GetWindow (void)			{return win_hwnd;}	
		inline LPDIRECTDRAW7 GetDD (void)		{return dd_main;}
		inline LPDIRECTSOUND GetDS (void)		{return ds_main;}
		inline LPDIRECT3DDEVICE7 Get3DD (void)	{return d3d_device;}
		inline IDirectMusicPerformance* GetDMP (void)	{return dm_performance;}
		inline IDirectMusicLoader* GetDML (void)		{return dm_loader;}
		inline bool SupportsDepth (int bpp) 	{return (bpp == 16 || bpp == 24 || bpp == 32);}
		inline bool CanAutoPause (void) 		{return (!win_request_no_auto_pause);}	
		inline bool CanHideCursor (void)		{return (!win_request_no_hide_cursor && (win_request_no_show_cursor || win_fullscreen));}	
		inline bool CanUseMouse (void)			{return (!win_request_no_mouse_input);} 
		inline bool CanShowRunInfo (void)		{return (win_request_show_run_info);}	
		inline bool CanDeleteMData (void)		{return (win_request_delete_mdata);}	
		inline bool CanDraw (void)				{return (win_init_done && win_status == GAMEX_READY && dd_back_buf!=NULL && !dd_back_buf->IsLost());}	
		inline bool CanPlayMP3 (void)			{return mp3_support;}
		inline bool IsEscapeReserved (void) 	{return !win_request_use_escape;}
		inline bool IsAltEnterReserved (void)	{return !win_request_use_alt_enter;}
		inline bool IsQuitDialogOpen (void) 	{return quit_dialog;}
		inline bool IsResizeWorkaroundEnabled (void) {return win_resize_workaround ? true : false;}
		inline int WasLost (void)				{return dd_was_lost;}
		inline bool SimulatingSoftwareOnly(void){return win_request_software_only;}
		inline char* GetName (void) 			{return win_name;}	
		inline LPDIRECTDRAWSURFACE7 GetBackSurf (void) {return dd_back_buf;}
		bool CheckQuit (void);
		void DisplayLoadingWithValue (int percent);
		void MemoryRecordChange(int change) {memory_used_current += change; memory_used_max = max(memory_used_max,memory_used_current);}
		int GetGraphicMemUsed(void) {return memory_used_current;}
		void SetImageResolutionLimit(int size) {d3d_max_tex_width = size; d3d_max_tex_height = size;}

		bool SetUpDirectShow (void); // filter graph init

		void HideCursor(void); // game code should NEVER call this!
		void UnhideCursor(void); // game code should NEVER call this! // naming this ShowCursor would create a name conflict with the windows API

		// Automatically called by GameX -- Copies what's been drawn so far to the screen. 
		// WARNING: The game should NEVER call this function except under certain special circumstances (i.e. drawing loading screen inside GameInit).
		int UpdateView (void);

		inline GameXStatus GetStatus (void) 		   {return win_status;} // to be used internally only
		inline void SetStatus (GameXStatus status)	   {win_status = status;} // to be used internally only
 
		bool LockBackBuffer (void);   // Don't call this function! Opening graphics output is automatic now (maybe should make private)
		inline void UnlockBackBuffer (void) { if(drawLocked) {dd_back_buf->Unlock (NULL) ; drawLocked = false;} }  // unlock drawing surface
		void HandleDirectShowEvent(void );

		int GetShift (int mask);
		int GetMax (int mask);
		LPDIRECTDRAWSURFACE7 CreateImageSurface (int width, int height, bool texture, int create_alpha, bool targetable);
		bool CreateDeviceForSurface (LPDIRECT3DDEVICE7& device, LPDIRECTDRAWSURFACE7& surface);

		//*** Windows Control Functions
		// Report a Problem Initializing
		void ReportProblem (void);		
		void ReportProblem (GameXStatus problem);
		// Initialization & Clean Up Functions		
		void InitWindowsInstance (HINSTANCE hThisInst, int nWinMode);	 
		void DirectDrawError (HRESULT status);
		void Direct3DError (HRESULT status);
		bool CleanUp (void);		
		// Internal State Control Functions
		inline void SetActivation (bool act) {win_active = act;}
		inline bool GetActivation (void) {return win_active;}
		void SetPosition (int x, int y);		
		void SetProportional (void);
		void SetWarningOnDraw (bool set) {win_warning_on_draw = set;}
		void InitMouseMove (void);
		void SetMouseMove (int x, int y); // to be used internally only, the game programmer should use SetMousePosition to change the mouse position
		void SetMouseStop (int disable_length) {di_mousestop = disable_length;} // to be used internally only

		// semi-private input functions
		void AddBufferedKeyPress (char ch);
		inline void SetMouseRelease (int button)		{mouse_down[button] = 0;}
		inline void SetMousePress (int button)			{mouse_down[button] = 1;}
		inline void SetMouseDoubleClick (int button)	{mouse_down[button] = 2;}

		// used internally when a MusicX object is loaded:
		inline void RegisterMusicX (MusicX* mxp) {dm_musicx[dm_musicx_index] = mxp; if(dm_musicx_index<255) dm_musicx_index++;}

		bool UpdateInput (void);					   // Retrieve input -- called internally

		bool SurfaceToClipboard(LPDIRECTDRAWSURFACE7 surface); // copies given surface to the clipboard -- internal use

		// Public Variables
		int 			win_ytable[4096+32]; // used (in pixel drawing) by GameX's static functions, for speed
		bool			win_active; // true if GameX window is in front and active
		int 			win_realx, win_realy; // width/height of visible window/fullscreen area, and bpp
		int 			win_global_counter, win_global_frame_total;
		int 			win_cps; // speed of game -- number of times GameRun() is called per second
		int 			win_fps; // smoothness of game -- number of times GameDraw() is called per second
		int 			win_max_cpf; // "jerkiness" limit -- max cycles that can be skipped without drawing -- set really high if you want no limit
		unsigned short	f_rate; // current frame rate so game can know how many fps it's getting
		unsigned short	c_rate; // current cycle rate so game can know how many cps it's getting
		int 			p_idle; // CPU time not uses (as a percent)
		hyper			sleep_record;
		bool			win_alternator;
		LPD3DDEVICEDESC7		d3d_device_desc;
		DDPIXELFORMAT			d3d_zbuffer_format;
		bool drawLocked;
		int 					win_xpos, win_ypos; 	
		int 					win_xoffset, win_yoffset;	
 
		int win_maskr,	win_maskg,	win_maskb;
		int win_shiftr, win_shiftg, win_shiftb;
		int win_maxr,	win_maxg,	win_maxb;
		int win_maskminr, win_maskming, win_maskminb; // minimum nonzero values out of 256 (like as 6 or 8)

		char*					win_pixel_dest_data;
		int*					win_pixel_dest_ytable;
		int 					win_pixel_dest_pitch;
		float					win_pixel_dest_xmult, win_pixel_dest_ymult;

	private:		
		//**** Private Functions		

		void InternalDrawMaster(ImageX* source); // master drawing function that most drawing functions use

		// Draw an image using Direct3D (hopefully 3D-accelerated)
		int InternalDrawImageD3D(LPDIRECT3DDEVICE7 device, LPDIRECTDRAWSURFACE7 image, RECT& src_rect, RECT& src_bounds, float* x, float* y, float* z, int surf_width, int surf_height, int alphatype);

		// Draw an image using DirectDraw (hopefully 2D-accelerated)
		int InternalDrawImageDD(LPDIRECTDRAWSURFACE7 p_src_surface, LPDIRECTDRAWSURFACE7 p_dst_surface, RECT& src_rect, RECT& dst_rect, int nomask);
		void InternalDrawRectDD(int red, int green, int blue, int xLeft, int yTop, int xRight, int yBottom); // fast but cannot use any advanced features like transparency or rotation

/* // currently broken -- needs support for 32-bit mode and active rotation, at the least.
		// Draw an image by copying the pixels ourselves (no possibility for acceleration)
		int InternalDrawPlain (XBYTE* source_pix, XBYTE* dest_pix, RECT& src_rect, RECT& dst_rect, RECT& src_bounds, RECT& dst_bounds, int src_pitch, int dst_pitch, int nomask);
		int InternalDrawBlended (XBYTE* source_pix, XBYTE* dest_pix, RECT& src_rect, RECT& dst_rect, RECT& src_bounds, RECT& dst_bounds, int src_pitch, int dst_pitch, int r, int g, int b, int a, int nomask);
		int InternalDrawAlphaUnmerged (XBYTE* source_pix, XBYTE* src_alpha_pix, XBYTE* dest_pix, RECT& src_rect, RECT& dst_rect, RECT& src_bounds, RECT& dst_bounds, int src_pitch, int dst_pitch, int alphafactor);
		int InternalDrawAlphaMerged (XBYTE* source_pix, XBYTE* dest_pix, RECT& src_rect, RECT& dst_rect, RECT& src_bounds, RECT& dst_bounds, int src_pitch, int dst_pitch, int alphafactor);
		int InternalDrawAdded (XBYTE* source_pix, XBYTE* dest_pix, RECT& src_rect, RECT& dst_rect, RECT& src_bounds, RECT& dst_bounds, int src_pitch, int dst_pitch, int ri, int gi, int bi);
		int InternalDrawAddedMerged (XBYTE* source_pix, XBYTE* dest_pix, RECT& src_rect, RECT& dst_rect, RECT& src_bounds, RECT& dst_bounds, int src_pitch, int dst_pitch, int ri, int gi, int bi);
		int InternalDrawInvert (XBYTE* source_pix, XBYTE* dest_pix, RECT& src_rect, RECT& dst_rect, RECT& src_bounds, RECT& dst_bounds, int src_pitch, int dst_pitch, int ri, int gi, int bi);
*/
		// Clipping Functions:
		// source to destination (image/viewport) boundary clipping function:
		int ClipRects (RECT& src_rect, RECT& dst_rect, const RECT& src_bounds, const RECT& dst_bounds); // clips source and dest rects to their boundaries
		void ClipTile (int x, int y, int xres, int yres, int &mx, int &my, int &offx, int &offy);
		RECT ConvertRECT(RECT& rect, ImageX* img);

		inline long ToAttenuation (float vol) {return (long) (vol<=0 ? -10000 : logf(vol)*1000.f);} // converts a value on a 0.0 to 1.0 volume scale to a logarithmic volume attenuation value

		// helper function for functions that want to convert 4 sets of (r,g,b,a) to (r,g,b)
		void ScaleByAlpha (ColorX& colors);

		inline HRESULT EndRenderBatch(void) {draw_render_batch_open = false; return draw_render_batch_device->EndScene();}
		
		// Dispatched Drawing Functions
		void (*FuncDrawPixel) (int x, int y, int r, int g, int b, int a);
		void (*FuncReadPixel) (int x, int y, int &r, int &g, int &b, int &a);

		// fills quadrilateral with screen vertices,
		// returns false if it isn't on the screen
		bool GetScreenVertices(float* x, float* y, float* z);

		bool PrepareDrawState (ImageX* source, bool immediate=false);


		void SyncWithRefresh (void); // Returns once the scanline is clear of the game window or screen

		void DoInitTests (void);

		GameXDrawState* ms_iter_merge(GameXDrawState* before, GameXDrawState* f1, int n1, GameXDrawState* f2, int n2);

		// Windows & DirectX Specific Initialization
		bool InitWindowSetup (void);
		bool InitDirectX (void);
/// 	   bool InitSoundX (void);
		bool InitWindowClass (void);							// Create a window class
		bool InitWindows (void);								 // Create a main window
		bool WindowDraw (void); 								// Draw window on screen
		bool InitDirectDraw (void); 							// Initialize direct draw
		bool InitDirect3D (void);
		bool InitDirectInput (void);
		bool InitDirectSound (void);
		bool InitDirectMusic (void);
		bool TestDirectShow (void);
		void InitVSync (void);
		bool ReInitScreen(void);
		bool DirectSoundFormat (void);
		bool InitDirectDrawSurfaces (void); 				// Initialize direct draw surfaces
		bool InitDirectDrawMonitor (void);					// Initialize monitor settings for direct draw
		bool DirectXCleanUp (void); 						// Cleanup direct draw

		// Sound Functions
		void UpdateSound (SoundX* snd);
		int AddSound (SoundX* snd);

		HINSTANCE				win_hinst;	
		int 					win_hmode;

		GameXStatus 			win_status; 
		bool					win_init_done;
		int 					win_request_xsize, win_request_ysize; // requested width and height of view area
		int 					win_request_bpp; // VIDEO_16BIT makes this 16, VIDEO_32BIT makes this 32, etc., 0 means no request
		int 					win_request_full; // 0 if VIDEO_WINDOWED flag on, 1 if VIDEO_FULLSCREEN flag on, 2 if neither is on, temporarily
		int 					win_request_vsync; // 0 unless VIDEO_ALLOWREFRESHSYNC flag on
		bool					win_request_no_sound; // false unless AUDIO_DISABLE flag on
		bool					win_request_antialias; // false unless VIDEO_ALLOWANTIALIAS flag on
		bool					win_request_can_resize; // true unless VIDEO_NORESIZE flag on
		bool					win_request_use_escape; // false unless RUN_USEESCAPEKEY flag on
		bool					win_request_delete_mdata; // false unless RUN_ASSUMEVRAMSAFE flag on
		bool					win_request_use_alt_enter; // false unless NOALTENTERTOGGLE flag on
		bool					win_request_software_only; // false unless VIDEO_SOFTWAREONLY flag on
		bool					win_request_no_auto_pause; // false unless RUN_BACKGROUND flag on
		bool					win_request_show_run_info; // false unless RUN_AUTOSHOWINFO flag on
		bool					win_request_low_resolution; // false unless VIDEO_LOWRES flag on
		bool					win_request_no_hide_cursor; // false unless RUN_ALWAYSKEEPCURSOR flag on
		bool					win_request_no_show_cursor; // false unless RUN_ALWAYSHIDECURSOR flag on
		bool					win_request_no_mouse_input; // false unless RUN_NOMOUSEINPUT flag on
		bool					win_request_no_confirm_quit; // false unless RUN_NOCONFIRMQUIT flag on
		bool					win_request_slow_debug_drawing; // false unless VIDEO_SLOWDEBUGDRAWING flag on

		bool					win_status_problem_ignore [GAMEX_TOTAL_STATUS_NUMBER]; // true for each GameXStatus problem user ignores

		bool					win_fullscreen; 
		bool					win_cursor_hidden;
		bool					win_vsync_enabled; // true if vsync is on and currently ready
		int 					win_size;
		int 					win_widbits;
		int 					win_scanline_speed, win_blit_latency;
		bool					win_back_buf_in_vid_mem;
		bool					win_warning_on_draw;

		HWND					win_hwnd;

		char					win_name [64];

		char*					win_screen;
		int 			win_height, win_width; // width/height of the virtual game screen
		int 			win_pitch, win_adjustedpitch; // pitch and bpp-adjusted pitch of the virtual game screen
		int 			win_screenx, win_screeny, win_screenbpp, win_screenrefresh; // width/height of visible window/fullscreen area, and bpp
		int 			win_viewx1, win_viewy1, win_viewx2, win_viewy2;

		bool					quit_dialog;

		LPDIRECTDRAWSURFACE7	win_pixel_dest_surface;

		bool					draw_render_batch_open;
		LPDIRECT3DDEVICE7		draw_render_batch_device;

		int 					draw_3D_depth_sort; // 1 if sorting a 3D scene, 2 if actually drawing it, 0 if done or not doing either
		SceneSortMode			draw_sort_mode;
		GameXDrawState*			draw_state; // current draw state
		GameXDrawState*			draw_state_head; // head of sorted drawstate list
		GameXDrawState*			draw_state_tail; // tail of sorted drawstate list
		int 					draw_state_num;
		
		bool					win_blur_supported; // if blurring of entire game screen is fast enough to allow (should = true except on old video cards)
		int 					win_subtract_workaround; // 0 if subtraction works, 1 if works for alpha images only, 2 if doesn't work and requires black-alpha subtraction simulation (should = 0 except on old video cards)
		int 					win_intensify_workaround; // 0 if intensification works, 1 if doesn't work and regular addition should be used instead
		int 					win_addsoft_workaround; // 0 if DRAW_ADDSOFT works, 1 if doesn't work and regular addition should be used instead
		int 					win_addsharp_workaround; // 0 if DRAW_ADDSHARP works, 1 if doesn't work and regular addition should be used instead
		int 					win_ghost_workaround; // 0 if DRAW_GHOST works, 1 if it requires a color-to-alpha copied workaround image drawn normally to work
		int 					win_burn_workaround; // 0 if DRAW_BURN works, 1 if it doesn't work and should instead DRAW_INVERT then non-color-shifted DRAW_SUBTRACT
		int 					win_alpha_blend_workaround; // 0 if alpha-blend shading works on non-alpha images, 1 otherwise
		int 					win_resize_workaround; // 1 if stretched blitting is too slow to allow window resizing to stretch the game screen, 0 otherwise

		int 					win_keys;
		char					win_keybuf[WINDX_BUFSIZE];

		int 					mouse_x, mouse_y;
		int 					mouse_down[4];

		float					master_music_tempo;
		float					master_music_volume;
		MusicX*					current_music;

		int 					memory_used_current; // graphics memory use record (in KB)
		int 					memory_used_max;

		// vital DirectX field variables:

		// DirectDraw 7.0		
		LPDIRECTDRAW7			dd_main;
		LPDIRECTDRAWSURFACE7	dd_front_buf;
		LPDIRECTDRAWSURFACE7	dd_back_buf;
		LPDIRECTDRAWSURFACE7	last_texture_surface; // to remember last texture to avoid excess calling of SetTexture()
		int 					dd_was_lost;
		HRESULT 				dd_last_error;
		bool					win_no_acc_err;
		ImageX*					dd_imagex [1024];
		int 					dd_imagex_index;

		// Direct3D 7.0
		LPDIRECT3D7 			d3d_main;	 // D3D main object
		LPDIRECT3DDEVICE7		d3d_device;  // D3D rendering device
		LPDIRECTDRAWSURFACE7	d3d_zbuffer; // Z-buffer used for per-pixel depth checking of 3D scene drawing
		bool					d3d_zbuffer_enabled;
		int						d3d_max_tex_width, d3d_max_tex_height; // maximum texture size allowed, lower for decreased resolution

		// DirectInput 7.0		
		LPDIRECTINPUT			di_main;
		LPDIRECTINPUTDEVICE 	di_keyboard;
		LPDIRECTINPUTDEVICE 	di_mouse;
		unsigned char			di_keystate[256];
		unsigned char			di_keypressed[256];
		DIMOUSESTATE			di_mousestate;
		BYTE					di_mousepressed[4];
		int 					di_mousestop;
		int 					di_statMouseX, di_statMouseY;

		// DirectSound 3.1
		LPDIRECTSOUND			ds_main;
		int 					num_sounds;
		int 					max_sounds;
		int 					num_playing;
		SoundXPtr*				sound_list;

		// DirectShow (see in MusicX)
		bool					mp3_support;

		// DirectMusic
		IDirectMusicPerformance* dm_performance;
		IDirectMusicLoader*		 dm_loader;
		MusicX*					 dm_musicx [256];
		int 					 dm_musicx_index;

		bool					sound_support;
	};

	//*** Switched Drawing Routines
	static void DrawPixel16 (int x, int y, int r, int g, int b, int a=255); // Sets pixel (16-bit) -- r,g,b,a out of 255
	static void DrawPixel32 (int x, int y, int r, int g, int b, int a=255); // Sets pixel (32-bit) -- r,g,b,a out of 255
	static void ReadPixel16 (int x, int y, int &r, int &g, int &b, int &a); // Gets pixel (16-bit) -- r,g,b,a out of 255
	static void ReadPixel32 (int x, int y, int &r, int &g, int &b, int &a); // Gets pixel (32-bit) -- r,g,b,a out of 255
	// For images that must be scaled due to limitations:
	static void DrawScaledPixel16 (int x, int y, int r, int g, int b, int a=255); // Sets pixel (16-bit) -- r,g,b,a out of 255
	static void DrawScaledPixel32 (int x, int y, int r, int g, int b, int a=255); // Sets pixel (32-bit) -- r,g,b,a out of 255
	static void ReadScaledPixel16 (int x, int y, int &r, int &g, int &b, int &a); // Gets pixel (16-bit) -- r,g,b,a out of 255
	static void ReadScaledPixel32 (int x, int y, int &r, int &g, int &b, int &a); // Gets pixel (32-bit) -- r,g,b,a out of 255
	// So we can error-check the usage of drawpixel and readpixel:
	static void DrawPixelError (int x, int y, int r, int g, int b, int a=255);
	static void ReadPixelError (int x, int y, int &r, int &g, int &b, int &a);

	#ifdef GAMEX_MAIN
		WindowsDX GameX;
	#else
		extern WindowsDX GameX;

		extern void GameInit (void);

		#ifndef GAMEX_RUN_UNMANAGED

			extern void GameRun (void); // this function should update the game state but NOT draw that state
			extern void GameDraw (void); // this function should draw the game state and do as little else as possible

		#else // if user specifically defined GAMEX_RUN_UNMANAGED to disable automatic timing code,

			extern void GameMain (void); // unmanaged game loop function -- does everything, called repeatedly

		#endif // !GAMEX_RUN_UNMANAGED

	#endif // GAMEX_MAIN

#endif // !WINDX_DEF

