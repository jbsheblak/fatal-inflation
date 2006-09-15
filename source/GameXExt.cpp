//---------------------------------------------------
// Name: Game : GameXExt
// Desc:  GameX Extensions
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include <stdio.h>
#include "Algorithms.h"
#include "ResourceCache.h"
#include "FileIO.h"

#include "GameXExt.h"

namespace Game
{
	//----------------------------------------------------
	// Name: LoadTGAData
	// Desc:  loads image data from a targa file
	//----------------------------------------------------
	bool LoadTGAData( uint8_t* inStream, uint32_t inStreamSize, uint8_t*& outStream, uint32_t& width, uint32_t& height, uint32_t& channels )
	{
		//code adapted from http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=24
		uint8_t TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
		uint8_t TGAcompare[12];							// Used To Compare TGA Header
		uint8_t header[6];									// First 6 Useful Bytes From The Header
		uint8_t temp;										// Temporary byte variable
		U32 bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
		U32 imageSize;									// Used To Store The Image Size When Setting Aside Ram	
		U32 bpp;
		uint8_t *imageData = NULL;

		if( !inStream || !inStreamSize )
			return false;

		memcpy( TGAcompare, inStream, sizeof(TGAcompare) ); 		
		inStream += sizeof(TGAcompare);		

		memcpy( TGAheader, TGAcompare, sizeof(TGAheader) );

		memcpy( header, inStream, sizeof(header) );		
		inStream += sizeof(header);		
		
		//determine width, height, and bpp
		width  = header[1] * 256 + header[0];			
		height = header[3] * 256 + header[2];			
		bpp	= header[4];								

 		if(	width <=0 || height	<=0	|| (bpp!=24 && bpp!=32))						
		{				
			return false;									
		}

		bytesPerPixel	= bpp/8;							// Divide By 8 To Get The Bytes Per Pixel
		imageSize		= width*height*bytesPerPixel;		// Calculate The Memory Required For The TGA Data
		channels		= bytesPerPixel;

		//setup stream
		outStream = new uint8_t[imageSize];
		if( !outStream )
			return false;
		
		imageData = (uint8_t*)outStream;	

		memcpy( imageData, inStream, imageSize );		

		//swap the 'r' and 'b' values
		for(U32 i=0; i<imageSize; i+=bytesPerPixel)			
		{													
			temp=imageData[i];								
			imageData[i] = imageData[i + 2];				
			imageData[i + 2] = temp;						
		}		

		return true;	
	}

	//----------------------------------------------------
	// Name: LoadTGAData
	// Desc:  loads image data from a targa file
	//----------------------------------------------------
	bool LoadTGAData( const char* szFile, uint8_t*& stream, uint32_t& width, uint32_t& height, uint32_t& channels )
	{
		uint8_t* buffer;
		uint32_t bufferSize = jbsCommon::Algorithm::ReadFileIntoBuffer( szFile, buffer );
		bool import = LoadTGAData( buffer, bufferSize, stream, width, height, channels );	
		delete [] buffer;
		return import;		
	}	

	//----------------------------------------------------
	// Name: LoadTGA
	// Desc:  loads TGA data into an ImageX from file
	//----------------------------------------------------
	bool LoadTGA( ImageX* image, char* szFile )
	{
		if( !image || !szFile )
			return false;		

		uint8_t* stream;
		uint32_t width, height, channels;

		if( LoadTGAData( szFile, stream, width, height, channels ) )
		{
			image->Create( width, height, channels > 3 );

			uint8_t* pixel = stream;			
			for( uint32_t i = 0; i < height; ++i )
			{
				for( uint32_t j = 0; j < width; ++j )
				{
					image->SetPixel( j, height-i, pixel[0], pixel[1], pixel[2], channels == 4 ? pixel[3] : 255 );
					pixel += channels;
				}
			}

			delete [] stream;
			return true;
		}

		return false;
	}

	//----------------------------------------------------
	// Name: LoadTGA
	// Desc:  loads TGA data into an ImageX from stream
	//----------------------------------------------------
	bool LoadTGA( ImageX* image, uint8_t* stream, uint32_t streamSize )
	{
		if( !image || !stream || !streamSize )		
			return false;

		uint8_t* dataStream;
		uint32_t width, height, channels;

		if( LoadTGAData( stream, streamSize, dataStream, width, height, channels ) )		
		{
			image->Create( width, height, channels > 3 );

			uint8_t* pixel = dataStream;			
			for( uint32_t i = 0; i < height; ++i )
			{
				for( uint32_t j = 0; j < width; ++j )
				{
					image->SetPixel( j, height-i, pixel[0], pixel[1], pixel[2], channels == 4 ? pixel[3] : 255 );
					pixel += channels;
				}
			}

			delete [] dataStream;
			return true;
		}		

		return false;
	}

	//----------------------------------------------------
	// Name: LoadMp3
	// Desc:  loads an mp3 from a stream
	//----------------------------------------------------
	bool Load_MP3( MusicX* music, uint8_t* stream, uint32_t streamSize )
	{
		// write data to file...
		char filename[256];
		//sprintf( filename, "t45063.mp3" );
		sprintf( filename, "test.mp3" );

		
		FILE* file = fopen( filename, "w+b" );
		if( !file )
			return false;

		fwrite( stream, streamSize, 1, file );
		fclose(file);

		//load from file

		if( !music->Load( filename ) )
			return false;

		//delete file
		DeleteFile( filename );
		return true;
	}

	//----------------------------------------------------
	// Name: LoadWAV
	// Desc:  loads a wav from a stream
	//----------------------------------------------------
	bool LoadWAV( SoundX* sound, uint8_t* stream, uint32_t streamSize )
	{
		// write data to file...
		char filename[256];
		sprintf( filename, "t45064.wav" );

		FILE* file = fopen( filename, "w+b" );
		if( !file )
			return false;

		fwrite( stream, streamSize, 1, file );
		fclose(file);

		//load from file
		if( !sound->Load( filename ) )
			return false;

		// for some reason the arrow sound needs to play once
		// or the data does not get initialized...							
		GameX.PlaySound( sound, PLAY_REWIND, 0.0f, 0, 1.0f );

		//delete file
		DeleteFile( filename );
		return true;
	}
	
	//----------------------------------------------------
	// Name: ResourceTypes
	// Desc:  different types of res that can be in cache
	//----------------------------------------------------
	enum ResourceTypes
	{
		kResType_Music,
		kResType_Sound,
		kResType_Image
	};

	//----------------------------------------------------
	// Name: GetResource
	// Desc:  Grabs a resource of type from cache
	//----------------------------------------------------
	void* GetResource( const char* fileName, uint32_t resType )
	{		
		static char cleaned[512];
		jbsCommon::Algorithm::CleanFilePath( cleaned, (char*)fileName );

		Resource* pRes = ResCache.GetResource( cleaned, resType );
		if( !pRes )
			return NULL;

		return pRes->GetResData();		
	}

	//----------------------------------------------------
	// Name: GetImage
	// Desc:  gets an image from the cache
	//----------------------------------------------------
	ImageX* GetImage( const char* fileName )
	{
		return (ImageX*)GetResource( fileName, kResType_Image );
	}

	//----------------------------------------------------
	// Name: GetSound
	// Desc:  gets a sound from the cache
	//----------------------------------------------------
	SoundX* GetSound( const char* fileName )
	{
		return (SoundX*)GetResource( fileName, kResType_Sound );
	}	

	//----------------------------------------------------
	// Name: GetMusic
	// Desc:  gets music from the cache
	//----------------------------------------------------
	MusicX* GetMusic( const char* fileName )
	{
		return (MusicX*)GetResource( fileName, kResType_Music );
	}

	//----------------------------------------------------
	// Name: AddAudioPackToCache
	// Desc:  add all of our audio to the cache
	//----------------------------------------------------
	void AddAudioPackToCache()
	{
		PackFile::PackElement packFile;

		// import mp3s
		if( SPackFile.GetPackElement( "MusicPackFile", packFile ) )
		{
			ImageFile::ImageEntryList list;
			if( ImageFile::Import( (uint8_t*)packFile.mData, packFile.mSize, list ) )
			{
				// add each image to the cache
				ImageFile::ImageEntryList::iterator itr;
				for( itr = list.begin(); itr != list.end(); ++itr )
				{		
					/*
					// generate our music structure	
					MusicX music;

					// attempt to load the mp3 music file
					if( Load_MP3( &music, (uint8_t*)itr->mpData, itr->mSize ) )
					{
						ResCache.AddRes( new TypedResource<MusicX>( kResType_Music,
																	itr->mImgNameHash,
																	&music ) );						
					}
					else
					{
						//delete music;
					}						
*/
					// delete imported memory
					if( itr->mpData )
						delete [] itr->mpData;
				}				
			}
		}

		// import wavs
		if( SPackFile.GetPackElement( "SoundPackFile", packFile ) )
		{
			ImageFile::ImageEntryList list;
			if( ImageFile::Import( (uint8_t*)packFile.mData, packFile.mSize, list ) )
			{
				// add each image to the cache
				ImageFile::ImageEntryList::iterator itr;
				for( itr = list.begin(); itr != list.end(); ++itr )
				{					
					SoundX* sound = new SoundX;

					if( LoadWAV( sound, (uint8_t*)itr->mpData, itr->mSize ) )
					{
						ResCache.AddRes( new TypedResource< SoundX >( kResType_Sound,
																	  itr->mImgNameHash,
																	  sound ) );						
					}
					else
					{
						delete sound;
					}							

					// delete imported memory
					if( itr->mpData )
						delete [] itr->mpData;
				}				
			}
		}
	}

	//-----------------------------------------------------------
	// Name: AddPackedImagesToCache
	// Desc:  add arrow images to cache based on arrow name
	//-----------------------------------------------------------
	void AddPackedImagesToCache()
	{
		PackFile::PackElement packImageFile;
		if( SPackFile.GetPackElement( "ImagePackFile", packImageFile ) )
		{
			ImageFile::ImageEntryList list;
			if( ImageFile::Import( (uint8_t*)packImageFile.mData, packImageFile.mSize, list ) )
			{
				// add each image to the cache
				ImageFile::ImageEntryList::iterator itr;
				for( itr = list.begin(); itr != list.end(); ++itr )
				{
					ImageX* image = new ImageX;					
					
					if( LoadTGA( image, (uint8_t*)itr->mpData, itr->mSize ) )
					{
						ResCache.AddRes( new TypedResource< ImageX >( kResType_Image,
																      itr->mImgNameHash,
																	  image ) );
					}
					else
					{
						delete image;
					}					

					// delete imported memory
					if( itr->mpData )
						delete [] itr->mpData;
				}				
			}
		}
	}
	
}; //end Game

    
