//---------------------------------------------------
// Name: Game : Log
// Desc:  logs information
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Log.h"

namespace Game
{	
	//-----------------------------------------------------------
	// Name: RegisterCallback
	// Desc:  registers a callback function to use when logging
	//-----------------------------------------------------------
	void Log::RegisterCallback( LogCallback cb )
	{
		if( cb )
		{
			mCallbacks.push_back(cb);
		}
	}

	//-----------------------------------------------------------
	// Name: Print
	// Desc:  pass a msg to all of our callbacks
	//-----------------------------------------------------------
	void Log::Print( const char* msg )
	{
		CallbackList::iterator itr;
		for( itr = mCallbacks.begin(); itr != mCallbacks.end(); ++itr )
		{
			(*itr)(msg);
		}
	}

	//-----------------------------------------------------------
	// Name: GetLog
	// Desc:  singleton pattern
	//-----------------------------------------------------------
	Log* Log::GetLog()
	{
		static Log* pLog = new Log;
		return pLog;
	}
	
}; //end Game
