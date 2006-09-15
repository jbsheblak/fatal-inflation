//---------------------------------------------------
// Name: Game : Log
// Desc:  logs information
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_LOG_H_
#define _GAME_LOG_H_

#include <vector>

namespace Game
{	
	//-----------------------------------------------------------
	// Name: Log
	// Desc:  the log serves as a central hub for all debug msg
	//        processing. Callback functions register themselves
	//		  with the log and the Log will pass them msgs. We
	//		  could have one callback that prints to screen and
	//		  another that prints to file and call SLog->Print()
	//		  will give the msgs to both.
	//-----------------------------------------------------------
	class Log
	{
	public:

		typedef void (*LogCallback)( const char* msg );

	public:

		void RegisterCallback( LogCallback cb );
		void Print( const char* msg );

		static Log* GetLog();

	private:

		typedef std::vector<LogCallback> CallbackList;

		CallbackList			mCallbacks;
	};		

#define SLog (Log::GetLog())
	
}; //end Game

#endif // end _GAME_LOG_H_
    
