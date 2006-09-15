//
// GameX - File Class Code 
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
#include <string.h>

#include "gamex-file.hpp"

File::File (void)
{
	filemodes = FILE_NOTOPEN;
	#ifdef FILE_COUNT_WRITES
		write_number = 0;
	#endif
}


File::File (char *fname, int fmodes)
{
	Open (fname, fmodes);
}

File::~File (void)
{
	Close ();	
}

int File::Exist (char *fname)
{
 	FILE *f = fopen (fname, "r");
	if (f==NULL) {
		return FILE_EXIST_NO;
	} else {
		fclose (f);
		return FILE_EXIST_YES;
	}
}

int File::Open (char *fname, int fmodes)
{
	char mode[10];
	
	// Initialize translation between Visual C++ fopen modes and File class open modes
	strcpy (mode, "\0");
	
	// Handle FILE_APPEND, FILE_READ, FILE_WRITE, FILE_AUTOCREATE, and FILE_LEAVEEOF
	if ((fmodes & FILE_APPEND) == FILE_APPEND) {
		if ((fmodes & FILE_READ) == FILE_READ) {
			#ifdef FILE_DEBUG
				debug.Output ("File::Open: Cannot open file in both APPEND and READ modes.\n", "");
			#endif
			return FILE_STATUS_READERROR;
		}
		if ((fmodes & FILE_WRITE) == FILE_WRITE) {
			#ifdef FILE_DEBUG
				debug.Output ("File::Open: Cannot open file in both APPEND and WRITE modes.\n", "");
			#endif
			return FILE_STATUS_WRITEERROR;
		}
		strcat (mode, "a");

		// If FILE_AUTOCREATE is off, make sure file exists before appending
		if ((fmodes & FILE_AUTOCREATE) != FILE_AUTOCREATE) {
			if (Exist(fname) == FILE_EXIST_NO) {
				#ifdef FILE_DEBUG
					debug.Output ("File::Open: File does not exist for append. (Use FILE_AUTOCREATE if desired)\n", "");
				#endif				
				return FILE_EXIST_NO;
			}
		}
		// If FILE_LEAVEEOF is off, default to remove EOF mode
		if ((fmodes & FILE_LEAVEEOF) != FILE_LEAVEEOF)
			strcat (mode, "+");
	} else {
		if ((fmodes & FILE_LEAVEEOF) == FILE_LEAVEEOF)
			#ifdef FILE_DEBUG
				debug.Output ("File::Open: (Warning only) FILE_LEAVEEOF only has meaning with FILE_APPEND.\n", "");
			#endif
		if ((fmodes & FILE_READ) == FILE_READ) {			
			if ((fmodes & FILE_WRITE) == FILE_WRITE) {				
				// If FILE_AUTOCREATE is on, autocreate file (otherwise, check for file existence)
				if ((fmodes & FILE_AUTOCREATE) == FILE_AUTOCREATE) {
					strcat (mode, "w+");
				} else {
					if (Exist(fname) == FILE_EXIST_NO) {
						#ifdef FILE_DEBUG
							debug.Output ("File::Open: File does not exist for read/write. (Use FILE_AUTOCREATE if desired)\n", "");
						#endif
						return FILE_EXIST_NO;
					}
					strcat (mode, "r+");
				}
			} else {
				if ((fmodes & FILE_AUTOCREATE) == FILE_AUTOCREATE) {
					#ifdef FILE_DEBUG
						debug.Output ("File::Open: (Warning only) FILE_AUTOCREATE has no meaning when using FILE_READ without FILE_APPEND or FILE_WRITE.\n", "");
					#endif
				}
				if (Exist(fname) == FILE_EXIST_NO) {
					#ifdef FILE_DEBUG
						debug.Output ("File::Open: File does not exist for read.\n", "");				
					#endif
					return FILE_EXIST_NO;
				}
				strcat (mode, "r");
			}
		} else if ((fmodes & FILE_WRITE) == FILE_WRITE) {
			if ((fmodes & FILE_AUTOCREATE) != FILE_AUTOCREATE) {
				if (Exist(fname) == FILE_EXIST_NO) {
					#ifdef FILE_DEBUG
						debug.Output ("File::Open: File does not exist for write (Use FILE_AUTOCREATE is desired).\n", "");						
					#endif
					return FILE_EXIST_NO;
				}
			}
			strcat (mode, "w");			
		} else {
			#ifdef FILE_DEBUG
				debug.Output ("File::Open: Must open file using FILE_APPEND, FILE_READ, or FILE_WRITE.\n", "");
			#endif
			return FILE_STATUS_READERROR;
		}
	}
	if ((fmodes & FILE_NOTOPEN) == FILE_NOTOPEN) {
		#ifdef FILE_DEBUG
			debug.Output ("File::Open: Cannot open a file using FILE_NOTOPEN command!!\n", "");
		#endif
		fmodes ^= FILE_NOTOPEN;
	}

	// Handle FILE_RANDOM and FILE_SEQUENTIAL (mutually exclusive)	
	if ((fmodes & FILE_RANDOM) == FILE_RANDOM) {
		if ((fmodes & FILE_SEQUENTIAL) == FILE_SEQUENTIAL) {
			#ifdef FILE_DEBUG
				debug.Output ("File::Open: Cannot open in SEQUENTIAL and RANDOM access modes.\n", "");
			#endif	
			return FILE_STATUS_READERROR;
		} else {
			strcat (mode, "b");
		}		
	} else if ((fmodes & FILE_SEQUENTIAL) == FILE_SEQUENTIAL) {
		if((fmodes & FILE_RAWDATA) == FILE_RAWDATA) {
			strcat (mode, "b");
		} else {
			strcat (mode, "t"); // can screw up proper loading of sequential binary data by counting certain values as end-of-file, even mid-file
		}
	} else {
		#ifdef FILE_DEBUG
			debug.Output ("File::Open: Must open in SEQUENTIAL or RANDOM access mode.\n", "");
		#endif		
		return FILE_STATUS_READERROR;
	}	
	
	// Open file
	strcpy (filename, fname);
	filemodes = fmodes;
	filehandle = fopen (filename, mode);
	if (filehandle==NULL)  {
		// happens if write access was requested and the file is marked as read-only
		filemodes |= FILE_NOTOPEN; // prevent error on close
		return FILE_STATUS_WRITEERROR; // return error so caller can react
	}
	filepos = 0;	
	return FILE_STATUS_OK;
}

void File::Close (void)
{
	
	if ((filemodes & FILE_NOTOPEN) != FILE_NOTOPEN) {
		fflush (filehandle);
		fclose (filehandle);
	}
	filemodes = FILE_NOTOPEN;
}

void File::Delete (char *fname)
{
	if(Exist(fname)) remove(fname);
}

int File::ReadC (int num, XCHAR *buf)
{
	size_t status = fread (buf, num, 1, filehandle);
	#ifdef FILE_CHECKS
		if ((filemodes & FILE_READ) != FILE_READ) return FILE_STATUS_NOREAD;
		if (status < 1) {
			if (feof(filehandle)!=0) return FILE_STATUS_EOF;
			return FILE_STATUS_READERROR;
		}
	#endif
	return FILE_STATUS_OK;	
}

int File::WriteC (int num, XCHAR *buf)
{
	size_t status;
	status = fwrite (buf, num, 1, filehandle);
	#ifdef FILE_CHECKS
		if ((filemodes & FILE_WRITE) != FILE_WRITE) return FILE_STATUS_NOWRITE;
		if (status < 1) return FILE_STATUS_WRITEERROR;
	#endif
	#ifdef FILE_COUNT_WRITES
		write_number += num;
	#endif
	return FILE_STATUS_OK;	
}

int File::WriteS (char * str)
{
	int n = 0;
	while(str[n++]);
	return WriteC(n-1,str);
}
int File::Read (int num, XBYTE *buf)
{
	buf[0] = 0;
	size_t status = fread (buf, 1, num, filehandle);
	#ifdef FILE_CHECKS
		if ((filemodes & FILE_READ) != FILE_READ) return FILE_STATUS_NOREAD;
		if (status < 1) {
			if (feof(filehandle)!=0)
				return FILE_STATUS_EOF;
			return FILE_STATUS_READERROR;
		}
	#endif
	return FILE_STATUS_OK;	
}

int File::Write (int num, XBYTE *buf)
{
	size_t status;
	status = fwrite (buf, 1, num, filehandle);
	#ifdef FILE_CHECKS
		if ((filemodes & FILE_WRITE) != FILE_WRITE) return FILE_STATUS_NOWRITE;
		if (status < 1) return FILE_STATUS_WRITEERROR;
	#endif
	#ifdef FILE_COUNT_WRITES
		write_number += num;
	#endif
	return FILE_STATUS_OK;	
}


int File::Read (XBYTE &c)
{
	c = (XBYTE) fgetc (filehandle);
	#ifdef FILE_CHECKS
		if ((filemodes & FILE_READ) != FILE_READ) return FILE_STATUS_NOREAD;
		if (feof(filehandle)!=0)
			return FILE_STATUS_EOF;
		if (ferror(filehandle)!=0) return FILE_STATUS_READERROR;
	#endif 		
	return FILE_STATUS_OK;
}

int File::Write (XBYTE c)
{
	fputc (c, filehandle);
	#ifdef FILE_CHECKS	
		if ((filemodes & FILE_WRITE) != FILE_WRITE) return FILE_STATUS_NOWRITE;
		if (ferror(filehandle)!=0) return FILE_STATUS_WRITEERROR;		
	#endif
	#ifdef FILE_COUNT_WRITES
		write_number ++;
	#endif
	return FILE_STATUS_OK;	
}

int File::Read2 (XBYTE2 &c)
{
	// !!! For now, Read2 uses the Read function above 
	// !!! This could be much faster if we read individual bytes directly
	return Read (2, (XBYTE *) &c);
}

int File::Write2 (XBYTE2 c)
{
	// !!! For now, Write2 uses the Write function above 
	// !!! This could be much faster if we read individual bytes directly
	return Write (2, (XBYTE *) &c);
}

int File::Read4 (XBYTE4 &c)
{
	// !!! For now, Read4 uses the Read function above 
	// !!! This could be much faster if we read individual bytes directly
	return Read (4, (XBYTE *) &c);
}

int File::Write4 (XBYTE4 c)
{
	// !!! For now, Write4 uses the Write function above 
	// !!! This could be much faster if we read individual bytes directly
	return Write (4, (XBYTE *) &c);
}

int File::ReadLine (char *buf)
{
	fgets (buf, FILE_LINELEN, filehandle);
	if (feof(filehandle)!=0) return FILE_STATUS_EOF;
	return FILE_STATUS_OK;
}

int File::WriteLine (char *buf)
{	
	fputs (buf, filehandle);
	return FILE_STATUS_OK;
}

void File::SetPosition (long pos)
{
	if (pos==FILE_POS_BEGIN) {
		fseek (filehandle, 0, SEEK_SET);
	} else if (pos==FILE_POS_END) {
		fseek (filehandle, 0, SEEK_END);
	} else {
		fseek (filehandle, pos, SEEK_SET);
	}
}

long File::GetPosition (void)
{
	return ftell (filehandle);
}

#ifdef FILE_TESTER
	void main (void)
	{
		File x;
		XBYTE c;

		x.Open ("filetest.txt", FILE_WRITE | FILE_AUTOCREATE | FILE_SEQUENTIAL);
		printf ("Opening filetest.txt for output: WRITE, AUTOCREATE, SEQUENTIAL\n\n");
		
		x.Write (12); printf ("Writing byte: value = 12\n");
		x.Write (28); printf ("Writing byte: value = 28\n");
		x.Write (33); printf ("Writing byte: value = 33\n\n");
		x.Close ();

		x.Open ("filetest.txt", FILE_READ | FILE_SEQUENTIAL);
		x.Read (c); printf ("Reading byte: value = %d\n", (int) c); 
		x.Read (c); printf ("Reading byte: value = %d\n", (int) c); 
		x.Read (c); printf ("Reading byte: value = %d\n\n", (int) c);
		x.Close ();

		printf ("File testing finished.\n");
		getch();
	}
#endif
