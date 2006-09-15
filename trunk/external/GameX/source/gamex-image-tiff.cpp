//
// GameX - TIFF-Handling Code
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
//
//**************************************
//
// TIFF Format - File Format support for ImageExtender class
//
// NOTE: CURRENT CAPABILITIES:
//		- Load supports:
//					1 bits/channel			BW
//					8,16,32 bit/channel		Grayscale (no alpha)
//					8,16,32 bit/channel		RGB (with or without alpha)
//		- Save supports:
//					8 bit					RGB (without alpha)
//
//**************************************

#include "gamex-image-tiff.hpp"

bool ImageExt::LoadTiffData (File &tiff, Buffer &code, unsigned long count, unsigned long offset, int row)
{
	XBYTE in[TIFF_BUFFER];
	XBYTE out[TIFF_BUFFER];
	XBYTE *pData, *pAlpha;
	int x, y, lastrow;	

	pData = m_pImg->m_data;							// Get ImageX color data
	pAlpha = m_pImg->m_alpha;						// Get ImageX alpha data 
	
	tiff.SetPosition (offset);						// Set file position to TIFF data
	lastrow = row + m_rps - 1;
	if (lastrow > m_yres-1) lastrow = m_yres-1;		// Determine last row to process
	
	switch (m_mode) {
	case TIFF_MODE_BW: {							// Black & White TIFF		
		XBYTE* pIn;
		for (y = row; y <= lastrow; y++) {			// Assume 1 Bit Per Pixel
			tiff.Read (m_bpr, in);					// Read row in TIFF
			code.UnstuffEachBit (in, out, m_bpr);	// Unstuff row from bits to bytes
			code.Scale (out, m_bpr*8, 255);			// Rescale row from 0-1 to 0-255
			if (m_photo == TIFF_PHOTO_WHITEZERO)	// Invert if photometrically stored
				code.Invert (out, m_bpr*8);			
			pIn = (XBYTE *) in;
			for (x=0; x < m_xres; x++) {
				*pData++ = (XBYTE) *pIn;				// Copy Black/White byte data 
				*pData++ = (XBYTE) *pIn;				// into Red, Green and Blue bytes.
				*pData++ = (XBYTE) *pIn++;
			}
		}		
	} break;
	case TIFF_MODE_GRAY: {							// Grayscale TIFF
		switch (m_bpc[CHAN_GRAY]) {
		case 8: {									// 8 Bits per Channel			
			XBYTE *pIn;			
			for (y = row; y <= lastrow; y++) {
				tiff.Read (m_bpr, in);
				pIn = (XBYTE *) in;
				for (x=0; x < m_xres; x++) {
					*pData++ = (XBYTE) *pIn;			// Copy Gray byte data 
					*pData++ = (XBYTE) *pIn;			// into Red, Green and Blue bytes.
					*pData++ = (XBYTE) *pIn++;
				}
			}
		} break;
		case 16: {									// 16 Bits per Channel			
			XBYTE2 *pIn;
			for (y = row; y <= lastrow;	y++) {
				tiff.Read (m_bpr, in);
				pIn = (XBYTE2 *) in;
				for (x=0; x < m_xres; x++) {
					*pData++ = (XBYTE) (*pIn >> 8);		// Copy Gray 2-byte data 
					*pData++ = (XBYTE) (*pIn >> 8);		// into Red, Green and Blue bytes.
					*pData++ = (XBYTE) (*pIn++ >> 8);
				}
			}
		 } break;
		case 32: {									// 32 Bits per Channel
 			XBYTE4 *pIn;
			for (y = row; y <= lastrow;	y++) {
				tiff.Read (m_bpr, in);
				pIn = (XBYTE4 *) in;
				for (x=0; x < m_xres; x++) {
					*pData++ = (XBYTE) (*pIn >> 24);		// Copy Gray 4-byte data
					*pData++ = (XBYTE) (*pIn >> 24);		// into Red, Green and Blue bytes.
					*pData++ = (XBYTE) (*pIn++ >> 24);
				}
			}
		} break;
		}
	} break;
	case TIFF_MODE_RGB: case TIFF_MODE_RGBA: {							// Full Color TIFF
		switch (m_bpc[CHAN_RED]) {					
		case 8: {									// 8 Bits per Channel
			XBYTE *pIn;							
			if (m_alpha==TIFF_ALPHA_YES) {					// Alpha included.
				for (y = row; y <= lastrow;	y++) {
					tiff.Read (m_bpr, in);
					pIn = (XBYTE *) in;
					for (x=0; x < m_xres; x++) {
						*pData++ = (XBYTE) *pIn++; 
						*pData++ = (XBYTE) *pIn++; 
						*pData++ = (XBYTE) *pIn++; 
						*pAlpha++ = (XBYTE) *pIn++;
					}
				}
			} else {										// No Alpha.
				for (y = row; y <= lastrow; y++) {
					tiff.Read (m_bpr, in);				
					pIn = (XBYTE *) in;
					for (x=0; x < m_xres; x++) {
						*pData++ = (XBYTE) *pIn++; 
						*pData++ = (XBYTE) *pIn++; 
						*pData++ = (XBYTE) *pIn++;					
					}
				}
			}
		} break;
		case 16: {									// 16 Bits per Channel
			XBYTE2 *pIn;
			if (m_alpha==TIFF_ALPHA_YES) {					// Alpha included.
				for (y = row; y <= lastrow;	y++) {
					tiff.Read (m_bpr, in);
					pIn = (XBYTE2 *) in;
					for (x=0; x < m_xres; x++) {
						*pData++ = (XBYTE) (*pIn++ >> 8); 
						*pData++ = (XBYTE) (*pIn++ >> 8);
						*pData++ = (XBYTE) (*pIn++ >> 8); 
						*pAlpha++ = (XBYTE) (*pIn++ >> 8);
					}
				}
			} else {
				for (y = row; y <= lastrow; y++) {			// No Alpha.
					tiff.Read (m_bpr, in);
					pIn = (XBYTE2 *) in;
					for (x=0; x < m_xres; x++) {
						*pData++ = (XBYTE) (*pIn++ >> 8); 
						*pData++ = (XBYTE) (*pIn++ >> 8);
						*pData++ = (XBYTE) (*pIn++ >> 8);					
					}
				}
			}
		} break;
		case 32: {									// 32 Bits per Channel
 			XBYTE4 *pIn;
			if (m_alpha==TIFF_ALPHA_YES) {				
				for (y = row; y <= lastrow;	y++) {
					tiff.Read (m_bpr, in);
					pIn = (XBYTE4 *) in;
					for (x=0; x < m_xres; x++) {
						*pData++ = (XBYTE) (*pIn++ >> 24); 
						*pData++ = (XBYTE) (*pIn++ >> 24);
						*pData++ = (XBYTE) (*pIn++ >> 24); 
						*pAlpha++ = (XBYTE) (*pIn++ >> 24);
					}
				}
			} else {
				for (y = row; y <= lastrow; y++) {
					tiff.Read (m_bpr, in);
					pIn = (XBYTE4 *) in;
					for (x=0; x < m_xres; x++) {
						*pData++ = (XBYTE) (*pIn++ >> 24); 
						*pData++ = (XBYTE) (*pIn++ >> 24);
						*pData++ = (XBYTE) (*pIn++ >> 24);					
					}
				}
			}
		} break;
		}
	} break;
	}

	return true;
}

bool ImageExt::LoadTiffStrips (File &tiff, Buffer &code)
{	
	XBYTE2 twobytes;
	XBYTE4 fourbytes;
	int pos, row;	
	unsigned long pos_offsets, pos_counts;
	unsigned long count, offset, strip;
	
	pos = tiff.GetPosition ();
	pos_counts = m_strip_counts;
	pos_offsets = m_strip_offsets;
	if (m_num_strips==1) {		
		count = pos_counts;
		offset = pos_offsets;
		row = 0;			
		LoadTiffData (tiff, code, count, offset, row);
	} else {		
		row = 0;
		for (strip=0; strip < m_num_strips; strip++) {
			tiff.SetPosition (pos_counts);
			tiff.Read2 (twobytes); count = code.ReadI (twobytes);
			tiff.SetPosition (pos_offsets);
			tiff.Read4 (fourbytes); offset = code.ReadL (fourbytes);

			LoadTiffData (tiff, code, count, offset, row);
			
			row += m_rps;
			pos_counts++;
			pos_offsets++;
		}
	}				
	tiff.SetPosition (pos);

	return true;
}

bool ImageExt::LoadTiffEntry (File &tiff, Buffer &code)
{
	XBYTE2 twobytes;
	XBYTE4 fourbytes;
	unsigned int tag, typ;
	unsigned long int count, offset;	
		
	// Read Entry Tag (WIDTH, HEIGHT, EXTRASAMPLES, BITSPERSAMPLE, COMPRESSION, etc.)
	tiff.Read2(twobytes); tag = code.ReadI(twobytes);
	// Read Entry Type and Count (SHORT or LONG)
	tiff.Read2(twobytes); typ = code.ReadI(twobytes);	
	tiff.Read4(fourbytes); count = code.ReadL(fourbytes);
	if (typ==TIFF_TYPE_SHORT && count==1) {
		tiff.Read2(twobytes); offset = code.ReadI(twobytes);
		tiff.Read2(twobytes);
	} else {
		tiff.Read4(fourbytes); offset = code.ReadL(fourbytes);
	}
	// DEBUG OUTPUT
	// printf ("tag:%u type:%u count:%lu offset:%lu\n", tag, typ, count, offset);
	
	// Add information to ImageFormat info based on Entry Tag
	switch (tag) {	
	case TIFF_TAG_IMAGEWIDTH: m_xres = offset; break;
	case TIFF_TAG_IMAGEHEIGHT: 	m_yres = offset; break;		
	case TIFF_TAG_EXTRASAMPLES: m_alpha = TIFF_ALPHA_YES; break;	
	case TIFF_TAG_BITSPERSAMPLE: {
		// NOTE: The TIFF specification defines 'samples',
		// while the Image class defines 'channels'.
		// Thus, bits-per-sample (in TIFF) is the same as bits-per-channel
		// And, samples-per-pixel (in TIFF) is the same as channels-per-pixel
		
		// Detemine samples-per-pixel here 
		// (this is just "count" of how many bits-per-sample 
		//  entries are present)		
		m_num_chan = count;
		switch (count) {		
		case 1: {			// One channel: B&W or GRAYSCALE
			m_bpc[CHAN_GRAY] = offset;
			m_bpcoff = 0;
			if (offset==1)	m_mode = TIFF_MODE_BW;
			else			m_mode = TIFF_MODE_GRAY;			
		} break;
		case 2: {			// Two channels: GRAYSCALE with ALPHA
			m_bpc[CHAN_GRAY] = offset;
			m_bpcoff = offset;
			m_mode = TIFF_MODE_GRAY;
		} break;		
		case 3: {			// Three channels: RGB
			m_bpcoff = offset;
			m_mode = TIFF_MODE_RGB;
		} break;
		case 4: {			// Four channels: RGB with ALPHA
			m_bpcoff = offset;	
			m_mode = TIFF_MODE_RGBA;
		} break;
		default: {
			printf ("Load:Tiff: Unknown TIFF mode.\n");
			return false;
		} break;
		}		
	} break;
	case TIFF_TAG_COMPRESSION: {
		m_compress = offset;
		if (m_compress!=1) {
#ifdef _DEBUG
			MessageBox (NULL, "TIF Load Error: LZW Compression not supported.\n\nGameX does not support LZW compressed TIF images. Save your TIF image again and be sure that the LZW compression option is turned off.\n", "GameX Error", MB_OK|MB_ICONSTOP);
#endif
			return false;
		}
	} break;	
	case TIFF_TAG_PHOTOMETRIC: {		
		m_photo = offset;
		if (m_photo==TIFF_PHOTO_RGBPAL)
			m_mode = TIFF_MODE_INDEX;
	} break;
	case TIFF_TAG_STRIPOFFSETS: m_strip_offsets = offset; break;
	case TIFF_TAG_ROWSPERSTRIP: {
		m_rps = offset;
		m_num_strips = (unsigned long ) ((m_yres+m_rps-1) / m_rps);
	} break;
	case TIFF_TAG_STRIPBYTECOUNTS: m_strip_counts = offset; break;
	}

	return true;
}

bool ImageExt::LoadTiffDirectory (File &tiff, Buffer &code)
{
	XBYTE2 twobytes;
	XBYTE4 fourbytes;
	unsigned int num;	
	unsigned long offset, pos;	
		
	// Read Number of TIFF Directory Entries
	tiff.Read2(twobytes);
	num = code.ReadI(twobytes);

	// Read TIFF Directory Entries to fill ImageFormat Info
	for (unsigned int n = 0; n<num; n++)	
		LoadTiffEntry (tiff, code);

	// Use ImageFormat to allocate and prepare ImageX 
	switch (m_mode) {						
	case TIFF_MODE_RGB: case TIFF_MODE_RGBA: {					// RGB TIFF Image
		pos = tiff.GetPosition();			
		tiff.SetPosition (m_bpcoff);		// Get bits per channel
		tiff.Read2 (twobytes); m_bpc[CHAN_RED] = code.ReadI(twobytes);
		tiff.Read2 (twobytes); m_bpc[CHAN_GREEN] = code.ReadI(twobytes);
		tiff.Read2 (twobytes); m_bpc[CHAN_BLUE] = code.ReadI(twobytes);
		if (m_alpha==TIFF_ALPHA_YES) {		// Get bits for alpha channel
			tiff.Read2 (twobytes); m_bpc[CHAN_ALPHA] = code.ReadI(twobytes);
		} else {
			m_bpc[CHAN_ALPHA] = 0;			// No alpha channel.
		}
		tiff.SetPosition (pos);
			
		m_ops = IMG_TRUECOLOR;				// Determine proper ImageX Pixel-Format options
		if (m_alpha==TIFF_ALPHA_YES) m_ops |= IMG_ALPHA;
		
		m_pImg->Size (m_xres, m_yres, m_ops);	// Resize the ImageX for loading
		
											// Calculate bits per pixel
		m_bpp = m_bpc[CHAN_RED] + m_bpc[CHAN_GREEN] + m_bpc[CHAN_BLUE] + m_bpc[CHAN_ALPHA];		
											// Calculate bytes per row
		m_bpr = (m_xres*m_bpp) / 8;
	} break;
	}

	// Load actual TIFF Image Data into ImageX	
	LoadTiffStrips (tiff, code);

	// Check if there are multiple images present in TIFF file
	tiff.Read4(fourbytes);
	offset = code.ReadL(fourbytes);	
	if (offset!=0)
		printf ("Load:Tiff: (Warning only) File contains multiple tiff images - reading only first.\n");

	return true;
}

// Function: LoadTiff
//
// Input:
//		m_name				Set to TIFF_FORMAT 
//		m_filename			Name of TIFF file to load
// Output:
//		m_success			Set to TRUE of FALSE
//		m_mode				Format mode (BW, GRAY, RGB, INDEX)
//		m_num_chan			Number of channels present
//		m_bpc[n]			Bits per channel for channel [n]
//		m_compress			Compression mode (NONE, PACKBITS, LZW)
//		m_photo			Photometric interpretation
//		m_num_strips		Number of strips
//		m_rps				Rows per strip

bool ImageExt::LoadTiff (char *filename, ImageX *img)
{
	XBYTE2 twobytes;
	XBYTE4 fourbytes;
	unsigned long pnt;

	File tiff;
	Buffer code;

	m_pImg = img;
	strcpy (m_filename, filename);
	if (tiff.Open (m_filename, FILE_READ | FILE_RANDOM) != FILE_EXIST_NO) {

		m_alpha = TIFF_ALPHA_NO;
		m_xres = 0;
		m_yres = 0;
		
		tiff.Read2 (twobytes);	
		if (twobytes == (XBYTE2) TIFF_BYTEORDER) {
			tiff.Read2(twobytes);
		} else {
			code.SetOrder (BUFFER_ORDER_MBF);		
			tiff.Read2(twobytes);
		}
		if (code.ReadI(twobytes) == TIFF_MAGIC) {
			tiff.Read4(fourbytes); 
			pnt = code.ReadL(fourbytes);
		
			tiff.SetPosition (pnt);
			LoadTiffDirectory (tiff, code);
		} else {
			printf ("Load:Tiff: File is corrupted, or is not a .TIFF file.\n");
			exit (-1);
			m_status = TIFF_STATUS_NOMAGIC;
		} 
		tiff.Close ();
		m_status = TIFF_STATUS_OK;	
		return true;
	} else {
		return false;
	}	
}

bool ImageExt::SaveTiffData (File &tiff, Buffer &code)
{	
	int x, y;
	GameX.AccessPixels(m_pImg);

	XBYTE out[TIFF_BUFFER];
	XBYTE *pOut;			
	switch (m_mode) {
	case TIFF_MODE_RGB: {
		int bpr = m_xres * 3;
		for (y=0; y < m_yres; y++) {				
			pOut = out;
			for (x=0; x < m_xres; x++) {
				int r,g,b;
				GameX.ReadPixel(x,y,r,g,b);
				*pOut++ = r;
				*pOut++ = g;
				*pOut++ = b;
			}
			tiff.Write(bpr, out);
		}
	}	break;
	case TIFF_MODE_RGBA: {
		int bpr = m_xres * 4;
		for (y=0; y < m_yres; y++) {				
			pOut = out;
			for (x=0; x < m_xres; x++) {
				int r,g,b,a;
				GameX.ReadPixel(x,y,r,g,b,a);
				*pOut++ = r;
				*pOut++ = g;
				*pOut++ = b;
				*pOut++ = a;
			}
			tiff.Write(bpr, out);
		}
	}	break;
	default: return false;
	}

	GameX.EndPixelAccess();
	return true;
}

bool ImageExt::SaveTiffEntry (File &tiff, Buffer &code, unsigned int tag)
{
	XBYTE2 twobytes;
	XBYTE4 fourbytes;
	unsigned int typ;
	unsigned long int count, offset;	
		
	switch (tag) {	
	case TIFF_TAG_NEWSUBTYPE: {
		typ = TIFF_TYPE_LONG;
		count = 1; 
		offset = 0;
	} break;
	case TIFF_TAG_IMAGEWIDTH: {
		typ = TIFF_TYPE_SHORT; count = 1; offset = m_xres;
	} break;
	case TIFF_TAG_IMAGEHEIGHT: {
		typ = TIFF_TYPE_SHORT; count = 1; offset = m_yres;
	} break;		
	case TIFF_TAG_BITSPERSAMPLE: {
		// NOTE: The TIFF specification defines 'samples',
		// while the Image class defines 'channels'.
		// Thus, bits-per-sample (in TIFF) is the same as bits-per-channel
		// And, samples-per-pixel (in TIFF) is the same as channels-per-pixel
		
		switch (m_mode) {
		case TIFF_MODE_BW: {typ = TIFF_TYPE_SHORT; count = 1; offset = 1;} break;
		case TIFF_MODE_GRAY: {
			typ = TIFF_TYPE_SHORT;
			count = 1;
			offset = m_bpp;
		} break;
		case TIFF_MODE_RGB: {
			typ = TIFF_TYPE_SHORT;
			count = 3;
			offset = TIFF_SAVE_POSBPC;
		} break;
		case TIFF_MODE_RGBA: {
			typ = TIFF_TYPE_SHORT;
			count = 4;
			offset = TIFF_SAVE_POSBPC;
		} break;
		default: {
			printf ("Save:Tiff: Unknown TIFF mode.\n");
			return false;
		} break;
		}		
		m_num_chan = count;
	} break;
	case TIFF_TAG_COMPRESSION: {typ = TIFF_TYPE_SHORT; count = 1; offset = m_compress;} break;
	case TIFF_TAG_PHOTOMETRIC: {
		typ = TIFF_TYPE_SHORT;
		count = 1;
		switch (m_mode) {
		case TIFF_MODE_BW: offset = TIFF_PHOTO_BLACKZERO; break;
		case TIFF_MODE_GRAY: offset = TIFF_PHOTO_BLACKZERO; break;
		case TIFF_MODE_RGB: offset = TIFF_PHOTO_RGB; break;
		case TIFF_MODE_RGBA: offset = TIFF_PHOTO_RGB; break;
		case TIFF_MODE_INDEX: offset = TIFF_PHOTO_RGBPAL; break;
		}
		m_photo = offset;
	} break;
	case TIFF_TAG_STRIPOFFSETS: {
		typ = TIFF_TYPE_LONG;
		count = 1;
		offset = TIFF_SAVE_POSDATA;
	} break;		
	case TIFF_TAG_SAMPLESPERPIXEL: {
		typ = TIFF_TYPE_SHORT;
		count = 1;
		offset = m_num_chan;	// calculated above
	} break;
	case TIFF_TAG_ROWSPERSTRIP: {
		typ = TIFF_TYPE_SHORT;
		count = 1;
		offset = m_yres;		
		m_rps = offset;
		m_num_strips = 1;
	} break;
	case TIFF_TAG_STRIPBYTECOUNTS: {
		typ = TIFF_TYPE_LONG;
		count = 1;
		switch (m_mode) {
		case TIFF_MODE_BW: m_bpr = (m_xres / 8) + 1; break;
		case TIFF_MODE_GRAY: m_bpr = (m_xres * m_bpp) / 8; break;
		case TIFF_MODE_RGB: m_bpr = (3 * m_xres * m_bpp) / 8; break;
		case TIFF_MODE_RGBA: m_bpr = (4 * m_xres * m_bpp) / 8; break;
		}		
		offset = (m_bpr * m_yres);
		m_strip_counts = offset;
	} break;
	case TIFF_TAG_XRES: {typ = TIFF_TYPE_RATIONAL; count = 1; offset = TIFF_SAVE_POSXRES;} break;
	case TIFF_TAG_YRES: {typ = TIFF_TYPE_RATIONAL; count = 1; offset = TIFF_SAVE_POSYRES;} break;
	case TIFF_TAG_RESUNIT: {typ = TIFF_TYPE_SHORT; count = 1; offset = 2;} break;
	case TIFF_TAG_COLORMAP: {typ = TIFF_TYPE_SHORT; count = 0; offset = 0;} break;
	}

	twobytes = code.WriteI (tag);
	tiff.Write2 (twobytes);

	twobytes = code.WriteI (typ);
	tiff.Write2 (twobytes);

	fourbytes = code.WriteL (count);
	tiff.Write4 (fourbytes);
	
	if (typ==TIFF_TYPE_SHORT && count==1) {
		twobytes = code.WriteI ((XBYTE2) offset);tiff.Write2 (twobytes);
		twobytes = code.WriteI (0);				tiff.Write2 (twobytes);		
	} else {
		fourbytes = code.WriteL (offset);		tiff.Write4 (fourbytes);
	}
	// DEBUG OUTPUT
	// printf ("-- tag:%u type:%u count:%lu offset:%lu\n", tag, typ, count, offset);

	return true;
}

bool ImageExt::SaveTiffExtras (File &tiff, Buffer &code, unsigned int tag)
{
	XBYTE2 twobytes;
	XBYTE4 fourbytes;

	switch (tag) {
	case TIFF_TAG_BITSPERSAMPLE: {
		// DEBUG OUTPUT
		// printf ("BPC pos: %u\n", tiff.GetPosition());
		// printf ("Est.BPC pos: %u\n", TIFF_SAVE_POSBPC);

		twobytes = code.WriteI (m_bpp/4);
		tiff.Write2 (twobytes);
		tiff.Write2 (twobytes);
		tiff.Write2 (twobytes);
		tiff.Write2 (twobytes);
	} break;	
	case TIFF_TAG_XRES: {
		// DEBUG OUTPUT
		// printf ("Xres pos: %u\n", tiff.GetPosition());
		// printf ("Est.Xres pos: %u\n", TIFF_SAVE_POSXRES);
		fourbytes = code.WriteL (1);
		tiff.Write4 (fourbytes);
		tiff.Write4 (fourbytes);
	} break;
	case TIFF_TAG_YRES: {
		// DEBUG OUTPUT
		// printf ("Yres pos: %u\n", tiff.GetPosition());
		// printf ("Est.Yres pos: %u\n", TIFF_SAVE_POSYRES);
		fourbytes = code.WriteL (1);
		tiff.Write4 (fourbytes);
		tiff.Write4 (fourbytes);
	} break;
	}

	return true;
}

bool ImageExt::SaveTiffDirectory (File &tiff, Buffer &code)
{
	XBYTE2 twobytes;
	XBYTE4 fourbytes;
		
	twobytes = code.WriteI (TIFF_SAVE_ENTRIES);
	tiff.Write2 (twobytes);

	switch (m_mode) {
	case TIFF_MODE_BW: m_bpc[CHAN_GRAY] = 1; break;
	case TIFF_MODE_GRAY: m_bpc[CHAN_GRAY] = m_bpp; break;
	case TIFF_MODE_RGB: {
		m_bpc[CHAN_RED] = m_bpp/3;
		m_bpc[CHAN_GREEN] = m_bpp/3;
		m_bpc[CHAN_BLUE] = m_bpp/3;
		m_bpc[CHAN_ALPHA] = 0;
	} break;
	case TIFF_MODE_RGBA: {
		m_bpc[CHAN_RED] = m_bpp/4;
		m_bpc[CHAN_GREEN] = m_bpp/4;
		m_bpc[CHAN_BLUE] = m_bpp/4;
		m_bpc[CHAN_ALPHA] = m_bpp/4;
	} break;
	}
	
	SaveTiffEntry (tiff, code, TIFF_TAG_NEWSUBTYPE);
	SaveTiffEntry (tiff, code, TIFF_TAG_IMAGEWIDTH);
	SaveTiffEntry (tiff, code, TIFF_TAG_IMAGEHEIGHT);
	SaveTiffEntry (tiff, code, TIFF_TAG_BITSPERSAMPLE);
	SaveTiffEntry (tiff, code, TIFF_TAG_COMPRESSION);
	SaveTiffEntry (tiff, code, TIFF_TAG_PHOTOMETRIC);
	SaveTiffEntry (tiff, code, TIFF_TAG_STRIPOFFSETS);
	SaveTiffEntry (tiff, code, TIFF_TAG_SAMPLESPERPIXEL);	
	SaveTiffEntry (tiff, code, TIFF_TAG_ROWSPERSTRIP);
	SaveTiffEntry (tiff, code, TIFF_TAG_STRIPBYTECOUNTS);
	SaveTiffEntry (tiff, code, TIFF_TAG_XRES);
	SaveTiffEntry (tiff, code, TIFF_TAG_YRES);
	SaveTiffEntry (tiff, code, TIFF_TAG_RESUNIT);
	SaveTiffEntry (tiff, code, TIFF_TAG_COLORMAP);

	fourbytes = code.WriteL(0);
	tiff.Write4(fourbytes);	

	SaveTiffExtras (tiff, code, TIFF_TAG_BITSPERSAMPLE);
	SaveTiffExtras (tiff, code, TIFF_TAG_XRES);
	SaveTiffExtras (tiff, code, TIFF_TAG_YRES);

	SaveTiffData (tiff, code);

	return true;
}

// Function: SaveTiff
//
// Input:
//		m_filename			Name of file to save
//		m_mode				Format mode (BW, GRAY, RGB, INDEX)
//		m_compress			Compression mode
//		m_bpp				Bits per pixel
// Output:
//		m_status

bool ImageExt::SaveTiff (char *filename, ImageX *img)
{
	if(img == NULL) return false;

	if(img->m_usage & LOAD_ALPHA || img->m_usage & LOAD_MASKED)
		m_mode = TIFF_MODE_RGBA;
	else
		m_mode = TIFF_MODE_RGB;
	m_compress = TIFF_COMPRESS_NONE;
	m_bpp = 32;

	XBYTE2 twobytes;
	XBYTE4 fourbytes;

	File tiff;
	Buffer code;

	m_pImg = img;
	strcpy (m_filename, filename);

	if(tiff.Open (m_filename, FILE_WRITE | FILE_RANDOM | FILE_AUTOCREATE) != FILE_STATUS_OK)
		return false;

	m_xres = m_pImg->m_xres;
	m_yres = m_pImg->m_yres;

	twobytes = (XBYTE2) TIFF_BYTEORDER;		// Byte Order LBF	
	tiff.Write2 (twobytes);
	
	twobytes = code.WriteI (TIFF_MAGIC);	// Magic Number
	tiff.Write2 (twobytes);

	fourbytes = code.WriteL (TIFF_SAVE_POSIFD);	// IFD Offset
	tiff.Write4 (fourbytes);
	
	SaveTiffDirectory (tiff, code);			// Write IFD
	
	tiff.Close ();
	m_status = TIFF_STATUS_OK;	
	
	return true;
}
