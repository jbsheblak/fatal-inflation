//
// GameX - ImageExt Class Header - TIFF Format Support
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
// 

#ifndef GAMEX_IMG_TIFF
	#define GAMEX_IMG_TIFF

	#include <math.h>
	#include "gamex-win-dx.hpp"

	#define TIFF_BUFFER					(10000)
	#define TIFF_BYTEORDER				0x4949
	#define TIFF_MAGIC					42

	#define TIFF_STATUS_OK				1
	#define TIFF_STATUS_NOMAGIC			2

	#define TIFF_TAG_NEWSUBTYPE			254
	#define TIFF_TAG_SUBTYPE			255
	#define TIFF_TAG_IMAGEWIDTH			256
	#define TIFF_TAG_IMAGEHEIGHT		257
	#define TIFF_TAG_BITSPERSAMPLE		258
	#define TIFF_TAG_COMPRESSION		259
		#define TIFF_COMPRESS_NONE		1
		#define TIFF_COMPRESS_LZW		5
		#define TIFF_COMPRESS_PACKBITS	32773
	#define TIFF_TAG_PHOTOMETRIC		262
		#define TIFF_PHOTO_WHITEZERO	0
		#define TIFF_PHOTO_BLACKZERO	1
		#define TIFF_PHOTO_RGB			2
		#define TIFF_PHOTO_RGBPAL		3
		#define TIFF_PHOTO_TRANS		4
		#define TIFF_PHOTO_CMYK			5
	#define TIFF_TAG_THRESHOLDING		263
	#define TIFF_TAG_DOCNAME			269
	#define TIFF_TAG_STRIPOFFSETS		273
	#define TIFF_TAG_ORIENTATION		274
	#define TIFF_TAG_SAMPLESPERPIXEL	277
	#define TIFF_TAG_ROWSPERSTRIP		278
	#define TIFF_TAG_STRIPBYTECOUNTS	279
	#define TIFF_TAG_XRES				282
	#define TIFF_TAG_YRES				283
	#define TIFF_TAG_RESUNIT			296
	#define TIFF_TAG_COLORMAP			320
	#define TIFF_TAG_EXTRASAMPLES		338

	#define TIFF_TYPE_BYTE				1
	#define TIFF_TYPE_ASCII				2
	#define TIFF_TYPE_SHORT				3
	#define TIFF_TYPE_LONG				4
	#define TIFF_TYPE_RATIONAL			5

	#define TIFF_MODE_BW				0
	#define TIFF_MODE_GRAY				1
	#define TIFF_MODE_RGB				2
	#define TIFF_MODE_INDEX				3
	#define TIFF_MODE_RGBA				4

	#define TIFF_ALPHA_NO				0
	#define TIFF_ALPHA_YES				1

	#define TIFF_SAVE_ENTRIES			14 // number of entries we save
	#define TIFF_SAVE_SIZEHEAD			8			// 8 bytes
	#define TIFF_SAVE_SIZEIFD			(2 + TIFF_SAVE_ENTRIES*12 + 4)
	#define TIFF_SAVE_SIZEBPC			(2*4)		// 4 ints
	#define TIFF_SAVE_SIZEXRES			(4*2)		// 2 longs
	#define TIFF_SAVE_SIZEYRES			(4*2)		// 2 longs

	#define TIFF_SAVE_POSHEAD			0
	#define TIFF_SAVE_POSIFD			(TIFF_SAVE_POSHEAD + TIFF_SAVE_SIZEHEAD)
	#define TIFF_SAVE_POSBPC			(TIFF_SAVE_POSIFD + TIFF_SAVE_SIZEIFD)
	#define TIFF_SAVE_POSXRES			(TIFF_SAVE_POSBPC + TIFF_SAVE_SIZEBPC)
	#define TIFF_SAVE_POSYRES			(TIFF_SAVE_POSXRES + TIFF_SAVE_SIZEXRES)
	#define TIFF_SAVE_POSDATA			(TIFF_SAVE_POSYRES + TIFF_SAVE_SIZEYRES)

#endif