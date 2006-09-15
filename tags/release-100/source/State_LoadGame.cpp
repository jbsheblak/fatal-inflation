//---------------------------------------------------
// Name: Game : StateLoadGame
// Desc:  the game
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "State_LoadGame.h"

#include "Algorithms.h"

#include "FileIO.h"
#include "ResourceCache.h"
#include "GameConstants.h"
#include "GameXExt.h"
#include "MasterFile.h"



namespace Game
{
	void State_LoadGame::Enter()
	{
#if COMPILE_FILES

		// compile the arrow descriptions
		CompileEntityDesc();			
		
		// parse and compile all level files
		CompileLevelFiles();		

		// now pack all of the level files into 1 file
		PackFiles();
#endif

		// Import our pack file
		SPackFile.HardClearData();
		SPackFile.Import( kGamePackFile );

		//Clear our resource cache and then refill it
		ResCache.Flush();
		AddPackedImagesToCache();				
		AddAudioPackToCache();
	}

	void State_LoadGame::Handle()
	{
		// move on to the start screen
		SMachine.RequestStateChange( "StartScreen" );
	}

	void State_LoadGame::CleanFilename( char* cleaned, char* dirty )
	{
		char cleaned1[512];					
		jbsCommon::Algorithm::GetBaseFilenameFromPath( cleaned1, dirty );
		jbsCommon::Algorithm::RemoveFileExtension( cleaned, cleaned1 );
	}

	void State_LoadGame::CompileEntityDesc()
	{
		// load all arrow descriptions and export them to binary format		
		EntityDescMap map;
		if( EntityDescFile::Parse( "EntityDescriptions.txt", &map ) )
			EntityDescFile::Export( "EntityDescriptions.bin", &map );	
	}

	void State_LoadGame::CompileLevelFiles()
	{
		std::vector< std::string > levelFiles;		
		jbsCommon::Algorithm::EnumerateTypedFilesInFolder( ".", ".raw_level", levelFiles );
		
		std::vector< std::string >::iterator itr;
		char buffer[512];

		for( itr = levelFiles.begin(); itr != levelFiles.end(); ++itr )
		{
			LevelFile::LevelEntry entry;
			if( LevelFile::Parse( itr->c_str(), entry ) )
			{				
				jbsCommon::Algorithm::RemoveFileExtension( buffer, (char*)itr->c_str() );
				std::string newName = std::string(buffer) + std::string( ".bin_level" );
				
				LevelFile::Export( newName.c_str(), entry );
			}
		}
	}

	void State_LoadGame::PackFiles()
	{
		// Pack all of the level files into the pack file
		std::vector< std::string > levelFiles;		
		jbsCommon::Algorithm::EnumerateTypedFilesInFolder( ".", ".bin_level", levelFiles );

		PackFile::PackElementList packList;

		std::vector< std::string >::iterator itr;
		for( itr = levelFiles.begin(); itr != levelFiles.end(); ++itr )
		{
			LevelFile::LevelEntry entry;
			if( LevelFile::Import( itr->c_str(), entry ) )
			{
				// load the arrow set
				EntitySetFile::EntitySetList arrowSetList;
				if( EntitySetFile::Import( entry.mEntitySet.c_str(), arrowSetList ) )
				{
					PackFile::PackElement packEntitySet;	

					uint8_t* buffer;
					uint32_t bufferSize;

					bufferSize = jbsCommon::Algorithm::ReadFileIntoBuffer( entry.mEntitySet.c_str(), buffer );

					char cleanName[512];

					CleanFilename( cleanName, (char*)entry.mEntitySet.c_str() );					

					packEntitySet.mVersion = 1;
					packEntitySet.mSignature = ResourceCache::DJBHash( cleanName );
					packEntitySet.mSize = bufferSize;
					packEntitySet.mData = buffer;

					PackFile::PackElement packLevel;					

					bufferSize = jbsCommon::Algorithm::ReadFileIntoBuffer( itr->c_str(), buffer );

					CleanFilename( cleanName, (char*)itr->c_str() );					

					packLevel.mVersion   = 1;
					packLevel.mSignature = ResourceCache::DJBHash( cleanName );
					packLevel.mSize      = bufferSize;
					packLevel.mData      = buffer;

					//push the pack elements
					packList.push_back( packEntitySet );
					packList.push_back( packLevel );
				}
			}
		}

		// Pack the arrow description file into the pack file
		{
			uint8_t* pBuffer = NULL;
			uint32_t bufferSize = jbsCommon::Algorithm::ReadFileIntoBuffer( "EntityDescriptions.bin", pBuffer );

			PackFile::PackElement packEntityDesc;
			packEntityDesc.mSignature = ResourceCache::DJBHash( "EntityDescriptions" );
			packEntityDesc.mVersion	 = 1;
			packEntityDesc.mSize      = bufferSize;
			packEntityDesc.mData      = pBuffer;
			
			packList.push_back( packEntityDesc );
		}

		// Pack images
		PackFile::PackElement packImages = PackDirectory( "textures", "ImagePackFile.pak" );
		packList.push_back( packImages );

		// Pack audio
		PackFile::PackElement packMusic = PackDirectory( "audio", "MusicPackFile.pak", ".mp3" );
		PackFile::PackElement packSound = PackDirectory( "audio", "SoundPackFile.pak", ".wav" );
		packList.push_back( packMusic );	
		packList.push_back( packSound );

		// export pack file
		PackFile::Export( "gameData.pack", packList );

		// go back and delete all the buffers we allocated
		PackFile::PackElementList::iterator packItr;
		for( packItr = packList.begin(); packItr != packList.end(); ++packItr )
		{
			if( packItr->mData )
				delete [] packItr->mData;
		}
	}

	PackFile::PackElement State_LoadGame::PackImages()
	{
		return PackDirectory( "textures", "ImagePackFile.pak" );
	}

	PackFile::PackElement State_LoadGame::PackAudio()
	{
		return PackDirectory( "audio", "AudioPackFile.pak" );
	}

	PackFile::PackElement State_LoadGame::PackDirectory( const char* dir, 
														 const char* packName,
														 char* ext )
	{
		//Step1: Write all files to a single file

		// enumerate all the files in the folder
		std::vector< std::string > files;
		if( ext )
		{
			jbsCommon::Algorithm::EnumerateTypedFilesInFolder( dir, ext, files );
		}
		else
		{
			jbsCommon::Algorithm::EnumerateFilesInFolder( dir, files );
		}

		ImageFile::ImageEntryList entryList;

		// re-use the image entry list code
		std::vector< std::string >::iterator itr;
		for( itr = files.begin(); itr != files.end(); ++itr )
		{
			if( jbsCommon::Algorithm::FileExists( (char*)itr->c_str() ) )
			{
				uint8_t* buffer;
				uint32_t bufferSize = jbsCommon::Algorithm::ReadFileIntoBuffer( itr->c_str(), buffer );
				if( bufferSize > 0 )
				{
					// clean up the name
					char cleaned[512];
					jbsCommon::Algorithm::CleanFilePath( cleaned, (char*)itr->c_str() );					

					ImageFile::ImageEntry entry;
					entry.mImgNameHash = ResCache.DJBHash( cleaned );
					entry.mpData = buffer;
					entry.mSize = bufferSize;

					entryList.push_back(entry);
				}
			}
		}

		// export image list		
		ImageFile::Export( packName, entryList );

		// free all the allocated memory
		ImageFile::ImageEntryList::iterator ieItr;
		for( ieItr = entryList.begin(); ieItr != entryList.end(); ++ieItr )
		{
			if( ieItr->mpData )
				delete [] ieItr->mpData;
		}


		// Step 2: pack this image pack file

		uint8_t* pImageBuffer;
		uint32_t imageBufferSize = jbsCommon::Algorithm::ReadFileIntoBuffer( packName, pImageBuffer );
        
		char cleanName[512];
		jbsCommon::Algorithm::CleanFilePath( cleanName, (char*)packName );

		PackFile::PackElement packElem;
		packElem.mSignature = ResCache.DJBHash( cleanName );
		packElem.mVersion	= 1;
		packElem.mData      = pImageBuffer;
		packElem.mSize      = imageBufferSize;

		return packElem;
	}	
	
}; //end Game
