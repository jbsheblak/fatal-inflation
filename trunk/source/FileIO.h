//---------------------------------------------------
// Name: Game : FileIO
// Desc:  handles reading / writing files
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_FILE_IO_H_
#define _GAME_FILE_IO_H_

#include "Types.h"

#include <string>
#include <vector>
#include <map>

#include "Arrow.h"

namespace Game
{	
	class Arrow;	

	// A file containing a set of timed entity placements
	namespace EntitySetFile
	{
		struct EntitySetEntry
		{
			std::string			mEntityName;  // name of entity (ie: RedArrow, BlueBeam)
			F32					mEntityTime;  // spawn time for entity
			F32					mEntityRotation; // spawn rotation
			int32_t				mStartX;	  // starting positions
			int32_t				mStartY;

			Entity*				mEntity;
		};

		typedef std::vector<EntitySetEntry> EntitySetList;		

		bool Export( const char* szFile, EntitySetList& entSet );
		bool Import( const char* szFile, EntitySetList& entSet );
		bool Import( uint8_t* stream, uint32_t streamSize, EntitySetList& entSet );

	}; //end ArrowSetFile

	// entity description file that specifies properties for base entity variants
	namespace EntityDescFile
	{
		bool Export( const char* szFile, EntityDescMap* descMap );
		bool Import( const char* szFile, EntityDescMap* descMap );
		bool Import( uint8_t* stream, uint32_t streamSize, EntityDescMap* descMap );

		bool Parse( const char* szFile, EntityDescMap* descMap );
	};

	// a level file containing an arrow set and other features
	namespace LevelFile
	{
		struct LevelEntry
		{
			std::string				mName;			
			std::string				mBackground;
			std::string				mMusic;
			std::string				mEntitySet;
			F32						mTimeLength;			
		};

		bool Export( const char* szFile, const LevelEntry& entry );
		bool Import( const char* szFile, LevelEntry& entry );
		bool Import( uint8_t* stream, uint32_t streamSize, LevelEntry& entry );

		bool Parse( const char* szFile, LevelEntry& entry );

	}; // end LevelFile

	// a packed file containing multiple buffers of data
	namespace PackFile
	{
		struct PackElement
		{
			uint32_t		 mSignature;
			uint32_t		 mVersion;
			uint32_t		 mSize;
			void*			 mData;
		};

		typedef std::vector<PackElement> PackElementList;

		bool Export( const char* szFile, PackElementList& list );
		bool Import( const char* szFile, PackElementList& list );
		bool Import( uint8_t* stream, uint32_t streamSize, PackElementList& list );

		class PackFileManager
		{
		public:

			bool Import( const char* szFile );

			bool GetPackElement( const char* signature, PackElement& elem );
			bool GetPackElement( uint32_t sigHash, PackElement& elem );

			void HardClearData();

			static PackFileManager* GetPFM();

		private:

			PackElementList			mPackList;
		};
	};	

#define SPackFile (*PackFile::PackFileManager::GetPFM())

	// a file for packing image files
	namespace ImageFile
	{
		struct ImageEntry
		{
			//std::string mImageName;
			uint32_t	mImgNameHash;
			uint32_t	mSize;
			void*		mpData;
		};		

		typedef std::vector< ImageEntry > ImageEntryList;

		bool Export( const char* szFile, ImageEntryList& list );
		bool Import( uint8_t* stream, uint32_t streamSize, ImageEntryList& list );
	};
	
}; //end Game

#endif // end _GAME_FILE_IO_H_
    
