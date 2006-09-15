//---------------------------------------------------
// Name: Game : StateLoadGame
// Desc:  the game
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_STATE_LOAD_GAME_H_
#define _GAME_STATE_LOAD_GAME_H_

#include "StateMachine.h"
#include "FileIO.h"

namespace Game
{
	class State_LoadGame : public State
	{
	public:		
	       
		void Enter();
		void Exit() {}
		void Handle();

	private:

		void CleanFilename( char* cleaned, char* dirty );

		void CompileEntityDesc();
		void CompileLevelFiles();
		void PackFiles();

		PackFile::PackElement PackDirectory( const char* dir, const char* packName, char* ext = NULL );
		PackFile::PackElement PackImages();
		PackFile::PackElement PackAudio();
	};
	
}; //end Game

#endif // end _GAME_STATE_LOAD_GAME_H_
    
