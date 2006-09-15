//---------------------------------------------------
// Name: jbsCommon : Algorithms
// Desc:  miscellaneous algorithms
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _JBSCOMMON_ALGORITHMS_ALGORITHMS_H_
#define _JBSCOMMON_ALGORITHMS_ALGORITHMS_H_

#include <string.h>
#include <assert.h>
#include "Types.h"
#include <vector>

namespace jbsCommon
{

namespace Algorithm
{

// -------------------------------------------------------------------------------
//  Name: CopyStrided
//  Desc:  Copy memory from one buffer to another in a strided fasion
//  Params:
//    pDest        : Destination byte buffer
//    destOffset   : Offset from start of dest buffer to copy to
//    destStride   : Intervals to copy to in dest buffer
//    pSrc         : Src byte buffer
//    srcOffset    : Offset from start of src buffer to copy from
//    srcStride    : Intervals to copy from in src buffer
//    elemSize     : size of one element to copy
//    elemCount    : number of elements to copy
// -------------------------------------------------------------------------------
void CopyStrided( uint8_t* pDest, uint32_t destOffset, uint32_t destStride,
	              uint8_t* pSrc,  uint32_t srcOffset,  uint32_t srcStride, 
				  uint32_t elemSize, uint32_t elemCount );

// -------------------------------------------------------------------------------
//  Name: CopyStridedIndexed
//  Desc:  Copy memory from one buffer to another in a strided fasion from an IB
//  Params:
//    pDest        : Destination byte buffer
//    destOffset   : Offset from start of dest buffer to copy to
//    destStride   : Intervals to copy to in dest buffer
//    pSrc         : Src byte buffer
//    srcOffset    : Offset from start of src buffer to copy from
//    srcStride    : Intervals to copy from in src buffer
//    pIndexArray  : Array of indices to use when copying
//    elemSize     : size of one element to copy
//    elemCount    : number of elements to copy
// -------------------------------------------------------------------------------y
template <typename T>
void CopyStridedIndexed( uint8_t* pDest, uint32_t destOffset, uint32_t destStride,
	                     uint8_t* pSrc,  uint32_t srcOffset,  uint32_t srcStride, 
				         T* pIndexArray, uint32_t elemSize, uint32_t elemCount )
{
	for( uint32_t idx = 0; idx < elemCount; ++idx )
	{
		uint32_t i = (uint32_t)pIndexArray[idx];
		memcpy( pDest + destOffset + destStride * idx, pSrc + srcOffset + srcStride * i, elemSize );
	}
}

//-----------------------------------------------------------
// Name: EnumerateFilesInFolder
// Desc:  enumerates all files in a folder and puts in string vector
//-----------------------------------------------------------
bool EnumerateFilesInFolder( const char* dir, std::vector< std::string >& retFiles );

//-----------------------------------------------------------
// Name: FilterByKeyword
// Desc:  takes a list of strings and throws out ones that dont have keyword
//-----------------------------------------------------------
bool FilterByKeyword( const char* keyword, std::vector< std::string >& srcStrings, std::vector< std::string >& dstStrings );

//-----------------------------------------------------------
// Name: EnumerateTypeFilesInFolder
// Desc:  enumerates all files of type in a folder
//-----------------------------------------------------------
bool EnumerateTypedFilesInFolder( const char* dir, const char* type, std::vector< std::string >& retFiles );

//-----------------------------------------------------------
// Name: GetBaseFilenameFromPath
// Desc:  removes everything but the filename and ext from path
//-----------------------------------------------------------
void GetBaseFilenameFromPath( char* base, char* path );

//-----------------------------------------------------------
// Name: RemoveFileExtension
// Desc:  removes a file extension from a path
//-----------------------------------------------------------
void RemoveFileExtension( char* cleaned, char* file_with_ext );

//-----------------------------------------------------------
// Name: GetFileExtension
// Desc:  retreives the file extension from a path
//-----------------------------------------------------------
void GetFileExtension( char* extension, char* file_with_ext );

//-----------------------------------------------------------
// Name: CleanFilePath
// Desc:  removes base path and extension
//-----------------------------------------------------------
void CleanFilePath( char* cleaned, char* path );

//-----------------------------------------------------------
// Name: GetFileSize
// Desc:  does file existance check and returns file size
//-----------------------------------------------------------
uint32_t GetFileSize( const char* szFile );

//-----------------------------------------------------------
// Name: ReadFileIntoBuffer
// Desc:  puts the entire file data into a buffer
//-----------------------------------------------------------
uint32_t ReadFileIntoBuffer( const char* szFile, uint8_t*& buffer );

//-----------------------------------------------------------
// Name: FileExists
// Desc:  does a file existance check
//-----------------------------------------------------------
bool FileExists( char* szPath );


}; //end Algorithm

}; //end jbsCommon


#endif //end _JBSCOMMON_ALGORITHMS_ALGORITHMS_H_
