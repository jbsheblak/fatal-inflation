//---------------------------------------------------
// Name: Game : Main
// Desc:  Application Entrance
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#define GAMEX_MAIN
#include "gamex.hpp"

#include "Types.h"
#include "GameConstants.h"

#include "State_StartScreen.h"
#include "State_Game.h"
#include "State_LoadGame.h"
#include "State_EditMode.h"

#include "Util/Tuner.h"

using namespace Game;


void GameInit()
{	
	// load our tuner variables
	gTuner.LoadTuners( "tuners.txt" );

	// setup init flags
	uint32_t flags = VIDEO_32BIT;

	flags |= ( kFullscreen? VIDEO_FULLSCREEN : VIDEO_WINDOWED );
	flags |= ( kUseVSync? VIDEO_ALLOWREFRESHSYNC : 0 );
	flags |= ( kResizeable? 0 : VIDEO_NORESIZE );

#if _DEBUG
	flags |= ( RUN_NOCONFIRMQUIT | RUN_AUTOSHOWINFO );
#endif

	// initialize GameX
    GameX.Initialize ( (char*)kWindowName, flags, kWindowWidth, kWindowHeight ); 

	// Register our states
	SMachine.AddState( "LoadGame", new State_LoadGame );
	SMachine.AddState( "StartScreen", new State_StartScreen );
	SMachine.AddState( "Game", new State_Game );
	SMachine.AddState( "EditMode", new State_EditMode );

	SMachine.RequestStateChange( "LoadGame" );
}

void GameRun (void)
{
     
}

void GameDraw (void)
{
	SMachine.Handle();
}