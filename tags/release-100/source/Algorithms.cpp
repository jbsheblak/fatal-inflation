//---------------------------------------------------
// Name: jbsCommon : Algorithms
// Desc:  miscellaneous algorithms
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Algorithms.h"

#include <windows.h>
#include <stdio.h>

namespace jbsCommon
{

namespace Algorithm
{

// Copy memory over in a  strided fasion
void CopyStrided( uint8_t* pDest, uint32_t destOffset, uint32_t destStride,
	              uint8_t* pSrc,  uint32_t srcOffset,  uint32_t srcStride, 
				  uint32_t elemSize, uint32_t elemCount )
{
	for( uint32_t i = 0; i < elemCount; ++i )
	{
		memcpy( pDest + destOffset + destStride * i, pSrc + srcOffset + srcStride * i, elemSize );
	}
}


// enumerate all files in a directory
bool EnumerateFilesInFolder( const char* dir, std::vector< std::string >& retFiles )
{	
	char oldDir[512];
	char fullNameDir[512];	
	GetCurrentDirectory( 512, oldDir  );	
	strcat( oldDir, "\\" );
	strcat( oldDir, dir );
	strcat( oldDir, "\\*");	

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
  
	hFind = FindFirstFile(oldDir, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return false;
	} 
	else 
	{
		retFiles.push_back( FindFileData.cFileName );
		while( FindNextFile( hFind, &FindFileData ) )
		{
			strcpy( fullNameDir, dir );
			strcat( fullNameDir, "\\" );
			strcat( fullNameDir, FindFileData.cFileName );
			retFiles.push_back( fullNameDir );
		}
		
		FindClose(hFind);
		return true;
	}
}

bool FilterByKeyword( const char* keyword, std::vector< std::string >& srcStrings, std::vector< std::string >& dstStrings )
{
	std::vector< std::string >::iterator itr;
	for( itr = srcStrings.begin(); itr != srcStrings.end(); ++itr )
	{
		if( itr->find(keyword) != std::string::npos )
		{
			dstStrings.push_back( *itr );
		}
	}

	return dstStrings.size() > 0;
}

// enumerate all files in a directory
bool EnumerateTypedFilesInFolder( const char* dir, const char* type, std::vector< std::string >& retFiles )
{
	std::vector< std::string > allFiles;
	EnumerateFilesInFolder( dir, allFiles );
	return FilterByKeyword( type, allFiles, retFiles );
}

void GetBaseFilenameFromPath( char* base, char* path )
{
	uint16_t len   = (uint16_t)strlen(path);
	uint16_t start = len;
	while( start > 0 && path[start] != '\\' && path[start] != '/' )
		--start;

	if( start > 0 )
		++start;

	memcpy( base, path + start, sizeof(char) * len-start );
	base[len-start] = '\0';
}

void RemoveFileExtension( char* cleaned, char* file_with_ext )
{
	uint16_t len   = (uint16_t)strlen(file_with_ext);
	uint16_t end = len;
	while( end > 0 && file_with_ext[end] != '.' )
		--end;	

	if( end != 0 )
	{
		memcpy( cleaned, file_with_ext, sizeof(char) * end );
		cleaned[end] = '\0';
	}
	else
	{
		strcpy( cleaned, file_with_ext );
	}
}

void GetFileExtension( char* extension, char* file_with_ext )
{
	uint16_t len = (uint16_t)strlen(file_with_ext);
	uint16_t start = len;
	while( start > 0 && file_with_ext[start] != '.' )
		--start;

	if( start > 0 )
		++start;

	memcpy( extension, file_with_ext + start, sizeof(char) * len-start );	
	extension[len-start] = '\0';
}

void CleanFilePath( char* cleaned, char* path )
{
	char buffer1[512];
	GetBaseFilenameFromPath( buffer1, path );
	RemoveFileExtension( cleaned, buffer1 );
}

uint32_t GetFileSize( const char* szFile )
{
	FILE* file = fopen( szFile, "r+b" );
	if( !file )
		return 0;			

	fseek( file, 0, SEEK_END );
	uint32_t bufferSize = (uint32_t)ftell(file);
	fclose(file);
	return bufferSize;
}

uint32_t ReadFileIntoBuffer( const char* szFile, uint8_t*& buffer )
{
	uint32_t size = GetFileSize(szFile);	
	buffer = NULL;
	
	FILE* file = fopen( szFile, "r+b" );
	if( !file )
		return 0;

	buffer = new uint8_t[size];
	fread( buffer, size, 1, file );
	fclose(file);

	return size;
}

// Check to see if a file exists
bool FileExists( char* szPath )
{
	FILE* file = fopen(szPath, "r+b");
	if( file )
	{
		fclose(file);
		return true;
	}

	return false;		
}


}; //end Algorithm

}; //end jbsCommon