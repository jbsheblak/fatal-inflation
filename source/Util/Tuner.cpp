//---------------------------------------------------
// Name: Game : Tuner
// Desc:  allows for tuning of variables
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Tuner.h"
#include "../FileIO.h"

namespace Game
{
	bool Tuner::LoadTuners( const char* szFile )
	{
		mTunerVariables.clear();

		FILE* file = fopen( szFile, "r+t" );
		if( !file )
			return false;

		std::string line;
		while( FileUtils::GetNextLine( line, file ) )
		{
			TokenArray tokens;
			Split( line, tokens, ' ' );

			if( tokens.size() < 3 )
				continue;

			std::string tk0 = tokens[0];
			std::string tk2 = tokens[2];

			mTunerVariables[ tk0 ] = tk2;
		}

		fclose(file);

		return true;
	}

	int32_t Tuner::GetInt( const char* name )
	{
		VariableMap::iterator itr;
		if( (itr = mTunerVariables.find(name) ) != mTunerVariables.end() )
		{
			return atoi( itr->second.c_str() );
		}
		return 0;
	}

	F32 Tuner::GetFloat( const char* name )
	{
		VariableMap::iterator itr;
		if( (itr = mTunerVariables.find(name) ) != mTunerVariables.end() )
		{
			return (F32)atof( itr->second.c_str() );
		}
		return 0;
	}

	uint32_t Tuner::GetUint( const char* name )
	{
		VariableMap::iterator itr;
		if( (itr = mTunerVariables.find(name) ) != mTunerVariables.end() )
		{
			return (uint32_t)atoi( itr->second.c_str() );
		}
		return 0;
	}

	std::string Tuner::GetString( const char* name )
	{
		VariableMap::iterator itr;
		if( (itr = mTunerVariables.find(name) ) != mTunerVariables.end() )
		{
			return itr->second;
		}
		return "";
	}

	Tuner* Tuner::GetTuner()
	{
		static Tuner sTuner;
		return &sTuner;
	}

	void Tuner::Split( std::string str, TokenArray& tokenArray, char delim )
	{
		char buffer[256];
		int c = 0;

		for( uint32_t i = 0; i < str.length(); ++i )
		{
			//check for comments
			if( i+1 < str.length() && str[i] == '/' && str[i+1] == '/' )
				break;

			if( str[i] == delim && c != 0 )
			{
				buffer[c] = '\0';
				tokenArray.push_back( std::string(buffer) );
				c = 0;
			}
			else if( str[i] != delim && str[i] != '\n' && str[i] != '\t' && str[i] != '\r' )
			{
				buffer[c] = str[i];
				++c;
			}
		}

		//do we need to push the end string?
		if( c > 0 )
		{
			buffer[c] = '\0';
			tokenArray.push_back( buffer );
		}
	}

}; //end Game
