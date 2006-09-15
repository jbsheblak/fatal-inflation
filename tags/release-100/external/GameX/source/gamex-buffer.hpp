//
// GameX - Buffer Class Header
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
#include <string.h>
#include <conio.h>

#include "gamex-debug.hpp"

#ifndef BUFFER_DEF
	#define BUFFER_DEF

	#define BUFFER_DEBUG
	// #define BUFFER_TESTER

	#ifndef XBYTE
		#define XBYTE				unsigned __int8
		#define XBYTE2				unsigned __int16
		#define XBYTE4				unsigned __int32
		#define XBYTE8				__int64
	#endif

	#define FALSE					0
	#define TRUE					1

	#define BUFFER_ORDER_LBF		0
	#define BUFFER_ORDER_MBF		1

	#define BUFFER_EMPTY		0
	#define BUFFER_DATA			1
	#define BUFFER_STRING		2
	#define BUFFER_CONSOLE		3
	#define BUFFER_ARRAY		4

	class Buffer {
	public:	
		Buffer (void);										// Initialize a TYPE_EMPTY buffer
		
		void CreateConsole (int r, int c);					// Console Functions
		char *GetLine (void);								// Get current line
		char *GetLineRelative (int n);						// Get a line relative to the current line
		void AddChar (char ch);								// Add a character
		void AddLine (char *line);							// Add a line
		void Scroll (void);									// Scroll the buffer

		void SetOrder (int order);							// Set byte order for interpretation

		void Scale (XBYTE *in, int width, int scale);		// Multiply each byte by 'scale'
		void Invert (XBYTE *in, int width);					// Invert all bytes (255-x)

		int PackBits (XBYTE *in, XBYTE *out, int width);		// Simple RLE encoding/decoding
		int UnpackBits (XBYTE *in, XBYTE *out, int width);
///		int UnstuffBits (XBYTE *in, XBYTE *out, int width, unsigned char *stuff_info);
		int UnstuffEachBit (XBYTE *in, XBYTE *out, int width);

		int ReadWord (char *line, char *word);				// Read words (space delimited)
		int ReadWord (char *line, char *word, char delim);	// Read words (arbitrary delimited)
															
		// Read Bytes, Words, DWords, Ints, Floats, Doubles according to Byte Order
		inline unsigned char ReadC (XBYTE c) {return (*FuncReadC) (c);}
		inline signed char ReadSC (XBYTE c) {return (*FuncReadSC) (c);}
		inline unsigned short int ReadI (XBYTE2 c) {return (*FuncReadI) (c);}	
		inline signed short int ReadSI (XBYTE2 c) {return (*FuncReadSI) (c);}
		inline unsigned long int ReadL (XBYTE4 c) {return (*FuncReadL) (c);}
		inline signed long int ReadSL (XBYTE4 c) {return (*FuncReadSL) (c);}
		inline float ReadF (XBYTE4 c) {return (*FuncReadF) (c);}
		inline double ReadD (XBYTE8 c) {return (*FuncReadD) (c);}

		// Write Bytes, Words, DWords, Ints, Floats, Doubles according to Byte Order
		inline XBYTE WriteC (unsigned char c) {return (*FuncWriteC) (c);}
		inline XBYTE WriteSC (signed char c) {return (*FuncWriteSC) (c);}
		inline XBYTE2 WriteI (unsigned short int c) {return (*FuncWriteI) (c);}	
		inline XBYTE2 WriteSI (signed short int c) {return (*FuncWriteSI) (c);}
		inline XBYTE4 WriteL (unsigned long int c) {return (*FuncWriteL) (c);}
		inline XBYTE4 WriteSL (signed long int c) {return (*FuncWriteSL) (c);}
		inline XBYTE4 WriteF (float c) {return (*FuncWriteF) (c);}
		inline XBYTE8 WriteD (double  c) {return (*FuncWriteD) (c);}
	private:
		int byte_order;
		short int buf_type;
		int rows, cols;
		int curr_row, curr_col;
		char *buf, *curr_pos;
		
		unsigned char (*FuncReadC) (XBYTE c);
		signed char (*FuncReadSC) (XBYTE c);
		unsigned short int (*FuncReadI) (XBYTE2 c);
		signed short int (*FuncReadSI) (XBYTE2 c);
		unsigned long int (*FuncReadL) (XBYTE4 c);
		signed long int (*FuncReadSL) (XBYTE4 c);
		float (*FuncReadF) (XBYTE4 c);
		double (*FuncReadD) (XBYTE8 c);

		XBYTE (*FuncWriteC) (unsigned char c);
		XBYTE (*FuncWriteSC) (signed char c);
		XBYTE2 (*FuncWriteI) (unsigned short int c);
		XBYTE2 (*FuncWriteSI) (signed short int c);
		XBYTE4 (*FuncWriteL) (unsigned long int c);
		XBYTE4 (*FuncWriteSL) (signed long int c);
		XBYTE4 (*FuncWriteF) (float c);
		XBYTE8 (*FuncWriteD) (double c);
	};

	#ifndef BUFFER_STATIC
		#define BUFFER_STATIC
		static XBYTE Mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};		
	#endif
	
	static inline unsigned char ReadC_LBF (XBYTE c) {return c;}		
	static inline signed char ReadSC_LBF (XBYTE c) {return c;}		
	static inline unsigned short int ReadI_LBF (XBYTE2 c) {return c;}		
	static inline signed short int ReadSI_LBF (XBYTE2 c) {return c;}
	static inline unsigned long int ReadL_LBF (XBYTE4 c) {return c;}
	static inline signed long int ReadSL_LBF (XBYTE4 c) {return c;}
	static inline float ReadF_LBF (XBYTE4 c) {return (float) c;}
	static inline double ReadD_LBF (XBYTE8 c) {return (double) c;}

	static inline XBYTE WriteC_LBF (unsigned char  c) {return c;}		
	static inline XBYTE WriteSC_LBF (signed char c) {return c;}		
	static inline XBYTE2 WriteI_LBF (unsigned short int c) {return c;}		
	static inline XBYTE2 WriteSI_LBF (signed short int c) {return c;}
	static inline XBYTE4 WriteL_LBF (unsigned long int c) {return c;}
	static inline XBYTE4 WriteSL_LBF (signed long int c) {return c;}
	static inline XBYTE4 WriteF_LBF (float c) {return (XBYTE4) c;}
	static inline XBYTE8 WriteD_LBF (double c) {return (XBYTE8) c;}

	static inline unsigned char ReadC_MBF (XBYTE c) {return c;}
	static inline signed char ReadSC_MBF (XBYTE c) {return c;}			
	static inline XBYTE WriteC_MBF (unsigned char c) {return c;}
	static inline XBYTE WriteSC_MBF (signed char c) {return c;}		
	
	#define Int2to1(c)		((c & 0xFF00) >> 8)
	#define Int1to2(c)		((c & 0xFF) << 8)
	#define IntReverse(c)	(Int2to1(c) | Int1to2(c))
	static inline unsigned short int ReadI_MBF (XBYTE2 c) {
		return (unsigned short int) IntReverse(c);		
	}
	static inline signed short int ReadSI_MBF (XBYTE2 c) {
		return (signed short int) IntReverse(c);		
	}
	static inline XBYTE2 WriteI_MBF (unsigned short int c) {
		return (XBYTE2) IntReverse(c);		
	}
	static inline XBYTE2 WriteSI_MBF (signed short int c) {
		return (XBYTE2) IntReverse(c);		
	}
	
	#define Long4to1(c)		((c & 0xFF000000) >> 24)
	#define Long3to2(c)		((c & 0xFF0000) >> 8)
	#define Long2to3(c)		((c & 0xFF00) << 8)
	#define Long1to4(c)		((c & 0xFF) << 24)
	#define LongReverse(c)	(Long4to1(c) | Long3to2(c) | Long2to3(c) | Long1to4(c))	
	static inline unsigned long int ReadL_MBF (XBYTE4 c) {
		return (unsigned long int) LongReverse(c);
	}	
	static inline signed long int ReadSL_MBF (XBYTE4 c) {
		return (signed long int) LongReverse(c);		
	}	
	static inline float ReadF_MBF (XBYTE4 c) {
		return (float) LongReverse(c);
	}
	static inline XBYTE4 WriteL_MBF (unsigned long int c) {
		return (XBYTE4) LongReverse(c);		
	}
	static inline XBYTE4 WriteSL_MBF (signed long int c) {
		return (XBYTE4) LongReverse(c);		
	}
	static inline XBYTE4 WriteF_MBF (float c) {
		return (XBYTE4) LongReverse((XBYTE4) c);
	}	

	#define DoubleHItoLO(c)	((c & 0xFFFFFFFF00000000) >> 32)
	#define DoubleLOtoHI(c) ((c & 0xFFFFFFFF) << 32)
	#define DoubleReverse(c) (DoubleHItoLO(LongReverse(c)) | DoubleLOtoHI(LongReverse(c)) )
	static inline double ReadD_MBF (XBYTE8 c) {
		printf ("ReadD_MBF: Reversal of 8-bytes and conversion to double is not implemented.\n", c);		
		// return (double) DoubleReverse(c);
		return (double) c;
	}
	static inline XBYTE8 WriteD_MBF (double c) {
		return (XBYTE4) DoubleReverse((XBYTE4) c);		
	}
#endif