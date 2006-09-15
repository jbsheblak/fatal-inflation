//---------------------------------------------------
// Name: Game : Timer
// Desc:  keeps track of time
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Timer.h"

#include <time.h>

namespace Game
{	
	//-----------------------------------------------------------
	// Name: Timer
	// Desc:  constructor
	//-----------------------------------------------------------
	Timer::Timer() :  mPaused(true)
					, mStartTime(0)
					, mAccumPauseTime(0)
					, mPauseStartTime(0)
					, mUnpauseTime(0)
	{}

	//-----------------------------------------------------------
	// Name: StartTimer
	// Desc:  unpauses and begins the timer
	//-----------------------------------------------------------
	void Timer::StartTimer()
	{
		mStartTime   = GetCurTime();
		mPaused	     = false;
		mUnpauseTime = -1.0f;
	}

	//-----------------------------------------------------------
	// Name: StopTimer
	// Desc:  pauses the timer with no intent to resume
	//-----------------------------------------------------------
	void Timer::StopTimer()
	{
		mPaused = true;
		mUnpauseTime = -1.0f;
	};

	//-----------------------------------------------------------
	// Name: PauseTimer
	// Desc:  pauses the timer with intent to resume
	//-----------------------------------------------------------
	void Timer::PauseTimer( F32 pauseLength )
	{
		mPaused = true;
		mPauseStartTime = GetCurTime();

		mUnpauseTime = pauseLength == -1.0f ? -1.0f : GetCurTime() + pauseLength;
	};

	//-----------------------------------------------------------
	// Name: UnpauseTimer
	// Desc:  restarts timer using the accumulated pause time
	//-----------------------------------------------------------
	void Timer::UnpauseTimer()
	{
		if( mPaused )
		{
			mAccumPauseTime += GetCurTime() - mPauseStartTime;
			mPaused = false;
			mUnpauseTime = -1.0f;
		}
	}

	//-----------------------------------------------------------
	// Name: ResetAndStopTimer
	// Desc:  
	//-----------------------------------------------------------
	void Timer::ResetAndStopTimer()
	{		
		mAccumPauseTime = 0;
		StopTimer();		
	}		

	//-----------------------------------------------------------
	// Name: GetTimeElapsed
	// Desc:  returns the elapased time since starting the timer
	//-----------------------------------------------------------
	F32 Timer::GetTimeElapsed()
	{
		return GetCurTime() - mStartTime - mAccumPauseTime;
	}

	//-----------------------------------------------------------
	// Name: IsPaused
	// Desc:  returns if the timer is paused or not
	//-----------------------------------------------------------
	bool Timer::IsPaused()
	{
		return mPaused;
	}

	//-----------------------------------------------------------
	// Name: Tick
	// Desc:  deals with frame by frame needs of timer
	//-----------------------------------------------------------
	void Timer::Tick()
	{
		if( mPaused && mUnpauseTime != -1.0f )
		{
			if( GetCurTime() > mUnpauseTime )
				UnpauseTimer();
		}
	}

	//-----------------------------------------------------------
	// Name: GetTimer
	// Desc:  singleton pattern
	//-----------------------------------------------------------
	Timer* Timer::GetTimer()
	{
		static Timer* timer = new Timer();
		return timer;
	}

	//-----------------------------------------------------------
	// Name: GetCurTime
	// Desc:  grabs the time in seconds
	//-----------------------------------------------------------
	F32 Timer::GetCurTime()
	{
		return (F32)clock() / 1000.0f;
	}
	
}; //end Game