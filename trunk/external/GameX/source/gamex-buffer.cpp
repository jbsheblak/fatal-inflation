//
// GameX - Buffer Class Code 
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
// 

#include "gamex-buffer.hpp"

Buffer::Buffer (void)
{
	buf_type = BUFFER_EMPTY;
	SetOrder (BUFFER_ORDER_LBF);
}

void Buffer::CreateConsole (int r, int c)
{	
	if (buf!=NULL) free (buf);
	rows = r; cols = c;
	buf_type = BUFFER_CONSOLE;
	buf = (char *) malloc (rows*cols);
	memset (buf, '\0', rows*cols);
	curr_row = 0;
	curr_col = 0;
	curr_pos = buf;
}

char *Buffer::GetLine (void)
{
	return buf + curr_row*cols;
}

char *Buffer::GetLineRelative (int n)
{
	char *line = buf + (curr_row-n)*cols;
	if (curr_row-n >= 0) {
		return line;
	} else {
		return NULL;
	}
}


void Buffer::AddChar (char ch)
{
	if (ch==13) {							// Return key
		AddLine (NULL);							// Goto next line
	} else if (ch==8) {						// Backspace key
		if (curr_col > 0) {
			curr_pos--;
			curr_col--;
			*curr_pos = '\0';
		}
	} else {								// Any other key
		if (curr_col==cols-1) {					// If end of line		
			if (curr_row==rows-1) {				// If end of buffer
				Scroll ();						// Scroll buffer			
				curr_pos = buf + curr_row*cols;	// Goto beginning of last line
			} else {							// Otherwise (ie. Not end of buffer)
				curr_row++;						// Goto next line in buffer
				curr_pos++;						// Goto start of next line
			}
			curr_col = 0;						// Goto beginning of line
			*curr_pos = ch;						// Insert character		
		} else {								// Otherwise (ie. Not end of line)
			*curr_pos = ch;						// Insert character
			curr_pos++;							// Goto next position 
			curr_col++;							// Goto next place in current line
		}
	}
}

// This function automatically advances to the next line, then
// outputs the new line specified.
void Buffer::AddLine (char *line)		
{
	if (curr_row==rows-1) {					// If end of buffer
		Scroll ();							// Scroll buffer		
	} else {								// Otherwise (not end of buffer)
		curr_row++;							// Advance to next line
	}
	curr_pos = buf + curr_row*cols;			// Set position to beginning of line
	curr_col = 0;							// Goto start of line
	if (line!=NULL) {
		strncpy (curr_pos, line, cols);			// Insert new line
		curr_col = strlen(line);				// Goto end of new line
	}
	curr_pos += curr_col;					// Set position to end of new line
}

void Buffer::Scroll (void)
{	
	memmove (buf, buf + cols, (rows-1)*cols);
	memset (buf + (rows-1)*cols, '\0', cols);
}

void Buffer::SetOrder (int order)
{
	byte_order = order;
	if (byte_order == BUFFER_ORDER_LBF) {		
		FuncReadC = &ReadC_LBF;		FuncWriteC = &WriteC_LBF;
		FuncReadSC = &ReadSC_LBF;	FuncWriteSC = &WriteSC_LBF;
		FuncReadI = &ReadI_LBF;		FuncWriteI = &WriteI_LBF;
		FuncReadSI = &ReadSI_LBF;	FuncWriteSI = &WriteSI_LBF;
		FuncReadL = &ReadL_LBF;		FuncWriteL = &WriteL_LBF;
		FuncReadSL = &ReadSL_LBF;	FuncWriteSL = &WriteSL_LBF;
		FuncReadF = &ReadF_LBF;		FuncWriteF = &WriteF_LBF;
		FuncReadD = &ReadD_LBF;		FuncWriteD = &WriteD_LBF;
	} else {
		FuncReadC = &ReadC_MBF;		FuncWriteC = &WriteC_MBF;
		FuncReadSC = &ReadSC_MBF;	FuncWriteSC = &WriteSC_MBF;
		FuncReadI = &ReadI_MBF;		FuncWriteI = &WriteI_MBF;
		FuncReadSI = &ReadSI_MBF;	FuncWriteSI = &WriteSI_MBF;
		FuncReadL = &ReadL_MBF;		FuncWriteL = &WriteL_MBF;
		FuncReadSL = &ReadSL_MBF;	FuncWriteSL = &WriteSL_MBF;
		FuncReadF = &ReadF_MBF;		FuncWriteF = &WriteF_MBF;
		FuncReadD = &ReadD_MBF;		FuncWriteD = &WriteD_MBF;
	}
}

void Buffer::Invert (XBYTE *in, int width)
{
	XBYTE *in_pnt;
	for (in_pnt = in; in_pnt < in+width; in_pnt++)
		*in_pnt = 255 - (*in_pnt);
}

void Buffer::Scale (XBYTE *in, int width, int scale)
{
	XBYTE *in_pnt;
	for (in_pnt = in; in_pnt < in+width; in_pnt++)
		*in_pnt *= scale;
}

int Buffer::PackBits (XBYTE *in, XBYTE *out, int width)
{
	int len, wid, cnt;

	wid = width;							// Begin with all bytes
	len = 0;								// Begin with no output
	
	while ( wid > 0 )						// Loop through all bytes
	{
		switch (wid) {
		case 1: {							// Input: 1 byte left
			out[0] = 0;
			out[1] = in[0];
			len += 2;						// Output: +2 bytes
			wid = 0;						// Done
		} break;
		case 2: {
			out[0] = 1;						// Input: 2 bytes left
			out[1] = in[0];	
			out[2] = in[1];
			len += 3;						// Output: +3 bytes
			wid = 0;						// Done
		} break;
		case 3: {							// Input: 3 bytes left
			if ( (in[0]==in[1]) && (in[1]==in[2]) ) {
				out[0] = (unsigned char) (257-3); // Bytes all same
				out[1] = in[0];
				len += 2;					// Output: +2 bytes
				wid = 0;					// Done
			} else {
				out[0] = 2;					// Bytes all different
				out[1] = in[0];
				out[2] = in[1];
				out[3] = in[2];
				len += 4;					// Output: +4 bytes
				wid = 0;					// Done
			}
		} break;
		default: {							// Input: >3 bytes
			if ( (in[0]==in[1]) && (in[1]==in[2]) ) {
				cnt = 3;					// Start with 3 equal bytes
				while ( (cnt<wid) && (cnt<127) && (in[0]==in[cnt]) )
					cnt++;					// Go until bytes are not same
				out[0] = (256 - cnt) + 1;	// Output number of repeated bytes (as neg. number)
				out[1] = in[0];				// Output value repeated
				out += 2;					// Goto end of out buffer
				in += cnt;					// Goto next unpacked byte in
				len += 2;					// Output: +2 bytes
				wid -= cnt;					// Done with 'count' number of bytes
			} else {
				cnt=0;						// Start with 0 equal bytes
				while ( (cnt<wid) && (cnt<127)		// Go until a run is found
					&& ! ( (in[cnt+0]==in[cnt+1])
					&& (in[cnt+1]==in[cnt+2]) ) ) {
						out[cnt+1] = in[cnt];		// Store byte
						cnt++;						// Goto next byte
				}
				out[0] = cnt - 1;			// Output number of bytes stored
				out += (cnt+1);				// Goto end of out buffer
				in += cnt;					// Goto next unpacked byte in
				len += (cnt+1);				// Output: +cnt+1 bytes
				wid -= cnt;					// Done with 'count' number of bytes				
			}
		} break;		
		}
	}
	return len;
}

int Buffer::UnpackBits (XBYTE *in, XBYTE *out, int width)
{
	int len, wid;
	unsigned char run;
	
	len = 0;
	wid = width;
	
	while ( wid > 0 )
	{
		run = *in++;
		if ( run >= 128 ) {
			len += (run = 257 - run);
			wid -= 2;
			while (run--)
				*out++ = *in;
			in++;
		} else {
			run++;
			len += run;
			wid -= run + 1;
			while (run--)
				*out++ = *in++;
		}		
	}
	return len;	
}

int Buffer::UnstuffEachBit (XBYTE *in, XBYTE *out, int width)
{
	XBYTE *in_pnt = in;
	XBYTE *out_pnt = out;
	XBYTE bit;
	int n, j;

	for (n = 0; n < width; n++) {
		for (j = 0; j<8; j++) {
			bit = ((*in_pnt) & Mask[j]) >> (7-j);
			*out_pnt++ = bit;
		}
		*in_pnt++;
	}
	return (width*8);
}
/*
int Buffer::UnstuffBits (XBYTE *in, XBYTE *out, int width, unsigned char *stuff_info)
{
	XBYTE *in_pnt = in;
	XBYTE *out_pnt = out;
	int n;
	
	for (n = 0; n < width; n++) {
		in_pnt++;
	}
	return width;
}
*/
int Buffer::ReadWord (char *line, char *word)
{
	return ReadWord (line, word, ' ');
}

int Buffer::ReadWord (char *line, char *word, char delim)
{
	char *buf_pos;
	char *start_pos;	

	// read past spaces/tabs, or until end of line/string
	for (buf_pos=line; (*buf_pos==' ' || *buf_pos=='\t') && *buf_pos!='\n' && *buf_pos!='\0';)
		buf_pos++;

	// if end of line/string found, then no words found, return null
	if (*buf_pos=='\n' || *buf_pos=='\0') {*word = '\0'; return FALSE;}

	// mark beginning of word, read until end of word
	for (start_pos = buf_pos; *buf_pos != delim && *buf_pos!='\t' && *buf_pos!='\n' && *buf_pos!='\0';)
		buf_pos++;
	
	if (*buf_pos=='\n' || *buf_pos=='\0') {	// buf_pos now points to the end of buffer
		strcpy (word, start_pos);			// copy word to output string
		*line = '\0';						// clear input buffer
	} else {
											// buf_pos now points to the delimiter after word
		*buf_pos = '\0';					// replace delimiter with end-of-word marker
		strcpy (word, start_pos);			// copy word(s) string to output string	
		buf_pos++;							// remove delimiter from buffer
											// move start_pos to beginning of entire buffer
		for (start_pos = line; *buf_pos!='\n' && *buf_pos!='\0'; )
			*start_pos++ = *buf_pos++;		// copy remainder of buffer to beginning of buffer
		*start_pos = '\0';					// mark end of new input buffer
	}
	return TRUE;						// return word(s) copied	
}

#ifdef BUFFER_TESTER
	void main (void)
	{
		Buffer code;
		
		char buf_start[50];
		char buf_pack[50];
		char buf_unpack[50];
		int num;

		// Test PackBits / UnpackBits
		printf ("Test Pack/Unpack Bits\n\n");
		strcpy (buf_start, "abcdeffffgggggghijklaaabbbbccdefgh");		
		printf ("Start: %s\n", buf_start);
		printf ("Start size: %d\n", strlen(buf_start));

		num = code.PackBits ((XBYTE*) buf_start, (XBYTE*) buf_pack, 34);
		printf ("Packed: %s\n", buf_pack);
		printf ("Packed size: %d\n", num);

		num = code.UnpackBits ((XBYTE*) buf_pack, (XBYTE*) buf_unpack, num);
		buf_unpack[num] = '\0';
		printf ("Unpacked: %s\n\n", buf_unpack);
		
		// Test Byte Decoding (LBF/MBF)
		
		XBYTE a = 0x11;
		XBYTE2 b = 0x1122;
		XBYTE4 c = 0x112233;
		XBYTE8 d = 0x11223344;
		printf ("Test Byte Decoding\n\n");
		printf ("ReadC should be 17: %d\n", (int) code.ReadC (a));				// should be 17
		printf ("ReadSC should be 17: %d\n", (int) code.ReadSC (a));			// should be 17
		printf ("ReadI should be 4386: %u\n", code.ReadI (b));					// should be 4386
		printf ("ReadSI should be 4386: %d\n", code.ReadSI (b));				// should be 4386
		printf ("ReadL should be 1122867: %lu\n", code.ReadL (c));				// should be 1122867
		printf ("ReadSL should be 1122867: %ld\n", code.ReadSL (c));			// should be 1122867
		printf ("ReadF should be 1122867.0: %f\n", (float) code.ReadF (c));		// should be 1122867.0
		printf ("ReadD should be 287454020.0: %f\n\n", (double) code.ReadD (d));// should be 287454020.0

		printf ("Buffer testing finished.\n");
		getch();
	}
#endif
