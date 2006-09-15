//---------------------------------------------------
// Name: Game : StateGame
// Desc:  the game
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_STATE_GAME_H_
#define _GAME_STATE_GAME_H_

#include "StateMachine.h"
#include "gamex.hpp"
#include "EntityGen.h"
#include "FileIO.h"
#include "Character.h"
#include "MasterFile.h"

#include <list>

namespace Game
{
	class Entity;

	class State_Game : public State
	{
	public:

		State_Game();

		void Enter();
		void Exit();
		void Handle();

	private:

		bool LoadEntityDesc();
		bool LoadLevel( const char* levelName );
		bool CreateEntityGen( EntitySetFile::EntitySetList& arrowSet );				

	private:

		EntityGen					mEntityGen;
		std::list<Entity*>			mActiveEntities;
		F32							mLevelEndTime;

		EntityDescMap				mEntityDescMap;

		ImageX*						mBackground;
		ImageX*						mTimerImg;
		MusicX						mMusic;

		Character					mPlayer;

		uint32_t					mCurLevel;
	};	
	
}; //end Game

#endif // end _GAME_STATE_GAME_H_
    
