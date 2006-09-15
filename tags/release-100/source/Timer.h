//---------------------------------------------------
// Name: Game : Timer
// Desc:  keeps track of time
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_TIMER_H_
#define _GAME_TIMER_H_

#include "Types.h"

namespace Game
{	
	//-----------------------------------------------------------
	// Name: Timer
	// Desc:  manages the timed elements
	//-----------------------------------------------------------
	class Timer
	{
	private:

		Timer();

	public:	
		
		void StartTimer();		
		void StopTimer();

		// Pauses the timer indefinately or for a given time amount
		void PauseTimer( F32 pauseLength = -1.0f );
		void UnpauseTimer();
		void ResetAndStopTimer();

		F32   GetTimeElapsed();
		bool  IsPaused();

		void Tick();

		// singleton pattern
		static Timer*	GetTimer();

	private:

		F32		GetCurTime();

		bool		mPaused;			// are we paused?

		F32			mStartTime;			// when we started timing
		F32			mPauseStartTime;	// when we began the pause
		F32			mAccumPauseTime;    // how much time we have spent paused
		F32			mUnpauseTime;       // when to unpause the timer
	};	

#define sTimer (*Timer::GetTimer())
	
}; //end Game

#endif // end _GAME_TIMER_H_
    
