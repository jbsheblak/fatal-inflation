//---------------------------------------------------
// Name: Game : GameXExt
// Desc:  GameX Extensions
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_GAMEX_EXT_H_
#define _GAME_GAMEX_EXT_H_

#include "Types.h"
#include "gamex.hpp"

namespace Game
{	
	//-----------------------------------------------------------
	// Name: LoadX
	// Desc:  Loads various file formats
	//-----------------------------------------------------------
	bool LoadTGA( ImageX* image, char* szFile );
	bool LoadTGA( ImageX* image, uint8_t* stream, uint32_t streamSize );
	bool Load_MP3( MusicX* music, uint8_t* stream, uint32_t streamSize );
	bool LoadWAV( SoundX* sound, uint8_t* stream, uint32_t streamSize );

	//-----------------------------------------------------------
	// Name: GetX
	// Desc:  utilities to grab resources from cache
	//-----------------------------------------------------------
	ImageX* GetImage( const char* fileName );	
	SoundX* GetSound( const char* fileName );
	MusicX* GetMusic( const char* fileName );		

	//-----------------------------------------------------------
	// Name: AddAudioPackToCache
	// Desc:  adds all audio to cache
	//-----------------------------------------------------------
	void AddAudioPackToCache();

	// add all images from pak file into cache
	void AddPackedImagesToCache();
	
}; //end Game

#endif // end _GAME_GAMEX_EXT_H_
    
