//
// GameX - BMP/JPG/GIF-Handling Code
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
//
//**************************************
//
// BMP Format - File Format support for ImageExtender class
//
// NOTE: CURRENT CAPABILITIES:
//		- Load supports:
//					 8 bit indexed color    (no alpha)
//					16 bit X1-R5-G5-B5 (without alpha)
//					24 bit    R8-G8-B8 (without alpha)
//					32 bit A8-R8-G8-B8 (with alpha)
//		- Save supports:
//					16 bit X1-R5-G5-B5 (without alpha)
//					32 bit A8-R8-G8-B8 (with alpha)
//
//	Other Formats: Can load JPG and GIF files (without alpha)
//
//**************************************

#include "gamex-image-bmp.hpp"

#include <olectl.h> // required for JPG/GIF reading

// (also need to link with ole32.lib and oleaut32.lib)
#pragma comment(lib,"ole32.lib") // auto-link ole32.lib
#pragma comment(lib,"oleaut32.lib ") // auto-link oleaut32.lib 

char last_filename [MAX_PATH];


// Function: LoadBMP
//
//		filename			Name of BMP file to load
//		img					ImageX object to fill with image data from bitmap 

bool ImageExt::LoadBMP (char *filename, ImageX *img)
{
	if(strcmp(last_filename, filename)==0)  // sometimes Windows inserts GUI element graphics where the BMP should be
		Sleep(120); // when loading the same file twice in a row without waiting a bit in-between

	m_pImg = img;
	strcpy (m_filename, filename);
	strcpy (last_filename, filename);

	// actually load the bitmap from file:
	HBITMAP hbmp = (HBITMAP) LoadImage(NULL,filename,IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	if(hbmp == NULL) {
		return false;
	} else {
		bool retVal = InterpretBits(hbmp); // interpret the bitmap data and return whether we succeeded:
		DeleteObject(hbmp);
		return retVal;
	}
}

// can load JPEG and GIF files (can also load BMP, WMF, and TIF, if they are in 24 bit format) 
bool ImageExt::LoadJPGorGIF (char *filename, ImageX *img)
{
	m_pImg = img;
	strcpy (m_filename, filename);

	IPicture * picture;
	IStream * stream;
	HGLOBAL hGlobal;
	FILE * file;

	file = fopen(filename,"rb"); // open file in read only mode
	if(file == NULL)
		return false;

	fseek(file,0,SEEK_END);
	int file_size = ftell(file);
	fseek(file,0,SEEK_SET);
	hGlobal = GlobalAlloc(GPTR, file_size); // allocates 112 k
	if(hGlobal == NULL) {
		fclose(file);
		return false;
	}
	fread((void*)hGlobal, 1, file_size, file);
	fclose(file);

	CreateStreamOnHGlobal(hGlobal,false,&stream);
	if(stream == NULL) {
		GlobalFree(hGlobal);
		return false;
	}

	// Decompress and load the JPG or GIF
	OleLoadPicture(stream,0,false,IID_IPicture,(void**)&picture); // 42,804 -> 43,844

	if(picture == NULL) { // allocates 1052 k
		stream->Release();
		GlobalFree(hGlobal);
		return false;
	}
	stream->Release();
	GlobalFree(hGlobal); // deallocates 52 k

	HBITMAP hB = 0;
	picture->get_Handle((unsigned int*)&hB);
	if(hB == 0) return false;

	HBITMAP hbmp = (HBITMAP)CopyImage(hB,IMAGE_BITMAP,0,0,LR_COPYRETURNORG);
	if(hbmp == 0) return false;

	picture->Release(); // deallocates 1024 k

	bool retVal = InterpretBits(hbmp);

	if(hB) DeleteObject(hB);
	if(hbmp) DeleteObject(hbmp);

	return retVal;
}



bool ImageExt::InterpretBits(HBITMAP hbmp)
{
	HBITMAP hbmp2;
	bool noMerge = false;
	bool ok = false;
	if(hbmp) {
		BITMAP bitmap;
		LPVOID bits;
		GetObject(hbmp,sizeof(bitmap),&bitmap);

		if(bitmap.bmBits != NULL) {
			// if the bits are conveniently where one would expect them to be (BMP)
			bits = bitmap.bmBits;
		} else {
			// if we have to do a little more work to locate the bits (JPG, GIF)
			int success = false;
			HDC hdc = CreateCompatibleDC(NULL);	// DC for Source Bitmap
			if(hdc) {
				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc,hbmp);
				// here hdc contains the bitmap
					
				HDC hdc2 = CreateCompatibleDC(NULL); // DC for working
				if (hdc2) {
					// get bitmap size
					BITMAP bm;
					GetObject(hbmp, sizeof(bm), &bm);
							
					BITMAPINFO bitmapinfo; 
					ZeroMemory(&bitmapinfo, sizeof(BITMAPINFO));
					bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bitmapinfo.bmiHeader.biWidth = bm.bmWidth;
					bitmapinfo.bmiHeader.biHeight = bm.bmHeight;
					bitmapinfo.bmiHeader.biPlanes = bm.bmPlanes;
					bitmapinfo.bmiHeader.biBitCount = 32;
					noMerge = true; // prevent from assuming 32-bit means alpha information

					UINT * pixels;	
					hbmp2 = CreateDIBSection(hdc2, (BITMAPINFO *)&bitmapinfo, 
													DIB_RGB_COLORS, (void **)&pixels, 
													NULL, 0);
					if(hbmp2) {
						HBITMAP hOldBitmap2 = (HBITMAP)SelectObject(hdc2, hbmp2);
						BitBlt(hdc2,0,0,
							   bm.bmWidth,bm.bmHeight,
							   hdc,0,0,SRCCOPY); // 42,768 -> 44,816
						SelectObject(hdc2, hOldBitmap2);
						success = true;
						bits = pixels; 
						bitmap.bmBitsPixel = bitmapinfo.bmiHeader.biBitCount;
						m_pImg->m_xres = bm.bmWidth;
						m_pImg->m_yres = bm.bmHeight;
						bitmap.bmWidthBytes = m_pImg->m_xres*bitmap.bmBitsPixel/8;
					}
				}
				SelectObject(hdc,hOldBitmap);
				if(hdc2) DeleteDC(hdc2); 
			}
			if(hdc) DeleteDC(hdc); 

			if(!success) {
///				if(bits) DeleteObject((HGDIOBJ) bits);
				return false;
			}
		}

/*		if(bitmap.bmWidth > 2048 || bitmap.bmHeight > 2048) {
			char errstr [256];
			sprintf(errstr,"The %dx%d image in file %s is too large for GameX to use.\nNeither width nor height may exceed 2048 pixels.",bitmap.bmWidth,bitmap.bmHeight,m_filename);
			MessageBox (NULL, errstr, "GameX Error", MB_OK|MB_ICONSTOP);
			if(bitmap.bmWidth > 2048) bitmap.bmWidth = 2048;
			if(bitmap.bmHeight > 2048) bitmap.bmHeight = 2048;
		}
*/
		int pitch = bitmap.bmWidthBytes;

		switch(bitmap.bmBitsPixel)
		{
		case 8: {
			HDC hdc = CreateCompatibleDC(NULL);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, hbmp);
			RGBQUAD palette[256];
			GetDIBColorTable(hdc, 0, 256, palette);
			m_pImg->Size(bitmap.bmWidth, bitmap.bmHeight, IMG_TRUECOLOR);
			XBYTE * pData = (XBYTE *) m_pImg->m_data;
			XBYTE * bmpData = (XBYTE *) bits;
			for(int y = 0 ; y < m_pImg->m_yres ; y++)
			for(int x = 0 ; x < m_pImg->m_xres ; x++) {
				int n = (x+y*m_pImg->m_xres)*3; // pixel address -- destination
				int yi = m_pImg->m_yres-y-1; // inverted y, because bmp's are stored upside-down
				int ni = x+yi*pitch; // inverted pixel address -- source
				pData[n+2] = palette[bmpData[ni]].rgbBlue;   // blue
				pData[n+1] = palette[bmpData[ni]].rgbGreen; // green
				pData[n]   = palette[bmpData[ni]].rgbRed; // red
			}
			SelectObject(hdc, hOldBitmap);
			if(hdc) DeleteDC(hdc);
			break;
		}
		case 16: {
			int bmp_mask_red = 0x7C00, bmp_mask_green = 0x03E0, bmp_mask_blue = 0x001F;
			int bmp_shift_red, bmp_shift_green;
 
			bmp_shift_red   = GameX.GetShift(bmp_mask_red);
			bmp_shift_green = GameX.GetShift(bmp_mask_green);

			m_pImg->Size(bitmap.bmWidth, bitmap.bmHeight, IMG_HIGHCOLOR);
			XBYTE2 * pData = (XBYTE2 *) m_pImg->m_data;
			XBYTE2 * bmpData = (XBYTE2 *) bits;
			for(int y = 0 ; y < m_pImg->m_yres ; y++)
			for(int x = 0 ; x < m_pImg->m_xres ; x++) {
				int n = x+y*m_pImg->m_xres; // pixel address -- destination
				int yi = m_pImg->m_yres-y-1; // inverted y, because bmp's are stored upside-down
				int ni = x+yi*pitch/2; // inverted pixel address -- source
				int clr = bmpData[ni] ;
				int blue  = clr & bmp_mask_blue;  // decode blue
				int green = (clr & bmp_mask_green) >> (bmp_shift_green-1);  // decode green
				int red   = (clr & bmp_mask_red) >> bmp_shift_red; // decode red
				pData[n] = ((red << 11) + (green << 5) + blue); // re-encode
			}
		 } break;
		case 24: {
			m_pImg->Size(bitmap.bmWidth, bitmap.bmHeight, IMG_TRUECOLOR);
			XBYTE * pData = (XBYTE *) m_pImg->m_data;
			XBYTE * bmpData = (XBYTE *) bits;
			for(int y = 0 ; y < m_pImg->m_yres ; y++)
			for(int x = 0 ; x < 3*m_pImg->m_xres ; x+=3) {
				int n = x+y*m_pImg->m_xres*3; // pixel address -- destination
				int yi = m_pImg->m_yres-y-1; // inverted y, because bmp's are stored upside-down
				int ni = x+yi*pitch; // inverted pixel address -- source
				pData[n+2] = bmpData[ni];   // blue // (not only is it upside-down, but RGB is backwards)
				pData[n+1] = bmpData[ni+1]; // green
				pData[n]   = bmpData[ni+2]; // red
			}
		 } break;
		case 32: {
			if(noMerge) m_pImg->Size(bitmap.bmWidth, bitmap.bmHeight, IMG_TRUECOLORX);
			else        m_pImg->Size(bitmap.bmWidth, bitmap.bmHeight, IMG_TRUECOLORX|IMG_MERGEALPHA);
			XBYTE * pData = (XBYTE *) m_pImg->m_data;
			XBYTE * bmpData = (XBYTE *) bits;
			for(int y = 0 ; y < m_pImg->m_yres ; y++)
			for(int x = 0 ; x < 4*m_pImg->m_xres ; x+=4) {
				int n = x+y*m_pImg->m_xres*4; // pixel address -- destination
				int yi = m_pImg->m_yres-y-1; // inverted y, because bmp's are stored upside-down
				int ni = x+yi*pitch; // inverted pixel address -- source
				pData[n+3] = bmpData[ni+3]; // alpha
				pData[n+2] = bmpData[ni+2]; // red
				pData[n+1] = bmpData[ni+1]; // green
				pData[n]   = bmpData[ni]; // blue
			}


		 } break;
		}
		DeleteObject(hbmp2);
		ok = true;
	}

	return ok;
}


bool ImageExt::SaveBmp(char *filename, HBITMAP hBitmap)
{
	bool ok = true;
	BITMAPINFO bmpInfo;
	ZeroMemory (&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	HDC hdc = GetDC(NULL);
	GetDIBits (hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS); 
	if(bmpInfo.bmiHeader.biSizeImage <= 0)
		bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth * abs(bmpInfo.bmiHeader.biHeight) * (bmpInfo.bmiHeader.biBitCount+7) / 8;
	LPVOID pixelBuffer = malloc(bmpInfo.bmiHeader.biSizeImage);
	if(pixelBuffer != NULL) {
		bmpInfo.bmiHeader.biCompression=BI_RGB;
		GetDIBits(hdc,hBitmap,0,bmpInfo.bmiHeader.biHeight,pixelBuffer, &bmpInfo, DIB_RGB_COLORS);
		FILE* file = fopen(filename,"wb");
		if(file != NULL) {
			BITMAPFILEHEADER bmpFileHeader;
			bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); 
			bmpFileHeader.bfReserved1 = 0;
			bmpFileHeader.bfReserved2 = 0;
			bmpFileHeader.bfSize = bmpFileHeader.bfOffBits + bmpInfo.bmiHeader.biSizeImage;
			bmpFileHeader.bfType = 'MB';
			fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,file);
			fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,file);
			fwrite(pixelBuffer,bmpInfo.bmiHeader.biSizeImage,1,file); 
		} else {
			debug.Output("ImageExt::SaveBmp: Unable to Create Bitmap File");
			ok = false;
		}
		if(file) fclose(file);
	} else {
		debug.Output("ImageExt::SaveBmp: Unable to Allocate Bitmap Memory");
		ok = false;
	}
	if(hdc) ReleaseDC(NULL,hdc); 
	if(pixelBuffer) free(pixelBuffer); 
	return ok;
}


