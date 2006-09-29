//---------------------------------------------------
// Name: Game : Tuner
// Desc:  allows for tuning of variables
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_TUNER_H_
#define _GAME_TUNER_H_

#include "Types.h"

#include <vector>
#include <map>
#include <string>

namespace Game
{	
	class Tuner
	{
	public:

		bool	LoadTuners( const char* szFile );

		int32_t		   GetInt( const char* name );
		F32			   GetFloat( const char* name );
		uint32_t	   GetUint( const char* name );
		std::string    GetString( const char* name );

		static Tuner* GetTuner();	

	private:

		typedef std::vector< std::string > TokenArray;
		void Split( std::string str, TokenArray& tokenArray, char delim );

	private:
        
		typedef std::map< std::string, std::string > VariableMap;
		VariableMap	mTunerVariables;

	}; //end Tuner

#define gTuner (*Tuner::GetTuner())
	
}; //end Game

#endif // end _GAME_TIMER_H_
    
