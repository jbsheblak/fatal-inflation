//---------------------------------------------------
// Name: Game : EntityGen
// Desc:  generates the ent a given times
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "EntityGen.h"
#include "Timer.h"

#include "EntityFactory.h"

namespace Game
{	
	EntityGen::EntityGen( const EntityCreateList& list )
	{
		EntityCreateList::const_iterator itr;
		for( itr = list.begin(); itr != list.end(); ++itr )
            AddEntity( *itr );
	}	

	// generate a single arrow
	bool EntityGen::AddEntity( const EntityCreateEntry& entry )
	{
		mEntityGenIdx = 0;

		EntityGenEntry e;
		e.mGenTime = entry.mGenTime;
		e.mEnt     = Game::EntityFactory( entry.mEntBase, (EntityDesc*)&entry.mEntDesc );		

		if( e.mEnt )
		{			
			e.mEnt->SetStartTime( e.mGenTime );
			mEntityGenList.push_back(e);
		}

		return true;
	}			

	Entity* EntityGen::GenEntity()
	{
		if( !sTimer.IsPaused() && mEntityGenIdx < (uint32_t)mEntityGenList.size() )
		{
			if( sTimer.GetTimeElapsed() >= mEntityGenList[mEntityGenIdx].mGenTime )
				return mEntityGenList[mEntityGenIdx++].mEnt;			
		}

		return NULL;
	}

	void EntityGen::StartGen()
	{
		sTimer.ResetAndStopTimer();
		sTimer.StartTimer();
	}

	void EntityGen::StopGen()
	{
		sTimer.StopTimer();
	}	
	
	void EntityGen::ResetGen()
	{
		sTimer.ResetAndStopTimer();
		mEntityGenIdx = 0;
	}	

	void EntityGen::PauseGen()
	{
		sTimer.PauseTimer();
	}

	void EntityGen::ClearGen()
	{
		EntityGenList::iterator itr;
		for( itr = mEntityGenList.begin(); itr != mEntityGenList.end(); ++itr )
		{
			if( itr->mEnt )
				delete itr->mEnt;
		}
		mEntityGenList.clear();
	}
	
}; //end Game