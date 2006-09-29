//---------------------------------------------------
// Name: Game : FileIO
// Desc:  handles reading / writing files
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "FileIO.h"
#include <stdio.h>
#include <map>
#include <cassert>

#include "Algorithms.h"
#include "ResourceCache.h"
#include "Ball.h"
#include "Beam.h"

namespace Game
{	

//------------------------------------------------------------------------------
// FileUtils
//------------------------------------------------------------------------------

	namespace FileUtils
	{
		void WriteString( const char* string, uint32_t length, FILE*& file )
		{
			//write string length then string
			fwrite( &length, sizeof(uint32_t), 1, file );
			fwrite( string, length, 1, file );
		}

		void WriteString( std::string str, FILE*& file )
		{
			WriteString( str.c_str(), (uint32_t)str.size(), file );
		}

		void WriteString( const char* string, uint32_t length, uint8_t*& stream )
		{
			memcpy( stream, &length, sizeof(uint32_t) );
			stream += sizeof(uint32_t);
			memcpy( stream, string, length );
			stream += length;
		}

		void WriteString( std::string str, uint8_t*& stream )
		{
			WriteString( str.c_str(), (uint32_t)str.size(), stream );
		}

		template <typename T>
		T Read( uint8_t*& stream )
		{
			T val = *(T*)stream;
			stream += sizeof(T);
			return val;
		}

		void ReadString( char* buffer, uint32_t bufferSize, uint8_t*& stream )
		{
			uint32_t size = Read<uint32_t>( stream );
			uint32_t read_size = bufferSize < size ? bufferSize : size;
			memcpy( buffer, stream, read_size );
			buffer[read_size] = '\0';
			stream += read_size;			
		}

		void ReadString( std::string& str, uint8_t*& stream )
		{			
			char buffer[512];
			ReadString( buffer, 512, stream );			
			str = std::string(buffer);			
		}

		bool GetNextLine( std::string& line, FILE*& file )
		{
			if( !file || feof(file) )
				return false;

			char szLine[512];
			fgets( szLine, 512, file );
			line = std::string(szLine);

			if( line[ line.size()-1 ] == 10 )
				line[ line.size()-1 ] = '\0';

			return true;
		}

		std::string GetWordFromString( uint32_t wordNum, char* str, char delim )
		{
			U32 idx = 0;
			U32 k   = 0;
			char res[256];
			strcpy( res, "" );

			while( idx < wordNum )
			{
				while( k < strlen(str) && str[k] != delim)	++k;
				if( k == strlen(str)) break;
				++idx;
				++k;
			}

			idx = 0;
			while( k < strlen(str) && str[k] != delim )
			{
				res[idx] = str[k];
				++idx;
				++k;
			}

			//cut off the crap
			if( res[idx-1] == 10 )	res[idx-1] = '\0';
			else					res[idx] = '\0';
			
			return std::string(res);
		}


	}; // end FileUtils

//------------------------------------------------------------------------------
// ParseUtils
//------------------------------------------------------------------------------
	namespace ParseUtils
	{
		enum DataType
		{
			kDT_int32,
			kDT_float,
			kDT_string
		};

		struct ParsePacket
		{
			std::string		mParseToken;	// the token we are looking for
			uint32_t		mDataTypeFlag;	// the type of data we want to get
			uint32_t		mDataAmt;		// the number of data items to gather

			uint32_t		mAssocFlag;		// flag associated with this parse
		};

		bool Parse( ParsePacket* pp, uint32_t packetCount, std::string parseStr, EntityProperty& prop )
		{
			uint8_t buffer[1024];

			std::string propType = FileUtils::GetWordFromString( 0, (char*)parseStr.c_str(), ' ' );

			// look through all of our parse packets for the token
			for( uint32_t i = 0; i < packetCount; ++i )
			{
				if( propType == pp[i].mParseToken )
				{
					uint32_t dataSize = 0;				
					uint8_t* dataStream = (uint8_t*)buffer;

					for( uint32_t j = 0; j < pp[i].mDataAmt; ++j )
					{
						std::string data = FileUtils::GetWordFromString( j+2, (char*)parseStr.c_str(), ' ' );

						switch( pp[i].mDataTypeFlag )
						{

						case kDT_int32:
							{
								int32_t val = (int32_t)atoi( data.c_str() );
								memcpy( dataStream, &val, sizeof(int32_t) );
								dataStream += sizeof(int32_t);
								dataSize += sizeof(int32_t);
								break;
							}

						case kDT_float:
							{
								F32 val = (F32)atof( data.c_str() );
								memcpy( dataStream, &val, sizeof(F32) );
								dataStream += sizeof(F32);
								dataSize += sizeof(F32);
								break;
							}

						case kDT_string:
							{
								uint32_t len = (uint32_t)data.size();
								memcpy( dataStream, data.c_str(), len );
								dataStream[len] = '\0';
								dataStream += len+1;
								dataSize += len+1;
								break;
							}
						}
					}

					if( dataSize > 0 )
					{
						prop.mDataSize = dataSize;
						prop.mData = new uint8_t[ dataSize ];
						memcpy( prop.mData, buffer, dataSize );
						prop.mFlag = pp[i].mAssocFlag;
						return true;
					}
					else
					{
						prop.mData = NULL;
						return false;
					}
				}
			}

			return false;
		}

	}; //end ParseUtils

//------------------------------------------------------------------------------
// EntitySetFile
//------------------------------------------------------------------------------
	namespace EntitySetFile
	{
		enum EntitySetFlags
		{
			kESF_EntityStringTable = 0x0001
		};

		struct EntitySetHeader
		{
			F32			 mVersion;
			uint32_t	 mFlags;
			uint32_t	 mReserved1;
			uint32_t	 mReserved2;
		};

		bool Export( const char* szFile, EntitySetList& entSet )
		{
			if( entSet.size() == 0 )
				return false;

			FILE* file = fopen(szFile, "w+b" );
			if( !file )
				return false;

			EntitySetHeader header;
			header.mVersion	= 1.0f;
			header.mFlags   = kESF_EntityStringTable;

			fwrite( &header, sizeof(EntitySetHeader), 1, file );

			//construct a string table
			std::vector< std::string > vStringTable;
			std::map< std::string, uint8_t > vStringMap;

			EntitySetList::iterator itr;
			for( itr = entSet.begin(); itr != entSet.end(); ++itr )
			{
				std::map< std::string, uint8_t>::iterator mapItr;
				if( (mapItr = vStringMap.find( itr->mEntityName )) == vStringMap.end() )
				{
					//add to map and table
					vStringTable.push_back( itr->mEntityName );
					vStringMap[ itr->mEntityName ] = (uint8_t)vStringTable.size()-1;
				}
			}

			//write the string table
			uint8_t tableSize = (uint8_t)vStringTable.size();
			fwrite( &tableSize, sizeof(uint8_t), 1, file );

			std::vector< std::string >::iterator stringItr;
			for( stringItr = vStringTable.begin(); stringItr != vStringTable.end(); ++stringItr )
			{
				uint32_t strLen = (uint32_t)stringItr->size();
				fwrite( &strLen, sizeof(uint32_t), 1, file );
				fwrite( stringItr->c_str(), strLen, 1, file );
			}

			//write the arrows
			uint32_t entCount = (uint32_t)entSet.size();
			fwrite( &entCount, sizeof(uint32_t), 1, file );

			for( itr = entSet.begin(); itr != entSet.end(); ++itr )
			{
				uint8_t stringTableIdx = vStringMap[ itr->mEntityName ];

				fwrite( &stringTableIdx, sizeof(uint8_t), 1, file );
				fwrite( &itr->mEntityTime, sizeof(F32), 1, file );
				fwrite( &itr->mEntityRotation, sizeof(F32), 1, file );
				fwrite( &itr->mStartX, sizeof(int32_t), 1, file );
				fwrite( &itr->mStartY, sizeof(int32_t), 1, file );				
			}

			fclose(file);
			return true;
		}

		// Import the EntitySet from a specified file
		bool Import( const char* szFile, EntitySetList& entSet )
		{	
			uint8_t* pBuffer;
			uint32_t size = jbsCommon::Algorithm::ReadFileIntoBuffer( szFile, pBuffer );
			if( size )
			{
				bool import = Import( pBuffer, size, entSet );
				delete [] pBuffer;
				return import;
			}
			
			return false;
		}

		// Import the arrow set from a given chunk of memory
		bool Import( uint8_t* stream, uint32_t streamSize, EntitySetList& entSet )
		{
			if( !stream || !streamSize )
				return false;

			EntitySetHeader* header = (EntitySetHeader*)stream;
			stream += sizeof(EntitySetHeader);			

			assert( header->mFlags & kESF_EntityStringTable && "No string table. Bad format" );    		

			// read in the string table
			std::vector< std::string > vStringTable;
            
			uint8_t tableSize = *(uint8_t*)stream;
			stream += sizeof(uint8_t);
			
			for( uint8_t i = 0; i < tableSize; ++i )
			{
				uint32_t stringSize = *(uint32_t*)stream;
				stream += sizeof(uint32_t);

				char buffer[512];

				memcpy( buffer, stream, stringSize );
				buffer[stringSize] = '\0';
				stream += stringSize;
				
				vStringTable.push_back( std::string(buffer) );
			}

			// read in the ents					
			uint32_t entCount = *(uint32_t*)stream;
			stream += sizeof(uint32_t);			

			for( uint32_t i = 0; i < entCount; ++i )
			{	
				uint8_t stringTableIdx = FileUtils::Read<uint8_t>( stream );

				EntitySetEntry entry;				
				entry.mEntityTime = FileUtils::Read<F32>( stream );
				entry.mEntityRotation = FileUtils::Read<F32>( stream );
				entry.mStartX  = FileUtils::Read<int32_t>( stream );
				entry.mStartY  = FileUtils::Read<int32_t>( stream );
				entry.mEntityName = vStringTable[ stringTableIdx ];	
				entry.mEntity     = NULL;

				entSet.push_back(entry);
			}				
			
			return true;
		}


	}; //end EntitySetFile

//////////////////////////////////////////////////////////////////////////////////////////

	// arrow description file that specifies properties for base arrow variants
	namespace EntityDescFile
	{
		struct EntityDescFileHeader
		{		
			F32			 mVersion;
			uint32_t	 mFlags;
			uint32_t	 mReserved1;
			uint32_t	 mReserved2;
		};

		bool Export( const char* szFile, EntityDescMap* descMap )
		{
			if( !szFile || !descMap || !descMap->size() )
				return false;

            FILE* file = fopen( szFile, "w+b" );
			if( !file )
				return false;

			EntityDescFileHeader header;
			header.mFlags = 0;
			header.mReserved1 = 0;
			header.mReserved2 = 0;
			header.mVersion = 1.0f;

			fwrite( &header, sizeof(EntityDescFileHeader), 1, file );

			uint32_t descCount = (uint32_t)descMap->size();
			fwrite( &descCount, sizeof(uint32_t), 1, file );

			EntityDescMap::iterator descItr;
			for( descItr = descMap->begin(); descItr != descMap->end(); ++descItr )
			{
				//write the arrow name
				FileUtils::WriteString( descItr->first, file );

				//write the arrow properties
				uint32_t propCount = (uint32_t)descItr->second.size();
				fwrite( &propCount, sizeof(uint32_t), 1, file );

				EntityDesc::iterator itr;
				for( itr = descItr->second.begin(); itr != descItr->second.end(); ++itr )
				{				
					uint32_t flag = (uint32_t)itr->mFlag;
					uint32_t size = (uint32_t)itr->mDataSize;

					fwrite( &flag, sizeof(uint32_t), 1, file );
					fwrite( &size, sizeof(uint32_t), 1, file );
					fwrite( itr->mData, size, 1, file );
				}
			}
			
			fclose(file);
			return true;
		}		
	
		// import the arrow desc from a file
		bool Import( const char* szFile, EntityDescMap* descMap )
		{	
			uint8_t* pBuffer;
			uint32_t size = jbsCommon::Algorithm::ReadFileIntoBuffer( szFile, pBuffer );
			if( size )
			{
				bool import = Import( pBuffer, size, descMap );
				delete [] pBuffer;
				return import;
			}
			
			return false;
		}

		bool Import( uint8_t* stream, uint32_t streamSize, EntityDescMap* descMap )
		{
			if( !stream || !streamSize || !descMap )
				return false;

			EntityDescFileHeader* header;
			header = (EntityDescFileHeader*)stream;
			stream += sizeof(EntityDescFileHeader);			

			uint32_t descCount = FileUtils::Read<uint32_t>( stream );
			for( uint32_t d = 0; d < descCount; ++d )
			{
				//read the arrow name
				std::string name;
				FileUtils::ReadString( name, stream );

				//read the arrow properties
				uint32_t propCount = FileUtils::Read<uint32_t>( stream );

				EntityDesc arrowDesc;
				for( uint32_t i = 0; i < propCount; ++i )
				{
					EntityProperty prop;				
					prop.mFlag			= FileUtils::Read<uint32_t>( stream );
					prop.mDataSize		= FileUtils::Read<uint32_t>( stream );
					prop.mData = new uint8_t[ prop.mDataSize ];
					memcpy( prop.mData, stream, prop.mDataSize );
					stream += prop.mDataSize;

					arrowDesc.push_back(prop);
				}

				(*descMap)[ name ] = arrowDesc;				
			}
			return true;
		}

		bool ParseProperty( EntityProperty& prop, std::string propString )
		{
			//parsing setup
			static ParseUtils::ParsePacket parsePackets [] = 
			{  
				{ std::string("BaseEntity"), ParseUtils::kDT_string, 1, kEntProp_BaseEntity },
				{ std::string("Position"),   ParseUtils::kDT_int32,  2, kArrowProp_StartPosition },
				{ std::string("Velocity"),   ParseUtils::kDT_int32,  2, kArrowProp_StartVelocity },
				{ std::string("Alpha"),		 ParseUtils::kDT_float,  1, kArrowProp_StartAlpha },
				{ std::string("Lifetime"),   ParseUtils::kDT_float,  1, kArrowProp_Lifetime },
				{ std::string("GenerationSound"), ParseUtils::kDT_string, 1, kArrowProp_GenSound },
				{ std::string("RotationSpeed"),   ParseUtils::kDT_float,  1, kBallProp_Rotation },
				{ std::string("FireFrequency"),   ParseUtils::kDT_float,  1, kBeamProp_FireFrequency },
				{ std::string("FireDuration"),    ParseUtils::kDT_float,  1, kBeamProp_FireDuration }
			};

			static uint32_t parsePacketCount = sizeof(parsePackets) / sizeof(ParseUtils::ParsePacket);

			return ParseUtils::Parse( parsePackets, parsePacketCount, propString, prop );
		}

		bool Parse( const char* szFile, EntityDescMap* descMap )
		{
			FILE* file = fopen( szFile, "r+t" );
			if( !file )
				return false;

			std::string arrowName;
			bool parsingEntity = false;
			
			EntityDesc      arrowDesc;
			EntityProperty  arrowProp;

			while( true  )
			{
				std::string line;
				if( !FileUtils::GetNextLine(line, file) )
					break;
				
				if( line.find("#Begin") != std::string::npos )
				{
					arrowDesc.clear();
					parsingEntity = true;
					arrowName = FileUtils::GetWordFromString( 1, (char*)line.c_str(), ' ' );
				}
				else if( parsingEntity )
				{
					if( line.find("#End") != std::string::npos )
					{						
						parsingEntity = false;
						(*descMap)[arrowName] = arrowDesc;						
					}
					else
					{
						if( ParseProperty( arrowProp, line ) )
							arrowDesc.push_back( arrowProp );
					}
				}
			}

			return true;
		}
	};


//////////////////////////////////////////////////////////////////////////////////////////

	// a level file containing an arrow set and other features
	namespace LevelFile
	{
		struct LevelFileHeader
		{		
			F32			 mVersion;
			uint32_t	 mFlags;
			uint32_t	 mReserved1;
			uint32_t	 mReserved2;
		};

		bool Export( const char* szFile, const LevelEntry& entry )
		{
			FILE* file = fopen( szFile, "w+b" );
			if( !file )
				return false;

			LevelFileHeader header;
			header.mFlags = 0;
			header.mReserved1 = 0;
			header.mReserved2 = 0;
			header.mVersion = 1.0f;
            
			fwrite( &header, sizeof(LevelFileHeader), 1, file );
			FileUtils::WriteString( entry.mName, file );
			FileUtils::WriteString( entry.mBackground, file );
			FileUtils::WriteString( entry.mMusic, file );
			FileUtils::WriteString( entry.mEntitySet, file );
			fwrite( &entry.mTimeLength, sizeof(F32), 1, file );

			fclose(file);
			return true;
		}


		bool Import( const char* szFile, LevelEntry& entry )
		{
			FILE* file = fopen( szFile, "r+b" );
			if( !file )
				return false;			

			fseek( file, 0, SEEK_END );
			uint32_t bufferSize = (uint32_t)ftell(file);
			rewind(file);

			uint8_t* pBuffer = new uint8_t[bufferSize];
			fread( pBuffer, bufferSize, 1, file );

			bool import = Import( pBuffer, bufferSize, entry );
			
			delete [] pBuffer;
			return import;
		}

		bool Import( uint8_t* stream, uint32_t streamSize, LevelEntry& entry )
		{
			if( !stream || !streamSize )
				return false;

			LevelFileHeader* header = (LevelFileHeader*)stream;
			stream += sizeof(LevelFileHeader);

			FileUtils::ReadString( entry.mName, stream );
			FileUtils::ReadString( entry.mBackground, stream );
			FileUtils::ReadString( entry.mMusic, stream );
			FileUtils::ReadString( entry.mEntitySet, stream );
			entry.mTimeLength = FileUtils::Read<F32>(stream);
			return true;
		}

		bool Parse( const char* szFile, LevelEntry& entry )
		{		
			FILE* file = fopen( szFile, "r+t" );			
			if( !file )
				return false;			

			bool parsingLevel = false;

			while( true  )
			{
				std::string line;
				if( !FileUtils::GetNextLine(line, file) )
					break;
				
				if( line.find("#Begin") != std::string::npos )
				{
					entry.mName = FileUtils::GetWordFromString( 1, (char*)line.c_str(), ' ' );
					parsingLevel = true;					
				}
				else if( parsingLevel )
				{
					if( line.find("#End") != std::string::npos )
					{				
						//Currently we only parse 1 level per file
						parsingLevel = false;
						return true;
					}
					else
					{
						std::string propType = FileUtils::GetWordFromString( 0, (char*)line.c_str(), ' ' );						

						if( !strcmp( propType.c_str(), "Background" ) )
						{
							entry.mBackground = FileUtils::GetWordFromString( 2, (char*)line.c_str(), ' ' );							
						}						

						else if( !strcmp( propType.c_str(), "Music" ) )
						{
							entry.mMusic = FileUtils::GetWordFromString( 2, (char*)line.c_str(), ' ' );
						}

						else if( !strcmp( propType.c_str(), "ArrowSet" ) )
						{
							entry.mEntitySet = FileUtils::GetWordFromString( 2, (char*)line.c_str(), ' ' );
						}						

						else if( !strcmp( propType.c_str(), "TimeLength" ) )
						{							
							entry.mTimeLength = (F32)atof( FileUtils::GetWordFromString( 2, (char*)line.c_str(), ' ' ).c_str() ); 							
						}						
					}
				}
			}

			return true;
		}


	}; // end LevelFile


//////////////////////////////////////////////////////////////////////////////////////////



	namespace PackFile
	{
		struct PackElementHeader
		{
			uint32_t         mSignature;
			uint32_t         mVersion;
			uint32_t         mOffset;
			uint32_t         mSize;
		};

		struct PackHeader
		{
			uint32_t           mVersion;
			uint32_t           mNumElements;
			uint32_t		   mFlags;			
		};

		// export information into a packed file
		bool Export( const char* szFile, PackElementList& list )
		{
			FILE* file = fopen( szFile, "w+b" );
			if( !file )
				return false;

			uint32_t version = 1;
			uint32_t numElements = (uint32_t)list.size();
			uint32_t flags = 0;

			// write the pack header
			fwrite( &version, sizeof(uint32_t), 1, file );
			fwrite( &numElements, sizeof(uint32_t), 1, file );
			fwrite( &flags, sizeof(uint32_t), 1, file );

			uint32_t offset = sizeof(uint32_t)*3 + sizeof(PackElementHeader) * numElements;

			// write the pack element headers
			PackElementList::iterator itr;
			for( itr = list.begin(); itr != list.end(); ++itr )
			{
				PackElementHeader peh;
				peh.mOffset			= offset;
				peh.mSignature		= itr->mSignature;
				peh.mSize			= itr->mSize;
				peh.mVersion		= itr->mVersion;
				fwrite( &peh, sizeof(PackElementHeader), 1, file );

				offset += itr->mSize;
			}

			//write the pack data
			for( itr = list.begin(); itr != list.end(); ++itr )
			{
				fwrite( itr->mData, itr->mSize, 1, file );
			}

			fclose(file);
			return true;
		}

		// Import packed information from a file
		bool Import( const char* szFile, PackElementList& list )
		{
			if( !szFile )
				return false;

			FILE* file = fopen(szFile, "r+b" );
			if( !file )
				return false;

			fseek( file, 0, SEEK_END );
			uint32_t bufferSize = (uint32_t)ftell(file);
			rewind(file);

			uint8_t* pBuffer = new uint8_t[bufferSize];
			fread( pBuffer, bufferSize, 1, file );

			bool import = Import( pBuffer, bufferSize, list );
			
			delete [] pBuffer;
			return import;
		}

		// Import packed information from a memory stream
		bool Import( uint8_t* stream, uint32_t streamSize, PackElementList& list )
		{
			if( !stream || !streamSize )
				return false;

			// read the pack header
			PackHeader* pHeader = (PackHeader*)stream;			
			assert( pHeader->mFlags == 0 );

			PackElementHeader* elements = (PackElementHeader*)(stream + sizeof(PackHeader));

			// read in the pack elements
			for( uint32_t i = 0; i < pHeader->mNumElements; ++i )
			{
				PackElement packElem;
				packElem.mSignature = elements[i].mSignature;
				packElem.mVersion   = elements[i].mVersion;
				packElem.mSize      = elements[i].mSize;

				packElem.mData = new uint8_t[ packElem.mSize ];
				memcpy( packElem.mData, stream + elements[i].mOffset, packElem.mSize );

				list.push_back(packElem);
			}

			return true;
		}

		bool PackFileManager::Import( const char* szFile )
		{
			return PackFile::Import( szFile, mPackList );
		}

		bool PackFileManager::GetPackElement( const char* signature, PackElement& elem )
		{
			return GetPackElement( ResourceCache::DJBHash(signature), elem );
		}

		bool PackFileManager::GetPackElement( uint32_t sigHash, PackElement& elem )
		{
			for( uint32_t i = 0; i < mPackList.size(); ++i )
			{
				if( mPackList[i].mSignature == sigHash )
				{						
					elem = mPackList[i];
					return true;
				}
			}

			return false;
		}

		void PackFileManager::HardClearData()
		{
			PackElementList::iterator itr;
			for( itr = mPackList.begin(); itr != mPackList.end(); ++itr )
			{
				if( itr->mData )
				{
					delete [] itr->mData;
				}
			}

			mPackList.clear();
		}

		PackFileManager* PackFileManager::GetPFM()
		{
			static PackFileManager* pPFM = new PackFileManager;
			return pPFM;
		}
			
	}; //end PackFile

	namespace ImageFile
	{
		struct ImageFileEntry
		{
			//std::string mImageName;
			uint32_t	mNameHash;
			uint32_t	mOffset;
			uint32_t	mSize;
		};

		struct ImageFileHeader
		{
			uint32_t	mNumEntries;
			uint32_t	mFlags;
		};

		bool Export( const char* szFile, ImageEntryList& list )
		{
			if( !szFile || list.size() <= 0 ) 
				return false;

			FILE* file = fopen( szFile, "w+b" );
			if( !file )
				return false;

			ImageFileHeader header;
			header.mNumEntries = (uint32_t)list.size();
			header.mFlags      = 0;

			fwrite( &header, sizeof(ImageFileHeader), 1, file );

			std::vector< ImageFileEntry > imageFileEntries;

			int32_t offset = (int32_t)(sizeof(ImageFileHeader) + sizeof(ImageFileEntry) * list.size());

			// write ImageFileEntries
			ImageEntryList::iterator entryItr;
			for( entryItr = list.begin(); entryItr != list.end(); ++entryItr )
			{
				ImageFileEntry e;
				//e.mImageName = entryItr->mImageName;
				e.mNameHash  = entryItr->mImgNameHash;
				e.mSize      = entryItr->mSize;
				e.mOffset    = offset;
				
				offset += entryItr->mSize;

				fwrite( &e, sizeof(ImageFileEntry), 1, file );
			}

			// write ImageFile Data
			for( entryItr = list.begin(); entryItr != list.end(); ++entryItr )
			{
				fwrite( entryItr->mpData, entryItr->mSize, 1, file );
			}

			fclose(file);
			return true;
		}	

		bool Import( uint8_t* stream, uint32_t streamSize, ImageEntryList& list )
		{
			if( !stream || !streamSize )
				return false;

			uint8_t* pStreamStart = stream;			

			ImageFileHeader* header;
			header = (ImageFileHeader*)stream;
			stream += sizeof(ImageFileHeader);			

			ImageFileEntry* entries = (ImageFileEntry*)stream;			

			for( uint32_t i = 0; i < header->mNumEntries; ++i )
			{
				ImageEntry e;
				e.mImgNameHash = entries[i].mNameHash;
				e.mSize		   = entries[i].mSize;
				e.mpData = new uint8_t[ e.mSize ];
				memcpy( e.mpData, pStreamStart + entries[i].mOffset, e.mSize );	

				list.push_back(e);
			}
			
			return true;
		}

	}; // end ImageFile

	namespace GameSaveFile
	{
		struct Header
		{
			float		mVersion;
			uint32_t	mFlags;
			uint32_t	mReserved1;
			uint32_t	mReserved2;
		};

		bool Export( const char* szFile, const SaveFile& saveFile )
		{
			FILE* file = fopen(szFile, "w+b" );
			if( !file )
				return false;

			Header h;
			h.mVersion = 1.0f;
			h.mFlags   = 0;
			h.mReserved1 = 0;
			h.mReserved2 = 0;

			fwrite( &h, sizeof(Header), 1, file );
			fwrite( &saveFile, sizeof(SaveFile), 1, file );

			fclose(file);
			return true;
		}

		bool Import( const char* szFile, SaveFile& saveFile )
		{
			uint32_t size;
			uint8_t* buffer = NULL;
			size = jbsCommon::Algorithm::ReadFileIntoBuffer( szFile, buffer );
			if( !buffer )
				return false;

			bool import = Import( buffer, size, saveFile );
			delete [] buffer;
			return import;
		}
			
		bool Import( uint8_t* stream, uint32_t streamSize, SaveFile& saveFile )
		{
			if( !stream )
				return false;

			Header* pHeader = (Header*)stream;
			stream += sizeof(Header);

			saveFile.mCompletedLevels = FileUtils::Read<uint32_t>(stream);
			return true;
		}

	}; //end GameSaveFile
	
}; //end Game
