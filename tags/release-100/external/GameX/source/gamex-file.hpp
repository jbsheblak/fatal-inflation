//
// GameX - File Class Header
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
// 

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "gamex-debug.hpp"			// Access debugging

#ifndef FILE_DEF
	#define FILE_DEF
									//**** File Class Switches
	#define FILE_DEBUG				// Required for File class
	// #define FILE_TESTER				// Tests File I/O
	#define FILE_CHECKS				// Extra File operation checks
	#define FILE_COUNT_WRITES

	#ifndef XCHAR
		#define XCHAR				char
		#define XBYTE				unsigned __int8
		#define XBYTE2				unsigned __int16
		#define XBYTE4				unsigned __int32
		#define XBYTE8				__int64
	#endif

	#define FILE_BUFFER			2000
	#define FILE_NAMELEN		260
	#define FILE_LINELEN		500

	#define FILE_NOTOPEN		0x001	// File not opened yet
	#define FILE_RANDOM			0x002	// Open file in random access mode
	#define FILE_SEQUENTIAL		0x004	// Open file in sequential mode
	#define FILE_READ			0x008	// Allow reading of file
	#define FILE_WRITE			0x010	// Allow writing of file (by overwrite)
	#define FILE_APPEND			0x020	// Allow writing of file (by append)
	#define FILE_AUTOCREATE		0x040	// Allow auto-creation of file if it does not exist
	#define FILE_LEAVEEOF		0x080	// Leave EOF marker when appending
	#define FILE_RAWDATA		0x100	// Read and write unaltered bytes of binary data, use if you encounter problems loading saved data sequentially

	#define FILE_EXIST_NO		0		// Exist status no (does not exist)
	#define FILE_EXIST_YES		1		// Exist status yes (does exist)

	#define FILE_STATUS_OK			2	// Return status ok
	#define FILE_STATUS_EOF			3	// Return status for end-of-file
	#define FILE_STATUS_READERROR	4	// Return status for read error
	#define FILE_STATUS_WRITEERROR	5	// Return status for write error
	#define FILE_STATUS_NOREAD		6	// Return status for no-read (file not opened in read mode)
	#define FILE_STATUS_NOWRITE		7	// Return status for no-write (file not opened in write mode)

	#define FILE_POS_BEGIN		-1		// Position at beginning of file
	#define FILE_POS_END		-2		// Position at end of file

	class File {
	public:
		File ();						// Constructor (no file given)
		File (char *fname, int fmodes);	// Constructor (open file)
		~File ();						// Destructor (close file)

		int Exist (char *fname);					// Exist file (check for file existence) (returns FILE_EXIST_NO if not)
		int Open (char *fname, int fmodes);			// Open file (using given modes)
		void Close (void);							// Close file
		void Delete (char *fname);					// Permanently deletes file (be careful!)
		
		int ReadC (int num, XCHAR *buf);			// Read many bytes (chars)
		int WriteC (int num, XCHAR *buf);			// Write many bytes
		int Read (int num, XBYTE *buf);				// Read many bytes (ints)
		int Write (int num, XBYTE *buf);			// Write many bytes

		int Read (XBYTE &c);						// Read one byte
		int Write (XBYTE c);						// Write one byte

		int Read2 (XBYTE2 &c);						// Read two bytes
		int Write2 (XBYTE2 c);						// Write two bytes

		int Read4 (XBYTE4 &c);						// Read four bytes
		int Write4 (XBYTE4 c);						// Write four bytes
		
		int ReadLine (char *buf);
		int WriteLine (char *buf);

		int WriteS (char * str);				// Writes a string

		void SetPosition (long pos);		// Set file position (random only)
		long GetPosition (void);			// Get file position (all modes)

		int EndOfFile (void) {return feof(filehandle);}

#ifdef FILE_COUNT_WRITES
		int write_number; // number of chars written total to this File, for debugging
#endif

	private:
		int filemodes;
		char filename[FILE_NAMELEN];
		FILE *filehandle;
		long filepos;

		unsigned char buffer[FILE_BUFFER];
		long buffer_pos;		
	};

#endif


// Notes:	Correct behavior for various file modes and functions
//			is outlined below
//
// Files may be opened supporting any of the following functions
//		RANDOM					File mode is random access
//		SEQUENTIAL				File mode is sequential access
//		READ					Allow reading of file in given mode
//		WRITE					Allow overwriting of file in given mode
//		APPEND					Allow append of file in given mode
//		AUTOCREATE				Allow automatic creation of file if does not exist
//		LEAVEEOF				Do not remove end-of-file marker for append
// (note: only overwrite and append are mutually exclusive, so only
// one of them can be used at a time. all others can be used together)
//
// Random access
//								File exists				File does not exist
//		read anywhere			read anywhere			error
//		write anywhere			write anywhere			write (may create)
//		read sequential			read sequential			error
//		overwrite sequential	overwrite sequential	write (may create)
//		append sequential		append sequential		write (may create)
//
// Sequential access
//								File exists				File does not exist
//		read anywhere			error					error
//		write anywhere			error					error
//		read sequential			read sequential			error
//		overwrite sequential	overwrite sequential	write (may create)
//		append sequential		append sequential		write (may create)
//
// Summary of interpretation of Visual C++ fopen modes:
//		"b"		RANDOM
//		"t"		SEQUENTIAL
//		"r"		READ
//		"w"		WRITE, AUTOCREATE
//		"a"		APPEND, AUTOCREATE, LEAVEEOF
//		"r+"	READ, WRITE
//		"w+"	READ, WRITE, AUTOCREATE
//		"a+"	APPEND, AUTOCREATE
//
// The implementation of file opening for the File class in the
// context of Visual C++ is determine by combining all the information
// above.


