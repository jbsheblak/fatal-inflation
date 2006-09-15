//
// GameX - WindowsDX Class Code (including WinMain and WinProc (WindowFunc()))
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
// 
// this is a version of GameX that has been modified (by Justin Pease)


// KNOWN ISSUES:

// R5G5B5 display mode (sometimes called 15-bit mode) may not be supported,
// because I've never actually seen a monitor that uses it.
// i.e., R5G5B5 mode *should* work but has not been tested, so it should be tested sometime soon.

// Certain video cards (old RAGE cards)
// don't draw alpha images using their shading's transparency values
// (i.e. drawing a normal alpha image at 50% transparency results in it being drawn at 100%)
// This is because these video cards don't have support for more than 1 processing layer
// but perhaps a workaround can be implemented somehow?

// when resuming MIDI music that has been paused, oftentimes a single note will play for quite a few seconds
// over the rest of the music which is continuing as normal.
// this is due to the tricky use of tempo change to pause the music,
// a better solution would be to remember where it is and stop it, then start again at that point on resume,
// but this requires making use of DirectMusic 8.0 interface for PlayEx()
// (which *is* possible to do while remaining compatible with computers that only have DX7)

// In windowed mode, if you click out of a game, then click once on the game's title bar to get back into the game,
// Windows forgets to visibly activate the game window even though GameX still gets the activation message,
// resulting in the game playing sort-of in the background until it's clicked on again.
// May not be universal across computers or specific to GameX, however.


#include "gamex-win-dx.hpp"

// WindowsDX Constructor
WindowsDX::WindowsDX (void)
{	
	win_request_xsize = -1;
	win_request_ysize = -1;
	win_status = GAMEX_NO_INIT;
	win_init_done = false;
	num_sounds = 0;
	max_sounds = 0;
	sound_list = NULL;
	sound_support = false;
	drawLocked = false;
	win_warning_on_draw = false;
	strcpy(win_name, "GameX");
	draw_state = new GameXDrawState();
	draw_state_head = NULL;
	draw_state_tail = NULL;
	draw_state->cam = new CameraX();
	draw_state->dst = VIEWPORT;
	ResetDrawStates();
	for(int i=0 ; i<GAMEX_TOTAL_STATUS_NUMBER ; i++)
		win_status_problem_ignore[i] = false;
}

WindowsDX::~WindowsDX (void)
{
	// WindowsDX::CleanUp() does the uninitializing
}

// WinMain and WindowFunc functions supplied by WindowsDX
LRESULT CALLBACK WindowFunc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_MOVE: // window movement detection (non-windowed mode)
		GameX.SetPosition (LOWORD(lParam), HIWORD(lParam)) ;
		GameX.SetMouseStop(1); // so window movement doesn't register as mouse movement to the game
		break;
	case WM_CHAR: // buffered keyboard input detection, for GetKey()
		GameX.AddBufferedKeyPress ((TCHAR) wParam) ; // add key char to buffer
		return 0;
	case WM_SYSKEYDOWN: // detects pressing of system keys such as ALT
		if(wParam == VK_MENU)
			return 0; // prevent pressing the alt key on its own from pausing/freezing the game
		break;		  // (returning 0 stops Windows from taking action, breaking lets it do what it wants)
	case WM_SYSCOMMAND: // detects system commands such as closing the window, etc.
		switch(wParam & 0xFFF0) {
			case SC_CLOSE:
				if(!GameX.IsQuitDialogOpen())
					GameX.Quit();
				return 0;
			case SC_MONITORPOWER:
				if(GameX.IsFullScreen())
					return 0;
			case SC_KEYMENU:
			case SC_PREVWINDOW:
				return 0; // prevents ALT-anykey from blinking the window and beeping
						  // and prevents ALT-F6 from pausing the game
						  // but allows ALT-F4 to continue functioning as quit button
		}
		// no break
	case WM_ACTIVATEAPP: // game switched to
		// no break 	  // or
	case WM_ENTERMENULOOP: // menu opened
		if(GameX.CanDraw() && GameX.CanAutoPause() && !GameX.IsQuitDialogOpen()) {
#ifndef GAMEX_RUN_UNMANAGED
			GameX.SetWarningOnDraw(false);
			GameDraw();
#endif											// to indicate game being paused because it is in the background 
			GameX.win_alternator = 0;						// or has a menu or system command being accessed,
			GameX.SpecialPauseFade(0.125f,0.25f,0,0,32,64,64,128,160); // partially fade the screen (blue-ish)
		}
		GameX.UpdateView() ;
		break;
	case WM_KILLFOCUS: // window deactivated
		if(GameX.win_active) {
			GameX.SetActivation(false) ;
			if(GameX.CanAutoPause()) {
				GameX.PauseMusic();
				char title_str [256];
				sprintf(title_str, "Paused: %s - GameX 5.0", GameX.GetName());
				SetWindowText(GameX.GetWindow(),title_str);
			}
			if(GameX.CanHideCursor()) GameX.UnhideCursor();
			if(GameX.CanAutoPause()) 
			GameX.SetMouseStop(5); // skip 5 mouse input loops, the mouse should be ready for input by then
		}
		break;
	case WM_SETFOCUS: // window activated
		if(!GameX.win_active && GameX.win_realy>1) {
			GameX.SetActivation (true) ;
			if(GameX.CanAutoPause()) {
				GameX.ResumeMusic();
				char title_str [256];
				sprintf(title_str, "%s - GameX 5.0", GameX.GetName());
				SetWindowText(GameX.GetWindow(),title_str);
			}
			if(GameX.CanHideCursor())
				GameX.HideCursor();
		}
		break;
	case WM_PAINT: // redraw of game screen requested
		GameX.UpdateView(); // (in addition to the frequent timed updates, in case game is suspended)
		break;
	case WM_CLOSE: // close box or command chosen
		if(GameX.CheckQuit()) {
			GameX.SetStatus(GAMEX_SHUTTINGDOWN);
			DestroyWindow (hWnd) ; // quit the game by activating WM_DESTROY case next
		}
		else
			return 0; // user decided against quitting, continue game
		break;
	case WM_DESTROY:
		PostQuitMessage(0) ; // post the message that actually quits the application/game
		break;

	// these are used to check to make sure DirectInput's mouse presses are within the window
	case WM_LBUTTONDOWN:	GameX.SetMousePress(MOUSE_LEFT);			return 0;
	case WM_MBUTTONDOWN:	GameX.SetMousePress(MOUSE_MIDDLE);			return 0;
	case WM_RBUTTONDOWN:	GameX.SetMousePress(MOUSE_RIGHT);			return 0;

	// these are used to allow double-click detection (only works if CS_DBLCLKS is set on our WNDCLASS.style)
	case WM_LBUTTONDBLCLK:	GameX.SetMouseDoubleClick(MOUSE_LEFT);		return 0;
	case WM_MBUTTONDBLCLK:	GameX.SetMouseDoubleClick(MOUSE_MIDDLE);	return 0;
	case WM_RBUTTONDBLCLK:	GameX.SetMouseDoubleClick(MOUSE_RIGHT); 	return 0;

	case WM_SIZE: // user resizes the game window in windowed mode
		if(GameX.GetStatus() == GAMEX_READY) {
			if(HIWORD(lParam) < 1) {
				PostMessage(hWnd,WM_KILLFOCUS,0,0);
			} else {
				PostMessage(hWnd,WM_SETFOCUS,0,0);
			}

			if(!GameX.IsResizeWorkaroundEnabled()) {
				GameX.win_realx = LOWORD(lParam);
				GameX.win_realy = HIWORD(lParam);
				GameX.SetProportional();
			} else {
				GameX.win_xoffset = (LOWORD(lParam) - GameX.GetWidth()) / 2;
				GameX.win_yoffset = (HIWORD(lParam) - GameX.GetHeight()) / 2;				
				GameX.win_realx = GameX.GetWidth();
				GameX.win_realy = GameX.GetHeight();
			}

			if(GameX.win_realy < 1) GameX.win_realy = 1; // avoids crash
			if(GameX.win_realx < 1) GameX.win_realx = 1;
			GameX.SetMouseStop(1); // so window resize doesn't register as mouse movement to the game
		}
		break;
	case WM_DIRECTSHOW_EVENT:
		GameX.HandleDirectShowEvent();
		break;
	case WM_KEYUP:
		if(wParam == VK_SNAPSHOT) { // ("Prnt Scrn" button released)
		#ifndef GAMEX_RUN_UNMANAGED
			if(GameX.CanShowRunInfo()) {GameX.SetWarningOnDraw(false); GameDraw();} // clear debug info if it's on
		#endif
			ImageExt ie; ie.Save("screenshot.bmp",VIEWPORT); // make taking screenshots easier
			GameX.CopyToClipboard(VIEWPORT); // copy back buffer directly to clipboard as well as to file
			return 0;
		}
		break;
	}
	
	return DefWindowProc (hWnd, message, wParam, lParam) ;		
}

void WindowsDX::HandleDirectShowEvent() {
	if(current_music == NULL) return;

	long evCode, param1, param2;
	HRESULT status;
	do {
		status = current_music->dsh_me->GetEvent(&evCode, &param1, &param2, 0);
		if(SUCCEEDED(status)) {
			status = current_music->dsh_me->FreeEventParams(evCode, param1, param2);
			if(SUCCEEDED(status)) { 
				switch(evCode) {
				case EC_COMPLETE:
					if(current_music->playing_loops_left)
						current_music->playing_loops_left--;
					else {
						current_music->dsh_mc->Stop();
					}

					current_music->dsh_mp->put_CurrentPosition(0);
					break;
				case EC_USERABORT:
					current_music->dsh_mc->Stop();
					break;
				}
			}
		}
	} while(SUCCEEDED(status));
}

void WindowsDX::InfoDialog(char * message)
{
	SpecialPauseFade(0.125f,0.25f,0,0,32,64,64,128,160);
	MessageBox (win_hwnd, message, "Notice", MB_OK|MB_DEFBUTTON1|MB_ICONINFORMATION);
}

void WindowsDX::WarningDialog(char * message)
{
	SpecialPauseFade(0.125f,0.25f,64,128,64,128,0,0,160);
	MessageBox (win_hwnd, message, "Warning", MB_OK|MB_DEFBUTTON1|MB_ICONWARNING);
}

int WindowsDX::ChoiceDialog(char * message, bool three_choices, int default_choice)
{
	SpecialPauseFade(0.125f,0.25f,0,0,64,128,32,64,160);
	int option = three_choices ? MB_YESNOCANCEL : MB_YESNO;
	switch(default_choice) {
		case 1:  option |= MB_DEFBUTTON1; break;
		case 0:  option |= MB_DEFBUTTON2; break;
		case -1: if(three_choices) option |= MB_DEFBUTTON3; break;
	}
	int choice = MessageBox (win_hwnd, message, "Query", option|MB_ICONQUESTION);
	switch(choice) {
	case IDCANCEL:
		return -1;
	case IDNO:
		return 0;
	default:
	case IDYES:
		return 1;
	}
}

void WindowsDX::ErrorDialog(char * message)
{
	SpecialPauseFade(0.125f,0.25f,64,128,0,0,32,64,160);
	MessageBox (win_hwnd, message, "Error", MB_OK|MB_DEFBUTTON1|MB_ICONERROR);
}

int WindowsDX::ErrorChoiceDialog(char * message, int default_choice)
{
	SpecialPauseFade(0.125f,0.25f,64,128,0,0,32,64,160);
	int option = MB_ABORTRETRYIGNORE;
	switch(default_choice) {
		case -1:  option |= MB_DEFBUTTON1; break;
		case 1:  option |= MB_DEFBUTTON2; break;
		case 0: option |= MB_DEFBUTTON3; break;
	}
	int choice = MessageBox (win_hwnd, message, "Error", option|MB_ICONERROR);
	switch(choice) {
	case IDABORT:
		return -1;
	case IDIGNORE:
		return 0;
	default:
	case IDRETRY:
		return 1;
	}
}

bool WindowsDX::CheckQuit(void)
{
	if(win_request_no_confirm_quit) { // if quit confirmation is off
		quit_dialog = false;
		return true; // signal to exit immediately
	}
	else {

		quit_dialog = true;
		int choice = 0;

		bool hidden_cursor = win_cursor_hidden;
		if(hidden_cursor) {
			UnhideCursor();
			win_request_no_hide_cursor = true; // avoids re-hiding
		}

		choice = MessageBox (win_hwnd, "Are you sure you want to exit?", GameX.GetName(), MB_YESNO|MB_DEFBUTTON1|MB_ICONQUESTION);

		if(hidden_cursor) {
			win_request_no_hide_cursor = false; // reset to hide
			HideCursor();
		}

		quit_dialog = false;

		return (choice == IDYES); // if true, the game quits, if false, the game keeps going
	}
}

// returns the current time since startup with as much precision as this hardware supports,
// this will probably be in some ultra-precise unit, could be in fractions of a nanosecond
// (use GetClocksPerSec() to find out exactly how many of these units are in a second)
inline static hyper GetClock(void)
{
	LARGE_INTEGER time;
	if(QueryPerformanceCounter(&time)) // get the High Performance Timer count
		return (hyper)time.QuadPart;
	else // if QueryPerformanceCounter() failed, that means the hardware doesn't support high-resolution timers
		return (hyper)clock(); // so use the built-in low-resolution timer, clock(), instead. (only accurate to a 100th of a second)
}

// returns the number of time units (that GetClock() counts in) that are in one second.
// for instance, if this function returns 3579545, that means there are 3,579,545 clock ticks per second.
// note: you can assume the return value is a constant, although not across different computers.
inline static hyper GetClocksPerSec(void)
{
	LARGE_INTEGER frequency;
	if(QueryPerformanceFrequency(&frequency)) // get the High Performance Timer frequency in Hz
		return (hyper)frequency.QuadPart;
	else // if QueryPerformanceFrequency() failed, that means the hardware doesn't support high-resolution timers
		return (hyper)CLOCKS_PER_SEC; // so use the low-resolution timer's "per second" value
}

// sleeps for at least the given number of time units,
// at most a bit over that amount,
// and increments GameX's idle counter accordingly.
// will only Sleep() in units of sleep_step milliseconds,
// so higher sleep_step means more accuracy but less actual sleeping  
// sleep_step should be an integer from 1 to 10 inclusive,
// clocks is the amount of time to sleep/delay in terms of GetClocksPerSec(), NOT milliseconds
inline static void AccurateSleep(hyper clocks, int sleep_step = 1)
{
	if(clocks <= 0) return; // wouldn't want to freeze upon sleeping a negative amount, also return quickly on 0 sleep
	hyper sleeptimer = GetClock();
	int sleepAmount = (int)((clocks+1)/(GetClocksPerSec()/CLOCKS_PER_SEC));
	Sleep((sleepAmount/sleep_step)*sleep_step); // sleep for the amount of time that fits into sleep_step millisecond units
	GameX.sleep_record += GetClock() - sleeptimer;
	while(GetClock() - sleeptimer < clocks) ; // loop tightly for any time leftover
}


int WINAPI WinMain (HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	MSG msg;
	
	GameX.InitWindowsInstance (hThisInst, nWinMode);

	GameInit() ; // defined by game, hopefully calls GameX.Initialize()

	if (GameX.GetStatus() != GAMEX_READY) {
		GameX.ReportProblem() ;
		GameX.SetStatus(GAMEX_READY); // after temp auto-init
	} else {
		GameX.DisplayLoadingWithValue (90);
		Sleep(750); // wait until async parts of DX init should be complete, to avoid pause on 1st frame
		GameX.DisplayLoadingWithValue (100);
		Sleep(750); // (total wait of 1500 milliseconds is sometimes necessary)
	}
#ifdef WINDX_DEBUG
	char str[128];
	sprintf(str,"-- Graphic/Sound memory in use after GameInit(): %d KB", GameX.GetGraphicMemUsed());
	debug.Output(str);
	debug.Output("WinDX: Running Game...");
#endif

	int frame=0, cycle=0; // so we can keep track of the frame rate
	GameX.sleep_record = 0; // to keep track of how much time we spend sleeping

	GameX.win_global_counter = 0;
	GameX.win_global_frame_total = 0;
	hyper cps = GameX.win_cps; // cycles per second (a cycle meaning a call to GameRun(), not a CPU cycle)
	hyper fps = GameX.win_fps; // frames per second (a frame meaning a call to GameDraw() and UpdateView())
	hyper tpc = GetClocksPerSec() / (cps?cps:1); // time per cycle (number of clock ticks between each game update)
	hyper tpf = GetClocksPerSec() / (fps?fps:1); // time per frame (number of clock ticks between each view update)
	bool needUpdate = false; // (start as false because GameRun() should be called first before GameDraw())
	bool update_auto_status = true;
	hyper cycleTime = GetClock(); // timer for game update
	hyper frameTime = GetClock(); // timer for view update
	hyper frameClock = GetClock(); // timer for framerate calculation update
	int leftover = 0;
	for(;;) {
		if(PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg) ;
			DispatchMessage(&msg) ;
		}
		if(!GameX.win_active && GameX.CanAutoPause()) { // if the window isn't active, and we can pause,
			Sleep(10) ; // sleep so we don't max out the cpu in the background
			cycleTime = GetClock(); // set the timers to the current time
			frameTime = GetClock(); // so we don't skip a ton of frames when resuming
		}
		else {

		#ifndef GAMEX_RUN_UNMANAGED

			// AUTOMATIC GAME SPEED AND FRAME RATE MANAGEMENT:
			// This ensures that the game always runs at a given speed (chosen by the game programmer)
			// regardless of how quickly the computer is able to execute the game's drawing/instructions.

			// CPS means Cycles Per Second. Higher CPS means faster gameplay.	GameRun() is called win_cps times per second.
			// FPS means Frames Per Second. Higher FPS means smoother gameplay. GameDraw() is called win_fps times per second.
			// Attaining the target CPS (game speed) is nearly guaranteed under normal circumstances, (< 10000 cps)
			// whereas attaining the target FPS (game smoothness) isn't always possible, depending on the game and cpu.

			// Warning: the following code is VERY "fragile" due to its time-sensitive nature, so even
			// the most innocent modification is likely to cause subtle problems if not drastic failure.

			// first check to see if the game changed the cps or fps and adjust accordingly
			if(cps != GameX.win_cps) {
				cps = GameX.win_cps;
				tpc = GetClocksPerSec() / cps;
				if(fps > cps) tpf = tpc;
				else tpf = GetClocksPerSec() / fps;
				update_auto_status = true;

			}
			if(fps != GameX.win_fps) {
				fps = GameX.win_fps;
				tpf = GetClocksPerSec() / fps;
				tpf = max(tpf,tpc);
				update_auto_status = true;
			}

			// sleep while draw update would exceed game update
			if(!needUpdate) AccurateSleep(tpc+cycleTime-GetClock());

			hyper thisTime = GetClock(); // variable to hold pre-action reference time

			int cpf_counter = 0; // number of times to update the game at this instant,
								 // if > 1 then we skip (cpf_counter-1) frames by updating again before drawing them

			// calculate how many times we need to update the game
			while(thisTime-cycleTime > tpc) {
				cycleTime += tpc;
				cpf_counter++;
			}
			// limit number of times to the maximum allowable updates per frame
			if(cpf_counter > GameX.win_max_cpf) {
				cpf_counter = GameX.win_max_cpf;
				frameTime = GetClock();
			}
/*
			// smooth out delay discrepencies between frames:
			// smoothing method 1:
			int new_todo = cpf_counter;
			int total_todo = new_todo + leftover;
			int trying = total_todo ? 1 + total_todo / 2 : 0;
			leftover = total_todo - trying;
			cpf_counter = trying;
*/
			// smoothing method 2:
			// (difficult to tell which is better -- method 1 is more correct in terms of achieving exactly the desired rate,
			//	but whatever rate it reaches sometimes appears slightly choppier than the same rate achieved with method 2)
			int ave = (cpf_counter+cpf_counter+cpf_counter + leftover) / 4;
			int total = cpf_counter + leftover;
			cpf_counter = total - ave; // takes into account the integer-divided nature of ave
			leftover = ave;

			if(cpf_counter) { // if we need to update the game state at all since last time:
				GameX.SetWarningOnDraw(true);
				while(cpf_counter--) { // update (without drawing) as many times as calculated
					GameX.UpdateInput(); // first update the input so GameRun() has fresh input data
					GameRun(); // call GameRun() function, whose contents decide the behaviour of the game
					cycle++; // increment counter so we can know how many cps we're really getting
					GameX.win_global_counter++;
					if(cycleTime > thisTime) { // correct overflow
						cycleTime = thisTime;
						cpf_counter = 0;
					}
				}
				needUpdate = true; // remember that the game screen now needs to be drawn again
				GameX.SetWarningOnDraw(false);
			}

			// sleep until the next draw update:
			AccurateSleep(tpf+frameTime-GetClock());

			// draw the game as necessary:
			thisTime = GetClock();
			if(thisTime-frameTime > tpf) { // executes at most once per loop
				if(needUpdate) {
					if(needUpdate && GameX.CanDraw()) {
						GameDraw(); // call GameDraw() function, whose contents draw the current game state
									// also draw game status if RUN_AUTOSHOWINFO is enabled:
						if(GameX.CanShowRunInfo()) {
							GameX.DrawStatus(update_auto_status);
							if(update_auto_status) update_auto_status = false;
						}
					}
					GameX.UpdateView(); // update onscreen view
					frame++; // increment counter so we can know how many fps we're really getting
					GameX.win_global_frame_total++;
					GameX.win_alternator = !GameX.win_alternator;
					frameTime = min(frameTime+tpf, thisTime);
					needUpdate = false;
				}
			} 

		#else
			// RUN_UNMANAGED (is defined):

			GameX.UpdateInput();
			GameMain();
			cycle++;
			frame++;
			GameX.win_global_counter++;
			GameX.win_global_frame_total++;

			if(GameX.CanShowRunInfo()) {
				GameX.DrawStatus(update_auto_status);
				if(update_auto_status) update_auto_status = false;
			}

			GameX.UpdateView();

		#endif

			hyper thetime = GetClock() - frameClock;// calculate the frame and cycle rate every second
			if(thetime >= GetClocksPerSec()) {		// so the game doesn't need to calculate it itself
				GameX.f_rate = frame;				// and can instead call GameX.GetCurrentFrameRate().
				GameX.c_rate = cycle;
				GameX.p_idle = (int)(100.0*(double)GameX.sleep_record/(double)GetClocksPerSec()); // calculate idle time percent (percent of power conserved)
				if(GameX.p_idle < 0 || GameX.p_idle > 99) GameX.p_idle = 99;
				frame = 0; cycle = 0; GameX.sleep_record = 0;
				frameClock = GetClock();
				frameTime = frameClock; // prevent long-term frame loss compensation
				update_auto_status = true;
			}
		}
	}
#ifdef WINDX_DEBUG
	debug.Output("WinDX: Exiting normally...");
#endif
	GameX.CleanUp() ;
	return msg.wParam;
}

// WindowsDX Member Functions

// The game can call this to exit
void WindowsDX::Quit (bool unrecoverable)
{
	if(unrecoverable) { // immediately quit if it's a fatal-error-type quit
#ifdef WINDX_DEBUG
		debug.Output("WinDX: Exiting immediately...");
#endif
		GameX.CleanUp() ;
		exit(-1);
	}

	if(!(win_request_no_confirm_quit)) {
		quit_dialog = true;
#ifndef GAMEX_RUN_UNMANAGED
		SetWarningOnDraw(false);
		GameDraw();
#endif

		SpecialPauseFade(0.125f,0.25f,64,128,32,64,0,0,160);
		UpdateView();
	}

	SendMessage(win_hwnd,WM_CLOSE,0,0); // SendMessage instead of PostMessage, for immediate action
}

// This function must be called by the game's GameInit() function
// Parameters: Name, Flags, ViewWidth, ViewHeight, [optional:]	Speed, Smoothness, SkipLimit 
bool WindowsDX::Initialize (char * name, GameXInitFlags options, int xres, int yres, int speed)
{
	strcpy(win_name,name); // remember game name

	ResetRandomSeed(); // set the random number seed in case the game forgets to

///#ifdef _DEBUG
	if(options & VIDEO_WINDOWED && options & VIDEO_FULLSCREEN) {
		MessageBox (win_hwnd, "WARNING: VIDEO_WINDOWED and VIDEO_FULLSCREEN are mutually exclusive flags.\nLeave out both flags if you want the user to choose.", "GameX Warning",MB_OK|MB_ICONWARNING) ;
		options ^= VIDEO_WINDOWED;
		options ^= VIDEO_FULLSCREEN;
	}
///#endif

	quit_dialog = false;

	if(options & VIDEO_WINDOWED)
		win_request_full = 0;
	else if(options & VIDEO_FULLSCREEN)
		win_request_full = 1;
	else
		win_request_full = 2;

	if(options & VIDEO_NORESIZE) {
		win_request_can_resize = false;
	} else {
		win_request_can_resize = true;
	}

	if(options & VIDEO_LOWRES) {
		win_request_low_resolution = true;
	} else {
		win_request_low_resolution = false;
	}


	if(win_request_full == 2) {
		int style = MB_YESNOCANCEL|MB_ICONQUESTION;

		#ifdef _DEBUG
			style |= MB_DEFBUTTON2;
		#endif

		int choice = MessageBox (NULL, "Do you want to play this game in fullscreen mode?", win_name, style) ;
		if(choice == IDYES)
			win_request_full = 1;
		else if(choice == IDNO)
			win_request_full = 0;
		else if(choice == IDCANCEL)
			Quit(true);
	}

	if(options & VIDEO_ALLOWREFRESHSYNC)
		win_request_vsync = 1;
	else
		win_request_vsync = 0;

	if(options & VIDEO_SOFTWAREONLY) {
		MessageBox (win_hwnd, "WARNING: Entering software-only mode because VIDEO_SOFTWAREONLY was turned on.\nThis is just a reminder: don't forget to remove that flag.", "GameX Notice",MB_OK|MB_ICONINFORMATION) ;
		win_request_software_only = true;
	}
	else
		win_request_software_only = false;

	if(options & VIDEO_SLOWDEBUGDRAWING) {
		MessageBox (win_hwnd, "WARNING: Entering ultra-slow drawing mode because the VIDEO_SLOWDEBUGDRAWING was set.\nJust a reminder: don't forget to remove that flag.", "GameX Notice",MB_OK|MB_ICONINFORMATION) ;
		win_request_slow_debug_drawing = true;
//		speed = 5;
	}
	else
		win_request_slow_debug_drawing = false;

	win_request_delete_mdata = true;

	win_request_bpp = 0;
	if(options & VIDEO_16BIT) {
		win_request_bpp = 16; // 16 bit color is fully supported by GameX
	} else if(options & VIDEO_24BIT) {
		win_request_bpp = 24;
		MessageBox (win_hwnd, "WARNING: 24-bit RGB color mode has not yet been tested.", "GameX Warning",MB_OK|MB_ICONWARNING) ;
	} else if(options & VIDEO_32BIT) {
		win_request_bpp = 32; // 32 bit color is fully supported by GameX -- may be a little slow though
	}

///#ifdef _DEBUG
	if(1 < /*!!(options & VIDEO_8BIT)+*/!!(options & VIDEO_16BIT)+!!(options & VIDEO_24BIT)+!!(options & VIDEO_32BIT)) {
		MessageBox (win_hwnd, "ERROR: You cannot select more than one video mode at once.\nPlease initialize GameX with only one VIDEO_??BIT flag.", "GameX Error",MB_OK|MB_ICONSTOP) ;
	} else if(win_request_bpp == -1) {
		MessageBox (win_hwnd, "WARNING: You should select a video mode by initializing GameX with a video color depth flag.\nFor example,\nGameX.Initialize(\"My Game\",VIDEO_16BIT,640,480);\nor\nGameX.Initialize(\"My Game\",VIDEO_32BIT|VIDEO_FULLSCREEN,800,600);", "GameX Warning",MB_OK|MB_ICONWARNING) ;
	}
///#endif

	win_request_xsize = xres;
	win_request_ysize = yres;

	win_cps = speed; // see WinGameRun() for usage

	if(win_cps < 5 || win_cps > 600) {
///#ifdef _DEBUG
		char disp [500] ;
		sprintf (disp, "WARNING: You have specified %d CPS, which is invalid. The CPS must be at least 5, at most 600, and normally should be between 30 and 120.", win_cps) ;
		MessageBox (win_hwnd, disp, "GameX Warning",MB_OK|MB_ICONWARNING) ;
///#endif
		if(win_cps < 5) win_cps = 5;
		else if(win_cps > 600) win_cps = 600;
	}

	win_fps = -1; // default of targeting the best possible framerate that's visible
	win_max_cpf = max(3,win_cps/5); // default to skip no more than 1/5 of a second of frames per instant


	if(options & AUDIO_DISABLE)
		win_request_no_sound = true;
	else
		win_request_no_sound = false;

	if(options & RUN_USEESCAPEKEY)
		win_request_use_escape = true;
	else
		win_request_use_escape = false;

	if(options & RUN_NOALTENTERTOGGLE)
		win_request_use_alt_enter = true;
	else
		win_request_use_alt_enter = false;

	if(options & RUN_AUTOSHOWINFO)
		win_request_show_run_info = true;
	else
		win_request_show_run_info = false;

	if(options & RUN_BACKGROUND)
		win_request_no_auto_pause = true;
	else
		win_request_no_auto_pause = false;

	if(options & RUN_ALWAYSKEEPCURSOR && options & RUN_ALWAYSHIDECURSOR) {
///#ifdef _DEBUG
		MessageBox (win_hwnd, "WARNING: RUN_ALWAYSKEEPCURSOR and RUN_ALWAYSHIDECURSOR are mutually exclusive flags.", "GameX Warning",MB_OK|MB_ICONWARNING) ;
///#endif
		options ^= RUN_ALWAYSKEEPCURSOR;
		options ^= RUN_ALWAYSHIDECURSOR;
	}

	if(options & RUN_ALWAYSKEEPCURSOR)
		win_request_no_hide_cursor = true;
	else
		win_request_no_hide_cursor = false;

	if(options & RUN_ALWAYSHIDECURSOR)
		win_request_no_show_cursor = true;
	else
		win_request_no_show_cursor = false;

	if(options & RUN_NOMOUSEINPUT)
		win_request_no_mouse_input = true;
	else
		win_request_no_mouse_input = false;

	if(options & RUN_NOCONFIRMQUIT)
		win_request_no_confirm_quit = true;
	else
		win_request_no_confirm_quit = false;

	bool success = true;

	debug.Open ("debug.txt") ;
#ifdef WINDX_DEBUG
	debug.Output ("WindowsDX: Initialization started.") ; 
#endif

	if (InitWindowSetup()==false) success = false;
	if(success)
	if (InitDirectX()==false) success = false;
	
	if(!success) {
		ReportProblem(GetStatus());
		debug.Output ("WindowsDX: Initialization had problems.") ;
		return false;
	}

	DoInitTests();

	DisplayLoadingWithValue (80);

#ifdef WINDX_DEBUG
	debug.Output ("WindowsDX: Initialization finished OK.") ;
#endif
	win_init_done = true;

	return true;
}

void WindowsDX::DoInitTests (void)
{
	win_status = GAMEX_CREATING_TEST_SURFACE;

	// test of blurring speed:
	win_blur_supported = true; // temporarily true to allow testing
	int trials = 10;
	hyper timer = GetClock();
	for(int i = 0 ; i < trials ; i++) {
		BlurScreen(0.51f);
	}
	timer = GetClock() - timer;
	double testTime = (double)(timer) / (double)GetClocksPerSec() / (double)trials;
	double minTime = 0.025*(double)(win_width*win_height)/(double)(800*600); // minimum acceptable time for test (5x slower than decent speed of 0.005 seconds/trial/(800*600)pixels)
	if(testTime > minTime) {
		debug.Output("WinDX: Blur speed test FAILED, blurring disabled.");
		win_blur_supported = false;
	} else {
		win_blur_supported = true;
	}



	// fill the background with 50% gray

	// (make sure it worked, while we're at it:)
	int r=0,g=0,b=0;
	int tol = 24; // tolerance allowed in each direction from expected color component -- necessary because of color format imprecision as well as slight deviations in video card processing results

	// create test images:
	ImageX testImage, testImage2, testAlphaImage, testDDImage;
	testImage.Create(16,16);
	testImage.Fill(255,0,0); // fill with bright red
	testImage2.Create(16,16);
	testImage2.Fill(192,128,64); // fill with brownish color
	testAlphaImage.Create(16,16,true);
	testAlphaImage.Fill(255,0,0,255); // fill with bright red (full opacity)
	testDDImage.Create(64,64);
	testDDImage.ConvertUsageTo(LOAD_NON3D);
	testDDImage.Fill(255,0,0); // fill with bright red

	// Draw Mode Tests:
	// Some video cards handle certain drawing modes improperly,
	// so we test for correctness here and enable work-arounds as necessary:

	// Subtractive (DRAW_SUBTRACT mode) Drawing Test:

	FillScreen(ColorX(128,128,128)); // (on top of a 50% gray background)

	win_subtract_workaround = 0; // try without a workaround first
	SetDrawMode(DRAW_SUBTRACT);
	DrawImage(&testImage, 0,0);

	AccessPixels(VIEWPORT);
	ReadPixel(0,0,r,g,b); // (r,g,b) should equal (0,128,128) after this line
	EndPixelAccess();
	if(r>0+tol || r<0-tol || g>128+tol || g<128-tol || b>128+tol || b<128-tol) {
		if(win_fullscreen) ToggleFullscreen();
		
		FillScreen(ColorX(128,128,128));
		SetDrawMode(DRAW_SUBTRACT);
		DrawImage(&testAlphaImage, 0,0);

		AccessPixels(VIEWPORT);
		ReadPixel(0,0,r,g,b); // (r,g,b) should equal (0,128,128) after this line
		EndPixelAccess();

		if(r>0+tol || r<0-tol || g>128+tol || g<128-tol || b>128+tol || b<128-tol) {
			debug.Output("WinDX: Subtractive drawing test FAILED, black-alpha work-around enabled.");
			win_subtract_workaround = 2; // black-alpha subtraction simulation required (worst case)  uses image with all black and varying alpha image to darken destination non-subtractively
		} else {
			debug.Output("WinDX: Subtractive drawing test FAILED, 32-bit work-around enabled.");
			win_subtract_workaround = 1; // subtractive drawing works with alpha images although not normally, so enable workaround that converts images to alpha if drawn subtractively
		}
		
	} else {
		win_subtract_workaround = 0; // no workaround required
	}

	// Multiplicatively Additive (DRAW_INTENSIFY mode) Drawing Test:

	FillScreen(ColorX(128,128,128)); // (on top of a 50% gray background)

	win_intensify_workaround = 0; // try without a workaround first
	SetDrawMode(DRAW_INTENSIFY);
	DrawImage(&testImage, 0,0);

	AccessPixels(VIEWPORT);
	ReadPixel(0,0,r,g,b); // (r,g,b) should equal (255,128,128) after this line
	EndPixelAccess();
	if(r>255+tol || r<255-tol || g>128+tol || g<128-tol || b>128+tol || b<128-tol) {
		debug.Output("WinDX: Intensify drawing test FAILED, additive work-around enabled.");
		win_intensify_workaround = 1; // workaround: simply use regular addition instead of multiplicative addition
	} else {
		win_intensify_workaround = 0; // no workaround required
	}

	// Color-valued Alpha (DRAW_GHOST mode) Drawing Test:

	FillScreen(ColorX(128,128,128)); // (on top of a 50% gray background)

	win_ghost_workaround = 0; // try without a workaround first
	SetDrawMode(DRAW_GHOST);
	DrawImage(&testImage2, 0,0);

	AccessPixels(VIEWPORT);
	ReadPixel(0,0,r,g,b); // (r,g,b) should equal (192,128,112) after this line
	EndPixelAccess();
	if(r>192+tol || r<192-tol || g>128+tol || g<128-tol || b>112+tol || b<112-tol) {
		debug.Output("WinDX: Ghost drawing test FAILED, alpha work-around enabled.");
		win_ghost_workaround = 1; // workaround required
	} else {
		win_ghost_workaround = 0; // no workaround required
	}

	// DRAW_BURN Drawing Test:

	FillScreen(ColorX(128,128,128)); // (on top of a 50% gray background)

	win_burn_workaround = 0; // try without a workaround first
	SetDrawMode(DRAW_BURN);
	DrawImage(&testImage2, 0,0);

	AccessPixels(VIEWPORT);
	ReadPixel(0,0,r,g,b); // (r,g,b) should equal (64,128,144) after this line
	EndPixelAccess();
	if(r>64+tol || r<64-tol || g>128+tol || g<128-tol || b>144+tol || b<144-tol) {
		debug.Output("WinDX: Burn drawing test FAILED, invert+subtract work-around enabled.");
		win_burn_workaround = 1; // workaround required
	} else {
		win_burn_workaround = 0; // no workaround required
	}

	
	// DRAW_ADDSOFT Drawing Test:

	FillScreen(ColorX(128,128,128)); // (on top of a 50% gray background)

	win_addsoft_workaround = 0; // try without a workaround first
	SetDrawMode(DRAW_ADDSOFT);
	DrawImage(&testImage2, 0,0);

	AccessPixels(VIEWPORT);
	ReadPixel(0,0,r,g,b); // (r,g,b) should equal (224,192,160) after this line
	EndPixelAccess();
	if(r>224+tol || r<224-tol || g>192+tol || g<192-tol || b>160+tol || b<160-tol) {
		debug.Output("WinDX: Soft additive drawing test FAILED, regular addition work-around enabled.");
		win_addsoft_workaround = 1; // workaround: simply use regular addition instead
	} else {
		win_addsoft_workaround = 0; // no workaround required
	}


	// DRAW_ADDSHARP Drawing Test:

	FillScreen(ColorX(0,0,0)); // (on top of a black background)

	win_addsharp_workaround = 0; // try without a workaround first
	SetDrawMode(DRAW_ADDSHARP);
	DrawImage(&testImage2, 0,0);

	AccessPixels(VIEWPORT);
	ReadPixel(0,0,r,g,b); // (r,g,b) should equal (255,255,64) after this line
	EndPixelAccess();
	if(r>255+tol || r<255-tol || g>255+tol || g<255-tol || b>64+tol || b<64-tol) {
		debug.Output("WinDX: Sharp additive drawing test FAILED, regular addition work-around enabled.");
		win_addsharp_workaround = 1; // workaround: simply use regular addition instead
	} else {
		win_addsharp_workaround = 0; // no workaround required
	}


	// Alpha-blending Test:
	// (some video cards don't apply alpha blending properly except on alpha images or non-textured polygons)
	FillScreen(ColorX(128,128,128)); // (on top of a 50% gray background)

	win_alpha_blend_workaround = 0; // try without a workaround first
	SetDrawMode(DRAW_PLAIN);
	SetDrawShading(ColorX(255,255,255,128));
	DrawImage(&testImage, 0,0);

	AccessPixels(VIEWPORT);
	ReadPixel(0,0,r,g,b); // (r,g,b) should about equal (192,64,64) after this line
	EndPixelAccess();
	if(r>192+tol || r<192-tol || g>64+tol || g<64-tol || b>64+tol || b<64-tol) {
		FillScreen(ColorX(128,128,128));
		SetDrawMode(DRAW_PLAIN);
		SetDrawShading(ColorX(255,255,255,128));
		DrawImage(&testAlphaImage, 0,0);

		AccessPixels(VIEWPORT);
		ReadPixel(0,0,r,g,b); // (r,g,b) should about equal (192,64,64) after this line
		EndPixelAccess();

		if(r>192+tol || r<192-tol || g>64+tol || g<64-tol || b>64+tol || b<64-tol) {
			debug.Output("WinDX: Alpha-blend shading test FAILED, some alpha shading disabled.");
			win_alpha_blend_workaround = 0; // no workaround known in this case
		} else {
			debug.Output("WinDX: Alpha-blend shading test FAILED, 32-bit work-around enabled.");
			win_alpha_blend_workaround = 1; // if works on alpha images, convert images to 32-bit if drawn with alpha shading
		}
	} else {
		win_alpha_blend_workaround = 0; // no workaround required
	}

	double nonStretchTime, stretchTime;
	{
		int trials = 50;
		for(int i = 0 ; i < trials ; i++)
			DrawTexturedRect(&testDDImage, 0,0,64,64);
	}
	{
		// test of non-stretching speed:
		int trials = 25;
		hyper timer = GetClock();
		for(int i = 0 ; i < trials ; i++) {
			DrawTexturedRect(&testDDImage, 0,0,64,64);
		} 
		timer = GetClock() - timer;
		nonStretchTime = 1000.0 * (double)(timer) / (double)GetClocksPerSec() / (double)trials;
	}
	{
		// test of stretching speed:
		int swidth = min(400,GameX.GetWidth());
		int sheight = min(400,GameX.GetHeight());
		int trials = 25;
		hyper timer = GetClock();
		for(int i = 0 ; i < trials ; i++) {
			DrawTexturedRect(&testDDImage, 0,0,swidth,sheight);
		}
		timer = GetClock() - timer;
		stretchTime = 1000.0 * (double)(timer) / (double)GetClocksPerSec() / (double)trials;
	}

	if(stretchTime > nonStretchTime*10.0f) {
		win_resize_workaround = 1;

		RECT rect; // in case window began as too large already:
		GetClientRect (win_hwnd, &rect) ;
		win_xoffset = (rect.right-rect.left - win_width) / 2;
		win_yoffset = (rect.bottom-rect.top - win_height) / 2;				
		win_realx = win_width;
		win_realy = win_height;
	}

	ClearScreen();

	win_status = GAMEX_READY;
}

bool WindowsDX::InitWindowSetup (void)
{
	if (InitWindowClass()==false)		return false;
	if (InitWindows()==false)			return false;
	if (WindowDraw()==false)			return false;
	return true;
}

bool WindowsDX::InitDirectX (void)
{	
	if(FAILED(CoInitialize(NULL))) {
		win_status = GAMEX_DDSTART_FAILED;
		return false;
	}

	bool success = true;

	if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		Quit(true);

	if(InitDirectDraw()==false) {
		success = false;
		ReportProblem();
		debug.Output("WinDX: DirectDraw init failed.");
	} else {
#ifdef WINDX_DEBUG
		debug.Output("WinDX: DirectDraw init succeeded.");
#endif
	}

	DisplayLoadingWithValue (10);

	if(InitDirect3D()==false) {
		success = false;
		ReportProblem();
		debug.Output("WinDX: Direct3D init failed.");
	} else {
#ifdef WINDX_DEBUG
		debug.Output("WinDX: Direct3D init succeeded.");
#endif
	}

	DisplayLoadingWithValue (20);

	if(InitDirectInput()==false) {
		success = false;
		ReportProblem();
		debug.Output("WinDX: DirectInput init failed.");
	} else {
#ifdef WINDX_DEBUG
		debug.Output("WinDX: DirectInput init succeeded.");
#endif
	}

	DisplayLoadingWithValue (30);
	
	if(InitDirectSound()==false) {
		success = false;
		ReportProblem();
		debug.Output("WinDX: DirectSound init failed.");
	} else {
#ifdef WINDX_DEBUG
		debug.Output("WinDX: DirectSound init succeeded.");
#endif
	}
	
	DisplayLoadingWithValue (40);

	if(InitDirectMusic()==false) {
		success = false;
		ReportProblem();
		debug.Output("WinDX: DirectMusic init failed.");
	} else {
#ifdef WINDX_DEBUG
		debug.Output("WinDX: DirectMusic init succeeded.");
#endif
	}

	DisplayLoadingWithValue (50);

	if(TestDirectShow()==false) {
		mp3_support = false;
		success = false;
		ReportProblem();
		debug.Output("WinDX: DirectShow init failed.");
	} else {
		mp3_support = true;
#ifdef WINDX_DEBUG
		debug.Output("WinDX: DirectShow init succeeded.");
#endif
	}

	DisplayLoadingWithValue (60);

	InitVSync(); // should be after other initializations

	DisplayLoadingWithValue (70);

	win_keys = 0;
	return success;
}

void WindowsDX::InitWindowsInstance (HINSTANCE hThisInst, int nWinMode)
{
	win_hinst = hThisInst;
	win_hmode = nWinMode;
}
		
bool WindowsDX::InitWindowClass (void)
{
	WNDCLASS wcl;
	wcl.hCursor 		= LoadCursor(NULL, IDC_ARROW) ;
	wcl.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcl.lpszMenuName	= "Menu" ;
	wcl.lpszClassName	= "GameX" ;
	wcl.hInstance		= win_hinst;
	wcl.style			= CS_VREDRAW | CS_HREDRAW;
	if(!win_request_no_mouse_input) wcl.style |= CS_DBLCLKS; // enable double-click recognition in the game window
	wcl.lpfnWndProc 	= (WNDPROC) WindowFunc;
	wcl.cbClsExtra		= 0;
	wcl.cbWndExtra		= 0;
	wcl.hbrBackground	= (HBRUSH) GetStockObject(BLACK_BRUSH) ;
	win_resize_workaround = 0;

	if (!RegisterClass(&wcl)) { 
		win_status = GAMEX_INITCLASS_FAILED;
		return false;
	}
	win_status = GAMEX_INITCLASS_OK;
	return true;
}

bool WindowsDX::InitWindows (void)
{
	DWORD style;

	// Initialize View Format (Bits-Per-Pixel)
	if(win_request_bpp != 0)
		win_screenbpp = win_request_bpp; // set bpp to requested setting
	else {
		HDC hdc = GetDC (win_hwnd) ; // current setting requested
		win_screenbpp = GetDeviceCaps (hdc, BITSPIXEL) ;
		ReleaseDC (win_hwnd, hdc) ;
	}

	// Initialize View Size & FullScreen Mode
	win_screenx = GetSystemMetrics(SM_CXSCREEN) ;
	win_screeny = GetSystemMetrics(SM_CYSCREEN) ;
	win_fullscreen = win_request_full ? true : false;
	win_no_acc_err = false;

	if(win_request_xsize == 0) win_request_xsize = win_screenx;
	if(win_request_ysize == 0) win_request_ysize = win_screeny;

	bool invalid_size = false;
	if(win_request_xsize > 2048) win_request_xsize = 2048, invalid_size=true;
	if(win_request_ysize >= 2048) win_request_ysize = 2047, invalid_size=true;
	if(win_request_xsize < 64) win_request_xsize = 64, invalid_size=true;
	if(win_request_ysize < 64) win_request_ysize = 64, invalid_size=true;

///#ifdef _DEBUG
	if(invalid_size) {
		char * str = "Warning: Invalid game window size requested.\nIt must be between 64x64 and 2048x2047 pixels.";
		MessageBox (win_hwnd, str, "GameX Notice", MB_ICONWARNING) ;
		debug.Output(str);
	}
///#endif
	
	// Initialize View Width & Height
	if (win_fullscreen==TRUE)	{
		style = WS_POPUP;	
		win_width = win_request_xsize;
		win_height = win_request_ysize;
	} else {
		style = WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;
		if(win_request_can_resize) style |= WS_SIZEBOX | WS_MAXIMIZEBOX;
		win_width = win_request_xsize;		
		win_height = win_request_ysize;
	}

	win_realx = win_screenx;
	win_realy = win_screeny;
	int window_x = min(win_width, win_fullscreen ? win_screenx : win_realx-4); // temporary variable for actual width of window
	int window_y = min(win_height, win_fullscreen ? win_screeny : win_realy-32);

	// Initialize Drawing Viewport
	SetView(0,0,win_width,win_height);
		
	// Create window adjusting for desired client area size
	RECT	rect;
	int 	sx, sy;
	HWND	temp_hwnd;

	char title_str [256];
	sprintf(title_str, "%s - GameX 5.0", GetName());

	// Create temporary window to determine extended window size
	temp_hwnd = CreateWindowEx(WS_EX_APPWINDOW, "GameX", title_str, style, 0, 0, window_x, window_y, 0, 0, win_hinst, 0) ;
	if (temp_hwnd==NULL) {
		win_status = GAMEX_INITWINTEMP_FAILED;
		return false;		
	}
	win_status = GAMEX_INITWINTEMP_OK;
	GetClientRect (temp_hwnd, &rect) ;

	// set the rect to be the proper size and center it onscreen while we're at it
	sx = (window_x*2) - rect.right;
	sy = (window_y*2) - rect.bottom;
	if (win_fullscreen==TRUE) {
		rect.left = 0; rect.top = 0;
	} else {
		rect.left = (win_screenx/2) - (sx/2) ;
		rect.top = (win_screeny/2) - (sy/2) ;
	}	
	rect.right = rect.left + sx;		
	rect.bottom = rect.top + sy;

	// Create actual window (with correct size) which will be used

	win_hwnd = CreateWindowEx(
					WS_EX_APPWINDOW, 
					"GameX", 
					title_str, 
					style, 
					rect.left, rect.top, 
					rect.right-rect.left, rect.bottom-rect.top,
					0, 
					0, 
					win_hinst, 
					0) ;
	if (win_hwnd==NULL) {
		win_status = GAMEX_INITWINREAL_FAILED;		
		return false;
	}

	// Double check to make sure we have correct client rect size	
	GetClientRect (win_hwnd, &rect) ;	

	win_cursor_hidden = false;

	win_status = GAMEX_INITWINREAL_OK;
	return true;
}

bool WindowsDX::WindowDraw (void)
{
	ShowWindow (win_hwnd, win_hmode) ;
	if (UpdateWindow (win_hwnd)==false) {
		win_status = GAMEX_SHOWWIN_FAILED;		
		return false;
	}
	FlashWindow(win_hwnd,0);
	BringWindowToTop(win_hwnd);
	win_status = GAMEX_SHOWWIN_OK;
	return true;
}

HRESULT CALLBACK DisplayEnumCallback(LPDDSURFACEDESC2 lpDDModeDesc, LPVOID lpContext)
{
	if(GameX.SupportsDepth(lpDDModeDesc->ddpfPixelFormat.dwRGBBitCount)
	&& lpDDModeDesc->ddpfPixelFormat.dwRGBBitCount == GameX.GetBPP()) { // if the bit depth matches
		if(lpDDModeDesc->dwWidth == GameX.GetWidth() && lpDDModeDesc->dwHeight == GameX.GetHeight()) { // if the resolution matches
			GameX.win_realx = GameX.GetWidth();
			GameX.win_realy = GameX.GetHeight();
			return DDENUMRET_CANCEL; // exact match, end enumeration with requested screen width and height
		}

		if((int)(lpDDModeDesc->dwWidth) >= GameX.GetWidth() && (int)(lpDDModeDesc->dwHeight) >= GameX.GetHeight() // if (this bigger than requested
		&& (int)(lpDDModeDesc->dwWidth) <= GameX.win_realx && (int)(lpDDModeDesc->dwHeight) <= GameX.win_realy // and this smaller than so far)
		|| (GameX.win_realx 			<  GameX.GetWidth() || GameX.win_realy				<  GameX.GetHeight()) // or (so far smaller than requested
		&& (int)(lpDDModeDesc->dwWidth) >= GameX.win_realx && (int)(lpDDModeDesc->dwHeight) >= GameX.win_realy) { // and this smaller than so far)
			GameX.win_realx = (int)(lpDDModeDesc->dwWidth);
			GameX.win_realy = (int)(lpDDModeDesc->dwHeight);
			return DDENUMRET_OK; // best match yet but not exact, remember this resolution and continue enumeration
		}
	}

	return DDENUMRET_OK; // rejected display mode, continue enumeration (or end automatically if there's no more to enumerate)
}

HRESULT CALLBACK DevicesEnumCallback(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc, LPVOID lpContext)
{
	if (IsEqualIID(lpD3DDeviceDesc->deviceGUID, IID_IDirect3DNullDevice))
		return DDENUMRET_OK;

	if (!GameX.SimulatingSoftwareOnly() && IsEqualIID(lpD3DDeviceDesc->deviceGUID, IID_IDirect3DTnLHalDevice) )
	{
		GameX.d3d_device_desc = lpD3DDeviceDesc;
		return D3DENUMRET_CANCEL; // done
	}
	else if (!GameX.SimulatingSoftwareOnly() && IsEqualIID(lpD3DDeviceDesc->deviceGUID, IID_IDirect3DHALDevice) )
	{
		GameX.d3d_device_desc = lpD3DDeviceDesc;
		return D3DENUMRET_OK;
	}
	else if (IsEqualIID(lpD3DDeviceDesc->deviceGUID, IID_IDirect3DRGBDevice))
	{
		GameX.d3d_device_desc = lpD3DDeviceDesc;
		return D3DENUMRET_OK;
	}
	return D3DENUMRET_OK;
}

HRESULT CALLBACK ZBufferFormatEnumCallback (LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
	GameX.d3d_zbuffer_format = *lpDDPixFmt;
	if(lpDDPixFmt != NULL && lpDDPixFmt->dwZBufferBitDepth == 16) return D3DENUMRET_CANCEL;

	return D3DENUMRET_OK;
}

bool WindowsDX::InitDirectDraw (void)
{
	dd_last_error = DD_OK;
	dd_was_lost = false;

	HRESULT status = DirectDrawCreateEx(NULL, (void **) &dd_main, IID_IDirectDraw7, NULL);

	if(FAILED(status)) {
		DirectDrawError(status);
		win_status = GAMEX_DDSTART_FAILED;
		return false;
	}
	win_status = GAMEX_DDSTART_OK;

	// set drawing state values to default
	draw_state->src_rect.left = 0;
	draw_state->src_rect.top = 0;
	draw_state->src_rect.right = DEFAULT;
	draw_state->src_rect.bottom = DEFAULT;
	draw_state->colors = ColorX();
	draw_state->color_effect = D3DTOP_MODULATE;
	draw_state->angle = 0.0f;
	draw_state->scalex = 1.0f;
	draw_state->scaley = 1.0f;
	draw_state->transx = 0.0f;
	draw_state->transy = 0.0f;
	draw_state->depth2D = 1.0f;
	draw_state->warp_mode = 0;
	draw_state->flags = DRAW_PLAIN;
	for(int i = 0 ; i < 4 ; i++)
		draw_state->warp[i] = 1.0f;
	draw_render_batch_open = false;
	draw_3D_depth_sort = 0;
	memory_used_current = 0;
	memory_used_max = 0;

	DDCAPS caps;
	ZeroMemory(&caps,sizeof(caps));
	caps.dwSize = sizeof(caps);
	if(SUCCEEDED(dd_main->GetCaps(&caps, NULL))) {

	#ifdef WINDX_DEBUG
		char stringy [64];
		sprintf(stringy,"WinDX: Video memory available: %d megabytes.",caps.dwVidMemFree/1024/1024);
		debug.Output(stringy);
	#endif
		int memfree = caps.dwVidMemFree;
		if(memfree < 1500000 && !win_request_software_only) { // if there are less than about 1.5 megabytes of VRAM available, assume there won't be much if any video acceleration
			char * str = "Warning: Lack of video acceleration detected!";
			MessageBox (win_hwnd, str, "GameX Notice", MB_ICONWARNING) ;
			debug.Output(str);
	/// 	if(memfree == 0) win_request_software_only = true; // assume no hardware if there's no hardware memory at all
		} else {
			int memtotal = caps.dwVidMemTotal;
			if(memtotal < 22000000 && !win_request_software_only) { // if there are less than about 22 megabytes of VRAM total, assume the video card is probably too old (yes, the memory number only gives a very, very rough estimate of how good the card is)
			char * str = "Warning: Your video card appears to be outdated.\nConsider updating it if this game plays slowly or looks odd.";
			MessageBox (win_hwnd, str, "GameX Notice", MB_ICONWARNING) ;
			debug.Output(str);
			}
		}
	}
 
	if(InitDirectDrawMonitor() == false)
		return false;

	if(InitDirectDrawSurfaces() == false)
		return false;

	return true;
}

bool WindowsDX::ToggleFullscreen(void)
{
///#ifdef _DEBUG
	debug.Output ("WindowsDX: Toggling full-screen mode...") ;
///#endif

	// Request full screen mode that's the opposite of the current one
	win_request_full = !win_fullscreen;

	bool ok = ReInitScreen();

#ifndef GAMEX_RUN_UNMANAGED
	SetWarningOnDraw(false); // leave off for the cycle
	GameDraw();
#endif

	return ok;
}

bool WindowsDX::ChangeGameSize(int xres, int yres)
{
	if(xres == win_width && yres == win_height)
		return true; // return if already set to size requested

	win_request_xsize = xres;
	win_request_ysize = yres;

	return ReInitScreen();
}

bool WindowsDX::ReInitScreen(void)
{
	// Set the status to something else than GAMEX_READY so UpdateView knows to temporarily do nothing
	win_status = GAMEX_DDSTART_OK;

	// Uninitialize the DirectX components that depend on the current window:

	// DirectInput
	if (di_mouse)		di_mouse->Unacquire(),		di_mouse->Release(),		di_mouse = NULL;
	if (di_keyboard)	di_keyboard->Unacquire(),	di_keyboard->Release(), 	di_keyboard = NULL;
	if (di_main)		di_main->Release(), 		di_main = NULL;

	// Direct3D
	if (d3d_device) 	d3d_device->Release(),		d3d_device = NULL;	
	if (d3d_main)		d3d_main->Release(),		d3d_main = NULL;	

	// DirectDraw
	if (dd_back_buf)	dd_back_buf->Release(), 	dd_back_buf = NULL;
	if (dd_front_buf)	dd_front_buf->Release(),	dd_front_buf = NULL;	
	
	// Hide the window
	ShowWindow(win_hwnd,false);

	// Re-Initialize the window
	if(InitWindows() == false) return false;
	if(WindowDraw() == false) return false;

	// Re-Initialize the necessary DirectX components:
	if(InitDirectDrawMonitor() == false) return false;
	if(InitDirectDrawSurfaces() == false) return false;
	if(InitDirect3D() == false) return false;
	if(InitDirectInput() == false) return false;
	InitVSync();

	// Reset the GameX status to be ready and return true since we succeeded:
	win_status = GAMEX_READY;
	return true;
}

bool WindowsDX::InitDirectDrawMonitor (void)
{
	int style;
	if(win_fullscreen)
		style = DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT;
	else
		style = DDSCL_NORMAL;

	HRESULT status = dd_main->SetCooperativeLevel (win_hwnd, style);

	if(FAILED(status)) {
		if(win_fullscreen) {
			style ^= (DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT); // try it without a couple flags
			status = dd_main->SetCooperativeLevel (win_hwnd, style);
			if(FAILED(status)) {
				win_fullscreen = false; // try it in windowed mode (maybe this computer doesn't allow fullscreen)
				style = DDSCL_NORMAL;
				status = dd_main->SetCooperativeLevel (win_hwnd, style);
			}
		}
		if(FAILED(status)) { // if we still failed
			win_status = GAMEX_DDSETLEVEL_FAILED;
			return false;
		}
	}
	win_status = GAMEX_DDSETLEVEL_OK;

	// now set the display mode:
	if (win_fullscreen==true) {

		win_realx = win_realy = 0;

		// enumerate available display modes and decide on one of them that best matches win_width, win_height, and win_screenbpp
		status = dd_main->EnumDisplayModes(0, NULL, NULL, (LPDDENUMMODESCALLBACK2) &DisplayEnumCallback);

		DirectDrawError(status); // in case of error or to prevent compiler mishandling aliasing after callbacks

		if(!win_realx || !win_realy) { // if DisplayEnumCallback didn't set a screen size,
			
			// maybe the requested bit depth isn't supported, try for something higher
			while(win_screenbpp <= 32 && (!win_realx || !win_realy || !SupportsDepth(win_screenbpp))) {
				win_screenbpp += 8;
				status = dd_main->EnumDisplayModes(0, NULL, NULL, (LPDDENUMMODESCALLBACK2) &DisplayEnumCallback);
				DirectDrawError(status);
			}
			// if that didn't work, try for something lower
			while(win_screenbpp > 0 && (!win_realx || !win_realy || !SupportsDepth(win_screenbpp))) {
				win_screenbpp -= 4;
				status = dd_main->EnumDisplayModes(0, NULL, NULL, (LPDDENUMMODESCALLBACK2) &DisplayEnumCallback);
				DirectDrawError(status);
			}
			if(!win_realx || !win_realy) {
				win_status = GAMEX_SETMODE_FAILED;
				return false;
			}
		}
		
		status = dd_main->SetDisplayMode (win_realx, win_realy, win_screenbpp,0,0);

		win_screenx = GetSystemMetrics(SM_CXSCREEN) ;
		win_screeny = GetSystemMetrics(SM_CYSCREEN) ;

		if(FAILED(status)) {
			DirectDrawError(status);
			win_status = GAMEX_SETMODE_FAILED;
			return false;
		}
		win_status = GAMEX_SETMODE_OK;

	} else {

		if(win_screenbpp != GetDeviceCaps (GetDC (win_hwnd), BITSPIXEL)) { // if we aren't in desired bit mode
			if(SupportsDepth(win_screenbpp))
				status = dd_main->SetDisplayMode (win_screenx, win_screeny, win_screenbpp,0,0) ; // change to that many bpp
			else
				status = DDERR_GENERIC;
			if(FAILED(status)) {
				// maybe the requested bit depth isn't supported, try for something higher
				while(win_screenbpp <= 32 && (FAILED(status) || !SupportsDepth(win_screenbpp))) {
					win_screenbpp += 8;
					status = dd_main->SetDisplayMode (win_screenx, win_screeny, win_screenbpp,0,0) ;
				}
				// if that didn't work, try for something lower
				while(win_screenbpp > 0 && (FAILED(status) || !SupportsDepth(win_screenbpp))) {
					win_screenbpp -= 4;
					status = dd_main->SetDisplayMode (win_screenx, win_screeny, win_screenbpp,0,0) ;
				}
				if(FAILED(status)) {
					// try something hard-coded that should definitely work, as a last resort
					win_screenx = 640;
					win_screeny = 480;
					win_screenbpp = 16;
					status = dd_main->SetDisplayMode (win_screenx, win_screeny, win_screenbpp,0,0) ;
					if(FAILED(status)) {
						win_status = GAMEX_SETMODE_FAILED;
						return false; // still failed... nothing else we can do in this case
					}
				}
			}
			UpdateWindow (win_hwnd); // redraw the window borders after switching or they'll all be black for a while
		}
		win_realx = min(win_width, win_realx-4); 
		win_realy = min(win_height, win_realy-32);

		win_status = GAMEX_SETMODE_OK;
	}

	DWORD monitor_frequency = 0;
	dd_main->GetMonitorFrequency(&monitor_frequency);
	if(monitor_frequency == 0) monitor_frequency = 60; // just in case
	win_screenrefresh = monitor_frequency;
	win_vsync_enabled = false; // will enable later if we can

#ifdef WINDX_DEBUG
		char str [128];
		sprintf(str,"WinDX: Chosen display mode: %dx%d %d-bit %dHz %sexclusive.", win_screenx, win_screeny, win_screenbpp, win_screenrefresh,win_fullscreen?"":"non");
		debug.Output(str);
#endif

	win_xpos = 0;
	win_ypos = 0;
	
	return true;
}

bool WindowsDX::InitDirectDrawSurfaces (void)
{
	HRESULT status;
	DDSURFACEDESC2 ddsd;	
	POINT pt;

	if (dd_back_buf)	dd_back_buf->Release(), 	dd_back_buf = NULL;
	if (dd_front_buf)	dd_front_buf->Release(),	dd_front_buf = NULL;	

	ZeroMemory (&ddsd, sizeof (ddsd)) ;
	ddsd.dwSize = sizeof (ddsd) ;
 
	// Set front surface description
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	// Try to create primary surface 
	status = dd_main->CreateSurface (&ddsd, &dd_front_buf, NULL);
	if (FAILED(status)) {			
		DirectDrawError(status);
		win_status = GAMEX_FRONTSURFACE_FAILED; // No good, return error
		return false;
	}
	
	win_status = GAMEX_FRONTSURFACE_OK;
	// Set back surface description
	ddsd.dwSize = sizeof (ddsd) ;
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
	if(win_request_software_only) ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = win_width;
	ddsd.dwHeight = win_height;

	// Try to create secondary surface 
	status = dd_main->CreateSurface (&ddsd, &dd_back_buf, NULL);
	if (FAILED(status)) {
		DirectDrawError(status);
		win_status = GAMEX_BACKSURFACE_FAILED; // No good, return error
		return false;
	}

	win_status = GAMEX_BACKSURFACE_OK;

	// Get Surface Description (pixel format, pitch, etc.)	
	ddsd.dwSize = sizeof(ddsd) ;
	ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
	dd_front_buf->GetSurfaceDesc(&ddsd) ;
	if (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
#ifdef WINDX_DEBUG 
		debug.Output ("WinDX: Front buffer is in video memory.") ;
#endif
	} else {
		debug.Output ("WinDX: Front buffer is NOT IN VIDEO MEMORY.") ;
	}
	dd_back_buf->GetSurfaceDesc(&ddsd) ;
	if (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
		win_back_buf_in_vid_mem = true;
#ifdef WINDX_DEBUG 
		debug.Output ("WinDX: Back buffer is in video memory.") ;
#endif
	} else {
		win_back_buf_in_vid_mem = false;
		debug.Output ("WinDX: Back buffer is NOT IN VIDEO MEMORY.") ;
	}

	// Initialize Drawing Functions, Pixel Masks, Pitch and Width 
	// based on Surface Description 

	win_pitch = ddsd.lPitch;
	win_adjustedpitch = win_pitch * 8 / win_screenbpp;

	win_maskr = ddsd.ddpfPixelFormat.dwRBitMask;
	win_maskg = ddsd.ddpfPixelFormat.dwGBitMask;		
	win_maskb = ddsd.ddpfPixelFormat.dwBBitMask;
	
	win_shiftr = GetShift (win_maskr) ; // 11 for 16 bit, 10 for 15 bit, 16 for 24 or 32 bit
	win_shiftg = GetShift (win_maskg) ; // 5 for 16 or 15 bit, 8 for 24 or 32 bit
	win_shiftb = GetShift (win_maskb) ; // 0

	win_maxr = GetMax (win_maskr) ;
	win_maxg = GetMax (win_maskg) ;
	win_maxb = GetMax (win_maskb) ;

	win_maskminr = 256/(win_maskr>>win_shiftr);
	win_maskming = 256/(win_maskg>>win_shiftg);
	win_maskminb = 256/(win_maskb>>win_shiftb);

	FuncDrawPixel = &DrawPixelError;
	FuncReadPixel = &ReadPixelError;

	// now set a clipper to the back surface so we don't have to clip bitmap blits ourselves
	{
		RECT boundary[1] = {0,0,win_width,win_height};
		LPDIRECTDRAWCLIPPER clipper;
		LPRGNDATA rgn_data;
		if (FAILED(dd_main->CreateClipper(0,&clipper,NULL))) {
			win_status = GAMEX_CLIPPER_FAILED;
			return false;
		}
		int rectSize = sizeof(RECT);
		int rgnHeaderSize = sizeof(RGNDATAHEADER);
		rgn_data = (LPRGNDATA)malloc(rgnHeaderSize+rectSize);
		memcpy(rgn_data->Buffer, boundary, rectSize);
		rgn_data->rdh.dwSize = rgnHeaderSize;
		rgn_data->rdh.nRgnSize = rectSize;
		rgn_data->rdh.nCount = 1;
		rgn_data->rdh.iType = RDH_RECTANGLES;
		rgn_data->rdh.rcBound = boundary[0];
		if (FAILED(clipper->SetClipList(rgn_data,0)) || FAILED(dd_back_buf->SetClipper(clipper))) {
			free(rgn_data);
			win_status = GAMEX_CLIPPER_FAILED;
			return false;
		}
		free(rgn_data);
	}

	LPDIRECTDRAWCLIPPER clipper = NULL; // hold the clipper
	dd_main->CreateClipper(0,&clipper,NULL);
	clipper->SetHWnd(0, win_hwnd);
	dd_front_buf->SetClipper(clipper);

	// Precompute: y-table look up 
	for (int y=0; y<=win_height; y++)
		win_ytable[y] = y*win_adjustedpitch;
		
	// Precompute: screen size (total number of bytes)
	win_size = win_pitch*win_height;
	
	// Precompute: client rectangle
	
	RECT destrect;

	GetClientRect(win_hwnd, &destrect) ;

	win_realx = destrect.right - destrect.left; // necessary on occasions where the OS gives us a different window size
	win_realy = destrect.bottom - destrect.top; // than we requested, such as when the requested width is very small or very large

	if(!IsResizeWorkaroundEnabled()) {
		SetProportional();
	} else {
		win_xoffset = (win_realx - win_width) / 2;
		win_yoffset = (win_realy - win_height) / 2; 			
		win_realx = win_width;
		win_realy = win_height;
	}

	pt.x = win_xpos;
	pt.y = win_ypos;
	ClientToScreen(win_hwnd, &pt) ;
	OffsetRect(&destrect, pt.x, pt.y) ;
	win_xpos = pt.x;
	win_ypos = pt.y;
	
	win_active = true;	
	win_status = GAMEX_READY;

	ClearScreen() ;
	UpdateView() ;

	return true;
}

void WindowsDX::InitVSync (void)
{
#ifdef WINDX_DEBUG
	char str [128];
	strcpy(str,"WinDX: Refresh synchronization OFF.");
#endif

	if(win_request_vsync && win_fps > win_screenrefresh || win_fps == -1)
		win_fps = win_screenrefresh; // set the frame rate to not exceed the refresh rate

	if(win_request_vsync) {
		
		// test scanline speed:
		// sample 80 times at 10 milliseconds of delay each,

		DWORD scanline [80]; // array of consecutive scanline locations
		int diff [80]; // array of valid scanline location differences
		int n; // number of valid (positive) differences

		hyper clocksPerMillisec = GetClocksPerSec()/1000;
		for(int i = 0 ; i < 80 ; i++) {
			hyper sleeptimer = GetClock();
			while(GetClock() - sleeptimer < clocksPerMillisec) ;

			dd_main->GetScanLine(&scanline[i]);
		}

		n = 0;

		for(i = 0 ; i < 80 ; i++) {
			if(i && scanline[i]) {
				if(scanline[i] > scanline[i-1]) {
					diff[n] = scanline[i] - scanline[i-1];
					n++;
				}
			}
		}

		if(n < 3)
			win_request_vsync = 0;
		else {
			int total = 0;

			for(int i = 0 ; i < n ; i++) {
				total += diff[i];
			}

			win_scanline_speed = total/n;
			if(win_scanline_speed > 0) {
				if(win_screeny/win_scanline_speed*win_screenrefresh > CLOCKS_PER_SEC) {
#ifdef WINDX_DEBUG
					sprintf(str,"WinDX: Refresh synchronization OFF; %d scanlines per millisecond is too high.", win_scanline_speed);
#endif
				} else {
					win_vsync_enabled = true;
#ifdef WINDX_DEBUG
					sprintf(str,"WinDX: Refresh synchronization on, %d scanlines per millisecond.", win_scanline_speed);
#endif
				}
			}
		}
	}

#ifdef WINDX_DEBUG
	{
		hyper resolution = GetClocksPerSec();
		char str [128];
		sprintf(str,"WinDX: Timer resolution: %.0f ticks per second.", (double)resolution);
		debug.Output(str);
	}
#endif

#ifdef WINDX_DEBUG
	debug.Output(str);
#endif
}

bool WindowsDX::InitDirect3D (void)
{
	// decide on a max texture size we'll allow for D3D surfaces
	// 1024 is a good limit; any higher wastes much speed and memory
	// note that it may also be further limited by hardware capability
	SetImageResolutionLimit (1024);

	// if DirectDraw is working, use it to intiialize Direct3D:
	if(dd_main==NULL || FAILED(dd_main->QueryInterface(IID_IDirect3D7, (LPVOID *)& d3d_main))) {
		debug.Output ("Direct3D: Device query failed.") ;
		win_status = GAMEX_D3DSTART_FAILED;
		return false; 
	}


	HRESULT status;

	// Enumerate available 3D rendering devices and decide on one:
	status = d3d_main->EnumDevices((LPD3DENUMDEVICESCALLBACK7) &DevicesEnumCallback, NULL);
	GUID guid = d3d_device_desc->deviceGUID;

	d3d_zbuffer_enabled = false;

	// make sure not to use a z-buffer unless we have access to video hardware
	if(guid != IID_IDirect3DRGBDevice) {

		DDPIXELFORMAT pixelzbuffer;
		d3d_main->EnumZBufferFormats(guid, (LPD3DENUMPIXELFORMATSCALLBACK) &ZBufferFormatEnumCallback, (VOID*)&pixelzbuffer );

		DDSURFACEDESC2 ddsdZ;
		ZeroMemory(&ddsdZ, sizeof(DDSURFACEDESC2));
		ddsdZ.dwSize = sizeof(DDSURFACEDESC2);
		ddsdZ.dwWidth = win_width;
		ddsdZ.dwHeight = win_height;
		ddsdZ.ddpfPixelFormat = d3d_zbuffer_format;
		ddsdZ.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_CAPS | DDSD_PIXELFORMAT;
		ddsdZ.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
		ddsdZ.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY; // require the z-buffer to be in video memory, otherwise it would be too slow

/// 	// used when software z-buffering is allowed (in which case the previous line should be disabled instead):
/// 	if(!win_back_buf_in_vid_mem || (d3d_device_desc->deviceGUID == IID_IDirect3DRGBDevice || d3d_device_desc->deviceGUID == IID_IDirect3DRefDevice))
/// 		ddsdZ.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		status = dd_main->CreateSurface(&ddsdZ, &d3d_zbuffer, NULL);
		if(SUCCEEDED(status)) {
			status = dd_back_buf->AddAttachedSurface(d3d_zbuffer);
			if(FAILED(status))
				debug.Output("WinDX: Failed to attach z buffer.");
			else
				d3d_zbuffer_enabled = true;
		} else {
			debug.Output("WinDX: Z-Buffering not supported.");
		}
	}
	// if z buffering hasn't been enabled at this point, depth sorting is used instead
	// (note that depth sorting is used even with z-buffering when drawing translucently)


	// make sure we assign a 3D device to the back buffer first,
	// or it probably won't get 3D hardware acceleration
	if(CreateDeviceForSurface(d3d_device, dd_back_buf) == false) {
		win_status = GAMEX_D3DSTART_FAILED;
		return false;
	}

	// set some render and texture state values, most of these may be the defaults, but just in case:

	d3d_device->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP); // we don't want texture wrapping,
	d3d_device->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP); // it doesn't work right on most image sizes anyway
	d3d_device->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	d3d_device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_LINEAR);
	d3d_device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_LINEAR);

	d3d_device->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE); // we have no D3D light sources, for now
	d3d_device->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE); // don't need specular
	d3d_device->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE); // we want to allow per-vertex coloring
	d3d_device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE); // we want to allow alpha blending
	d3d_device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE); // and dithering, except when temporarily disabled later
	d3d_device->SetRenderState(D3DRENDERSTATE_ANTIALIAS, FALSE); // set antialiasing off
	d3d_device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW); // turn off culling of CW defined polygons if developer so desires
	d3d_device->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE); // the z buffer is only enabled while drawing in 3D
	d3d_device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE); 
	d3d_device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
	d3d_device->SetRenderState(D3DRENDERSTATE_ALPHAREF, 248);
	d3d_device->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);

	SetView(win_viewx1,win_viewy1,win_viewx2,win_viewy2); // now that D3D has been initialized set the view again for it

	draw_render_batch_device = NULL;

	if(d3d_zbuffer_enabled)
		d3d_device->Clear(0,NULL,D3DCLEAR_ZBUFFER,0,1.0f,0);

	return true;
}

bool WindowsDX::CreateDeviceForSurface (LPDIRECT3DDEVICE7 & device, LPDIRECTDRAWSURFACE7 & surface)
{
	if(win_request_software_only || FAILED(d3d_main->CreateDevice(IID_IDirect3DTnLHalDevice, surface, &device))) { // the best 3D hardware device
		if(win_request_software_only || FAILED(d3d_main->CreateDevice(IID_IDirect3DHALDevice, surface, &device))) { // second-best 3D hardware
			if(FAILED(d3d_main->CreateDevice(IID_IDirect3DMMXDevice, surface, &device))) { // best 3D software device
				// failing up until now is normal, although undesirable
				HRESULT status = d3d_main->CreateDevice(IID_IDirect3DRGBDevice, surface, &device); // backup 3D software
				if(FAILED(status)) {
					// that last try should never fail, something's very wrong if we get here
					DirectDrawError(status);
					Direct3DError(status);
					win_status = GAMEX_D3DSTART_FAILED;
					debug.Output ("Direct3D: Device create failed.") ;
					return false; // should never get here; even if there's no 3D device, the D3DRGB device is software emulated
				}
			}
		}
	}
	return true;
}

bool WindowsDX::InitDirectSound (void)
{
	if(win_request_no_sound) return true;

	if(FAILED(DirectSoundCreate (NULL, &ds_main, NULL))) {
		win_status = GAMEX_SOUNDINIT_FAILED;
		ds_main = NULL;
		sound_support = false;
		return false;
	} else
	if(FAILED(ds_main->SetCooperativeLevel(win_hwnd, DSSCL_PRIORITY))) {
		ds_main->Release() ;
		win_status = GAMEX_SOUNDCOOP_FAILED;
		ds_main = NULL;
		sound_support = false;
		return false;
	}

	master_music_tempo = master_music_volume = 1.0f;

	sound_support = true;
	return true;
}

bool WindowsDX::InitDirectMusic (void)
{
	if(win_request_no_sound) return true;

	// set up DirectMusic and performance/ports:

	if(FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC,
								IID_IDirectMusicPerformance, (void**)&dm_performance)))
		return(false);

	if(FAILED(dm_performance->Init(NULL, (sound_support ? ds_main : NULL), win_hwnd)))
		return(false);

	if(FAILED(dm_performance->AddPort(NULL)))
		return(false);

	// prepare DirectMusic for loading MIDI files:

	if(FAILED(CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC,
								IID_IDirectMusicLoader, (void**)&dm_loader)))
		return(false);

	dm_musicx_index = 0;

	return true;
}

bool WindowsDX::TestDirectShow (void)
{
	IGraphBuilder * dsh_gb;
	HRESULT status = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&dsh_gb);
	if(FAILED(status)) {
		return false;
	} else {
		if (dsh_gb) 		dsh_gb->Release(),			 dsh_gb = NULL;
		return true;
	}
}


bool WindowsDX::InitDirectInput (void)
{
	// Create the DirectInput object:

	if(FAILED(DirectInputCreate(win_hinst, DIRECTINPUT_VERSION, &di_main, NULL)))
		return false;

	bool failed = false;

	// Create and set up the keyboard device:

	if(SUCCEEDED(di_main->CreateDevice(GUID_SysKeyboard, &di_keyboard, NULL))) {

		HRESULT status = DDERR_GENERIC;
		if(!win_request_no_auto_pause) {
			status = di_keyboard->SetCooperativeLevel(win_hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY);
			if(SUCCEEDED(status))
				status = di_keyboard->SetDataFormat(&c_dfDIKeyboard);
			if(SUCCEEDED(status))
				status = di_keyboard->Acquire();
		}
		if(FAILED(status)) {
			status = di_keyboard->SetCooperativeLevel(win_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
			if(SUCCEEDED(status))
				status = di_keyboard->SetDataFormat(&c_dfDIKeyboard);
			if(SUCCEEDED(status))
				status = di_keyboard->Acquire();
		}
		if(FAILED(status))
			failed = true;
		else
			for(int k = 0 ; k < 255 ; k++)
				di_keystate[k] = di_keypressed[k] = 0;

	} else
		failed = true;

	if(!win_request_no_mouse_input) {

		// Create and set up the mouse device:

		if(SUCCEEDED(di_main->CreateDevice(GUID_SysMouse, &di_mouse, NULL))) {
			HRESULT status = DDERR_GENERIC;
			if(!win_request_no_auto_pause) {
				status = di_mouse->SetCooperativeLevel(win_hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
				if(SUCCEEDED(status))
					status = di_mouse->SetDataFormat(&c_dfDIMouse);
				if(SUCCEEDED(status))
					status = di_mouse->Acquire();
			}
			if(FAILED(status)) {
				status = di_mouse->SetCooperativeLevel(win_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
				if(SUCCEEDED(status))
					status = di_mouse->SetDataFormat(&c_dfDIMouse);
				if(SUCCEEDED(status))
					status = di_mouse->Acquire();
			}

			if(FAILED(status))
				failed = true;
		} else {
			failed = true;
			win_request_no_mouse_input = true;
		}
	} 

	UpdateInput(); // so the game can check for initial input inside GameInit() right after calling GameX.Initialize()

	return !failed;
}

bool WindowsDX::UpdateInput (void)
{
	bool skip_keyboard = (di_keyboard == NULL);
	bool skip_mouse = (di_mouse == NULL) || win_request_no_mouse_input;

	if(!skip_keyboard) {
		if(FAILED(di_keyboard->GetDeviceState(256, (LPVOID)di_keystate))) {
			if(SUCCEEDED(di_keyboard->Acquire())) {
				if(FAILED(di_keyboard->GetDeviceState(256, (LPVOID)di_keystate))) {
					skip_keyboard = true;
				}
			}
		}

		if(!skip_keyboard) {

			{
				for(KeyID k = 0 ; k < 255 ; k++) {
					if(di_keypressed[k] & INPUT_PRESSED) // if just pressed last time,
						di_keypressed[k] ^= (INPUT_HELD|INPUT_PRESSED); // switch pressed off, held on
				}
			}

			// fix windows oddity that makes the right shift button register as unpressed
			// if an arrow key is pressed while it's down
			if(!IsKeyDown(KEY_RSHIFT) && (GetAsyncKeyState(VK_RSHIFT) & 0x8000))
				di_keystate[KEY_RSHIFT] |= INPUT_PRESSED;

			// transfer/record unique (non-held) keypresses
			for(KeyID k = 0 ; k < 255 ; k++) {
				if(di_keypressed[k] & INPUT_HELD) {
					if(!di_keystate[k])
						di_keypressed[k] = di_keystate[k];
				} else {
					di_keypressed[k] = di_keystate[k];
				}
			}

			if(IsEscapeReserved() && GetKeyPress(KEY_ESCAPE) // ESC is reserved to quit unless RUN_USEESCAPEKEY is on
			|| IsCtrlDown() && GetKeyPress(KEY_Q) // whenever CTRL is down, Q is reserved to quit (nonstandard but intuitive)
			|| IsAltDown() && GetKeyPress(KEY_F4)) // whenever ALT is down, F4 is reserved to quit (faster than in WindowFunc)
				Quit();


			if(IsAltEnterReserved() && GetKeyPress(KEY_ENTER) && IsAltDown()) // ALT+ENTER is reserved to toggle fullscreen mode unless RUN_NOALTENTERTOGGLE is on
				GameX.ToggleFullscreen();
		}
	}

	if(!skip_mouse) {

		if(FAILED(di_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&di_mousestate))) {
			if(SUCCEEDED(di_mouse->Acquire())) {
				if(FAILED(di_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&di_mousestate))) {
					skip_mouse = true;
				}
			}
		}

		if(!skip_mouse) {

			{
				for(MouseButtonID b = 0 ; b < 4 ; b++) {
					if(di_mousepressed[b] & INPUT_PRESSED) // if just pressed last time,
						di_mousepressed[b] ^= (INPUT_HELD|INPUT_PRESSED); // switch pressed off, held on
				}
			}
			
			// if the mouse isn't outside of the game,
			if(win_fullscreen || (mouse_x>0 && mouse_y>0 && GetMouseX()<win_width && GetMouseY()<win_height)) {
				// transfer/record unique (non-held) mousepresses
				for(MouseButtonID b = 0 ; b < 4 ; b++) {
					if(!di_mousestate.rgbButtons[b])
						mouse_down[b] = 0;
					else if(!mouse_down[b])
		/// 			di_mousestate.rgbButtons[b] = 0; // seems to cause mouse click detection to lag after typing
						;
					else if(mouse_down[b] == 2 && di_mousestate.rgbButtons[b])
						di_mousestate.rgbButtons[b] |= 0x01;
					if(di_mousepressed[b] & INPUT_HELD) {
						if(!di_mousestate.rgbButtons[b])
							di_mousepressed[b] = di_keystate[b];
					} else {
						di_mousepressed[b] = di_mousestate.rgbButtons[b];
					}
				}
			}
			if(di_mousestop) {
				di_mousestate.lX = di_mousestate.lY = di_mousestate.lZ = 0;
				if(win_active)
					di_mousestop--;

				// unregister all clicks at this time (so the game doesn't respond to window-activating clicks)
				for(MouseButtonID b = 0 ; b < 4 ; b++) {
					mouse_down[b] = 0;
					di_mousepressed[b] = 0;
				}
			}

			POINT mousePoint;
			GetCursorPos(&mousePoint);
			mousePoint.x -= win_xpos+win_xoffset;
			mousePoint.y -= win_ypos+win_yoffset;
			SetMouseMove(mousePoint.x,mousePoint.y);
		}
	}

	return true;
}

bool WindowsDX::AnyKeyPress (void)	
{
	for(int n=0; n<256; n++)
		if(GetKeyPress(n))
			// some keys don't count as "any key", such as volume control keys
			if(n != DIK_CAPITAL && n != DIK_MUTE && n != DIK_VOLUMEDOWN && n != DIK_VOLUMEUP && n != DIK_SYSRQ
			&& n != DIK_POWER && n != DIK_SLEEP && n != DIK_WAKE && n != DIK_NUMLOCK && n != DIK_SCROLL
			&& n != DIK_NEXTTRACK && n != DIK_WEBSEARCH && n != DIK_WEBFAVORITES && n != DIK_WEBREFRESH && n != DIK_WEBSTOP
			&& n != DIK_WEBHOME && n != DIK_WEBFORWARD && n != DIK_WEBBACK && n != DIK_MYCOMPUTER && n != DIK_MAIL
			&& n != DIK_MEDIASELECT && n != DIK_PLAYPAUSE && n != DIK_MEDIASTOP && n != DIK_CALCULATOR)
				return true;
	return false;
}

bool WindowsDX::PeekAnyKeyPress (void)	
{
	for(int n=0; n<256; n++)
		if(PeekKeyPress(n))
			// some keys don't count as "any key", such as volume control keys
			if(n != DIK_CAPITAL && n != DIK_MUTE && n != DIK_VOLUMEDOWN && n != DIK_VOLUMEUP && n != DIK_SYSRQ
			&& n != DIK_POWER && n != DIK_SLEEP && n != DIK_WAKE && n != DIK_NUMLOCK && n != DIK_SCROLL
			&& n != DIK_NEXTTRACK && n != DIK_WEBSEARCH && n != DIK_WEBFAVORITES && n != DIK_WEBREFRESH && n != DIK_WEBSTOP
			&& n != DIK_WEBHOME && n != DIK_WEBFORWARD && n != DIK_WEBBACK && n != DIK_MYCOMPUTER && n != DIK_MAIL
			&& n != DIK_MEDIASELECT && n != DIK_PLAYPAUSE && n != DIK_MEDIASTOP && n != DIK_CALCULATOR)
				return true;
	return false;
}

void WindowsDX::SetMousePosition (int x, int y)
{
	if(!di_mousestop) {
		mouse_x = x*win_realx/win_width;
		mouse_y = y*win_realy/win_height;
		SetCursorPos(win_xpos+win_xoffset+mouse_x, win_ypos+win_yoffset+mouse_y);
	}
}

void WindowsDX::DisplayLoadingWithValue (int percent)
{
	char str [64];
	sprintf(str, "Loading %d%%...", percent);
	ClearScreen() ;
	DrawText(win_viewx2-120,win_viewy2-30,str) ;
	UpdateView() ;
	if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		Quit(true);
}

void WindowsDX::ResetRandomSeed(void)
{
	srand((unsigned int)time(NULL)<<2);
}

void WindowsDX::SetRandomSeed(int seed)
{
	srand(seed<<2);
}

void WindowsDX::SetGameSpeed (int cps)
{
	if(cps < 1) cps = 1;
	if(cps > 600) cps = 600;
	win_cps = cps;
}

void WindowsDX::SetLagLimit (int max_cpf)
{
	if(max_cpf==0) max_cpf = 0xFFFF; // 0 means no limit (for networked games)
	if(max_cpf<3) max_cpf = 3;
	win_max_cpf = max_cpf;
}

void WindowsDX::SetForcedFrameRate (int fps)
{
	win_fps = fps;
	if(fps>win_screenrefresh) win_vsync_enabled = false;
}

void WindowsDX::ResetForcedFrameRate (void)
{
	win_fps = -1;
}

// gets amount to shift a color component of the given mask
int WindowsDX::GetShift (int mask)
{
	if(mask == 0) return 0;
	int count;
	for (count=0; (mask & 1) == 0; count++)
		mask >>= 1;
	return count;
}

int WindowsDX::GetMax (int mask)
{
	if(mask == 0) return 0;
	int count, max; 
	for (count=0; (mask & 1) == 0; count++)
		mask >>= 1;
	for (max=0; mask > 0; max++)
		mask >>= 1;
	return max;
}

LPDIRECTDRAWSURFACE7 WindowsDX::CreateImageSurface (int width, int height, bool texture, int create_alpha, bool targetable)
{
	if(dd_main == NULL) {
		ReportProblem(GAMEX_NO_INIT) ; // loading image before GameX was initialized
	}

	LPDIRECTDRAWSURFACE7 surface; // the surface we'll create and return

	DDSURFACEDESC2 ddsd;
	ZeroMemory (&ddsd, sizeof(DDSURFACEDESC2)) ;
	ddsd.dwSize=sizeof(ddsd);

	DDPIXELFORMAT ddpf;
	ZeroMemory (&ddpf, sizeof(DDPIXELFORMAT)) ;
	ddpf.dwSize = sizeof(ddpf);

	ddsd.dwHeight=height;
	ddsd.dwWidth=width;
	ddsd.dwFlags=DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue = 0; // specify black as transparent if drawn masked
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue = 0;
	if(texture && d3d_device != NULL) {
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		D3DDEVICEDESC7 ddDesc;
		d3d_device->GetCaps(&ddDesc);
		if (ddDesc.deviceGUID == IID_IDirect3DHALDevice || ddDesc.deviceGUID == IID_IDirect3DTnLHalDevice) {
			ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
		}
		else {
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		}

		if(targetable) { // && texture
			ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE; // to allow the surface to request a 3D device
			ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_HINTDYNAMIC; // could speed up images that are frequently drawn to
		} else {
			ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_HINTSTATIC; // could speed up images that are never drawn to
		}

		if(ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) {
			// if the texture dimensions must be a power of 2,
			// increase the surface size accordingly (the change is transparent to the user and programmer)
			for(ddsd.dwWidth  = 1; (int)ddsd.dwWidth  < width ; ddsd.dwWidth  <<= 1);
			for(ddsd.dwHeight = 1; (int)ddsd.dwHeight < height; ddsd.dwHeight <<= 1);
		}
		if(ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY || targetable) { // for some reason DDSCAPS_3DDEVICE causes non-square textures to fail drawing
			// if the texture has to be square
			if(ddsd.dwWidth > ddsd.dwHeight) ddsd.dwHeight = ddsd.dwWidth;
			else							 ddsd.dwWidth  = ddsd.dwHeight;
		}
		int maxWidth = min((DWORD)d3d_max_tex_width, ddDesc.dwMaxTextureWidth);
		int maxHeight = min((DWORD)d3d_max_tex_height, ddDesc.dwMaxTextureHeight);
		if(ddsd.dwWidth  > (DWORD)maxWidth)  ddsd.dwWidth  = (DWORD)maxWidth;
		if(ddsd.dwHeight > (DWORD)maxHeight) ddsd.dwHeight = (DWORD)maxHeight;
		if(win_request_low_resolution) ddsd.dwWidth /= 2, ddsd.dwHeight /= 2;
		if(ddsd.dwWidth  < ddDesc.dwMinTextureWidth)  ddsd.dwWidth  = ddDesc.dwMinTextureWidth;
		if(ddsd.dwHeight < ddDesc.dwMinTextureHeight) ddsd.dwHeight = ddDesc.dwMinTextureHeight;
	} else {
		// if we're not making a 3D accelerated surface, just try for a 2D accelerated one
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	}

			if(create_alpha) {
				dd_back_buf->GetPixelFormat(&ddpf);
				ddpf.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
				ddpf.dwRGBBitCount	   = 32;
				ddpf.dwRGBAlphaBitMask = 0xFF000000;
				ddpf.dwRBitMask 	   = 0x00FF0000;
				ddpf.dwGBitMask 	   = 0x0000FF00;
				ddpf.dwBBitMask 	   = 0x000000FF;
				ddsd.dwFlags |= DDSD_PIXELFORMAT;
				ddsd.ddpfPixelFormat = ddpf;
			} 
			
	HRESULT status = dd_main->CreateSurface(&ddsd, &surface, NULL);
	if(FAILED(status)) {
		if(ddsd.dwFlags & DDSD_PIXELFORMAT) {
			ddsd.dwFlags ^= DDSD_PIXELFORMAT; // try again, this time accepting any format it wants to use
			status = dd_main->CreateSurface(&ddsd, &surface, NULL);
		}
		if(FAILED(status)) {
			DirectDrawError(status);
			debug.Output("Failed to allocate image memory!");
			Quit(true);
			return NULL;
		}
	}

	if(win_status != GAMEX_NO_INIT) {
		surface->GetPixelFormat(&ddpf); // get the pixel format DirectDraw made the surface in,
		int size = (ddsd.dwWidth*ddsd.dwHeight*ddpf.dwRGBBitCount/8)/1024; if(size < 1) size = 1;
#ifdef WINDX_DEBUG
		if(GetStatus() == GAMEX_READY) {
			char sizeStr [8];
			sprintf(sizeStr, "%05d", size);
			for(int z = 0 ; sizeStr[z] == '0' ; z++) sizeStr[z] = ' ';
			char stringy [128];
			sprintf(stringy,"%s KB, %dx%d%s -> %dx%dx%d",sizeStr,width,height,(create_alpha)?"+A":"", ddsd.dwWidth,ddsd.dwHeight,ddpf.dwRGBBitCount);
			debug.Output(stringy);
		}
#endif
		MemoryRecordChange(size);
	}

	return surface;
}


bool WindowsDX::DirectXCleanUp (void)
{
	// DirectMusic MIDI songs:
	while(dm_musicx_index--)
		if(dm_musicx[dm_musicx_index] != NULL)
			dm_musicx[dm_musicx_index]->Release(),	dm_musicx[dm_musicx_index] = NULL;

	// DirectMusic
	if (dm_performance) dm_performance->CloseDown(), dm_performance->Release(), dm_performance = NULL;
	if (dm_loader)		dm_loader->Release(),		 dm_loader = NULL;
	
	// DirectSound
	if (ds_main)		ds_main->Release(), 		ds_main = NULL;

	// DirectInput
	if (di_mouse)		di_mouse->Unacquire(),		di_mouse->Release(),		di_mouse = NULL;
	if (di_keyboard)	di_keyboard->Unacquire(),	di_keyboard->Release(), 	di_keyboard = NULL;
	if (di_main)		di_main->Release(), 		di_main = NULL;

	// Direct3D
	if (d3d_device) 	d3d_device->Release(),		d3d_device = NULL;	
	if (d3d_main)		d3d_main->Release(),		d3d_main = NULL;	

	// DirectDraw
	if (dd_back_buf)	dd_back_buf->Release(), 	dd_back_buf = NULL;
	if (dd_front_buf)	dd_front_buf->Release(),	dd_front_buf = NULL;	
	if (dd_main)		dd_main->Release(), 		dd_main = NULL;

	CoUninitialize();
	return true;
}

bool WindowsDX::CleanUp (void)
{
	#ifdef WINDX_DEBUG
		char str[128];
		sprintf(str,"-- Maximum Graphic/Sound memory in use: %d KB", memory_used_max);
		debug.Output(str);
		sprintf(str,"-- Graphic/Sound memory in use on exit: %d KB", memory_used_current);
		debug.Output(str);
	#endif
	if (DirectXCleanUp()==FALSE) {
		debug.Output ("WinDX::CleanUp: Unable to properly shut down DirectX.") ;
		return false;
	}
	return true;
}

int WindowsDX::UpdateView (void)
{
	if(win_status != GAMEX_READY)
		return 0;

	// end the scene here if it's still open
	if(draw_render_batch_open)
		EndRenderBatch();

	// DirectX 8.1
	//dd_device->Present (NULL, NULL, win_hwnd, NULL) ;

	// DirectDraw
	HRESULT status; 
	
	if(dd_front_buf->IsLost()) {
		dd_front_buf->Restore();

		dd_was_lost=1;

#ifdef WINDX_DEBUG
		if(!dd_front_buf->IsLost())
			debug.Output("WinDX: DirectX surfaces were lost, but have now been properly restored.");
#endif
		if(dd_front_buf->IsLost())
			return 0;
		else {
			dd_main->RestoreAllSurfaces();

			// now we restore the 3D drawing device.
			// for some reason, if we don't do so RIGHT HERE
			// strange and inexplicable image corruption occurs

			UnlockBackBuffer() ;
			if(d3d_device != NULL) { 
				unsigned long temp_long = -1;
				status = d3d_device->ValidateDevice(&temp_long);

				if(status == DDERR_SURFACELOST) { // in newer versions of DirectX this is really D3DERR_DEVICELOST, which makes more sense
					d3d_device->Release();
					bool success = CreateDeviceForSurface(d3d_device, dd_back_buf); // may fail if UnlockBackBuffer() is not done before this
					d3d_device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE); //Turn on alpha blending

					if(success) {
#ifdef WINDX_DEBUG
						debug.Output("WinDX: The 3D rendering device has been lost and is now properly restored.");
#endif
					} else {
						debug.Output("WinDX: The 3D rendering device has been lost and could NOT be restored.");
					}
				}
			}
		}
	}
	
	UnlockBackBuffer() ;

	// use Blt since we might want to scale the viewport to the screen
	// and, if in windowed mode, because we have a clipper set to the window

	// set the source and destination rectangles:
	RECT src_rect = {0, 0, win_width, win_height} ;
	RECT dst_rect = {win_xpos+win_xoffset, win_ypos+win_yoffset, win_xpos+win_xoffset+win_realx, win_ypos+win_yoffset+win_realy} ;

	if(win_vsync_enabled) {  // if vsync hasn't been disabled,
		SyncWithRefresh(); // wait until we're synchronized with the screen's refresh

		// debugging, delete the following code once vsync has been perfected:
		/*
				#ifdef WINDX_DEBUG
				// this code draws a red line where the scanline is,
				// so the success of refresh synchronization can be seen
				// by how infrequently the red line is over the game
				// unfortunately the actual drawing takes a little time so the uncertainty principle applies
					DWORD scanline = -1;
					dd_main->GetScanLine(&scanline);
					int drawline = (scanline-(win_ypos+win_yoffset))*win_height/win_realy;
					if(drawline > 0 && drawline < win_height)
						InternalDrawRectDD(255,0,0, 0,drawline,win_width,drawline+2); // red line for after refreshsync
					UnlockBackBuffer() ;
				#endif
		*/
		// end of deletable debugging code
	}

	// do the blit:
	status = dd_front_buf->Blt(&dst_rect, dd_back_buf,
							   &src_rect,
							   DDBLT_WAIT,
							   NULL);

	// we might clear the back buffer at this point to avoid visual remnants of previous frames hanging around,
	// but it might be intentional in some circumstances for such remnants to be visible,
	// so, to allow greater freedom for the game developer, the back buffer is not automatically cleared.
	// (and the z-buffer clearing is handled elsewhere, in End3DScene)

	if(SUCCEEDED(status))
		return 1;
	else
		return 0;
}

// VSync method
void WindowsDX::SyncWithRefresh (void)
{
	// (the reason we don't use dd_main->WaitForVerticalBlank here
	//  is because it wastes a huge amount of CPU processing time.)

	DWORD scanline = 0; dd_main->GetScanLine(&scanline); // scanline position in pixels from top

	if((int)scanline <= win_ypos+win_yoffset || (int)scanline >= win_realy+win_ypos+win_yoffset)
		return; // return right away if we can already draw at this instant without tearing glitches

	hyper timecalc = (win_realy+win_ypos+win_yoffset - scanline) * (GetClocksPerSec() / CLOCKS_PER_SEC) / win_scanline_speed;

	AccurateSleep(timecalc,5); // sleep until we think we can draw without tearing glitches
							   // the 5 is to increase accuracy at the expense of a little processing power
}

inline void WindowsDX::SetPosition (int x, int y)
{
	if(x > win_screenx) x -= 65535; // reverse unsigning to avoid totally wrong window position
	if(y > win_screeny) y -= 65535;

	win_xpos = x;
	win_ypos = y;
}

void WindowsDX::SetProportional (void)
{
	int screenx = win_realx;
	int screeny = win_realy;

	if(win_width * screeny > win_height * screenx) {
		win_realx = win_width*screenx/win_width;
		win_realy = win_height*screenx/win_width;
	} else {
		win_realx = win_width*screeny/win_height;
		win_realy = win_height*screeny/win_height;
	}

	win_xoffset = (screenx - win_realx)/2; // temporarily acts as initial offset, which doesn't change
	win_yoffset = (screeny - win_realy)/2; // for fullscreen mode, allowing the viewport to be centered
}

void WindowsDX::SetView (int left, int top, int right, int bottom)
{
	win_viewx1 = left;
	win_viewy1 = top;
	win_viewx2 = right;
	win_viewy2 = bottom;
}

void WindowsDX::ResetView (void)
{
	SetView (0, 0, win_width, win_height);
}

void WindowsDX::GetView (int& xLeft, int& yTop, int& xRight, int& yBottom)
{
	 xLeft = win_viewx1;
	 yTop = win_viewy1;
	 xRight = win_viewx2;
	 yBottom = win_viewy2;
}

inline void WindowsDX::AddBufferedKeyPress (char ch)
{
	win_keybuf[win_keys++] = ch;
	if (win_keys > WINDX_BUFSIZE)
		win_keys = WINDX_BUFSIZE;
}

bool WindowsDX::LockBackBuffer (void)
{
	if(drawLocked) return true;
	
	// since this means a switch from D3D to DD drawing,
	// we have to interrupt batch rendering here
	if(draw_render_batch_open)
		EndRenderBatch();

	DDSURFACEDESC2		ddsd;

	ddsd.dwSize = sizeof(ddsd) ;

	HRESULT status = dd_back_buf->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);	// lock drawing surface
	if(FAILED(status)) {
		DirectDrawError(status);
		return false;
	}

	win_screen = (char *) ddsd.lpSurface;

	drawLocked = true;

	return true;
}

void WindowsDX::Begin3DScene (void)
{
	draw_sort_mode = SCENESORTZ|SCENESORTD|SCENESORT2;

	if(!d3d_zbuffer_enabled) // resort to depth sorting is Z-buffering isn't working
		if(draw_sort_mode != 0)
			draw_sort_mode = SCENESORTD;

	if(draw_sort_mode & SCENESORTD) {
		draw_3D_depth_sort = 1;
	}
	if(draw_sort_mode & SCENESORTZ) {
		d3d_device->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
		d3d_device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	}
}

GameXDrawState * WindowsDX::ms_iter_merge(GameXDrawState * before, GameXDrawState * f1, int n1, GameXDrawState * f2, int n2)
{
	GameXDrawState * first = NULL;
	GameXDrawState * last = NULL;
	GameXDrawState * temp = NULL;
	first = last = ((f1->zcomp >= f2->zcomp) ? f1 : f2);
	for(int i = 0, j = 0 ; i < n1 || j < n2; ) {
		if (i < n1 && (j >= n2 || f1->zcomp >= f2->zcomp)) {
			temp = f1;
			f1 = f1->next;
			i++;
		} else {
			temp = f2;
			f2 = f2->next;
			j++;
		}
		last->next = temp;
		last = temp;
	}
	if(before == NULL)
		draw_state_head = first;
	else
		before->next = first;
	last->next = f2;
	return last;	   
}

void WindowsDX::End3DScene(void)
{
	if(draw_sort_mode & SCENESORTZ)
		d3d_device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);

	if(draw_sort_mode & SCENESORTD) {


		for(int i = 1 ; i < draw_state_num ; i <<= 1) {
			int n1 = i;
			int n2 = i;
			GameXDrawState * before = NULL;
			for(int j = 0 ; j + n1 < draw_state_num ; j += i<<1) {
				GameXDrawState *f1, *f2;
				f1 = f2 = (before == NULL) ? draw_state_head : before->next;
				for(int k = 0 ; k < n1 ; k++) 
					f2 = f2->next;
				n2 = min(n2, draw_state_num-j-n1);	   
				before = ms_iter_merge(before, f1, n1, f2, n2);
			}
		}

		
		draw_3D_depth_sort = 2;

		GameXDrawState * initial = draw_state;
		draw_state = draw_state_head;
		while(draw_state != NULL) {
			if(draw_state->src != VIEWPORT && draw_state->src != NULL
			&& draw_state->flags & DRAWOP_ALPHAINTERSECT && draw_state->flags & DRAW_PLAIN
			&& draw_state->src->m_usage & LOAD_ALPHA
			&& draw_sort_mode & SCENESORTZ && draw_state->dst == VIEWPORT) {
				// if it meets certain conditions and DRAWOP_ALPHAINTERSECT is enabled,
				// draw it twice to allow the alpha image to intersect in 3D:
				d3d_device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
				d3d_device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
				bool keep = (draw_state->flags & DRAWOP_KEEPSTATES) ? true : false;
				if(!keep) draw_state->flags |= DRAWOP_KEEPSTATES;
				InternalDrawMaster(0);
				d3d_device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
				d3d_device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);
				if(!keep) draw_state->flags ^= DRAWOP_KEEPSTATES;
				InternalDrawMaster(0);
			} else // otherwise, draw once as normal
				InternalDrawMaster(0);
			if(draw_state != NULL) {
				GameXDrawState * next = draw_state->next;
				if(draw_state == draw_state_head) draw_state_head = NULL; // causes hard-to-track pointer errors
				if(draw_state == draw_state_tail) draw_state_tail = NULL; // if we forget to reset head+tail to NULL when deleting
				if(draw_state != initial)
					delete draw_state;
				draw_state_num--;
				draw_state = NULL;
				draw_state = next;
			}
		}

		draw_state = initial;
		draw_state->next = NULL;
		// prev should already be NULL

		draw_3D_depth_sort = 0;
	}

	if(draw_sort_mode & SCENESORTZ) {
		d3d_device->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE); 

		if(d3d_device != NULL) {
			if(draw_sort_mode & SCENESORTS) {
				if(!win_alternator)
					d3d_device->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);
				else
					d3d_device->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
			} else {
				d3d_device->Clear(0,NULL,D3DCLEAR_ZBUFFER,0,1.0f,0); // clear the z-buffer
			}
		}
	}

	draw_sort_mode = 0;
}

bool WindowsDX::PrepareDrawState (ImageX * source, bool immediate)
{
	// if drawing normally or storing sorted states
	if(draw_3D_depth_sort == 0 || draw_3D_depth_sort == 1) {
		draw_state->src = source;
		bool ok = GetScreenVertices(draw_state->x,draw_state->y,draw_state->z);
		if(!ok) return false;
		draw_state->zcomp = draw_state->z[0]+draw_state->z[1]+draw_state->z[2]+draw_state->z[3];
	}

	if(draw_3D_depth_sort == 1) { // if depth sorting, store this drawing command (we'll merge-sort the list of stored commands later)

		if(immediate) return true; // unless we're told we don't need to store it
		GameXDrawState * new_state = new GameXDrawState(*draw_state); // make shallow clone of the draw state
		draw_state_num++;
		if(draw_state_head == NULL) {
			// insert at start==end of list
			draw_state_head = new_state;
			draw_state_tail = new_state;
		} else {
			// insert at end of list
			draw_state_tail->next = new_state;
			new_state->prev = draw_state_tail;
			draw_state_tail = new_state;
		}
	}
	return true;
}

void WindowsDX::DirectDrawError (HRESULT status)
{
	if(SUCCEEDED(status) || status == dd_last_error) return;
	dd_last_error = status;
	switch (status) {
	case DDERR_ALREADYINITIALIZED: debug.Output ("DirectDraw: This object is already initialized.") ; break;
	case DDERR_BLTFASTCANTCLIP: debug.Output ("DirectDraw: Return if a clipper object is attached to the source surface passed into a BltFast call.") ; break;
	case DDERR_CANNOTATTACHSURFACE: debug.Output ("DirectDraw: This surface can not be attached to the requested surface.") ; break; 
	case DDERR_CANNOTDETACHSURFACE: debug.Output ("DirectDraw: This surface can not be detached from the requested surface.") ;break;
	case DDERR_CANTCREATEDC: debug.Output ("DirectDraw: Windows can not create any more DCs") ; break;
	case DDERR_CANTDUPLICATE: debug.Output ("DirectDraw: Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.") ; break;
	case DDERR_CLIPPERISUSINGHWND: debug.Output ("DirectDraw: An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.") ; break;
	case DDERR_COLORKEYNOTSET: debug.Output ("DirectDraw: No src color key specified for this operation.") ; break;
	case DDERR_CURRENTLYNOTAVAIL: debug.Output ("DirectDraw: Support is currently not available.") ; break;
	case DDERR_DIRECTDRAWALREADYCREATED: debug.Output ("DirectDraw: A DirectDraw object representing this driver has already been created for this process.") ; break;
	case DDERR_EXCEPTION: debug.Output ("DirectDraw: An exception was encountered while performing the requested operation.") ; break;
	case DDERR_EXCLUSIVEMODEALREADYSET: debug.Output ("DirectDraw: An attempt was made to set the cooperative level when it was already set to exclusive.") ; break;
	case DDERR_GENERIC: debug.Output ("DirectDraw: Generic failure. Unfortunately, this means that DirectX has no idea what went wrong.") ; break;
	case DDERR_HEIGHTALIGN: debug.Output ("DirectDraw: Height of rectangle provided is not a multiple of reqd alignment.") ; break;
	case DDERR_HWNDALREADYSET: debug.Output ("DirectDraw: The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.") ; break;
	case DDERR_HWNDSUBCLASSED: debug.Output ("DirectDraw: HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.") ; break;
	case DDERR_IMPLICITLYCREATED: debug.Output ("DirectDraw: This surface can not be restored because it is an implicitly created surface.") ; break;
	case DDERR_INCOMPATIBLEPRIMARY: debug.Output ("DirectDraw: Unable to match primary surface creation request with existing primary surface.") ; break;
	case DDERR_INVALIDCAPS: debug.Output ("DirectDraw: One or more of the caps bits passed to the callback are incorrect.") ; break;
	case DDERR_INVALIDCLIPLIST: debug.Output ("DirectDraw: DirectDraw does not support the provided cliplist.") ; break;
	case DDERR_INVALIDDIRECTDRAWGUID: debug.Output ("DirectDraw: The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.") ; break;
	case DDERR_INVALIDMODE: debug.Output ("DirectDraw: DirectDraw does not support the requested mode.") ; break;
	case DDERR_INVALIDOBJECT: debug.Output ("DirectDraw: DirectDraw received a pointer that was an invalid DIRECTDRAW object.") ; break;
	case DDERR_INVALIDPARAMS: debug.Output ("DirectDraw: One or more of the parameters passed to the function are incorrect.") ; break;
	case DDERR_INVALIDPIXELFORMAT: debug.Output ("DirectDraw: The pixel format was invalid as specified.") ; break;
	case DDERR_INVALIDPOSITION: debug.Output ("DirectDraw: Returned when the position of the overlay on the destination is no longer legal for that destination->") ; break;
	case DDERR_INVALIDRECT: debug.Output ("DirectDraw: Rectangle provided was invalid.") ; break;
	case DDERR_LOCKEDSURFACES: debug.Output ("DirectDraw: Operation could not be carried out because one or more surfaces are locked.") ; break;
	case DDERR_NO3D: debug.Output ("DirectDraw: There is no 3D present.") ; break;
	case DDERR_NOALPHAHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no alpha accleration hardware present or available.") ; break;
	//case DDERR_NOANTITEARHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no hardware support for synchronizing blits to avoid tearing.	") ; break;
	case DDERR_NOBLTHW: debug.Output ("DirectDraw: No blitter hardware present.") ; break;
	//case DDERR_NOBLTQUEUEHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no hardware support for asynchronous blitting.") ; break;
	case DDERR_NOCLIPLIST: debug.Output ("DirectDraw: No cliplist available.") ; break;
	case DDERR_NOCLIPPERATTACHED: debug.Output ("DirectDraw: No clipper object attached to surface object.") ; break;
	case DDERR_NOCOLORCONVHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no color conversion hardware present or available.") ; break;
	case DDERR_NOCOLORKEY: debug.Output ("DirectDraw: Surface doesn't currently have a color key") ; break;
	case DDERR_NOCOLORKEYHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no hardware support of the destination color key.") ; break;
	case DDERR_NOCOOPERATIVELEVELSET: debug.Output ("DirectDraw: Create function called without DirectDraw object method SetCooperativeLevel being called.") ; break;
	case DDERR_NODC: debug.Output ("DirectDraw: No DC was ever created for this surface.") ; break;
	case DDERR_NODDROPSHW: debug.Output ("DirectDraw: No DirectDraw ROP hardware.") ; break;
	case DDERR_NODIRECTDRAWHW: debug.Output ("DirectDraw: A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.") ; break;
	case DDERR_NOEMULATION: debug.Output ("DirectDraw: Software emulation not available.") ; break;
	case DDERR_NOEXCLUSIVEMODE: debug.Output ("DirectDraw: Operation requires the application to have exclusive mode but the application does not have exclusive mode.") ; break;
	case DDERR_NOFLIPHW: debug.Output ("DirectDraw: Flipping visible surfaces is not supported.") ; break;
	case DDERR_NOGDI: debug.Output ("DirectDraw: There is no GDI present.") ; break;
	case DDERR_NOHWND: debug.Output ("DirectDraw: Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.") ; break;
	case DDERR_NOMIRRORHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no hardware present or available.") ; break;
	case DDERR_NOOVERLAYDEST: debug.Output ("DirectDraw: Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination->") ; break;
	case DDERR_NOOVERLAYHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no overlay hardware present or available.") ; break;
	case DDERR_NOPALETTEATTACHED: debug.Output ("DirectDraw: No palette object attached to this surface.") ; break;
	case DDERR_NOPALETTEHW: debug.Output ("DirectDraw: No hardware support for 16 or 256 color palettes.") ; break;
	case DDERR_NORASTEROPHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no appropriate raster op hardware present or available.") ; break;
	case DDERR_NOROTATIONHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no rotation hardware present or available.") ; break;
	case DDERR_NOSTRETCHHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no hardware support for stretching.") ; break;
	case DDERR_NOT4BITCOLOR: debug.Output ("DirectDraw: DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.") ; break;
	case DDERR_NOT4BITCOLORINDEX: debug.Output ("DirectDraw: DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.") ; break;
	case DDERR_NOT8BITCOLOR: debug.Output ("DirectDraw: DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.") ; break;
	case DDERR_NOTAOVERLAYSURFACE: debug.Output ("DirctX: Returned when an overlay member is called for a non-overlay surface.") ; break;
	case DDERR_NOTEXTUREHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no texture mapping hardware present or available.") ; break;
	case DDERR_NOTFLIPPABLE: debug.Output ("DirectDraw: An attempt has been made to flip a surface that is not flippable.") ; break;
	case DDERR_NOTFOUND: debug.Output ("DirectDraw: Requested item was not found.") ; break;
	case DDERR_NOTLOCKED: debug.Output ("DirectDraw: Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.") ; break;
	case DDERR_NOTPALETTIZED: debug.Output ("DirectDraw: The surface being used is not a palette-based surface.") ; break;
	case DDERR_NOVSYNCHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.") ; break;
	case DDERR_NOZBUFFERHW: debug.Output ("DirectDraw: Operation could not be carried out because there is no hardware support for zbuffer blitting.") ; break;
	case DDERR_NOZOVERLAYHW: debug.Output ("DirectDraw: Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.") ; break;
	case DDERR_OUTOFCAPS: debug.Output ("DirectDraw: The hardware needed for the requested operation has already been allocated.") ; break;
	case DDERR_OUTOFMEMORY: debug.Output ("DirectDraw: DirectDraw does not have enough memory to perform the operation.") ; break;
	case DDERR_OUTOFVIDEOMEMORY: debug.Output ("DirectDraw: DirectDraw does not have enough memory to perform the operation.") ; break;
	case DDERR_OVERLAYCANTCLIP: debug.Output ("DirectDraw: The hardware does not support clipped overlays.") ; break;
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: debug.Output ("DirectDraw: Can only have ony color key active at one time for overlays.") ; break;
	case DDERR_OVERLAYNOTVISIBLE: debug.Output ("DirectDraw: Returned when GetOverlayPosition is called on a hidden overlay.") ; break;
	case DDERR_PALETTEBUSY: debug.Output ("DirectDraw: Access to this palette is being refused because the palette is already locked by another thread.") ; break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS: debug.Output ("DirectDraw: This process already has created a primary surface.") ; break;
	case DDERR_REGIONTOOSMALL: debug.Output ("DirectDraw: Region passed to Clipper::GetClipList is too small.") ; break;
	case DDERR_SURFACEALREADYATTACHED: debug.Output ("DirectDraw: This surface is already attached to the surface it is being attached to.") ; break;
	case DDERR_SURFACEALREADYDEPENDENT: debug.Output ("DirectDraw: This surface is already a dependency of the surface it is being made a dependency of.") ; break;
	case DDERR_SURFACEBUSY: debug.Output ("DirectDraw: Access to this surface is being refused because the surface is locked.\nIf the problem is with the back buffer, try calling GameX.UnlockBackBuffer() first.") ; break;
	case DDERR_SURFACEISOBSCURED: debug.Output ("DirectDraw: Access to surface refused because the surface is obscured.") ; break;
	case DDERR_SURFACELOST: debug.Output ("DirectDraw: Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.") ; break;
	case DDERR_SURFACENOTATTACHED: debug.Output ("DirectDraw: The requested surface is not attached.") ; break;
	case DDERR_TOOBIGHEIGHT: debug.Output ("DirectDraw: Height requested by DirectDraw is too large.") ; break;
	case DDERR_TOOBIGSIZE: debug.Output ("DirectDraw: Size requested by DirectDraw is too large --	the individual height and width are OK.") ; break;
	case DDERR_TOOBIGWIDTH: debug.Output ("DirectDraw: Width requested by DirectDraw is too large.") ; break;
	case DDERR_UNSUPPORTED: debug.Output ("DirectDraw: Action not supported.") ; break;
	case DDERR_UNSUPPORTEDFORMAT: debug.Output ("DirectDraw: FOURCC format requested is unsupported by DirectDraw.") ; break;
	case DDERR_UNSUPPORTEDMASK: debug.Output ("DirectDraw: Bitmask in the pixel format requested is unsupported by DirectDraw.") ; break;
	case DDERR_VERTICALBLANKINPROGRESS: debug.Output ("DirectDraw: Vertical blank is in progress.") ; break;
	case DDERR_WASSTILLDRAWING: debug.Output ("DirectDraw: Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.") ; break;
	case DDERR_WRONGMODE: debug.Output ("DirectDraw: This surface can not be restored because it was created in a different mode.") ; break;
	default: debug.Output ("DirectDraw: Unknown Error.") ; dd_last_error = DD_OK; break;
	}
}

void WindowsDX::Direct3DError (HRESULT status)
{
	if(SUCCEEDED(status) || status == dd_last_error) return;
	dd_last_error = status;
	switch (status) {
	case D3DERR_WRONGTEXTUREFORMAT: debug.Output ("Direct3D: D3DERR_WRONGTEXTUREFORMAT.") ; break;
	case D3DERR_UNSUPPORTEDCOLOROPERATION: debug.Output ("Direct3D: D3DERR_UNSUPPORTEDCOLOROPERATION.") ; break;
	case D3DERR_UNSUPPORTEDCOLORARG: debug.Output ("Direct3D: D3DERR_UNSUPPORTEDCOLORARG.") ; break;
	case D3DERR_UNSUPPORTEDALPHAOPERATION: debug.Output ("Direct3D: D3DERR_UNSUPPORTEDALPHAOPERATION.") ; break;
	case D3DERR_UNSUPPORTEDALPHAARG: debug.Output ("Direct3D: D3DERR_UNSUPPORTEDALPHAARG.") ; break;
	case D3DERR_TOOMANYOPERATIONS: debug.Output ("Direct3D: D3DERR_TOOMANYOPERATIONS.") ; break;
	case D3DERR_CONFLICTINGTEXTUREFILTER: debug.Output ("Direct3D: D3DERR_CONFLICTINGTEXTUREFILTER.") ; break;
	case D3DERR_CONFLICTINGRENDERSTATE: debug.Output ("Direct3D: D3DERR_CONFLICTINGRENDERSTATE.") ; break;
	case D3DERR_UNSUPPORTEDTEXTUREFILTER: debug.Output ("Direct3D: D3DERR_UNSUPPORTEDTEXTUREFILTER.") ; break;
	case D3DERR_CONFLICTINGTEXTUREPALETTE: debug.Output ("Direct3D: D3DERR_CONFLICTINGTEXTUREPALETTE.") ; break;
	case E_NOINTERFACE: debug.Output ("Direct3D: E_NOINTERFACE.") ; break;
	default: debug.Output ("Direct3D: Unknown Error.") ; dd_last_error = DD_OK; break;
	}
}

void WindowsDX::DrawStatus(bool continuously)
{
	if(!continuously && f_rate!=0 && win_global_frame_total%(f_rate/((win_fullscreen)?4:1))!=0)
		return;

	if(!win_request_no_mouse_input && (continuously || (f_rate!=0 && win_global_frame_total%f_rate == 0))) {
		di_statMouseX = GameX.GetMouseX();
		di_statMouseY = GameX.GetMouseY();
	}

	char mouseStr [64];
	if(win_request_no_mouse_input) sprintf(mouseStr,"");
	else sprintf(mouseStr," - (%d, %d) mouse",di_statMouseX,di_statMouseY);

	char str [256];
	sprintf(str,"%d/%d fps - %d/%d cps - %d/%d idle%s",GetCurrentFrameRate(),GetTargetFrameRate(),GetCurrentCycleRate(),GetTargetCycleRate(),GetCurrentIdlePercent(),100,mouseStr);

	if(!win_fullscreen)
		SetWindowText(win_hwnd,str);
	else
		DrawOutlinedText(12,win_height-24,str);
}

// convert draw modes to the closest matching combiner operator
// NOTE: this is COMPLETELY UNRELATED to drawing an image in a certain draw mode like DRAW_ADD,
// this is only used to set how color shading is done, i.e. how the color set by SetDrawShading is used.
// ALSO NOTE: this does not affect how the alpha component of the color is used.
void WindowsDX::SetDrawShadingEffect(DrawFlags effect)
{
	switch(effect & DRAWMODESMASK) {
		case 0: draw_state->color_effect = D3DTOP_SELECTARG1; break; // if no draw mode given, disable shading
		case DRAW_PLAIN: draw_state->color_effect = D3DTOP_SELECTARG2; break; // plain -> the color takes over
		case DRAW_ADD: case DRAW_ADDSHARP: draw_state->color_effect = D3DTOP_ADD; break; // shade color is added to the image being drawn
		case DRAW_GHOST: draw_state->color_effect = D3DTOP_ADDSIGNED; break; // can lighten or darken, usually changes lightness instead of color, often faint
		case DRAW_INTENSIFY: case DRAW_ADDSOFT: draw_state->color_effect = D3DTOP_ADDSMOOTH; break; // similar to with DRAW_ADD
		case DRAW_SUBTRACT: case DRAW_BURN: draw_state->color_effect = D3DTOP_SUBTRACT; break; // shade color subtracted from image -- interesting effect
		case DRAW_INVERT: draw_state->color_effect = D3DTOP_DOTPRODUCT3; break; // not inversion, but it's interesting, albeit black-and-white
		default: case DRAW_MULTIPLY: draw_state->color_effect = D3DTOP_MODULATE; break; // shade color modulates image color -- normally desired effect
	}
}

int WindowsDX::ClipRects (RECT & src_rect, RECT & dst_rect, const RECT & src_bounds, const RECT & dst_bounds)
{
	if(dst_rect.right >= dst_bounds.left && dst_rect.left <= dst_bounds.right) { // In-View Horizontally
		if(dst_rect.bottom >= dst_bounds.top && dst_rect.top <= dst_bounds.bottom) { // In-View Vertically 

			if(dst_rect.right-dst_rect.left == src_rect.right-src_rect.left
			&& dst_rect.bottom-dst_rect.top == src_rect.bottom-src_rect.top) { // If we don't need to scale

				if(dst_rect.left < dst_bounds.left) {
					src_rect.left += dst_bounds.left - dst_rect.left;
					dst_rect.left = dst_bounds.left;
				}
				if(dst_rect.top < dst_bounds.top) {
					src_rect.top += dst_bounds.top - dst_rect.top;
					dst_rect.top = dst_bounds.top;
				}
				if(dst_rect.right > dst_bounds.right) {
					src_rect.right += dst_bounds.right - dst_rect.right;
					dst_rect.right = dst_bounds.right;
				}
				if(dst_rect.bottom > dst_bounds.bottom) {
					src_rect.bottom += dst_bounds.bottom - dst_rect.bottom;
					dst_rect.bottom = dst_bounds.bottom;
				}
				// clip the source too... we could skip this is we were lazy, but then
				// giving an invalid source position would cause the game to crash
				if(src_rect.left < src_bounds.left) {
					dst_rect.left += src_bounds.left - src_rect.left;
					src_rect.left = src_bounds.left;
				}
				if(src_rect.top < src_bounds.top) {
					dst_rect.top += src_bounds.top - src_rect.top;
					src_rect.top = src_bounds.top;
				}
				if(src_rect.right > src_bounds.right) {
					dst_rect.right += src_bounds.right - src_rect.right;
					src_rect.right = src_bounds.right;
				}
				if(src_rect.bottom > src_bounds.bottom) {
					dst_rect.bottom += src_bounds.bottom - src_rect.bottom;
					src_rect.bottom = src_bounds.bottom;
				}
				if(dst_rect.left >= dst_rect.right || dst_rect.top >= dst_rect.bottom) return 0;
				return 1; // Image is in view and should be drawn as-is with the src_rect and dst_rect that we just (possibly) modified
			} else { // need to scale, and if we clip it we need to adjust the clipping amount to match
				
				// the clipping used here isn't perfect since it doesn't allow for pixels to be partially clipped,
				// but it's better than blindly assuming a 1:1 source:destination ratio

				if(dst_rect.left < dst_bounds.left) {
					src_rect.left += (dst_bounds.left - dst_rect.left) * (src_rect.right - src_rect.left) / (dst_rect.right - dst_rect.left);
					dst_rect.left = dst_bounds.left;
				}
				if(dst_rect.top < dst_bounds.top) {
					src_rect.top += (dst_bounds.top - dst_rect.top) * (src_rect.bottom - src_rect.top) / (dst_rect.bottom - dst_rect.top);
					dst_rect.top = dst_bounds.top;
				}
				if(dst_rect.right > dst_bounds.right) {
					src_rect.right += (dst_bounds.right - dst_rect.right) * (src_rect.right - src_rect.left) / (dst_rect.right - dst_rect.left);
					dst_rect.right = dst_bounds.right;
				}
				if(dst_rect.bottom > dst_bounds.bottom) {
					src_rect.bottom += (dst_bounds.bottom - dst_rect.bottom) * (src_rect.bottom - src_rect.top) / (dst_rect.bottom - dst_rect.top);
					dst_rect.bottom = dst_bounds.bottom;
				}
				if(src_rect.left < src_bounds.left) {
					dst_rect.left += (src_bounds.left - src_rect.left) * (dst_rect.right - dst_rect.left) / (src_rect.right - src_rect.left);
					src_rect.left = src_bounds.left;
				}
				if(src_rect.top < src_bounds.top) {
					dst_rect.top += (src_bounds.top - src_rect.top) * (dst_rect.bottom - dst_rect.top) / (src_rect.bottom - src_rect.top);
					src_rect.top = src_bounds.top;
				}
				if(src_rect.right > src_bounds.right) {
					dst_rect.right += (src_bounds.right - src_rect.right) * (dst_rect.right - dst_rect.left) / (src_rect.right - src_rect.left);
					src_rect.right = src_bounds.right;
				}
				if(src_rect.bottom > src_bounds.bottom) {
					dst_rect.bottom += (src_bounds.bottom - src_rect.bottom) * (dst_rect.bottom - dst_rect.top) / (src_rect.bottom - src_rect.top);
					src_rect.bottom = src_bounds.bottom;
				}
				if(dst_rect.left >= dst_rect.right || dst_rect.top >= dst_rect.bottom) return 0;
				return 2; // Image is in view and should be drawn SCALED with the src_rect and dst_rect that we just (possibly) modified
			}
		}
	}
	return 0; // Image is not in view at all and so shouldn't be drawn
}


void WindowsDX::SetMouseMove (int x, int y)
{
	mouse_x = x;
	mouse_y = y;

	if(!win_fullscreen && win_active && CanHideCursor()) {
		int dx = 2*GetMouseDX();
		int dy = 2*GetMouseDY();
		if(x < 0 || y < 0 || x >= win_realx || y >= win_realy) {
			UnhideCursor();
		} else {
			HideCursor();
		}
	}
}

// pauses whatever song is currently playing (has only been tested internally to GameX)
void WindowsDX::PauseMusic (void)
{
	if(current_music == NULL) return;
	if(current_music->GetPlayState() != 1) return;

	switch(current_music->format) {
		case MUSIC_TYPE_MIDI: {
			float current_tempo = master_music_tempo;
			SetMusicTempo(DMUS_MASTERTEMPO_MIN*2);
			master_music_tempo = current_tempo;

			float current_volume = master_music_volume;
			SetMasterMusicVolume(0);
			master_music_volume = current_volume;
		}	break;
		case MUSIC_TYPE_MP3: {
			current_music->dsh_mc->Pause();
		}	break;
	}
	current_music->SetPlayState(2);
}

// resumes whatever song is currently playing (has only been tested internally to GameX)
// due to the way it is paused, a stray note might play for a while after resuming MIDIs
void WindowsDX::ResumeMusic (void)
{
	if(current_music == NULL) return;

	switch(current_music->format) {
		case MUSIC_TYPE_MIDI: {
			SetMusicTempo(master_music_tempo);
			SetMasterMusicVolume(master_music_volume);
		}	break;
		case MUSIC_TYPE_MP3: {
			if(current_music->GetPlayState() != 0)
				current_music->dsh_mc->Run();
		}	break;
	}
	current_music->SetPlayState(1);
}

void WindowsDX::HideCursor(void)
{
	if(!win_cursor_hidden) {
		ShowCursor(false);
		win_cursor_hidden = true;
	}
}

void WindowsDX::UnhideCursor(void)
{
	if(win_cursor_hidden) {
		ShowCursor(true);
		win_cursor_hidden = false;
	}
}

void WindowsDX::ClipTile (int x, int y, int xres, int yres, int &mx, int &my, int &offx, int &offy)
{
	if (x<0) {
		mx = ((int) (x / xres)) - 1;
		offx = (x % xres) + xres;
	} else {
		mx = ((int) (x / xres)) ;
		offx = x % xres;
	}
	if (y<0) {
		my = ((int) (y / yres)) - 1;		
		offy = (y % yres) + yres;
	} else {
		my = ((int) (y / yres)) ;		
		offy = y % yres;
	}	
}

// Params: left, top, right, bottom, red (out of 255), green (out of 255), blue (out of 255)
// (this is the toned-down but ultra-fast version of DrawRect)
void WindowsDX::InternalDrawRectDD (int r, int g, int b, int x1, int y1, int x2, int y2)
{
	if(x2 <= x1) {
		if(draw_state->flags & DRAWOP_NOCULL && x1 != x2) {
			int t = x2;
			x2 = x1;
			x1 = t;
		} else return;
	}
	if(y2 <= y1) {
		if(draw_state->flags & DRAWOP_NOCULL && y1 != y2) {
			int t = y2;
			y2 = y1;
			y1 = t;
		} else return;
	}

	RECT dst_rect = {x1,y1,x2,y2};

	UnlockBackBuffer() ; // unlock drawing surface if necessary

	DDBLTFX ddbltfx;
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = (((r >> (8-GameX.win_maxr)) << win_shiftr) + ((g >> (8-GameX.win_maxg)) << win_shiftg) + ((b >> (8-GameX.win_maxb)) << win_shiftb));

	HRESULT status = dd_back_buf->Blt(&dst_rect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &ddbltfx);
	if(FAILED(status))
		DirectDrawError(status);
	else return;
	
	// if we failed, try it the old way:

	LockBackBuffer() ;

	unsigned short *start, *pixel;
	int i, j, clr;

	clr = ddbltfx.dwFillColor;
	start = (unsigned short *) win_screen;
	start += y1*win_pitch + x1;
	for (j=y1; j<=y2; j++) {
		pixel = start;
		for (i=x1; i<=x2; i++)
			*pixel++ = clr;
		start += win_pitch;
	}
}

// 2D drawing functions:
void WindowsDX::DrawPoint	(ColorX & clr, int x, int y)
{
	draw_state->colors = clr; 
	draw_state->vertices_3D = 0;
	draw_state->dst_quad2d = Quad2D((float)x,(float)y,DEFAULT2,DEFAULT2);
	InternalDrawMaster(NULL);
}
void WindowsDX::DrawLine	(ColorX & clr, int x1,int y1, int x2,int y2) 
{
	draw_state->colors = clr;
	draw_state->angle = 0.0f;
	draw_state->vertices_3D = -2;
	draw_state->dst_quad2d = Quad2D((float)x1,(float)y1,(float)x2,(float)y2);
	InternalDrawMaster(NULL);
}
void WindowsDX::DrawRect	(ColorX & clr, int xLeft, int yTop, int xRight, int yBottom)
{
	draw_state->colors = clr;
	draw_state->vertices_3D = 0;
	draw_state->dst_quad2d = Quad2D((float)xLeft,(float)yTop,(float)xRight,(float)yBottom);
	InternalDrawMaster(NULL);
}

void WindowsDX::DrawPolygon (ColorX & clr, int x1,int y1, int x2,int y2, int x3,int y3, int x4,int y4)
{
	draw_state->colors = clr;
	draw_state->vertices_3D = 0;
	draw_state->dst_quad2d = Quad2D((float)x1,(float)y1,(float)x2,(float)y2,(float)x3,(float)y3,(float)x4,(float)y4);
	if(!(draw_state->flags & DRAWOP_NOCULL))
		draw_state->flags |= DRAWOP_NOCULL;
	InternalDrawMaster(NULL);
}

// 2D image drawing functions:
void WindowsDX::DrawImage			(ImageX * img, int x, int y)
{
	draw_state->vertices_3D = 0;
	draw_state->dst_quad2d = Quad2D((float)x,(float)y,(float)DEFAULT2,(float)DEFAULT2);
	InternalDrawMaster(img);
}

void WindowsDX::DrawImage(ImageX* img, int x, int y, int width, int height )
{
	RECT r = { 0, 0, width, height };

	draw_state->vertices_3D = 0;	
	draw_state->src_rect = r;
	draw_state->dst_quad2d = Quad2D((float)x,(float)y, (float)(DEFAULT2), (float)(DEFAULT2) );
	InternalDrawMaster(img);
}

void WindowsDX::DrawImage			(ImageX * img, int x, int y, float angle, float scale) /* shorthand for calling SetDrawAngle, SetDrawScale before drawing DrawImage */
{
	SetDrawAngle(angle);
	SetDrawScale(scale);
	DrawImage(img,x,y);
}
void WindowsDX::DrawLineImage		(ImageX * img, int x1,int y1, int x2,int y2, bool pixelUnitScale)
{
	draw_state->angle = 0.0f;
	draw_state->vertices_3D = pixelUnitScale ? -2 : -1;
	draw_state->dst_quad2d = Quad2D((float)x1,(float)y1,(float)x2,(float)y2);
	InternalDrawMaster(img);
}
void WindowsDX::DrawTexturedRect	(ImageX * img, int xLeft, int yTop, int xRight, int yBottom)
{
	draw_state->src = img;
	draw_state->vertices_3D = 0;
	draw_state->dst_quad2d = Quad2D((float)xLeft,(float)yTop,(float)xRight,(float)yBottom);
	InternalDrawMaster(img);
}
void WindowsDX::DrawTexturedPolygon (ImageX * img, int x1,int y1, int x2,int y2,  int x3,int y3,  int x4,int y4)
{
	draw_state->src = img;
	draw_state->vertices_3D = 0;
	draw_state->dst_quad2d = Quad2D((float)x1,(float)y1,(float)x2,(float)y2,(float)x3,(float)y3,(float)x4,(float)y4);
	if(!(draw_state->flags & DRAWOP_NOCULL)) draw_state->flags |= DRAWOP_NOCULL; // culling of 2D polys would cause confusion
	InternalDrawMaster(img);
}

// 3D drawing functions:
void WindowsDX::DrawPoint3D   (ColorX & clr, Vector3DF & v)
{
	draw_state->colors = clr;
	draw_state->vertices_3D = 3;
	draw_state->dst_point3d = v;
	InternalDrawMaster(NULL);
}
void WindowsDX::DrawLine3D	  (ColorX & clr, Vector3DF & v1, Vector3DF & v2)
{
	draw_state->colors = clr;
	draw_state->vertices_3D = 5;
	draw_state->dst_quad3d = Quad3D(v1,v2,v1,v2);
	InternalDrawMaster(NULL);
}
void WindowsDX::DrawPolygon3D (ColorX & clr, Vector3DF & v1, Vector3DF & v2, Vector3DF & v3, Vector3DF & v4)
{
	draw_state->colors = clr;
	draw_state->vertices_3D = 1;
	draw_state->dst_quad3d = Quad3D(v1,v2,v3,v4);
	InternalDrawMaster(NULL);
}

// 3D image drawing functions:
void WindowsDX::DrawImage3D 		  (ImageX * img, Vector3DF & v, bool worldUnitScale)
{
	draw_state->vertices_3D = worldUnitScale?3:2;
	draw_state->dst_point3d = v;
	InternalDrawMaster(img);
}
void WindowsDX::DrawLineImage3D 	  (ImageX * img, Vector3DF & v1, Vector3DF & v2, bool worldUnitScale)
{
	draw_state->src = img;
	draw_state->vertices_3D = worldUnitScale ? 5 : 4;
	draw_state->dst_quad3d = Quad3D(v1,v2,v1,v2);
	if(draw_state->warp_mode == 0) draw_state->warp_mode = 2; // auto-perspective-correct
	InternalDrawMaster(img);
}
void WindowsDX::DrawTexturedPolygon3D (ImageX * img, Vector3DF & v1, Vector3DF & v2, Vector3DF & v3, Vector3DF & v4)
{
	draw_state->src = img;
	draw_state->vertices_3D = 1;
	draw_state->dst_quad3d = Quad3D(v1,v2,v3,v4);
	if(draw_state->warp_mode == 0) draw_state->warp_mode = 2; // auto-perspective-correct
	InternalDrawMaster(img);
}
void WindowsDX::ClearScreen (void)
{
	if(d3d_main)
		FillScreen(ColorX(0,0,0));
	else
		InternalDrawRectDD(0,0,0, win_viewx1,win_viewy1,win_viewx2,win_viewy2);
}
void WindowsDX::FillScreen	(ColorX & clr)
{
	draw_state->colors = clr;
	draw_state->vertices_3D = 0;
	draw_state->dst_quad2d = Quad2D(0,0,DEFAULT,DEFAULT);
	InternalDrawMaster(NULL);
}
void WindowsDX::BlurScreen	(float amount)
{
	BlurRect(amount, 0,0,DEFAULT,DEFAULT);
}

// fills quadrilateral with screen vertices,
// returns false if it isn't on the screen
bool WindowsDX::GetScreenVertices(float * x, float * y, float * z)
{
	switch(draw_state->vertices_3D) {
		case 0: // regular 2D sprite or rectangle or polygon
			{
				for(int i = 0 ; i < 4 ; i++) {
					x[i] = draw_state->dst_quad2d.x[i];
					y[i] = draw_state->dst_quad2d.y[i];
					z[i] = draw_state->depth2D;
				}

				if(x[3] != DEFAULT2 && (draw_state->scalex != 1.0f || draw_state->scaley != 1.0f)) {
					float xavg = (x[0]+x[1]+x[2]+x[3])/4.0f;
					float yavg = (y[0]+y[1]+y[2]+y[3])/4.0f;
					for(int i = 0 ; i < 4 ; i++) {
						x[i] = ((x[i] - xavg) * draw_state->scalex) + xavg;
						y[i] = ((y[i] - yavg) * draw_state->scaley) + yavg;
					}
				}
			}
			break;
		case 4: // 3D line (line sprite)
		case 5: // 3D line with scale in world units of width instead of as width factor
			{
				Vector3DF top = draw_state->dst_quad3d.v[1];
				Vector3DF bottom = draw_state->dst_quad3d.v[0];
				Vector3DF along = top; along -= bottom;
				
				float relative_width;
				if(draw_state->vertices_3D == 4) {
					relative_width = draw_state->scalex/2.0f;
					if(draw_state->src != NULL) {
						RECT rect = ConvertRECT(draw_state->src_rect, draw_state->src);
						relative_width *= (float)(rect.right-rect.left)/(float)(rect.bottom-rect.top);
					}
				} else {
					float distance = along.Length();
					if(distance == 0.0f)
						return false;
					relative_width = draw_state->scalex / distance;
				}
				if(relative_width == 0.0f) return false;

				relative_width *= along.Length();
				
				Vector3DF into = draw_state->cam->GetDirVector();
				Vector3DF side = Vector3DF(along);
				side.Cross(into);
				side.Normalize();

				// do special rotation about line axis here
				if(draw_state->angle != 0.0f)
				{
					float angle = draw_state->angle * -DEGtoRAD;
					float sinAng = sinf(angle);
					float cosAng = cosf(angle);

					// rotate the "side" vector about the arbitrary axis "along":
					along.Normalize();
					side = (side*cosAng) + (along*((1.0f-cosAng)*(side.Dot(along)))) + (along.Cross(side)*sinAng);

					draw_state->angle = 0.0f; // angle converted to quad coords; prevent extra 2D rotation
				}

				side *= relative_width;

				draw_state->dst_quad3d.v[0] = top+side;
				draw_state->dst_quad3d.v[1] = top-side;
				draw_state->dst_quad3d.v[2] = bottom+side;
				draw_state->dst_quad3d.v[3] = bottom-side;

				draw_state->vertices_3D = 1; // line converted to quad
			}
			// no break, must be followed by case 1
		case 1: // quad of 4 Vector3DF's   (polygon)
			{
				bool ok = false;
				float xd, yd, zd;
				for(int i = 0 ; i < 4 ; i++) {
					
					draw_state->cam->Project(draw_state->dst_quad3d.v[i], xd, yd, zd);

///					if(zd < draw_state->cam->GetNear())
///						return false;
					if(zd > draw_state->cam->GetNearClip())
						ok = true;

					x[i] = xd;
					y[i] = yd;
					z[i] = zd;

					if(draw_state->warp_mode == 2) {
						if(zd != 0.0f)	draw_state->warp[i] = 1.0f/(float)zd;
						else			draw_state->warp[i] = 1.0f;
					}
				}
				if(!ok) return false;
				draw_state->angle = 0.0f;
			}
			break;
		case 2: // 3D point, or
		case 3: // 3D point with 3D scaling    (point sprite)
			{
				float xd, yd, zd;			
				draw_state->cam->Project(draw_state->dst_point3d, xd, yd, zd);

				if(zd < draw_state->cam->GetNearClip())
					return false;

				if(draw_state->vertices_3D == 3) {
					float div = 2.0f*max(0.02f,zd)/draw_state->cam->GetPixelsPerUnit();
					float xp = draw_state->scalex/div;
					float yp = draw_state->scaley/div;

					x[0] = xd-xp;
					x[1] = xd+xp;
					x[2] = xd-xp;
					x[3] = xd+xp;
					y[0] = yd-yp;
					y[1] = yd-yp;
					y[2] = yd+yp;
					y[3] = yd+yp;
					z[0] = zd;
					z[1] = zd;
					z[2] = zd;
					z[3] = zd;
				} else {
					x[0] = xd;
					x[2] = xd;
					x[3] = (float)DEFAULT2;
					y[0] = yd;
					y[1] = yd;
					z[0] = zd;
					z[1] = zd;
					z[2] = zd;
					z[3] = zd;
				}

			}
			break;
		case -1: // 2D line (line sprite)
		case -2: // 2D line with scale in pixels of width instead of as width factor
			{
				float xd[2], yd[2];

				xd[0] = draw_state->dst_quad2d.x[0];
				yd[0] = draw_state->dst_quad2d.y[0];
				xd[1] = draw_state->dst_quad2d.x[1];
				yd[1] = draw_state->dst_quad2d.y[2];

				float rise = yd[1]-yd[0];
				float run = xd[0]-xd[1];

				float relative_width;
				if(draw_state->vertices_3D == -1) {
					relative_width = draw_state->scalex/2.0f;
					if(draw_state->src != NULL) {
						RECT rect = ConvertRECT(draw_state->src_rect, draw_state->src);
						relative_width *= (float)(rect.right-rect.left)/(float)(rect.bottom-rect.top);
					}
				} else {
					float distance = sqrtf(run*run+rise*rise);
					if(distance == 0.0f) return false;
					relative_width = draw_state->scalex/2.0f / distance;
				}
				if(relative_width == 0.0f) return false;

				rise *= relative_width;
				run *= relative_width;

				x[0] = xd[1] + rise;
				y[0] = yd[1] + run;
				z[0] = draw_state->depth2D;

				x[1] = xd[1] - rise;
				y[1] = yd[1] - run;
				z[1] = draw_state->depth2D;

				x[2] = xd[0] + rise;
				y[2] = yd[0] + run;
				z[2] = draw_state->depth2D;

				x[3] = xd[0] - rise;
				y[3] = yd[0] - run;
				z[3] = draw_state->depth2D;
			}
			break;
	}

	if(draw_state->dst == VIEWPORT) {
		draw_state->dst_bounds.left = win_viewx1;
		draw_state->dst_bounds.top = win_viewy1;
		draw_state->dst_bounds.right = win_viewx2;
		draw_state->dst_bounds.bottom = win_viewy2;
	} else if(draw_state->dst != NULL) {
		draw_state->dst_bounds.left = 0;
		draw_state->dst_bounds.top = 0;
		draw_state->dst_bounds.right = draw_state->dst->m_xres;
		draw_state->dst_bounds.bottom = draw_state->dst->m_yres;
	}
	if(draw_state->vertices_3D > 0) { // if it's 3D
		int x1, y1, x2, y2;
		draw_state->cam->GetWindow(x1, y1, x2, y2);
		if(x1 > draw_state->dst_bounds.left && x1 < draw_state->dst_bounds.right)
			draw_state->dst_bounds.left = x1;
		if(y1 > draw_state->dst_bounds.top && y1 < draw_state->dst_bounds.bottom)
			draw_state->dst_bounds.top = y1;
		if(x2 > draw_state->dst_bounds.left && x2 < draw_state->dst_bounds.right)
			draw_state->dst_bounds.right = x2;
		if(y2 > draw_state->dst_bounds.top && y2 < draw_state->dst_bounds.bottom)
			draw_state->dst_bounds.bottom = y2;
	}

	if(x[3] == DEFAULT) { // default value for entire destination
		if(draw_state->dst == VIEWPORT) {
			x[0] = x[2] = (float)win_viewx1;
			y[0] = y[1] = (float)win_viewy1;
			x[1] = x[3] = (float)win_viewx2;
			y[2] = y[3] = (float)win_viewy2;
		} else {
			x[1] = (float)draw_state->dst->m_xres;
			y[2] = (float)draw_state->dst->m_yres;
			if(draw_state->dst->m_xmult != 1.0f) x[1] *= draw_state->dst->m_xmult;
			if(draw_state->dst->m_ymult != 1.0f) y[2] *= draw_state->dst->m_ymult;
			x[3] = x[1];
			y[3] = y[2];
		}
	}
	float scalex = draw_state->scalex;
	float scaley = draw_state->scaley;
	if(draw_state->flags & DRAWOP_NOCULL) {
		if(scalex == 0.0f || scaley == 0.0f) return false; // allows 2D negative scale flipping when culling is off
	} else {
		if(scalex <= 0.0f || scaley <= 0.0f) return false;
	}

	RECT sourceRect = ConvertRECT(draw_state->src_rect, draw_state->src);

	if(x[3] == DEFAULT2) { // default value that means we have to convert a point into a rectangle

		float xOff = scalex*(sourceRect.right - sourceRect.left);
		float yOff = scaley*(sourceRect.bottom - sourceRect.top);

		if(draw_state->src != NULL && draw_state->src != VIEWPORT) {
			if(draw_state->src->m_xmult != 1.0f) xOff /= draw_state->src->m_xmult;
			if(draw_state->src->m_ymult != 1.0f) yOff /= draw_state->src->m_ymult;
		}

		x[2] = x[0];
		x[3] = x[0] + xOff;
		x[1] = x[0] + xOff;
		y[3] = y[0] + yOff;
		y[2] = y[0] + yOff;

		if(draw_state->flags & DRAWOP_CENTERED) { // if the point is the center, not the top-left corner, of the destination
			float halfWidth = floorf(((x[1]+x[3]) - (x[0]+x[2])) / 4.0f); // if we make halfWidth a float, it results in higher accuracy but more blurriness with images that should have 1 to 1 pixel to texel ratio
			float halfHeight = floorf(((y[2]+y[3]) - (y[0]+y[1])) / 4.0f);
			for(int i = 0 ; i < 4 ; i++) {
				x[i] = floorf(x[i] - halfWidth);
				y[i] = floorf(y[i] - halfHeight);
			}
			draw_state->flags ^= DRAWOP_CENTERED;
		}
	}

	if(draw_state->transx || draw_state->transy)
	for(int t = 0 ; t < 4 ; t++) {
		x[t] += draw_state->transx;
		y[t] += draw_state->transy;
	}

	float angle = draw_state->angle;
	if(angle != 0.0f) {
		float x_offset = (x[0]+x[1]+x[2]+x[3]) / 4;
		float y_offset = (y[0]+y[1]+y[2]+y[3]) / 4;
		angle = (360.0f-angle)*DEGtoRAD; // convert to CCW radians
		float cosAngle = cosf(angle);
		float sinAngle = sinf(angle);

		// rotate the image by simply rotating the four points
		// of the rectangular polygon that the image is being mapped onto
		// like so:
		//				   >			  0
		//				  / 			/\				
		//	0----1		 0----1 	   /  \ 			
		//	|	 |		 |	  |\	2 /    \	   
		//	|	 |		 |	  | v	 /		\	   
		//	|	 |	   ^ |	  | 	 \		/		 
		//	|	 |		\|	  | 	  \    / 1		   
		//	2----3	->	 2----3   ->   \  / 				
		//					 /			\/				  
		//					<		   3	 

		for(int i = 0 ; i < 4 ; i++) {
			x[i] -= x_offset;
			y[i] -= y_offset;
			float xr = x[i]*cosAngle - y[i]*sinAngle;
			float yr = x[i]*sinAngle + y[i]*cosAngle;
			x[i] = xr + x_offset;
			y[i] = yr + y_offset;
		}
	}

	// catch the things that are completely out of bounds and skip drawing them:
	bool inside = false;
	for(int i = 0 ; i < 4 ; i++) {
		if(x[i] >= draw_state->dst_bounds.left && x[i] <= draw_state->dst_bounds.right && y[i] >= draw_state->dst_bounds.top && y[i] <= draw_state->dst_bounds.bottom) {
			inside = true;
			break;
		}
	}
	if(!inside) {
		// none of the points are in bounds, but that doesn't necessarily mean it's safe to skip drawing it...
		// we'll just do a simple check to see if all of the ones that were out of bounds
		// were on the same side, in which case they couldn't possibly intersect the boundary:
		
		int i;
		bool allout;

		allout = true;
		for(i = 0 ; i < 4 ; i++) {
			if(x[i] >= draw_state->dst_bounds.left) {
				allout = false;
				break;
			}
		}
		if(allout) return false; // all points are too far left; don't draw

		allout = true;
		for(i = 0 ; i < 4 ; i++) {
			if(y[i] >= draw_state->dst_bounds.top) {
				allout = false;
				break;
			}
		}
		if(allout) return false; // all points are too far up; don't draw

		allout = true;
		for(i = 0 ; i < 4 ; i++) {
			if(x[i] <= draw_state->dst_bounds.right) {
				allout = false;
				break;
			}
		}
		if(allout) return false; // all points are too far right; don't draw

		allout = true;
		for(i = 0 ; i < 4 ; i++) {
			if(y[i] <= draw_state->dst_bounds.bottom) {
				allout = false;
				break;
			}
		}
		if(allout) return false; // all points are too far down; don't draw
		
		// if we got here, it *probably* needs to be drawn...
		// here is the only no-draw case we may have missed:
		//
		//			polygon (with image on it)
		//			  /\
		//			 /	\
		//			/	 \
		//		   <	  \
		//		   .\	   >
		//		   . \	  /
		// +--------+ \  /
		// |		|..\/
		// |		|
		// |		|
		// |		|
		// +--------+
		// screen (or other destination boundary)
		// 
		// we could do boundary intersection checks at this point but it wouldn't be worth it.
		// since all completely out of bound images would have to go through the check
		// every single time they're drawn, and the time used for intersection checks could add up quickly
	}

	return true;
}

// convert lowered (from 255) alpha into lowered r,g,b
// used for drawing modes that draw using red, green, blue, effective opacity determined by brightness,
void WindowsDX::ScaleByAlpha (ColorX & colors)
{
	for(int i = 0 ; i < 4 ; i++) {
		if(colors.c[i][3]!=255) {
			for(int j = 0 ; j < 3 ; j++)
				colors.c[i][j] = colors.c[i][j] * colors.c[i][3] / 255;
			colors.c[i][3] = 255;
		}
	}
}

void WindowsDX::SpecialPauseFade(float blur1, float blur2, int r1, int r2, int g1, int g2, int b1, int b2, int a)
{
	bool warn = win_warning_on_draw; SetWarningOnDraw(false);
	int xv1 = win_viewx1, yv1 = win_viewy1, xv2 = win_viewx2, yv2 = win_viewy2;
	SetView(0,0,win_width,win_height);
	BlurScreen(blur1);
	BlurScreen(blur2);
	ColorX c;
	if(win_fullscreen || win_screenbpp != 16)
		c = ColorX(GetRandomInt(r1,r2),GetRandomInt(g1,g2),GetRandomInt(b1,b2),a);
	else
		c = ColorX(GetRandomInt(r1,r2),GetRandomInt(g1,g2),GetRandomInt(b1,b2),a,
				   GetRandomInt(r1,r2),GetRandomInt(g1,g2),GetRandomInt(b1,b2),a,
				   GetRandomInt(r1,r2),GetRandomInt(g1,g2),GetRandomInt(b1,b2),a,
				   GetRandomInt(r1,r2),GetRandomInt(g1,g2),GetRandomInt(b1,b2),a);
	FillScreen(c);
	SetView(xv1,yv1,xv2,yv2);
	if(warn) SetWarningOnDraw(true);
}

// blurs the target image or the viewport by scaling down then up
// amount of 0.0f means not blurred at all, 0.5f means somewhat blurred, 1.0f means blurred all the way into a single color
void WindowsDX::BlurRect(float amount, int x1, int y1, int x2, int y2)
{
	if(win_warning_on_draw && draw_state->dst == VIEWPORT) {
		ReportProblem(GAMEX_INVALID_DRAW_TIME_FAIL);
		ResetDrawStates();
		return;
	}

	if(amount <= 0.01f || (!win_blur_supported && draw_state->dst == VIEWPORT)) return;

	if(x2 <= x1 && x2 != DEFAULT) {
		if(draw_state->flags & DRAWOP_NOCULL && x1 != x2) {
			int t = x2;
			x2 = x1;
			x1 = t;
		} else return;
	}
	if(y2 <= y1 && y2 != DEFAULT) {
		if(draw_state->flags & DRAWOP_NOCULL && y1 != y2) {
			int t = y2;
			y2 = y1;
			y1 = t;
		} else return;
	}
	
	amount = (1.0f - amount) * (1.0f - amount); // invert about 0.5 and adjust amount toward 0
	LPDIRECTDRAWSURFACE7 surface;
	if(draw_state->dst == VIEWPORT) {
		surface = dd_back_buf;
		UnlockBackBuffer();
	} else if(draw_state->dst == NULL) {
		return;
	} else {
		surface = draw_state->dst->m_surface;
	}

	if(x2 == DEFAULT) { // blur entire thing
		if(draw_state->dst == VIEWPORT) {
			x1 = win_viewx1;
			y1 = win_viewy1;
			x2 = win_viewx2;
			y2 = win_viewy2;
		} else {
			x2 = draw_state->dst->m_xres;
			y2 = draw_state->dst->m_yres;
			if(draw_state->dst->m_xmult != 1.0f) x2 = (int)((float)x2 * draw_state->dst->m_xmult);
			if(draw_state->dst->m_ymult != 1.0f) y2 = (int)((float)y2 * draw_state->dst->m_ymult);
		}
	}

	int width = (x2-x1);
	int height = (y2-y1);
	RECT full_rect = {x1,y1,x2,y2};
	RECT mini_rect = {x1+(int)(width*(1-amount))/2, y1+(int)(height*(1-amount))/2, x1+(int)(width*(1+amount))/2, y1+(int)(height*(1+amount))/2} ;
	if(mini_rect.right == mini_rect.left) mini_rect.left--;
	if(mini_rect.bottom == mini_rect.top) mini_rect.top--;

	InternalDrawImageDD(surface, surface, full_rect, mini_rect, 1);
	InternalDrawImageDD(surface, surface, mini_rect, full_rect, 1);
}


void WindowsDX::DrawText (int x, int y, char *msg, int r, int g, int b)
{
	if(win_warning_on_draw) {
		ReportProblem(GAMEX_INVALID_DRAW_TIME_FAIL);
		ResetDrawStates();
		return;
	}

	LockBackBuffer();

	HDC hdc = NULL;
	HRESULT status = dd_back_buf->GetDC(&hdc); // Get the device context handle.
	if(FAILED(status)) {
		DirectDrawError(status);
		return;
	}
										// Write the message.
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(r,g,b));
	TextOut(hdc, x, y, msg, strlen(msg)); // just use the Windows function for now, although it's slow

	dd_back_buf->ReleaseDC(hdc);		// Release the device context.
	return;
}

void WindowsDX::DrawOutlinedText (int x, int y, char *msg, int r, int g, int b, int ro, int go, int bo)
{
	if(win_warning_on_draw) {
		ReportProblem(GAMEX_INVALID_DRAW_TIME_FAIL);
		ResetDrawStates();
		return;
	}

	LockBackBuffer();

	HDC hdc = NULL;
	HRESULT status = dd_back_buf->GetDC(&hdc); // Get the device context handle.
	if(FAILED(status)) {
		DirectDrawError(status);
		return;
	}
										// Write the message.
	SetBkMode(hdc, TRANSPARENT);

	// Draw an outline around the text
	const int dir[8] = {1,1,1,0,-1,-1,-1,0}; // direction table for outline
	SetTextColor(hdc, RGB(ro,go,bo));
	for (int i=0 ; i<8 ; i++)
		TextOut(hdc, x+dir[i], y+dir[(i+6)%8], msg, strlen(msg));

	// Now draw the text normally
	SetTextColor(hdc, RGB(r,g,b));
	TextOut(hdc, x, y, msg, strlen(msg));

	dd_back_buf->ReleaseDC(hdc);		// Release the device context.
	return;
}


// draws an image tiled over the entire screen,
// good for things such as drawing backgrounds
void WindowsDX::DrawTiled (ImageX * img, float translation_x, float translation_y, float rotation_center_x, float rotation_center_y)
{
	bool clear_states = !(draw_state->flags & DRAWOP_KEEPSTATES);
	draw_state->flags |= DRAWOP_KEEPSTATES;

	int map_x1, map_y1;
	int map_x2, map_y2; 
	int off_x, off_y;
	float img_x, img_y; 
	float mx, my;

	RECT source_rect = ConvertRECT(draw_state->src_rect, img);

	float xres = draw_state->scalex * (float)(source_rect.right - source_rect.left);
	float yres = draw_state->scaley * (float)(source_rect.bottom - source_rect.top);
	if(img != NULL && draw_state->src != VIEWPORT) {
		if(img->m_xmult != 1.0f) xres /= img->m_xmult;
		if(img->m_ymult != 1.0f) yres /= img->m_ymult;
	}
	if(xres < 2 || yres < 2) return;

	ClipTile ((int)translation_x+win_viewx2,(int)translation_y+win_viewy2, (int)xres, (int)yres, map_x2, map_y2, off_x, off_y) ;
	ClipTile ((int)translation_x+win_viewx1,(int)translation_y+win_viewy1, (int)xres, (int)yres, map_x1, map_y1, off_x, off_y) ;

	float map_numx = (float)(map_x2 - map_x1 + 1);
	float map_numy = (float)(map_y2 - map_y1 + 1);

	if(draw_state->angle == 0.0f) {
		draw_state->vertices_3D = 0;
		for (my = 0; my < map_numy; my++) {
			img_y = my * yres - off_y;
			img_x = (float)(-off_x);
			for (mx = 0; mx < map_numx; mx++) {
				draw_state->dst_quad2d = Quad2D(img_x,img_y,DEFAULT2,DEFAULT2);
				InternalDrawMaster(img);
				img_x += xres;
			}
		}
	} else {

		if(rotation_center_x == DEFAULT)
			rotation_center_x = (float)(win_viewx1+win_viewx2)/2;
		if(rotation_center_y == DEFAULT)
			rotation_center_y = (float)(win_viewy1+win_viewy2)/2;

		float sinAngle, cosAngle;

		sinAngle = sinf((360-draw_state->angle)*DEGtoRAD);
		cosAngle = cosf((360-draw_state->angle)*DEGtoRAD);

		map_numx *= 3;
		map_numy *= 3;

		draw_state->vertices_3D = 0;
		for (my = -map_numy; my < map_numy; my++) {
			img_y = my * yres;
			img_x = -map_numx * xres;
			for (mx = -map_numx; mx < map_numx; mx++) {
				float real_x, real_y, x_offset, y_offset;
				x_offset = rotation_center_x - xres/2;
				y_offset = rotation_center_y - yres/2;
				img_x -= x_offset;
				img_y -= y_offset;
				real_x = img_x * cosAngle - img_y * sinAngle - translation_x;
				real_y = img_x * sinAngle + img_y * cosAngle - translation_y;
				real_x += x_offset;
				real_y += y_offset;
				img_x += x_offset;
				img_y += y_offset;
				draw_state->dst_quad2d = Quad2D(real_x,real_y,DEFAULT2,DEFAULT2);
				InternalDrawMaster(img);
				img_x += xres;
			}
		}
	}
	if(clear_states)
		ResetDrawStates();

	return;
}

void WindowsDX::AccessPixels (ImageX * destination)
{
	if(win_warning_on_draw && destination == VIEWPORT) {
		ReportProblem(GAMEX_INVALID_DRAW_TIME_FAIL);
		ResetDrawStates();
		return;
	}
	if(destination == VIEWPORT || destination == NULL) {
		win_pixel_dest_surface = dd_back_buf;
		win_pixel_dest_data = win_screen;
		win_pixel_dest_pitch = win_adjustedpitch;
		win_pixel_dest_ytable = win_ytable;
		LockBackBuffer();

		if(win_screenbpp == 32) {
			FuncDrawPixel = &DrawPixel32;
			FuncReadPixel = &ReadPixel32;
		} else {
			FuncDrawPixel = &DrawPixel16;
			FuncReadPixel = &ReadPixel16;
		}
	} else {
		win_pixel_dest_surface = destination->m_surface;
		
		if(win_pixel_dest_surface == NULL) {
			win_pixel_dest_data = (char *) destination->m_data;
			win_pixel_dest_pitch = destination->m_xres*8/win_screenbpp;
			win_pixel_dest_ytable = destination->m_ytable;

			if(win_screenbpp == 32) {
				FuncDrawPixel = &DrawPixel32;
				FuncReadPixel = &ReadPixel32;
			} else {
				FuncDrawPixel = &DrawPixel16;
				FuncReadPixel = &ReadPixel16;
			}
		}
		else {
			if(destination->m_surf_bpp == 32) {
				if(destination->m_xmult == 1.0f && destination->m_ymult == 1.0f) {
					FuncDrawPixel = &DrawPixel32;
					FuncReadPixel = &ReadPixel32;
				} else {
					FuncDrawPixel = &DrawScaledPixel32;
					FuncReadPixel = &ReadScaledPixel32;
					win_pixel_dest_xmult = destination->m_xmult;
					win_pixel_dest_ymult = destination->m_ymult;
				}
			} else {
				if(destination->m_xmult == 1.0f && destination->m_ymult == 1.0f) {
					FuncDrawPixel = &DrawPixel16;
					FuncReadPixel = &ReadPixel16;
				} else {
					FuncDrawPixel = &DrawScaledPixel16;
					FuncReadPixel = &ReadScaledPixel16;
					win_pixel_dest_xmult = destination->m_xmult;
					win_pixel_dest_ymult = destination->m_ymult;
				}
			}

			DDSURFACEDESC2 ddsd;
			ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2)) ;
			ddsd.dwSize = sizeof(ddsd) ;
			DirectDrawError(win_pixel_dest_surface->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL));
			win_pixel_dest_data = (char *) ddsd.lpSurface;
			win_pixel_dest_pitch = ddsd.lPitch * 8 / destination->m_surf_bpp;
			win_pixel_dest_ytable = destination->m_ytable;

			destination->m_non_empty = true;
		}
	}
}

void WindowsDX::EndPixelAccess (void)
{
	if(win_pixel_dest_data != win_screen && win_pixel_dest_surface != NULL) {
		win_pixel_dest_surface->Unlock(NULL);

		FuncDrawPixel = &DrawPixelError;
		FuncReadPixel = &ReadPixelError;
	}
}

// this function is very slow if compiler optimizations are off (i.e. in debug mode)
// because it is made to be inlined.
static void DrawPixel16 (int x, int y, int r, int g, int b, int a) // r, g, and b are out of 255
{
//	((unsigned short *)GameX.win_pixel_dest_data)[x+GameX.win_ytable[y]] = (((r >> (8-GameX.win_maxr)) << GameX.win_shiftr) + ((g >> (8-GameX.win_maxg)) << GameX.win_shiftg) + ((b >> (8-GameX.win_maxb)) << GameX.win_shiftb));
	((unsigned short *)GameX.win_pixel_dest_data)[x+GameX.win_pixel_dest_ytable[y]] = (((r >> 3) << GameX.win_shiftr) + ((g >> (8-GameX.win_maxg)) << 5) + (b >> 3)); // optimized
}

static void DrawPixel32 (int x, int y, int r, int g, int b, int a) // r, g, and b are out of 255
{
//	((unsigned long *)GameX.win_pixel_dest_data)[x+GameX.win_ytable[y]] = (((r >> (8-GameX.win_maxr)) << GameX.win_shiftr) + ((g >> (8-GameX.win_maxg)) << GameX.win_shiftg) + ((b >> (8-GameX.win_maxb)) << GameX.win_shiftb));
	((unsigned long *)GameX.win_pixel_dest_data)[x+GameX.win_pixel_dest_ytable[y]] = ((a << 24) + (r << 16) + (g << 8) + b); // optimized
}

static void ReadPixel16 (int x, int y, int &r, int &g, int &b, int &a)
{
	unsigned short v = ((unsigned short *)GameX.win_pixel_dest_data)[x+GameX.win_pixel_dest_ytable[y]];
	b =  (v & 0x1F) << 3;
	g = ((v & GameX.win_maskg) >> 5) << (8-GameX.win_maxg) ;
	r = ((v & GameX.win_maskr) >> GameX.win_shiftr) << 3;
	a = 255;
}

static void ReadPixel32 (int x, int y, int &r, int &g, int &b, int &a)
{
	unsigned long v = ((unsigned long *)GameX.win_pixel_dest_data)[x+GameX.win_pixel_dest_ytable[y]];
	b =  v & 0x000000FF;
	g = (v & 0x0000FF00) >> 8;
	r = (v & 0x00FF0000) >> 16;
	a = (v & 0xFF000000) >> 24;
}

static void DrawPixelError (int x, int y, int r, int g, int b, int a)
{
	GameX.ReportProblem(GAMEX_PIXEL_ACCESS_FAILURE);
}

static void ReadPixelError (int x, int y, int &r, int &g, int &b, int &a)
{
	GameX.ReportProblem(GAMEX_PIXEL_ACCESS_FAILURE);
}

// pixel setter/getter function to be used when the destination is smaller than it's pretending to be

static void DrawScaledPixel16 (int x, int y, int r, int g, int b, int a) // r, g, and b are out of 255
{
	// prevent multiple writes to the same pixel:
	if(GameX.win_pixel_dest_xmult != 1.0f && x != 0 && (int)(GameX.win_pixel_dest_xmult * (float)(x-1)) == (int)(GameX.win_pixel_dest_xmult * (float)(x))) return;
	if(GameX.win_pixel_dest_ymult != 1.0f && y != 0 && (int)(GameX.win_pixel_dest_ymult * (float)(y-1)) == (int)(GameX.win_pixel_dest_ymult * (float)(y))) return;

	// scale the x axis (the y axis is already scaled in the ytable):
	x = (int)((float)x * GameX.win_pixel_dest_xmult);

	// set the pixel
	((unsigned short *)GameX.win_pixel_dest_data)[x+GameX.win_pixel_dest_ytable[y]] = (((r >> 3) << GameX.win_shiftr) + ((g >> (8-GameX.win_maxg)) << 5) + (b >> 3)); // optimized
}

static void DrawScaledPixel32 (int x, int y, int r, int g, int b, int a) // r, g, and b are out of 255
{
	// prevent multiple writes to the same pixel:
	if(GameX.win_pixel_dest_xmult != 1.0f && x != 0 && (int)(GameX.win_pixel_dest_xmult * (float)(x-1)) == (int)(GameX.win_pixel_dest_xmult * (float)(x))) return;
	if(GameX.win_pixel_dest_ymult != 1.0f && y != 0 && (int)(GameX.win_pixel_dest_ymult * (float)(y-1)) == (int)(GameX.win_pixel_dest_ymult * (float)(y))) return;

	// scale the x axis (the y axis is already scaled in the ytable):
	x = (int)((float)x * GameX.win_pixel_dest_xmult);

	// set the pixel
	((unsigned long *)GameX.win_pixel_dest_data)[x+GameX.win_pixel_dest_ytable[y]] = ((a << 24) + (r << 16) + (g << 8) + b); // optimized
}

static void ReadScaledPixel16 (int x, int y, int &r, int &g, int &b, int &a)
{
	x = (int)((float)x * GameX.win_pixel_dest_xmult);
	unsigned short v = ((unsigned short *)GameX.win_pixel_dest_data)[x+GameX.win_pixel_dest_ytable[y]];
	b =  (v & 0x1F) << 3;
	g = ((v & GameX.win_maskg) >> 5) << (8-GameX.win_maxg) ;
	r = ((v & GameX.win_maskr) >> GameX.win_shiftr) << 3;
	a = 255;
}

static void ReadScaledPixel32 (int x, int y, int &r, int &g, int &b, int &a)
{
	x = (int)((float)x * GameX.win_pixel_dest_xmult);
	unsigned long v = ((unsigned long *)GameX.win_pixel_dest_data)[x+GameX.win_pixel_dest_ytable[y]];
	b =  v & 0x000000FF;
	g = (v & 0x0000FF00) >> 8;
	r = (v & 0x00FF0000) >> 16;
	a = (v & 0xFF000000) >> 24;
}


void WindowsDX::InternalDrawMaster(ImageX * source)
{
///#ifdef _DEBUG
	if(win_warning_on_draw && draw_state->dst == VIEWPORT) {
		ReportProblem(GAMEX_INVALID_DRAW_TIME_FAIL);
		ResetDrawStates();
		return;
	}
///#endif

	if(!(draw_state->flags & DRAWMODESMASK))
		draw_state->flags |= DRAW_PLAIN; // assume no drawing mode means plain drawing mode

	if(draw_state->dst != VIEWPORT && !(draw_state->dst->m_usage & LOAD_TARGETABLE))
		draw_state->dst->ConvertUsageTo(draw_state->dst->m_usage | LOAD_TARGETABLE); // auto-convert to targetable if needed

	bool skip = false;
	bool immediate = false;

	if(draw_3D_depth_sort == 2) { // replaying drawing
		source = draw_state->src;
	} else {
		if(draw_sort_mode & SCENESORTZ && draw_sort_mode & SCENESORTD) {
			if(draw_state->flags & DRAW_PLAIN && !draw_state->colors.HasTransparency() && (source == NULL || !(source->m_usage & LOAD_ALPHA)))
				immediate = true;
		}
		skip = !PrepareDrawState(source,immediate);
	}
	if(draw_3D_depth_sort == 1 && !immediate)
		skip = true;

	if(source != NULL && source != VIEWPORT && source->status != IMG_OK) {
		ReportProblem(GAMEX_UNINIT_IMG_DRAW_FAIL);
		skip = true;
	}

	RECT sourceRect;
	float * x, * y, * z;
	if(!skip) {
		if(draw_state->src != NULL)
			sourceRect = ConvertRECT(draw_state->src_rect, draw_state->src);

		x = draw_state->x; // pointer to an array of 4 variable-purpose x screen coordinates
		y = draw_state->y; // pointer to an array of 4 variable-purpose y screen coordinates
		z = draw_state->z; // used for depth sorting

		draw_state->colors.CapComponents();

		if(!(draw_state->flags & DRAWOP_NOBLENDALPHA)) {
			if(!(draw_state->flags & DRAW_PLAIN)) {
				// scale the luminosity by the alpha which would otherwise be ignored
				if(draw_state->dst == VIEWPORT || !(draw_state->dst->m_usage & LOAD_ALPHA)) {
					ScaleByAlpha(draw_state->colors);
					if(draw_state->color_effect == D3DTOP_MODULATE)
						if(!(draw_state->colors.HasColor()))
							skip = true; // return if there isn't any luminosity
				}
			} else {
				if(!(draw_state->colors.HasOpacity()))
					skip = true; // return if there isn't any opacity
			}
		}
	}

	if(skip) {
		if(!(draw_state->flags & DRAWOP_KEEPSTATES))
			ResetDrawStates();
		return;
	}

	int sourceHasSurface, destHasSurface;
	bool sourceHasData, sourceHasAlpha, destHasData, destHasAlpha, destHasDevice;
	LPDIRECTDRAWSURFACE7 p_src_surface, p_dst_surface;
	LPDIRECT3DDEVICE7 p_dst_device;
	XBYTE * p_src_data;
	XBYTE * p_src_alpha;
	XBYTE * p_dst_data;
	XBYTE * p_dst_alpha;
	RECT src_bounds = {0,0,0,0};
	int src_pitch, dst_pitch;

	if(draw_state->src == VIEWPORT) {
		sourceHasSurface = 1;
		p_src_surface = dd_back_buf;
		sourceHasAlpha = false;
		src_pitch = win_adjustedpitch;
		src_bounds.right = win_width;
		src_bounds.bottom = win_height;
	} else if(draw_state->src != NULL) {
		if(draw_state->src->status != IMG_OK) return;
		if(draw_state->src->m_hassurface) {
			sourceHasSurface = draw_state->src->m_hassurface;
			p_src_surface = draw_state->src->m_surface;
		}
		else sourceHasSurface = 0;

		if(draw_state->src->m_data != NULL) {
			sourceHasData = true;
			p_src_data = draw_state->src->m_data;
		}
		else
			sourceHasData = false;

		if(draw_state->src->m_alpha != NULL) {
			sourceHasAlpha = true;
			p_src_alpha = draw_state->src->m_alpha;
		}
		else
			sourceHasAlpha = false;

		src_pitch = draw_state->src->m_xres; // may have to change this line to do depths other than 16 bit
		src_bounds.right = draw_state->src->m_xres;
		src_bounds.bottom = draw_state->src->m_yres;
		if(draw_state->src->m_xmult != 1.0f) src_bounds.right = (LONG)((float)src_bounds.right * draw_state->src->m_xmult);
		if(draw_state->src->m_ymult != 1.0f) src_bounds.bottom = (LONG)((float)src_bounds.bottom * draw_state->src->m_ymult);
	} else {
		sourceHasSurface = 2;
		p_src_surface = NULL;
		sourceHasData = false;
		sourceHasAlpha = false;
	}

	if(draw_state->dst == VIEWPORT) {
		destHasSurface = 3;
		p_dst_surface = dd_back_buf;

		destHasDevice = true;
		p_dst_device = d3d_device;

		destHasAlpha = false;

		dst_pitch = win_adjustedpitch;
	} else {
		if(draw_state->dst->status != IMG_OK) return;
		if(draw_state->dst->m_hassurface) {
			destHasSurface = draw_state->dst->m_hassurface;
			p_dst_surface = draw_state->dst->m_surface;
		}
		else {
			dst_pitch = draw_state->dst->m_xres; // may have to change this line to do depths other than 16 bit...maybe
			destHasSurface = 0;
		}

		if(draw_state->dst->m_data != NULL) {
			destHasData = true;
			p_dst_data = draw_state->dst->m_data;
		}
		else
			destHasData = false;

		if(draw_state->dst->m_alpha != NULL) {
			destHasAlpha = true;
			p_dst_alpha = draw_state->dst->m_alpha;
		}
		else
			destHasAlpha = false;

		if(draw_state->dst->m_device != NULL) {
			destHasDevice = true;
			p_dst_device = draw_state->dst->m_device;
		}
		else
			destHasDevice = false;
	}

	int alphatype = 0;
	if(draw_state->src != VIEWPORT && draw_state->src != NULL) {
		if((draw_state->src->m_usage & LOAD_MASKED))
			alphatype = 1;
		else if((draw_state->src->m_usage & LOAD_ALPHA)) {
			if(draw_state->dst == VIEWPORT || !(draw_state->dst->m_usage & LOAD_ALPHA))
			alphatype = 2; // alpha channel in image
		}
	}

	// NOTE: support for all advanced features is not complete for if we can't draw using 3D,
	//		 but that *should* only happen when drawing FROM the back buffer to somewhere,

	int ok = 0;
	if(sourceHasSurface==2 && destHasDevice) {
		if(p_src_surface)
			ok = InternalDrawImageD3D(p_dst_device, p_src_surface, sourceRect, src_bounds, x, y, z, draw_state->src->m_surf_width, draw_state->src->m_surf_height, alphatype);
		else
			ok = InternalDrawImageD3D(p_dst_device, NULL, FULLRECT, FULLRECT, x, y, z, win_width, win_height, alphatype);
	}
	if(!ok) {
///		bool can_2D_accelerate = (sourceHasSurface && destHasSurface && p_src_surface != NULL && (draw_state->flags & DRAW_PLAIN) && alphatype != 2 && draw_state->angle==0.0f && !draw_state->colors.HasShading() && (draw_state->vertices_3D==0||draw_state->vertices_3D==2||draw_state->vertices_3D==3));
		bool can_2D_accelerate = destHasSurface ? true : false;

		RECT destRect;
		destRect.left = (LONG)(x[0]+x[2])/2;
		destRect.top = (LONG)(y[0]+y[1])/2;
		destRect.right = (LONG)(x[1]+x[3])/2;
		destRect.bottom = (LONG)(y[2]+y[3])/2;

		if(can_2D_accelerate) {
			if(draw_state->src == VIEWPORT || draw_state->dst == VIEWPORT)
				UnlockBackBuffer();
			if(dd_was_lost) {
				if(draw_state->src && draw_state->src != VIEWPORT) {p_src_surface = draw_state->src->m_surface;}
				if(draw_state->dst && draw_state->dst != VIEWPORT) {p_dst_surface = draw_state->dst->m_surface;}
			}
			if(sourceHasSurface && p_src_surface != NULL) // plain directdraw draw
				ok = InternalDrawImageDD(p_src_surface, p_dst_surface, sourceRect, destRect, !alphatype);
			else { // solid fill color
				InternalDrawRectDD(draw_state->colors.c[0][0],draw_state->colors.c[0][1],draw_state->colors.c[0][2],
									destRect.left, destRect.top, destRect.right, destRect.bottom);
				ok = true;
			}
		}
/*
		// currently broken -- needs support for 32-bit mode and active rotation, at the least.

		if(!ok) {
			if(draw_state->dst == VIEWPORT) {
				if(LockBackBuffer()) {
					destHasData = true;
					p_dst_data = (XBYTE *) win_screen;
				} else destHasData = false;
			}

			if(draw_state->src == VIEWPORT) {
				if(LockBackBuffer()) {
					sourceHasData = true;
					p_src_data = (XBYTE *) win_screen;
				} else sourceHasData = false;
			}

			if(sourceHasData && destHasData) {

#ifdef _DEBUG
	if(!win_no_acc_err) { // so we only warn once instead of every frame which would effectively freeze the game
		win_no_acc_err = true;
		if(draw_state->src == VIEWPORT) {
			MessageBox (win_hwnd, "Warning: The VIEWPORT is being drawn without the help of DirectDraw or Direct3D.\nThis can cause the game to perform extremely poorly.\nTo correct this issue, make sure to only draw the VIEWPORT into an image that is loaded with the LOAD_NON3D flag,\nand only draw the VIEWPORT with DRAW_PLAIN. If you need to draw it with a special mode such as DRAW_ADD,\ncopy it from the NON3D image into a normal image and draw from that.", "GameX Warning", MB_OK|MB_ICONWARNING) ;
		} else {
			MessageBox (win_hwnd, "Warning: An image is being drawn without the help of DirectDraw or Direct3D.\nThis can cause the game to perform extremely poorly.\nPerhaps you are using the LOAD_NON3D flag when it should not be used.", "GameX Warning", MB_OK|MB_ICONWARNING) ;
		}
		if(win_screenbpp != 16)
			MessageBox (win_hwnd, "Warning: Drawing images without 2D or 3D acceleration\nis currently only supported in 16-bit (HIGHCOLOR) display mode.", "GameX Warning", MB_OK|MB_ICONWARNING) ;
	}
#endif

	// the following code is the slow, hopefully rarely-used backup code
				ok = true;
				if(draw_state->flags & DRAW_PLAIN) {
					if(alphatype != 2) { // non-alpha or mask
						if(draw_state->colors.c[0][3]!=255)
							InternalDrawBlended(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][0],draw_state->colors.c[0][1],draw_state->colors.c[0][2],draw_state->colors.c[0][3], !alphatype);
						else
							InternalDrawPlain(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, !alphatype);
					} else { // draw using image's alpha channel
						if(sourceHasAlpha)
							InternalDrawAlphaUnmerged(p_src_data, p_src_alpha, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][3]);
						else
							InternalDrawAlphaMerged(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][3]);
					}
				}
				else if(draw_state->flags & DRAW_ADD || draw_state->flags & DRAW_ADDSHARP || draw_state->flags & DRAW_ADDSOFT) {
					if(alphatype != 2) { // non-alpha
						InternalDrawAdded(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][0],draw_state->colors.c[0][1],draw_state->colors.c[0][2]);
					} else { // draw added AND take alpha channel into account
						InternalDrawAddedMerged(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][0],draw_state->colors.c[0][1],draw_state->colors.c[0][2]);
					}
				}
				else if(draw_state->flags & DRAW_SUBTRACT) {
					InternalDrawAdded(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, -draw_state->colors.c[0][0],-draw_state->colors.c[0][1],-draw_state->colors.c[0][2]);
				}
				else if(draw_state->flags & DRAW_INVERT) {
					InternalDrawInvert(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][0],draw_state->colors.c[0][1],draw_state->colors.c[0][2]);
				}
				else if(draw_state->flags & DRAW_GHOST) {
					// not yet fully supported without D3D, this is just a bad approximation:
					InternalDrawAdded(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, -draw_state->colors.c[0][0],-draw_state->colors.c[0][1],-draw_state->colors.c[0][2]);
					InternalDrawAdded(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][0],draw_state->colors.c[0][1],draw_state->colors.c[0][2]);
				}
				else if(draw_state->flags & DRAW_INTENSIFY) {
					// not yet fully supported without D3D, this is just a bad approximation:
					InternalDrawAdded(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][0]/2,draw_state->colors.c[0][1]/2,draw_state->colors.c[0][2]/2);
				}
				else if(draw_state->flags & DRAW_BURN) {
					// not yet fully supported without D3D, this is just a bad approximation:
					InternalDrawAdded(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, (draw_state->colors.c[0][0]-255)/2,(draw_state->colors.c[0][1]-255)/2,(draw_state->colors.c[0][2]-255)/2);
					InternalDrawAdded(p_src_data, p_dst_data, sourceRect, destRect, src_bounds, draw_state->dst_bounds, src_pitch,dst_pitch, draw_state->colors.c[0][0]/2,draw_state->colors.c[0][1]/2,draw_state->colors.c[0][2]/2);
				} else
					ok = false; // mode unsupported by pixel-by-pixel drawing
			}
		}
*/
	}

	if(draw_state->dst != VIEWPORT && destHasSurface == 2) {
		// if the destination is a texture, we have to tell D3D to refresh it by
		// locking and unlocking it, or else the image won't change after the first time it's drawn.
		// note that this makes drawing to somewhere else than the screen even slower...
		DDSURFACEDESC2 ddsd;
		ddsd.dwSize = sizeof(ddsd);
		p_dst_surface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		p_dst_surface->Unlock(NULL);
	}
	if(draw_3D_depth_sort != 2)
		if(!(draw_state->flags & DRAWOP_KEEPSTATES))
			ResetDrawStates();

	if(win_request_slow_debug_drawing && win_init_done && draw_state->dst == VIEWPORT) {
		if((GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
			win_request_slow_debug_drawing = false; // must be false before choice dialog
			if(ChoiceDialog("Stop drawing slowly?") != 1)
				win_request_slow_debug_drawing = true;
		} else {
			UpdateView();
			Sleep(60);
		}
	}
} // end of InternalDrawMaster

// Draws a 2D image using 3D acceleration and features
int WindowsDX::InternalDrawImageD3D(LPDIRECT3DDEVICE7 device, LPDIRECTDRAWSURFACE7 image, RECT & src_rect, RECT & src_bounds, float * x, float * y, float * z, int surf_width, int surf_height, int alphatype)
{
	if(device == NULL) return 0;

	if(draw_state->dst == VIEWPORT) {
		UnlockBackBuffer(); // must be "closed" (actually called unlocked) to draw with D3D (or DD for that matter)
	}

	if(d3d_device != NULL) {
		D3DVIEWPORT7 view;
		view.dwX = draw_state->dst_bounds.left;
		view.dwY = draw_state->dst_bounds.top;
		view.dwWidth = draw_state->dst_bounds.right-draw_state->dst_bounds.left;
		view.dwHeight = draw_state->dst_bounds.bottom-draw_state->dst_bounds.top; 
		view.dvMinZ = 0.0f;
		view.dvMaxZ = 1.0f;
		d3d_device->SetViewport(&view);
	}

	// Because Direct3D does not support quadrilateral primitives,
	// create vertices for our quad with points ordered like so:
	//
	//	0----1
	//	.	/.
	//	.  / .
	//	. /  .
	//	./	 .
	//	2----3
	//
	// to allow for D3D to use it for a triangle strip.
	float angle = draw_state->angle;
	int angle_adjust_counter = 999; // prevent infinite angle adjusting when divide-by-zero is passed as angle
	while(angle >= 360.0f && angle_adjust_counter) angle -= 360.0f, angle_adjust_counter--; // restrict to 0-360 range to allow easier angle checking
	while(angle < 0.0f	  && angle_adjust_counter) angle += 360.0f, angle_adjust_counter--; // and also increase cosine and sine function accuracy
	int rotate;
	if(angle==	0.0f || angle== 45.0f || angle== 90.0f || angle==135.0f
	|| angle==180.0f || angle==225.0f || angle==270.0f || angle==315.0f)
		rotate = 0; // if the angle is exactly a multiple of 45, we don't have to adjust its inset as much
	else
		rotate = 1;

	float x_adjust, y_adjust; // source inset amount
	float leftMap, topMap, rightMap, bottomMap;

	if(&src_bounds != &FULLRECT) {
		if(src_rect.right - src_rect.left == src_bounds.right - src_bounds.left) {
			x_adjust = 0.0005f;
		} else {
			float scalex = (float)(x[3]-x[0])/(src_rect.right-src_rect.left);
			x_adjust = ((scalex <= 1.0f) ? (0.0005f) : (0.525f-0.5f/scalex));
			if(rotate) x_adjust = min(0.5f, x_adjust+0.25f);
		}
		if(src_rect.bottom - src_rect.top == src_bounds.bottom - src_bounds.top) {
			y_adjust = 0.0005f;
		} else {
			float scaley = (float)(y[3]-y[0])/(src_rect.bottom-src_rect.top);
			y_adjust = ((scaley <= 1.0f) ? (0.0005f) : (0.525f-0.5f/scaley));
			if(rotate) y_adjust = min(0.5f, y_adjust+0.25f);
		}

		// respond to mirroring request flags
		if(draw_state->flags & DRAWOP_VMIRROR) { // flip the top and bottom mapping values if we want to mirror the image vertically
			int temp = src_rect.top;
			src_rect.top = src_rect.bottom;
			src_rect.bottom = temp;
			y_adjust = -y_adjust;
		}
		if(draw_state->flags & DRAWOP_HMIRROR) { // flip the left and right mapping values if we want to mirror the image horizontally
			int temp = src_rect.left;
			src_rect.left = src_rect.right;
			src_rect.right = temp;
			x_adjust = -x_adjust;
		}

		leftMap   = (src_rect.left	 + x_adjust) / surf_width;	// determine source mapping values
		topMap	  = (src_rect.top	 + y_adjust) / surf_height; // (the pixel adjustments fix a
		rightMap  = (src_rect.right  - x_adjust) / surf_width;	//	"sometimes off by one" bug)
		bottomMap = (src_rect.bottom - y_adjust) / surf_height;
	} else {
		leftMap  = 0.0f; topMap    = 0.0f;
		rightMap = 1.0f; bottomMap = 1.0f;
	}

	int color_shift = 0; // amount to left-shift brightness

	if(draw_state->flags & DRAWOP_BRIGHT)
		color_shift++; // multiply by 2 if BRIGHT

	// Determine which workarounds, if any, we need to apply right now:
	bool work_inten = win_intensify_workaround	 == 1 && draw_state->flags & DRAW_INTENSIFY;
	bool work_subt1 = win_subtract_workaround	 == 1 && draw_state->flags & DRAW_SUBTRACT && alphatype != 2;
	bool work_subt2 = win_subtract_workaround	 == 2 && draw_state->flags & DRAW_SUBTRACT;
	bool work_alpha = win_alpha_blend_workaround == 1 && draw_state->flags & DRAW_PLAIN && alphatype != 2 && draw_state->colors.HasTransparency();
	bool work_ghost = win_ghost_workaround		 == 1 && draw_state->flags & DRAW_GHOST;
	bool work_burn	= win_burn_workaround		 == 1 && draw_state->flags & DRAW_BURN;
	bool work_addso = win_addsoft_workaround	 == 1 && draw_state->flags & DRAW_ADDSOFT;
	bool work_addsh = win_addsharp_workaround	 == 1 && draw_state->flags & DRAW_ADDSHARP;

	// Implement work-around methods as necessary:
	// (these are to work around problems with older video cards, as decided in DoInitTests())
	if(work_inten) {
		draw_state->flags ^= DRAW_ADD | DRAW_INTENSIFY; // switch from intensify to regular additive mode
		draw_state->colors /= 2.0f; // cut brightness in half
	}
	if(work_addso)
		draw_state->flags ^= DRAW_ADD | DRAW_ADDSOFT; // switch from soft add to regular additive mode
	if(work_addsh)
		draw_state->flags ^= DRAW_ADD | DRAW_ADDSHARP; // switch from sharp add to regular additive mode
	if(work_burn) // note that part of the burn workaround happens later, also
		draw_state->flags ^= DRAW_INVERT | DRAW_BURN; // switch from burn to inversion
	if(work_subt1 || work_subt2 || work_ghost || work_alpha) {
		if(image == NULL || image == dd_back_buf) {
			if(work_subt1 || work_subt2) {
				image = NULL;
				draw_state->flags ^= DRAW_PLAIN | DRAW_SUBTRACT; // switch from subtractive to regular alpha mode
				for(int i = 0 ; i < 4 ; i++) {
					draw_state->colors.c[i][3] = (int)((float)draw_state->colors.c[i][0]*RED_LUMINANCE + (float)draw_state->colors.c[i][1]*GREEN_LUMINANCE + (float)draw_state->colors.c[i][2]*BLUE_LUMINANCE) * draw_state->colors.c[i][3]/255;
					draw_state->colors.c[i][2] = 0;
					draw_state->colors.c[i][1] = 0;
					draw_state->colors.c[i][0] = 0;
				}
			}
		} else {
			int compat_index = (work_ghost) ? 2 : ((work_subt2) ? 1 : 0);
			ImageX * work_img = draw_state->src->m_internal_compatibility_img[compat_index];
			if(work_img == NULL) {
				SetStatus(GAMEX_CREATING_TEST_SURFACE); // prevent from registering as user-created
				work_img = new ImageX();
				draw_state->src->m_internal_compatibility_img[compat_index] = work_img;
				work_img->Create(draw_state->src->m_xres, draw_state->src->m_yres, true);
				SetStatus(GAMEX_READY);
				draw_state->src->CopyTo(work_img);
				if(work_subt2 || work_ghost) {
					AccessPixels(work_img);
					for(int y = 0 ; y < work_img->m_yres ; y++) {
						for(int x = 0 ; x < work_img->m_xres ; x++) {
							int r,g,b,a;
							ReadPixel(x,y,r,g,b,a);
							a = (int)((float)r*RED_LUMINANCE + (float)g*GREEN_LUMINANCE + (float)b*BLUE_LUMINANCE) * a/255;
							if(work_subt2) r = 0, g = 0, b = 0;
							DrawPixel(x,y,r,g,b,a);
						}
					}
					EndPixelAccess();
				}
			}
			image = work_img->m_surface;
			if(work_subt2 || work_ghost) {
				draw_state->flags |= DRAWMODESMASK, draw_state->flags ^= DRAWMODESMASK; // turn off all draw modes
				alphatype = 2, draw_state->flags | DRAW_PLAIN; // turn on alpha with plain drawing mode
				for(int i = 0 ; i < 4 ; i++) // scale shading transparency with brightness
					draw_state->colors.c[i][3] = min(255, (int)((float)draw_state->colors.c[i][0]*RED_LUMINANCE + (float)draw_state->colors.c[i][1]*GREEN_LUMINANCE + (float)draw_state->colors.c[i][2]*BLUE_LUMINANCE) *2* draw_state->colors.c[i][3]/255);
			}
		}
	}

	switch(draw_state->flags & DRAWMODESMASK) {
		default:
		case DRAW_PLAIN: // plain, masked, or alpha drawing
			if(alphatype != 2 && !draw_state->colors.HasTransparency()) {
				device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_ONE); // Cs = 1
				device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ZERO); // Cd = 0
				// so C = source * Cs + dest * Cd
				//		= source * (1) + dest * (0)
				//		= source
			} else {
				device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA); // Cs = alpha
				device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA); // Cd = 1-alpha
				// so C = source * Cs + dest * Cd
				//		= source * (alpha) + dest * (1-alpha)
				//		= dest + source*alpha - dest*alpha
			}
			break;
		case DRAW_ADD: // additive -- r, g, b at 255 is full additive, reduce r for cyan-tinted added, etc.
			if(alphatype != 2) device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_ONE); // Cs = 1, or
			else			   device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA); // Cs = alpha
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE); // Cd = 1
			// so C = source * Cs + dest * Cd
			//		= source * (1 [* alpha]) + dest * (1)
			//		= dest + source [* alpha]
			break;
		case DRAW_SUBTRACT: // subtractive -- r, g, b at 255 is full subtractive, reduce r to subtract less red, etc.
			// note: not totally accurate subtraction because this does not use DirectX 8.0
			//		 so the closest thing in DX7 is this, the inverse of DRAW_INTENSIFY
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_ZERO); // Cs = 0, or
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR); // Cd = 1-source
			// so C = source * Cs + dest * Cd
			//		= source * (0) + dest * (1-source)
			//		= dest - source*dest
			color_shift++; // increase effect 2x because it's usually too pale otherwise (so we don't have to render it twice)
			//	  C = dest - (2*source)*dest
			break;
		case DRAW_GHOST: // color varying alpha blend -- r, g, b determine amount, produces interesting auto-alpha effect
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCCOLOR); // Cs = source
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR); // Cd = 1-source
			// so C = source * Cs + dest * Cd
			//		= source * (source) + dest * (1-source)
			//		= dest + source*source - source*dest
			break;
		case DRAW_INVERT: // invert by the amount of color in the source -- r,g,b at 255 is full inversion, otherwise hard to describe
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_INVDESTCOLOR); // Cs = 1-dest
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR); // Cd = 1-source
			// so C = source * Cs + dest * Cd
			//		= source * (1-dest) + dest * (1-source)
			//		= dest + 2*source - 2*source*dest
			break;
		case DRAW_MULTIPLY: // multiply source by destination
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_ZERO); // Cs = 0
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_SRCCOLOR); // Cd = source
			// so C = source * Cs + dest * Cd
			//		= source * (0) + dest * (source)
			//		= dest * source
			break;
		case DRAW_BURN: // similar to invert but peaks at midway brightness
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_INVSRCCOLOR); // Cs = 1-source
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR); // Cd = 1-source
			// so C = source * Cs + dest * Cd
			//		= source * (1-source) + dest * (1-source)
			//		= source - source*source + dest - source*dest
			break;
		case DRAW_INTENSIFY: // add, but darker as destination becomes darker
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_DESTCOLOR); // Cs = dest
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE); // Cd = 1
			// so C = source * Cs + dest * Cd
			//		= source * (dest) + dest * (1)
			//		= dest + source*dest
			color_shift++; // increase effect 2x because it's usually too pale otherwise (so we don't have to render it twice)
			//	  C = dest + (2*source)*dest
			break;
		case DRAW_ADDSHARP: // similar to DRAW_ADD but colors show more sharply (increases contrast)
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCCOLOR); // Cs = source
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE); // Cd = 1
			// so C = source * Cs + dest * Cd
			//		= source * (source) + dest * (1)
			//		= dest + source*source
			color_shift++; // increase effect 2x to compensate for brightness lost in contrast change
			//	  C = dest + (2*source)*(2*source)
			break;
		case DRAW_ADDSOFT: // similar to DRAW_ADD but adds less to brighter areas (prevents overexposure)
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_INVDESTCOLOR); // Cs = 1-dest
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE); // Cd = 1
			// so C = source * Cs + dest * Cd
			//		= source * (1-dest) + dest * (1)
			//		= dest + source*(1-dest)
			//		= dest + source - source*dest
			break;

	}
	if(alphatype == 1) { // if masked
		device->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE,TRUE); // turn on color keying
	}

	if(draw_state->flags & DRAWOP_INVERTED) { // if inverting, set to use the complement of the source texture
		device->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE|D3DTA_COMPLEMENT);
	}

	if(draw_state->flags & DRAWOP_NOCULL)
		d3d_device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

	D3DTEXTUREOP shade_combine_effect = draw_state->color_effect;

	if(color_shift) {
		if(shade_combine_effect == D3DTOP_MODULATE) {
			if(color_shift == 1)
				shade_combine_effect = D3DTOP_MODULATE2X;
			else
				shade_combine_effect = D3DTOP_MODULATE4X;
		}
		else if(shade_combine_effect == D3DTOP_ADDSIGNED)
			shade_combine_effect = D3DTOP_ADDSIGNED2X;
	}

	device->SetTextureStageState(0,D3DTSS_COLOROP, shade_combine_effect);

	if(draw_state->flags & DRAWOP_NOFILTER) {
		device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_POINT);
		device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_POINT);
	}

	if(draw_state->flags & DRAWOP_NODITHER)
		device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, FALSE);

	if(draw_state->flags & DRAWOP_NOBLENDALPHA && draw_state->flags & DRAW_PLAIN)
		device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);


	HRESULT status = D3D_OK;

	bool new_batch = false;

	if(draw_render_batch_device != device) {
		if(draw_render_batch_open == true)
			status = EndRenderBatch();
		draw_render_batch_device = device;
		new_batch = true;
	}

	if(new_batch || image != last_texture_surface) { // note: image NULL means blank/untextured poly
		last_texture_surface = image;
		status = device->SetTexture(0,image); // set the texture image (or nothing if NULL) to draw
	}

	if(draw_render_batch_open == false) {
		if(SUCCEEDED(status)) {
			status = device->BeginScene();
			if(SUCCEEDED(status))
				draw_render_batch_open = true;
		}
	}

	if(SUCCEEDED(status)) {
		D3DTLVERTEX quad[4];
		float near_plane = draw_state->cam->GetNear();
		float far_plane = draw_state->cam->GetFar();
		float plane_sep = far_plane - near_plane;
		for(int i = 0 ; i < 4 ; i++) {
			quad[i].sx = x[i] - 0.5f;
			quad[i].sy = y[i] - 0.5f;

			if(draw_sort_mode & SCENESORTS)
				if(win_alternator)
					quad[i].sz = 1.0f - (z[i]-near_plane)/(plane_sep)/2.0f; // for signed z-buffer
				else
					quad[i].sz = (z[i]-near_plane)/(plane_sep)/2.0f; // for signed z-buffer
			else
				quad[i].sz = (z[i]-near_plane)/(plane_sep); // for normal z-buffer

			quad[i].rhw = (draw_state->warp_mode!=0) ? draw_state->warp[i] : 1.0f;
			quad[i].color = RGBA_MAKE(draw_state->colors.c[i][0], draw_state->colors.c[i][1], draw_state->colors.c[i][2], draw_state->colors.c[i][3]);
			quad[i].specular = 0;
			quad[i].tu = (i==0 || i==2) ? (leftMap) : (rightMap);
			quad[i].tv = (i==0 || i==1) ? (topMap) : (bottomMap);
		}

		status = device->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,quad,4,0);

		// rest of DRAW_BURN workaround is implemented here, unless DRAW_SUBTRACT workaround also needed:
		if(work_burn && win_subtract_workaround != 2 && (win_subtract_workaround != 1 || alphatype == 2)) {
			device->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_ZERO); // Cs = 0, or
			device->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR); // Cd = 1-source
			device->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,quad,4,0); // draw again with DRAW_SUBTRACT:
		}
	}

	// reset DirectX renderstates for next time:

	if(draw_state->flags & DRAWOP_NOBLENDALPHA && draw_state->flags & DRAW_PLAIN)
		device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

	if(draw_state->flags & DRAWOP_NOFILTER) {
		device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_LINEAR);
		device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_LINEAR);
	}

	if(draw_state->flags & DRAWOP_NODITHER)
		device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);

	if(alphatype == 1) { // if masked
		device->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE,FALSE);
	}

	if(draw_state->flags & DRAWOP_INVERTED) {
		device->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
	}

	if(draw_state->flags & DRAWOP_NOCULL)
		d3d_device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);

	// rest of DRAW_BURN workaround is implemented here if the DRAW_SUBTRACT workaround is also needed:
	if(work_burn && !(win_subtract_workaround != 2 && (win_subtract_workaround != 1 || alphatype == 2))) {
		draw_state->flags ^= DRAW_SUBTRACT | DRAW_INVERT; // switch from inversion to subtraction
		draw_state->colors /= 2.0f;
		InternalDrawImageD3D(device, image, src_rect, src_bounds, x, y, z, surf_width, surf_height, alphatype);
		draw_state->colors *= 2.0f;
	}

	if(work_inten) draw_state->colors *= 2.0f; // reverse color change from DRAW_INTENSIFY workaround

	// report our success or failure to do the actual drawing:

	if(FAILED(status)) {
		Direct3DError(status);
		DirectDrawError(status);
		return 0;
	} else {
		return 1;
	}
}

// the simplest image drawing function -- using offscreen surfaces, usually very fast
// includes an option for masked drawing too, just input nomask=0
int WindowsDX::InternalDrawImageDD (LPDIRECTDRAWSURFACE7 p_sourceSurface, LPDIRECTDRAWSURFACE7 p_destSurface, RECT & src_rect, RECT & dst_rect, int nomask)
{
	// this function assumes:
	// both source and destination have already been UNlocked (if not, will refuse to draw anything)
	// the destination has a valid clipper attached to it (if not, won't clip and might crash)
	// (if the source and destination rectangles differ, Blt() will automatically scale the image,
	//	using hardware acceleration which will probably instantly scale it with bicubic filtering or something)

	int options = DDBLT_WAIT;
	if(!nomask) options |= DDBLT_KEYSRC;

	HRESULT status = p_destSurface->Blt(&dst_rect, p_sourceSurface,
										&src_rect,
										options,
										NULL);
	if(FAILED(status)) {
		DirectDrawError(status);
		return 0;
	}
	return 1;
}

/*
	// following are currently broken -- needs support for 32-bit mode and active rotation, at the least.


// includes an option for masked drawing too, just input nomask=0
int WindowsDX::InternalDrawPlain (XBYTE * source_pix, XBYTE * dest_pix, RECT & src_rect, RECT & dst_rect, RECT & src_bounds, RECT & dst_bounds, int src_pitch, int dst_pitch, int nomask)
{
	int drawType = ClipRects(src_rect, dst_rect, src_bounds, dst_bounds);

	if(drawType == 0) return 1; // image is entirely out of bounds, so just return

	XBYTE2 *img_start;
	XBYTE2 *view_start;
	int img_xs = dst_rect.right-dst_rect.left;
	int img_ys = dst_rect.bottom-dst_rect.top;

	int img_offset = src_rect.top * src_pitch + src_rect.left;
	int view_offset = dst_rect.top * dst_pitch + dst_rect.left;


	if(drawType == 1) {
		// if source and destination rectangles are exactly the same size,
		// use this optimized code that assumes no scaling:

		if(nomask) {
			
			// even slightly more optimized code that assumes no mask

			img_start = (XBYTE2 *) source_pix + img_offset;
			view_start = (XBYTE2 *) dest_pix + view_offset;
			for (int y=img_ys; y--;) {			
				memcpy (view_start, img_start, img_xs<<1) ; // the	<< 1  is 16-bit specific
				view_start += dst_pitch;
				img_start += src_pitch;
			} 
		} else {

			XBYTE2 *img_now;
			XBYTE2 *view_pix;

			int src_height = src_rect.bottom-src_rect.top;
			int src_width = src_rect.right-src_rect.left;

			img_start = (XBYTE2 *) source_pix + img_offset;
			view_start = (XBYTE2 *) dest_pix + view_offset;
			for (int y=0 ; y < img_ys; y++) {

				view_pix = view_start;
				img_now = img_start;
				for (int x=0 ; x < img_xs; x++) {
					if (* img_now) * view_pix = * img_now;
					view_pix++; img_now++;
				}
				view_start += dst_pitch;
				img_start += src_pitch;
			}
		}
	} else {
		// if drawType == 2, that means we have to stretch or shrink the image accordingly:
		// (using nearest-neighbor technique, since it's the easiest to implement)

		XBYTE2 *img_rowstart;
		XBYTE2 *img_now;
		XBYTE2 *view_pix;

		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE2 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;
		for (int y=0 ; y < img_ys; y++) {

			img_rowstart = img_start + y*src_height/img_ys*src_pitch;
			view_pix = view_start;
			for (int x=0 ; x < img_xs; x++) {
				img_now = img_rowstart + x*src_width/img_xs;
				if(nomask || *img_now) * view_pix = * img_now;
				view_pix++;
			}
			view_start += dst_pitch;
		}
	}
	return 1;
}

int WindowsDX::InternalDrawAlphaUnmerged (XBYTE * source_pix, XBYTE * src_alpha_pix, XBYTE * dest_pix, RECT & src_rect, RECT & dst_rect, RECT & src_bounds, RECT & dst_bounds, int src_pitch, int dst_pitch, int alphafactor)
{
	// Unfortunately, DirectDraw doesn't support alpha blending (and never will).
	// Fortunately, Direct3D does support alpha blending, but this function is for in case we can't use D3D

	int drawType = ClipRects(src_rect, dst_rect, src_bounds, dst_bounds);

	if(drawType == 0) return 1; // image is entirely out of bounds, so just return

	if(drawType == 1) {
		// if source and destination rectangles are exactly the same size:

		XBYTE2 *img_start;
		XBYTE2 *img_now;

		XBYTE *alpha_start;
		XBYTE *alpha_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE2 *) source_pix + img_offset;
		alpha_start = (XBYTE *) src_alpha_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
			view_pix = view_start;
			img_now = img_start;
			alpha_now = alpha_start;
			for (int x=0 ; x < img_xs; x++) {
				int a = (255 - *alpha_now) * alphafactor / 255;
				if (a) {
					int i = *img_now;
					int v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
					*view_pix = (((((v & win_maskb) * (255-a)) >> 8) & win_maskb) + ((((i & win_maskb) * a) >> 8) & win_maskb))
							  | (((((v & win_maskg) * (255-a)) >> 8) & win_maskg) + ((((i & win_maskg) * a) >> 8) & win_maskg))
							  | (((((v & win_maskr) * (255-a)) >> 8) & win_maskr) + ((((i & win_maskr) * a) >> 8) & win_maskr));
				}
				view_pix++; img_now++; alpha_now++;
			}
			view_start += dst_pitch;
			img_start += src_pitch;
			alpha_start += src_pitch;
		}
		delete [] temp;

	} else {
		// if ClipRects() returned 2, that means we have to stretch or shrink the image accordingly:
		// (using nearest-neighbor technique, since it's the easiest to implement)

		XBYTE2 *img_start;
		XBYTE2 *img_rowstart;
		XBYTE2 *img_now;

		XBYTE *alpha_start;
		XBYTE *alpha_rowstart;
		XBYTE *alpha_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE2 *) source_pix + img_offset;
		alpha_start = (XBYTE *) src_alpha_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...

			int yOffset = y*src_height/img_ys*src_pitch;
			img_rowstart = img_start + yOffset;
			alpha_rowstart = alpha_start + yOffset;
			view_pix = view_start;
			for (int x=0 ; x < img_xs; x++) {
				int xOffset = x*src_width/img_xs;
				img_now = img_rowstart + xOffset;
				alpha_now = alpha_rowstart + xOffset;

				int a = (255 - *alpha_now) * alphafactor / 255;
				if (a) {
					int i = *img_now;
					int v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
					*view_pix = (((((v & win_maskb) * (255-a)) >> 8) & win_maskb) + ((((i & win_maskb) * a) >> 8) & win_maskb))
							  | (((((v & win_maskg) * (255-a)) >> 8) & win_maskg) + ((((i & win_maskg) * a) >> 8) & win_maskg))
							  | (((((v & win_maskr) * (255-a)) >> 8) & win_maskr) + ((((i & win_maskr) * a) >> 8) & win_maskr));
				}
				view_pix++;
			}
			view_start += dst_pitch;
		}
		delete [] temp;
	}
	return 1;
}

int WindowsDX::InternalDrawAlphaMerged (XBYTE * source_pix, XBYTE * dest_pix, RECT & src_rect, RECT & dst_rect, RECT & src_bounds, RECT & dst_bounds, int src_pitch, int dst_pitch, int alphafactor)
{
	// Unfortunately, DirectDraw doesn't support alpha blending (and never will).
	// Fortunately, Direct3D does support alpha blending, but this function is for in case we can't use D3D

	int drawType = ClipRects(src_rect, dst_rect, src_bounds, dst_bounds);

	if(drawType == 0) return 1; // image is entirely out of bounds, so just return

	if(drawType == 1) {
		// if source and destination rectangles are exactly the same size:

		XBYTE4 *img_start;
		XBYTE4 *img_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE4 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
			view_pix = view_start;
			img_now = img_start;
			for (int x=0 ; x < img_xs; x++) {
				int i = *img_now;
				int a = ((i & 0xFF000000) >> 24);
				if (a) {
					int r = ((i & 0x00FF0000) >> 16);
					int g = ((i & 0x0000FF00) >> 8);
					int b = ((i & 0x000000FF) >> 0);
					i = ((r >> 3) << win_shiftr) | ((g >> 2) << win_shiftg) | ((b >> 3) << win_shiftb);
					int v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
					*view_pix = (((((v & win_maskb) * (255-a)) >> 8) & win_maskb) + ((((i & win_maskb) * a) >> 8) & win_maskb))
							  | (((((v & win_maskg) * (255-a)) >> 8) & win_maskg) + ((((i & win_maskg) * a) >> 8) & win_maskg))
							  | (((((v & win_maskr) * (255-a)) >> 8) & win_maskr) + ((((i & win_maskr) * a) >> 8) & win_maskr));
				}
				view_pix++; img_now++;
			}
			view_start += dst_pitch;
			img_start += src_pitch;
		}
		delete [] temp;

	} else {
		// if ClipRects() returned 2, that means we have to stretch or shrink the image accordingly:
		// (using nearest-neighbor technique, since it's the easiest to implement)

		XBYTE4 *img_start;
		XBYTE4 *img_rowstart;
		XBYTE4 *img_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE4 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...

			int yOffset = y*src_height/img_ys*src_pitch;
			img_rowstart = img_start + yOffset;
			view_pix = view_start;
			for (int x=0 ; x < img_xs; x++) {
				int xOffset = x*src_width/img_xs;
				img_now = img_rowstart + xOffset;

				int i = *img_now;
				int a = ((i & 0xFF000000) >> 24);
				if (a) {
					int r = ((i & 0x00FF0000) >> 16);
					int g = ((i & 0x0000FF00) >> 8);
					int b = ((i & 0x000000FF) >> 0);
					i = ((r >> 3) << win_shiftr) | ((g >> 2) << win_shiftg) | ((b >> 3) << win_shiftb);
					int v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
					*view_pix = (((((v & win_maskb) * (255-a)) >> 8) & win_maskb) + ((((i & win_maskb) * a) >> 8) & win_maskb))
							  | (((((v & win_maskg) * (255-a)) >> 8) & win_maskg) + ((((i & win_maskg) * a) >> 8) & win_maskg))
							  | (((((v & win_maskr) * (255-a)) >> 8) & win_maskr) + ((((i & win_maskr) * a) >> 8) & win_maskr));
				}
				view_pix++;
			}
			view_start += dst_pitch;
		}
		delete [] temp;
	}
	return 1;
}


// ri, gi, bi are RGB intensity values, they default to 255 if not passed in
// 255 means full additive, -255 means full subtractive,
// 128 means half additive, -128 means half subtractive,
// 255,0,0 means full-add red only, 0,0,-255 means full-subtract blue only, etc.
// they can go outside the -255 to 255 range too, such as 512 for doubly-additive
int WindowsDX::InternalDrawAdded (XBYTE * source_pix, XBYTE * dest_pix, RECT & src_rect, RECT & dst_rect, RECT & src_bounds, RECT & dst_bounds, int src_pitch, int dst_pitch, int ri, int gi, int bi)
{
	if(!(ri>0||gi>0||bi>0)) return 1; // if no intensity at all, return to save processor time

	int drawType = ClipRects(src_rect, dst_rect, src_bounds, dst_bounds);

	if(drawType == 0) return 1; // image is entirely out of bounds, so just return

	if(drawType == 1) {
		// if source and destination rectangles are exactly the same size:

		XBYTE2 *img_start;
		XBYTE2 *img_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE2 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		if(ri==255 && gi==255 && bi==255) { // slightly faster code in fully additive case (which r,g,b default to)
			for (int y=0 ; y < img_ys; y++) {
				memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
				view_pix = view_start;
				img_now = img_start;
				for (int x=0 ; x < img_xs; x++) {
					int i, v;
					if ((i = *img_now) && ((v = temp[view_pix-view_start]) != 65535)) { // if destination pixel isn't white and source pixel isn't black
						*view_pix = min(((v & win_maskb) + (i & win_maskb)), win_maskb)
								  | min(((v & win_maskg) + (i & win_maskg)), win_maskg)
								  | min(((v & win_maskr) + (i & win_maskr)), win_maskr) ;
					}
					view_pix++; img_now++;
				}
				view_start += dst_pitch;
				img_start += src_pitch;
			}
		}
		else {
			for (int y=0 ; y < img_ys; y++) {
				memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
				view_pix = view_start;
				img_now = img_start;
				for (int x=0 ; x < img_xs; x++) {
					int i, v;
					if ((i = *img_now)) { // if source pixel isn't black
						v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
						int blue  = ((v & win_maskb) + (((((i & win_maskb) >> win_shiftb) * bi) >> 8) << win_shiftb)) ;
						int green = ((v & win_maskg) + (((((i & win_maskg) >> win_shiftg) * gi) >> 8) << win_shiftg)) ;
						int red   = ((v & win_maskr) + (((((i & win_maskr) >> win_shiftr) * ri) >> 8) << win_shiftr)) ;
						*view_pix = ((bi < 0) ? max(0,blue)  : min(win_maskb, blue))
								  | ((gi < 0) ? max(0,green) : min(win_maskg, green))
								  | ((ri < 0) ? max(0,red)	 : min(win_maskr, red)) ;
					}
					view_pix++; img_now++;
				}
				view_start += dst_pitch;
				img_start += src_pitch;
			}
		}
		delete [] temp;

	} else {
		// if ClipRects() returned 2, that means we have to stretch or shrink the image accordingly:
		// (using nearest-neighbor technique, since it's the easiest to implement)

		XBYTE2 *img_start;
		XBYTE2 *img_rowstart;
		XBYTE2 *img_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE2 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
			img_rowstart = img_start + y*src_height/img_ys*src_pitch;
			view_pix = view_start;
			for (int x=0 ; x < img_xs; x++) {
				img_now = img_rowstart + x*src_width/img_xs;

				// assignment, not equality... and v = temp[view_pix] is the speedy alternative to: v = *view_pix
				int i, v;
				if ((i = *img_now)) { // if source pixel isn't black
					v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
					int blue  = ((v & win_maskb) + (((((i & win_maskb) >> win_shiftb) * bi) >> 8) << win_shiftb)) ;
					int green = ((v & win_maskg) + (((((i & win_maskg) >> win_shiftg) * gi) >> 8) << win_shiftg)) ;
					int red   = ((v & win_maskr) + (((((i & win_maskr) >> win_shiftr) * ri) >> 8) << win_shiftr)) ;
					*view_pix = ((bi < 0) ? max(0,blue)  : min(win_maskb, blue))
							  | ((gi < 0) ? max(0,green) : min(win_maskg, green))
							  | ((ri < 0) ? max(0,red)	 : min(win_maskr, red)) ;
				}

				view_pix++;
			}
			view_start += dst_pitch;
		}
		delete [] temp;
	}
	return 1;
}

int WindowsDX::InternalDrawAddedMerged (XBYTE * source_pix, XBYTE * dest_pix, RECT & src_rect, RECT & dst_rect, RECT & src_bounds, RECT & dst_bounds, int src_pitch, int dst_pitch, int ri, int gi, int bi)
{
	if(!(ri>0||gi>0||bi>0)) return 1; // if no intensity at all, return to save processor time

	int drawType = ClipRects(src_rect, dst_rect, src_bounds, dst_bounds);

	if(drawType == 0) return 1; // image is entirely out of bounds, so just return

	if(drawType == 1) {
		// if source and destination rectangles are exactly the same size:

		XBYTE4 *img_start;
		XBYTE4 *img_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE4 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		if(ri==255 && gi==255 && bi==255) { // slightly faster code in fully additive case (which r,g,b default to)
			for (int y=0 ; y < img_ys; y++) {
				memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
				view_pix = view_start;
				img_now = img_start;
				for (int x=0 ; x < img_xs; x++) {
					int i = *img_now;
					int a = ((i & 0xFF000000) >> 24);
					int v;
					if(a && (i & 0x00FFFFFF) && ((v = temp[view_pix-view_start]) != 65535)) { // if destination pixel isn't white and source pixel isn't black
						int r = (((i & 0x00FF0000) >> 16) * a) >> 8; // >> 8 is approximation for / 255
						int g = (((i & 0x0000FF00) >> 8) * a) >> 8; // >> 8 is approximation for / 255
						int b = (((i & 0x000000FF) >> 0) * a) >> 8; // >> 8 is approximation for / 255
						i = ((r >> 3) << win_shiftr) | ((g >> 2) << win_shiftg) | ((b >> 3) << win_shiftb);
						*view_pix = min(((v & win_maskb) + (i & win_maskb)), win_maskb)
								  | min(((v & win_maskg) + (i & win_maskg)), win_maskg)
								  | min(((v & win_maskr) + (i & win_maskr)), win_maskr) ;
					}
					view_pix++; img_now++;
				}
				view_start += dst_pitch;
				img_start += src_pitch;
			}
		}
		else {
			for (int y=0 ; y < img_ys; y++) {
				memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
				view_pix = view_start;
				img_now = img_start;
				for (int x=0 ; x < img_xs; x++) {
					int i = *img_now;
					int a = ((i & 0xFF000000) >> 24);
					int v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
					if(a && (i & 0x00FFFFFF)) { // if source pixel isn't black
						int r = ((((i & 0x00FF0000) >> 16) * a*ri)) >> 16; // >> 16 is approximation for / 255 / 255
						int g = ((((i & 0x0000FF00) >> 8) * a*gi)) >> 16; // >> 16 is approximation for / 255 / 255
						int b = ((((i & 0x000000FF) >> 0) * a*bi)) >> 16; // >> 16 is approximation for / 255 / 255
						i = ((r >> 3) << win_shiftr) | ((g >> 2) << win_shiftg) | ((b >> 3) << win_shiftb);
						r = min(((v & win_maskr) + (i & win_maskr)), win_maskr);
						g = min(((v & win_maskg) + (i & win_maskg)), win_maskg);
						b = min(((v & win_maskb) + (i & win_maskb)), win_maskb);
						*view_pix = ((bi < 0) ? max(0,b) : min(win_maskb, b))
								  | ((gi < 0) ? max(0,g) : min(win_maskg, g))
								  | ((ri < 0) ? max(0,r) : min(win_maskr, r)) ;
					}
					view_pix++; img_now++;
				}
				view_start += dst_pitch;
				img_start += src_pitch;
			}
		}
		delete [] temp;

	} else {
		// if ClipRects() returned 2, that means we have to stretch or shrink the image accordingly:
		// (using nearest-neighbor technique, since it's the easiest to implement)

		XBYTE4 *img_start;
		XBYTE4 *img_rowstart;
		XBYTE4 *img_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE4 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
			img_rowstart = img_start + y*src_height/img_ys*src_pitch;
			view_pix = view_start;
			for (int x=0 ; x < img_xs; x++) {
				img_now = img_rowstart + x*src_width/img_xs;
				int i = *img_now;
				int a = ((i & 0xFF000000) >> 24);
				int v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
				if(a && (i & 0x00FFFFFF)) { // if source pixel isn't black
					int r = ((((i & 0x00FF0000) >> 16) * a*ri)) >> 16; // >> 16 is approximation for / 255 / 255
					int g = ((((i & 0x0000FF00) >> 8) * a*gi)) >> 16; // >> 16 is approximation for / 255 / 255
					int b = ((((i & 0x000000FF) >> 0) * a*bi)) >> 16; // >> 16 is approximation for / 255 / 255
					i = ((r >> 3) << win_shiftr) | ((g >> 2) << win_shiftg) | ((b >> 3) << win_shiftb);
					r = min(((v & win_maskr) + (i & win_maskr)), win_maskr);
					g = min(((v & win_maskg) + (i & win_maskg)), win_maskg);
					b = min(((v & win_maskb) + (i & win_maskb)), win_maskb);
					*view_pix = ((bi < 0) ? max(0,b) : min(win_maskb, b))
							  | ((gi < 0) ? max(0,g) : min(win_maskg, g))
							  | ((ri < 0) ? max(0,r) : min(win_maskr, r)) ;
				}

				view_pix++;
			}
			view_start += dst_pitch;
		}
		delete [] temp;
	}
	return 1;
}


// ro, go, bo are RGB opacity values, they default to 128 if not passed in,
// 255,255,255 means fully opaque, 0,0,0 means fully transparent, 128,128,128 means half transparent,
// 255,128,0 means fully opaque red, half transparent green and fully transparent blue, etc.
int WindowsDX::InternalDrawBlended (XBYTE * source_pix, XBYTE * dest_pix, RECT & src_rect, RECT & dst_rect, RECT & src_bounds, RECT & dst_bounds, int src_pitch, int dst_pitch, int r, int g, int b, int a, int nomask)
{
	if(a<=0) return 1; // if no opacity at all, return to save processor time

	int drawType = ClipRects(src_rect, dst_rect, src_bounds, dst_bounds);

	if(drawType == 0) return 1; // image is entirely out of bounds, so just return

	XBYTE2 *img_start;
	XBYTE2 *img_now;
	XBYTE2 *view_start;
	XBYTE2 *view_pix;
	int img_offset = src_rect.top * src_pitch + src_rect.left;
	int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
	int img_xs = dst_rect.right-dst_rect.left;
	int img_ys = dst_rect.bottom-dst_rect.top;

	if(drawType == 1) {
		// if source and destination rectangles are exactly the same size:


		img_start = (XBYTE2 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
			view_pix = view_start;
			img_now = img_start;
			for (int x=0 ; x < img_xs; x++) {
				int i, v;
				if ((i = *img_now) || nomask) { // if source pixel isn't black
					v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
					*view_pix =
						  ((((((v & win_maskb) >> win_shiftb) *   (255-a)) >> 8) << win_shiftb)
						 + (((((i & win_maskb) >> win_shiftb) * (b*a/255)) >> 8) << win_shiftb))
						| ((((((v & win_maskg) >> win_shiftg) *   (255-a)) >> 8) << win_shiftg)
						 + (((((i & win_maskg) >> win_shiftg) * (g*a/255)) >> 8) << win_shiftg))
						| ((((((v & win_maskr) >> win_shiftr) *   (255-a)) >> 8) << win_shiftr)
						 + (((((i & win_maskr) >> win_shiftr) * (r*a/255)) >> 8) << win_shiftr)) ;
				}
				view_pix++; img_now++;
			}
			view_start += dst_pitch;
			img_start += src_pitch;
		}
		delete [] temp;

	} else {
		// if ClipRects() returned 2, that means we have to stretch or shrink the image accordingly:
		// (using nearest-neighbor technique, since it's the easiest to implement)

		XBYTE2 *img_rowstart;

		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE2 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
			img_rowstart = img_start + y*src_height/img_ys*src_pitch;
			view_pix = view_start;
			for (int x=0 ; x < img_xs; x++) {
				img_now = img_rowstart + x*src_width/img_xs;

				// assignment, not equality... and v = temp[view_pix] is the speedy alternative to: v = *view_pix
				int i, v;
				if ((i = *img_now) || nomask) { // if source pixel isn't black
					v = temp[view_pix-view_start] ; // speedy alternative to: v = *view_pix
					*view_pix =
						  ((((((v & win_maskb) >> win_shiftb) *   (255-a)) >> 8) << win_shiftb)
						 + (((((i & win_maskb) >> win_shiftb) * (b*a/255)) >> 8) << win_shiftb))
						| ((((((v & win_maskg) >> win_shiftg) *   (255-a)) >> 8) << win_shiftg)
						 + (((((i & win_maskg) >> win_shiftg) * (g*a/255)) >> 8) << win_shiftg))
						| ((((((v & win_maskr) >> win_shiftr) *   (255-a)) >> 8) << win_shiftr)
						 + (((((i & win_maskr) >> win_shiftr) * (r*a/255)) >> 8) << win_shiftr)) ;
				}

				view_pix++;
			}
			view_start += dst_pitch;
		}
		delete [] temp;
	}
	return 1;
}

int WindowsDX::InternalDrawInvert (XBYTE * source_pix, XBYTE * dest_pix, RECT & src_rect, RECT & dst_rect, RECT & src_bounds, RECT & dst_bounds, int src_pitch, int dst_pitch, int ri, int gi, int bi)
{
	if(!(ri>0||gi>0||bi>0)) return 1; // if no intensity at all, return to save processor time

	int drawType = ClipRects(src_rect, dst_rect, src_bounds, dst_bounds);

	if(drawType == 0) return 1; // image is entirely out of bounds, so just return

	if(drawType == 1) {
		// if source and destination rectangles are exactly the same size:

		XBYTE2 *img_start;
		XBYTE2 *img_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE2 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
			view_pix = view_start;
			img_now = img_start;
			for (int x=0 ; x < img_xs; x++) {
				int i = *img_now;
				int v = temp[view_pix-view_start];

				int iBlue = ((i & win_maskb) >> win_shiftb) * bi / 255;
				int iGreen = ((i & win_maskg) >> win_shiftg) * gi / 255;
				int iRed = ((i & win_maskr) >> win_shiftr) * ri / 255;
				int vBlue = (v & win_maskb) >> win_shiftb;
				int vGreen = (v & win_maskg) >> win_shiftg;
				int vRed = (v & win_maskr) >> win_shiftr;
				int bSub = ((iBlue) * (vBlue)) / 4;
				
				// almost right...
				// C  = i + v - 2*i[r]*v[r] - 2*i[g]*v[g] - 2*i[b]*v[b]
				// should work though...

				int blue  = ((v & win_maskb) + (i & win_maskb) - ((( (iBlue)  * (vBlue))  / 16) << win_shiftb)) ;
				int green = ((v & win_maskg) + (i & win_maskg) - ((( (iGreen) * (vGreen)) / 32) << win_shiftg)) ;
				int red   = ((v & win_maskr) + (i & win_maskr) - ((( (iRed)   * (vRed))   / 16) << win_shiftr)) ;
				*view_pix = ((bi < 0) ? max(0,blue)  : min(win_maskb, blue))
						  | ((gi < 0) ? max(0,green) : min(win_maskg, green))
						  | ((ri < 0) ? max(0,red)	 : min(win_maskr, red)) ;

				view_pix++; img_now++;
			}
			view_start += dst_pitch;
			img_start += src_pitch;
		}
		delete [] temp;

	} else {
		// if ClipRects() returned 2, that means we have to stretch or shrink the image accordingly:
		// (using nearest-neighbor technique, since it's the easiest to implement)

		XBYTE2 *img_start;
		XBYTE2 *img_rowstart;
		XBYTE2 *img_now;

		XBYTE2 *view_start;
		XBYTE2 *view_pix;

		int img_offset = src_rect.top * src_pitch + src_rect.left;
		int view_offset = dst_rect.top * dst_pitch + dst_rect.left;
		int img_xs = dst_rect.right-dst_rect.left;
		int img_ys = dst_rect.bottom-dst_rect.top;
		int src_height = src_rect.bottom-src_rect.top;
		int src_width = src_rect.right-src_rect.left;

		img_start = (XBYTE2 *) source_pix + img_offset;
		view_start = (XBYTE2 *) dest_pix + view_offset;

		XBYTE2 *temp = new XBYTE2[img_xs];

		for (int y=0 ; y < img_ys; y++) {
			memcpy (temp, view_start, img_xs<<1) ; // copy a source-length line of the destination -- the slowest part by far, but there doesn't seem to be a way to optimize it any more...
			img_rowstart = img_start + y*src_height/img_ys*src_pitch;
			view_pix = view_start;
			for (int x=0 ; x < img_xs; x++) {
				img_now = img_rowstart + x*src_width/img_xs;

				int i = *img_now;
				int v = temp[view_pix-view_start];

				int iBlue = ((i & win_maskb) >> win_shiftb) * bi / 255;
				int iGreen = ((i & win_maskg) >> win_shiftg) * gi / 255;
				int iRed = ((i & win_maskr) >> win_shiftr) * ri / 255;
				int vBlue = (v & win_maskb) >> win_shiftb;
				int vGreen = (v & win_maskg) >> win_shiftg;
				int vRed = (v & win_maskr) >> win_shiftr;
				int bSub = ((iBlue) * (vBlue)) / 4;
				
				// almost right...
				// C  = i + v - 2*i[r]*v[r] - 2*i[g]*v[g] - 2*i[b]*v[b]
				// should work though...

				int blue  = ((v & win_maskb) + (i & win_maskb) - ((( (iBlue)  * (vBlue))  / 16) << win_shiftb)) ;
				int green = ((v & win_maskg) + (i & win_maskg) - ((( (iGreen) * (vGreen)) / 32) << win_shiftg)) ;
				int red   = ((v & win_maskr) + (i & win_maskr) - ((( (iRed)   * (vRed))   / 16) << win_shiftr)) ;
				*view_pix = ((bi < 0) ? max(0,blue)  : min(win_maskb, blue))
						  | ((gi < 0) ? max(0,green) : min(win_maskg, green))
						  | ((ri < 0) ? max(0,red)	 : min(win_maskr, red)) ;

				view_pix++;
			}
			view_start += dst_pitch;
		}
		delete [] temp;
	}
	return 1;
}
*/

RECT WindowsDX::ConvertRECT(RECT & rect, ImageX * img)
{
	// somewhat kludgy way of getting a source rectangle:
	RECT rect2;
	if(rect.right != DEFAULT) {
		memcpy(&rect2,&rect,sizeof(RECT));

		if(img != VIEWPORT && img != NULL) {
			if(img->m_xmult != 1.0f) rect2.left = (LONG)((float)rect2.left * img->m_xmult), rect2.right = (LONG)((float)rect2.right * img->m_xmult);
			if(img->m_ymult != 1.0f) rect2.top = (LONG)((float)rect2.top * img->m_ymult), rect2.bottom = (LONG)((float)rect2.bottom * img->m_ymult);
		}
	} else {
		if(img == VIEWPORT) {
			rect2.left = win_viewx1;
			rect2.top = win_viewy1;
			rect2.right = win_viewx2;
			rect2.bottom = win_viewy2;
		} else if(img == NULL) {
			rect2.left = 0;
			rect2.top = 0;
			rect2.right = 1;
			rect2.bottom = 1;
		} else {
			rect2.left = 0;
			rect2.top = 0;
			rect2.right = img->m_xres;
			rect2.bottom = img->m_yres;
			if(img->m_xmult != 1.0f) rect2.right = (LONG)((float)rect2.right * img->m_xmult);
			if(img->m_ymult != 1.0f) rect2.bottom = (LONG)((float)rect2.bottom * img->m_ymult);
		}
	}
	return rect2;
}

void WindowsDX::ReportProblem (GameXStatus problem)
{
	win_status = problem;
	ReportProblem() ;
}

void WindowsDX::ReportProblem (void)
{	
	if(win_status_problem_ignore[win_status])
		win_status = GAMEX_READY;
	if(win_status == GAMEX_READY)
		return; // if there's no problem or it's already been reported and ignored

	win_status_problem_ignore[win_status] = true;

	char error[128] ;
	char recommend[640] ;	
	char disp[1280] ;
	
	switch (win_status) {
	case GAMEX_NO_INIT:
		sprintf (error, "Error: NO_INIT (User did not initialize GameX)") ;
		sprintf (recommend, "You must start GameX by calling:\n  GameX.Initialize (name, flags, xres, yres, speed)\n\nfrom the GameInit() function.\nFor example:\n  GameX.Initialize(\"My Game\",VIDEO_16BIT, 640, 480, 60);\n\nIf you are sure that you are making this call before anything else in GameInit(), make sure that you don't have any\nstatically constructed objects whose constructors use GameX in some way, such as by loading an image.\n\nSee documentation for details on starting GameX.") ;
		break;
	case GAMEX_INITCLASS_FAILED:
		sprintf (error, "Error: INITCLASS_FAILED (Cannot Initialize Window Class)") ;
		sprintf (recommend, "Are you using Windows 98/2000/NT/XP? GameX requires one of these. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_INITWINTEMP_FAILED:
		sprintf (error, "Error: INITWINTEMP_FAILED (Cannot Initialize Temp Window)") ;
		sprintf (recommend, "Are you using Windows 98/2000/NT/XP? GameX requires one of these. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_INITWINREAL_FAILED:
		sprintf (error, "Error: INITWINREAL_FAILED (Cannot Initialize Main Window)") ;
		sprintf (recommend, "Are you using Windows 98/2000/NT/XP? GameX requires one of these. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_SHOWWIN_FAILED:
		sprintf (error, "Error: SHOWWIN_FAILED (Unable to show Main Window)") ;
		sprintf (recommend, "Are you using Windows 98/2000/NT/XP? GameX requires one of these. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_DDSTART_FAILED:
		sprintf (error, "Error: DDSTART_FAILED (Unable to start DirectDraw or DirectX)") ;
		sprintf (recommend, "Have you installed DirectX 7.0 or later? GameX requires it. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_D3DSTART_FAILED:
		sprintf (error, "Error: D3DSTART_FAILED (Unable to start Direct3D)") ;
		sprintf (recommend, "Have you installed DirectX 7.0 or later? GameX requires it. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_DDSETLEVEL_FAILED:
		sprintf (error, "Error: DDSETLEVEL_FAILED (Unable to set DirectX Cooperative Level)") ;
		sprintf (recommend, "Have you installed DirectX 7.0 or later? GameX requires it. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_SETMODE_FAILED:
		sprintf (error, "Error: SETMODE_FAILED (Unable to set screen mode)") ;
		sprintf (recommend, "Have you installed DirectX 7.0 or later? GameX requires it. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_FRONTSURFACE_FAILED:
		sprintf (error, "Error: FRONTSURFACE_FAILED (Unable to create primary surface)") ;
		sprintf (recommend, "Have you installed DirectX 7.0 or later? GameX requires it. Still having trouble? Please report error and system configuration.") ;	
		break;
	case GAMEX_BACKSURFACE_FAILED:
		sprintf (error, "Error: BACKSURFACE_FAILED (Unable to create secondary surface)") ;
		sprintf (recommend, "Have you installed DirectX 7.0 or later? GameX requires it. Still having trouble? Please report error and system configuration.") ;
		break;
	case GAMEX_REQSIZE_FAILED:
		sprintf (error, "Error: REQSIZE_FAILED (Requested size invalid)") ;
		sprintf (recommend, "The requested game size was invalid.\nThe absolute maximum size allowed is 2048x2047, and the absolute minimum allowed is 64x64.\nTry to request a reasonable size though, such as something between 320x240 and 1600x1200.\nIf you are unsure of screen size, try initializing GameX as follows:\nGameX.Initialize (\"My Game\",VIDEO_16BIT, 800, 600);") ;
		break;
	case GAMEX_SOUNDINIT_FAILED:
		sprintf (error, "Error: SOUNDINIT_FAILED (Unable to initialize Direct Sound)") ;
		sprintf (recommend, "GameX was unable to initialize sound. Is sound card installed and working?") ;
		break;
	case GAMEX_SOUNDCOOP_FAILED:
		sprintf (error, "Error: SOUNDCOOP_FAILED (Unable to set Direct Sound Cooperative Level)") ;
		sprintf (recommend, "GameX was unable to set Direct Sound cooperative level. Is sound card installed and working?") ;
		break;	
	case GAMEX_SOUNDCREATE_FAILED:
		sprintf (error, "Error: SOUNDCREATE_FAILED (Unable to create Direct Sound buffer)") ;
		sprintf (recommend, "GameX was unable to play a sound, probably because it did not load correctly. Make sure to save your sounds in a standard format\n (8- or 16-bits, 11025- or 22050- or 44100- samples per second, and in PCM format).") ;
		break;
	case GAMEX_SOUNDMATCH_FAILED:
		sprintf (error, "Error: SOUNDMATCH_FAILED (Loaded sounds do not match)") ;
		sprintf (recommend, "You are attempting to load several sounds with different quality, resolution, and/or channel info. All sounds to be played simultaneously must have the same resolution (bits per sample), same quality (samples per second), and same number of channels (mono vs. stereo). Edit WAV files accordinging.") ;
		break;
	case GAMEX_SOUNDCAPS_FAILED:
		sprintf (error, "Error: SOUNDCAPS_FAILED (Unable to get Sound Buffer Capabilities)") ;
		sprintf (recommend, "GameX was unable to retrieve sound buffer capabilities. You may be running out of memory. Close any open applications. Make sure you have DirectX 7.0 or later.") ;
		break;
	case GAMEX_SOUNDBUFLEN_FAILED:
		sprintf (error, "Error: SOUNDBUFLEN_FAILED (Direct Sound buffer and SoundX buffers do not match)") ;
		sprintf (recommend, "Direct Sound and SoundX buffer lengths do not match. Internal error. Please report this problem and the code that generated it.") ;	
		break;
	case GAMEX_SOUNDLOST_FAIL:
		sprintf (error, "Error: SOUNDLOST_FAIL (Direct Sound buffer was lost)") ;
		sprintf (recommend, "Direct Sound buffer was lost. Make sure you are using DirectX 7.0 or later. Close any open applications. Try restarting system. If you are using a large or high quality sound file, try reducing it.") ;
		break;
	case GAMEX_SOUNDINVALID_FAIL:
		sprintf (error, "Error: SOUNDINVALID_FAIL (Direct Sound Invalid Call)") ;
		sprintf (recommend, "DirectX, via GameX, made an invalid sound call. Make sure you are using DirectX 7.0 or later. Close any open applications. Try restarting system. If you are using a large or high quality sound file, try reducing it.") ;
		break;
	case GAMEX_SOUNDPARAM_FAIL:
		sprintf (error, "Error: SOUNDPARAM_FAIL (Direct Sound Invalid Parameter)") ;
		sprintf (recommend, "DirectX, via GameX, used an invalid parameter. Make sure you are using DirectX 7.0 or later. Close any open applications. Try restarting system. If you are using a large or high quality sound file, try reducing it.") ;
		break;
	case GAMEX_SOUNDPRIOLEVEL_FAIL:
		sprintf (error, "Error: SOUNDPARAM_FAIL (Direct Sound needs Priority Level)") ;
		sprintf (recommend, "DirectX, via GameX, is using an invalid priority level. Make sure you are using DirectX 7.0 or later. Close any open applications. Try restarting system. If you are using a large or high quality sound file, try reducing it.") ;		
		break;	
	case GAMEX_UNINIT_IMG_DRAW_FAIL:
		sprintf (error, "Error: UNINIT_IMG_DRAW_FAIL (Attempted to draw an ImageX that was not ready to be drawn)") ;
		sprintf (recommend,"You must Load() or Create() each ImageX before it can be drawn.\nAlso, make sure you do not delete, Destroy(), or\nallow an ImageX to go out of scope while it is in use.\n(When an image is drawn, it can remain in use until GameDraw() returns.)");
		break;
	case GAMEX_INVALID_DRAW_TIME_FAIL:
		sprintf (error, "Error: INVALID_DRAW_TIME_FAIL (Attempted to draw an ImageX or other graphic to the screen from GameRun())") ;
		sprintf (recommend,"You cannot use the drawing functions to draw to the screen in GameRun() or in ANY functions that GameRun() calls.\n(Or in anything they in turn call, etc.)\nThese drawing functions should only be called via GameDraw(), not GameRun().\nKeep in mind that the purpose of GameRun() is to update the game state without drawing it,\nand the purpose of GameDraw() is to draw the game state without updating it.");
		break;
	case GAMEX_PIXEL_ACCESS_FAILURE:
		sprintf (error, "Error: PIXEL_ACCESS_FAILURE (Attempted to call DrawPixel or ReadPixel without first calling AccessPixels())") ;
 		sprintf (recommend,"You must call GameX.AccessPixels(&image) immediately before directly drawing to or reading from an image's pixels,\nand you must call GameX.EndPixelAccess() when done (and before calling any other drawing functions).\n\nIf you want to draw pixels directly to the screen, the access call is GameX.AccessPixels(VIEWPORT).\n");
		break;
	case GAMEX_CLIPPER_FAILED:
		sprintf (error, "Error: CLIPPER_FAILED (Failed to create or set a clipper for the back buffer)") ;
		sprintf (recommend, "Are you using DirectX 7.0 or later? GameX requires one of these. Still having trouble? Please report error and system configuration.") ;
		break;
	default:
		sprintf (error, "Error: UNKNOWN") ;
		sprintf (recommend, "Unregistered error with GameX. Please report. (Unless you caused this error by editing GameX)") ;
		break;
	}
	sprintf (disp, "GameX %s\n", error) ;
	debug.Output (disp, "") ; 
	int choice;

	if(win_status == GAMEX_DDSTART_FAILED) {
		sprintf (disp, "GameX has encountered a fatal error.\n\n%s\n\n%s\n\nIf restarting does not fix this problem, please (re)install DirectX and try again.", error, recommend) ;
		MessageBox (win_hwnd, disp, "GameX Error", MB_ICONSTOP) ;
		choice = IDNO;
	} else {
		sprintf (disp, "GameX has encountered an error and may not be able to continue.\n\n%s\n\n%s\n\nAttempt to continue execution anyway?", error, recommend) ;
		if(dd_main != NULL)
			win_status = GAMEX_READY;
		choice = MessageBox (win_hwnd, disp, "GameX Error", MB_YESNO|MB_DEFBUTTON2|MB_ICONSTOP) ;
	}
	if(choice == IDNO)
		Quit(true);
	else if (win_status == GAMEX_NO_INIT)
		Initialize("Improperly Initialized Game", VIDEO_16BIT|VIDEO_WINDOWED,640,480);

} // insert breakpoint here :-)

// vol: 0.0 means silent, 1.0 means loud, etc.
// pan: -1.0 means left only, 0.0 means center (both), 1.0 means right only, 1.5 means right only half volume, -1.5 means left only half volume, <=-2.0 and >=2.0 mean no volume, etc.
// freq: 1.0 means normal frequency, 2.0 means twice as high-pitched, 0.5 means half pitch, etc.
void WindowsDX::PlaySound (SoundX * snd, SoundPlayMode mode, float vol, float pan, float freq)
{
	if(sound_support) {
		if(snd->NeedUpdate()==true) UpdateSound((SoundXPtr)snd) ;	
		LPDIRECTSOUNDBUFFER buffer = snd->GetDSBuffer();
		if(buffer == NULL) return;
		if(pan > 1.0f) {
			vol *= max(0.0f, 1.0f - (pan-1.0f));
			pan = 1.0f;
		} else if(pan < -1.0f) {
			vol *= max(0.0f, 1.0f - (-pan-1.0f));
			pan = -1.0f;
		}
		if(vol > 1.0f) vol = 1.0f; // DirectSound does not support sound amplification (volume > 100%)
		if(vol < 0.0f) vol = 0.0f;
		if(mode & PLAY_NOTIFLESSVOL) {
			DWORD status;
			buffer->GetStatus(&status);
			if(status & DSBSTATUS_PLAYING) {
				long old_vol;
				buffer->GetVolume(&old_vol);
				if(ToAttenuation(vol) < old_vol)
					return;
			}
		}
		if(mode & PLAY_REWIND) buffer->SetCurrentPosition(0) ;

		buffer->SetVolume(ToAttenuation(vol)) ;
		buffer->SetPan((long) (pan*2048.0)) ;
		buffer->SetFrequency((DWORD)(snd->GetSamplesPerSec()*freq)) ;
		buffer->Play(0,0,(mode & PLAY_LOOP ? DSBPLAY_LOOPING : 0)) ;
	}
}


// Plays a MusicX object, and immediately stops playing any other music that may be playing
// times of 0 means infinite loop, 1 means play once, 2 means play twice, etc.
// speed_factor of 1.0 means normal speed, 2.0 means double speed, etc.
void WindowsDX::PlayMusic (MusicX * music, int times, float volume, float fade_in_seconds, float tempo, float pitch)
{
	StopMusic(current_music,0.0f);
	current_music = music;
	if(music == NULL) return;
	music->playing_loops_left = times ? times : 32767;
	switch(music->format) {
		case MUSIC_TYPE_MIDI: {
			if(music->GetSegment() == NULL) {
///				#ifdef _DEBUG
					MessageBox(GetWindow(), "Error: Attempting to play a music file that did not load properly.\nMake sure your music files are in the proper location and in .mid format.\nAlso make sure any music files needed by the game are not open in another program.", "GameX Error",MB_OK|MB_ICONSTOP) ;
///				#endif
				// if we're not in debug mode, don't bother the player with the music failure message
				return;
			}

			VolumeFadeMusic(music, 0.0f, 0.0f);

			if(music->GetPlayState()) { // if already playing, have to wait for change to take effect
				int clocker = clock();
				while(clock() - clocker < 80) {}
			}

			if(times) times--;
			else times = DMUS_SEG_REPEAT_INFINITE;
			music->GetSegment()->SetRepeats(times);
			SetMusicTempo(tempo);
			IDirectMusicSegmentState * idmss = music->GetSegmentState();
			dm_performance->PlaySegment(music->GetSegment(), 0, 0, &idmss);
		}	break;
		case MUSIC_TYPE_MP3: {
			HRESULT status = music->dsh_mc->Run();

			if(FAILED(status)) {
				debug.Output("WinDX: Failed to Run MP3");
				return;
			}
		}	break;
		case MUSIC_TYPE_NONE:
			return;
	}

	PitchBendMusic(music, pitch,0.0f);
	VolumeFadeMusic(music, volume, fade_in_seconds);
	music->SetPlayState(1);
}

// Fades the volume to 0 over given seconds (if seconds>0), then stops the music from playing
// If it's an MP3, fade not supported yet so acts as if seconds = 0.
void WindowsDX::StopMusic (MusicX * music, float seconds)
{
	if(music == NULL) music = current_music;
	if(music == NULL) return;
	switch(music->format) {
		case MUSIC_TYPE_MIDI: {
			if(music->GetSegment() == NULL) return;
			VolumeFadeMusic(music, 0.0f, seconds); // fade for given seconds

			// calculate time until after fade
			DMUS_TEMPO_PARAM tempo;
			music->GetSegment()->GetParam(GUID_TempoParam, 0xFFFF, 0, 0, NULL, &tempo);
			seconds *= (float)(tempo.dblTempo * 10.0);
			MUSIC_TIME musictime;
			dm_performance->GetTime(NULL, &musictime);
			musictime += (MUSIC_TIME)seconds;

			dm_performance->Stop(music->GetSegment(), NULL, musictime, 0); // stop after fade
			music->SetPlayState(seconds?2:0);
		}	break;
		case MUSIC_TYPE_MP3: {
			current_music->dsh_mc->Stop();
			current_music->dsh_mp->put_CurrentPosition(0);
			music->SetPlayState(0);
		}	break;
	}
	current_music = NULL;
}

// Fades the music up or down to the given volume factor, over the given time
// seconds is how many seconds to alter volume over
// target_factor of 0.0f is fade to silence, 0.5f is fade to half master volume, 1.0f is no change, 1.27f is fade to max amplified
void WindowsDX::VolumeFadeMusic(MusicX * music, float volume_factor, float seconds)
{
	switch(music->format) {
	case MUSIC_TYPE_MIDI:
		if(seconds < 0.05f) seconds = 0.05f;
		music->Curve((long)(1000*seconds), (int)max(-20,min(127, 100*volume_factor)), DMUS_CURVET_CCCURVE, DMUS_CURVES_LINEAR, (int)max(-20,min(127, 100*music->GetVolume())));
		music->RememberVol(volume_factor);
		break;
	case MUSIC_TYPE_MP3:
		current_music->dsh_ba->put_Volume(ToAttenuation(volume_factor*master_music_volume));
		break;
	}
}

// Bends the pitch of the mucis up or down to the given pitch factor, over the given time
// seconds is how many seconds to alter pitch over. This method does not change speed of play
// pitch_factor of 0.5f is half pitch (low frequency), 2.0f is double pitch (high frequency), 1.0f is no change, etc.
void WindowsDX::PitchBendMusic(MusicX * music, float pitch_factor, float seconds)
{
	switch(music->format) {
	case MUSIC_TYPE_MIDI:
		if(seconds < 0.05f) seconds = 0.05f;
		music->Curve((long)(1000*seconds), (int)max(1000, min(32767, 8000*pitch_factor)), DMUS_CURVET_PBCURVE, DMUS_CURVES_LINEAR);
		break;
	}
}

// Sets the tempo, or speed of play, of all music -- does not change the pitch
// 0.5f is half speed, 1.0f is normal speed, 2.0f is double speed, etc.
// Can be called before or after you start playing the song
// note: there may be a small time delay after calling this function before the change takes effect
void WindowsDX::SetMusicTempo(float tempo_factor)
{
	tempo_factor = min(DMUS_MASTERTEMPO_MAX, max(DMUS_MASTERTEMPO_MIN, tempo_factor));
	dm_performance->SetGlobalParam(GUID_PerfMasterTempo, &tempo_factor, sizeof(float));
	master_music_tempo = tempo_factor;
}

// Sets the volume of all music
// Can be called before or after you start playing a song
// If an individual music song has its volume faded to 50% and the master volume is 50%
// that will result in the music being played at 25% volume
void WindowsDX::SetMasterMusicVolume(float volume_factor)
{
	long volume = ToAttenuation(volume_factor);
	dm_performance->SetGlobalParam(GUID_PerfMasterVolume, &volume, sizeof(long));
	master_music_volume = volume_factor;
}

// returns true if the given song is still playing, looping, or paused, false otherwise
bool WindowsDX::IsMusicPlaying(MusicX * music)
{
	switch(music->format) {
	case MUSIC_TYPE_MIDI:
		return(dm_performance->IsPlaying(music->GetSegment(),NULL) == S_OK);
	case MUSIC_TYPE_MP3:
		long lEvent;
		current_music->dsh_me->WaitForCompletion(0,&lEvent); // wait time = 0, just to get playing status
		return(lEvent != EC_COMPLETE);
	}
	return false;
}



bool WindowsDX::IsSoundPlaying(SoundX * snd)
{
	if(snd->NeedUpdate()==true) UpdateSound(snd) ;	
	LPDIRECTSOUNDBUFFER buffer = snd->GetDSBuffer();
	if(buffer == NULL) return false;

	DWORD status;
	buffer->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
		return true;
	else
		return false;
}

void WindowsDX::RewindSound (SoundX * snd)
{
	if (sound_support) {
		if (snd->NeedUpdate()==true) UpdateSound (snd) ;	
		snd->GetDSBuffer()->SetCurrentPosition(0) ;
	}
}


void WindowsDX::StopSound (SoundX * snd)
{
	if (sound_support) {
		snd->GetDSBuffer()->Stop() ;
	}
}

void WindowsDX::UpdateSound (SoundXPtr snd)
{
	signed short *buf_a, *buf_b;	
	unsigned long bytes_a, bytes_b; 

	int num_samples, num_channels;
	int result;
	int n, c;
	
	// Determine Direct Sound buffer to use based on SoundX object.
	LPDIRECTSOUNDBUFFER buffer = snd->GetDSBuffer();

	if(buffer==NULL) {
		snd->PrepareDSBuffer();
		buffer = snd->GetDSBuffer();
		MemoryRecordChange(snd->GetNumBytes()/1024);
	}

	if(buffer==NULL) return;

	// Get requested Direct Sound buffer capabilities
	DSBCAPS caps;
	ZeroMemory(&caps, sizeof(caps)) ;
	caps.dwSize = sizeof(caps) ;
	if (DS_OK != buffer->GetCaps (&caps)) ReportProblem(GAMEX_SOUNDCAPS_FAILED) ;
		
	// Get and check number of samples (SoundX sample count should match DS buffer count)
	num_channels = snd->GetNumChannels() ;
	num_samples = caps.dwBufferBytes / num_channels;

	if (num_samples!=snd->GetNumSamples()) num_samples = snd->GetNumSamples();
		
	
	// Lock Direct Sound buffer
	result = buffer->Lock (0, caps.dwBufferBytes, (void**) &buf_a, &bytes_a, (void**) &buf_b, &bytes_b, 0) ;
	n = 0;
	while (result==DSERR_BUFFERLOST && n<10) {
		buffer->Restore() ;
		result = buffer->Lock (0, caps.dwBufferBytes, (void**) &buf_a, &bytes_a, (void**) &buf_b, &bytes_b, 0) ;
		n++;
	}
	// Check for possible buffer errors
	if (DS_OK!=result) {		
		if (result==DSERR_BUFFERLOST)			win_status = GAMEX_SOUNDLOST_FAIL;	
		else if (result==DSERR_INVALIDCALL) 	win_status = GAMEX_SOUNDINVALID_FAIL;
		else if (result==DSERR_INVALIDPARAM)	win_status = GAMEX_SOUNDPARAM_FAIL;
		else if (result==DSERR_PRIOLEVELNEEDED) win_status = GAMEX_SOUNDPRIOLEVEL_FAIL;
		ReportProblem() ;
	}
	// Copy SoundX data to Direct Sound buffer
	if (snd->GetBPS()<=8) {
		unsigned char *data = (unsigned char*) snd->GetData() ;
		unsigned char *curr_a = (unsigned char*) buf_a;
		for (n = 0; n < num_samples; n++)
			for (c=0; c < num_channels; c++)
				*curr_a++ = *data++;
	} else {
		signed short *data = (signed short*) snd->GetData() ;
		signed short *curr_a = buf_a;
		curr_a = buf_a;
		for (n = 0; n < num_samples; n++)
			for (c=0; c < num_channels; c++) 
				*curr_a++ = *data++;			
	}
	
	// Unlock Direct Sound buffer
	buffer->Unlock (&buf_a, bytes_a, &buf_b, bytes_b) ;

	snd->SetRefresh (false) ;
}

bool WindowsDX::CopyToClipboard(ImageX * img)
{
	LPDIRECTDRAWSURFACE7 p_src_surface;

	if(img == VIEWPORT) {
		p_src_surface = dd_back_buf;
	} else {
		if(img == NULL || img->status != IMG_OK || !img->m_hassurface || img->m_surface == NULL)
			return false;
		p_src_surface = img->m_surface;
	}

	return SurfaceToClipboard(dd_back_buf);
}

bool WindowsDX::SurfaceToClipboard(LPDIRECTDRAWSURFACE7 surface)
{
	bool ok = true;

	HDC hdc;
	DDSURFACEDESC2 ddsd;
	surface->GetDC (&hdc);
	HDC hdc2 = CreateCompatibleDC (hdc); 
	ZeroMemory (&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	surface->GetSurfaceDesc (&ddsd);
	HBITMAP bitmap = CreateCompatibleBitmap (hdc, ddsd.dwWidth, ddsd.dwHeight);
	HBITMAP oldBitmap = (HBITMAP) SelectObject (hdc2, bitmap);
	BitBlt (hdc2, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdc, 0, 0, SRCCOPY);

	if (OpenClipboard (win_hwnd)) { 
		EmptyClipboard();
		SetClipboardData (CF_BITMAP,bitmap);
		CloseClipboard(); 
	} else
		ok = false;
	
	SelectObject (hdc2,oldBitmap);
	DeleteDC (hdc2);
	surface->ReleaseDC (hdc);
	return ok;
}
