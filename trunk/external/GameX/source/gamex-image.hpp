//
// GameX - ImageX Class Header 
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
										// **** Image includes
#include "gamex-buffer.hpp"
#include "gamex-file.hpp"
#include "gamex-win-dx.hpp"
#include "gamex-defines.hpp"

#ifndef IMAGEX_DEF
	#define	IMAGEX_DEF
										// **** Image Switches
	#define IMAGEX_DEBUG				// Image debugging (bounds, null data, etc.)
	//#define IMAGEX_SUPPORT_RAW		// Image support for RAW file load/save
	#define IMAGEX_SUPPORT_TIFF			// Image support for TIFF file load/save
	#define IMAGEX_SUPPORT_BMP			// Image support for BMP file load/save
	
	typedef unsigned char		ImageOps;	

	class ImageExt; // forward referencing

	class ImageX {
	public:		
		ImageX ();
		~ImageX ();

		//**** Loading or Initializing the Image:
		// (NOTE: unless otherwise noted, bool values returned by these methods mean success if true, failure if false)

		// Loads an image from file, with alpha from another file, into this ImageX
		bool Load (char * filename, char * alphafilename);

		// Loads an image from file into this ImageX
		bool Load (char * filename, bool alpha=false);

		bool LoadIntoAlpha (char * filename); // loads a grayscale image into this image's alpha channel

		// Creates a blank image to be drawn after you draw into it
		void Create (int xr, int yr, bool alpha=false);
		void Destroy (void); // deallocates image memory

		// Saves the image to file
		bool Save (char * filename);

		// Saves the image to the clipboard
		bool CopyToClipboard (void);

		// Getting basic image properties:

		inline int GetWidth (void)	{return m_xres;}
		inline int GetHeight (void) {return m_yres;}
		inline ImageUsage GetUsage (void) {return m_usage;}

		void Resize (int new_xres, int new_yres);
		void ConvertUsageTo (ImageUsage use); // converts image usage -- is slow, avoid using

		// Get or set image's pixels:
		// NOTE: These are very slow when called more than once.
		//       To improve speed, convert calls of SetPixel to the more efficient
		//       DrawPixel commands surrounded by BeginDrawingPixels and EndDrawingPixels

		void SetPixel (int x, int y, int r, int g, int b, int a=255); 
		void GetPixel (int x, int y, int &r, int &g, int &b) {int a; GetPixel(x,y,r,g,b,a);} // same as previous comment
		void GetPixel (int x, int y, int &r, int &g, int &b, int &a); // same as previous comment

		// Basic filters that you can run on images before drawing them:
		// NOTE: these are too slow to call every frame, but they're good for preparing images

		void Fill (int r, int g, int b, int a=255); // fills image with r, g, b, a out of 255
		void Invert (bool preserve_color = false); // inverts the image
		void ChangeColors(ColorSetType a_settype, ColorSetType b_settype, float a_set_mult=1.0f, float b_set_mult=1.0f, float a_app_alpha=1.0f, float b_app_alpha=1.0f, ColorSetCondition a_acond=COLOR_COND_ANYTHING, ColorSetCondition a_bcond=COLOR_COND_ANYTHING, ColorSetCondition b_acond=COLOR_COND_ANYTHING, ColorSetCondition b_bcond=COLOR_COND_ANYTHING); // advanced color manipulation
		void SwapColors (ColorSwapType type); // rotates or swaps the image's color values
		void SetChannel (int channel, int value); // sets all of one channel to one value -- value is out of 255, channel is 0 for red, 1 for green, 2 for blue, 3 for alpha
		void ChangeBrightness (int brightness, float contrast); // changes brightness and contrast -- 0 brightness and 1 contrast means no change
		void ChangeBrightnesses (int brightness_r, int brightness_g, int brightness_b, int brightness_a, float contrast_r, float contrast_g, float contrast_b, float contrast_a); // changes brightness and contrast of each color component
		void ChangeSaturation (float sat); // 0.0f means grayscale, 1.0f means normal, 1.5f means extra color, etc.
		void ChangeSaturations (float sat_r, float sat_g, float sat_b); // reduces or enhances color while keeping brightness the same
		void ConvertToGrayscale (void);
		void AddNoise (int noise); // adds random noise to the image's red, green, and blue
		void AddNoise (int noise_r, int noise_g, int noise_b, int noise_a=0); // adds random noise to the image
		void GaussianBlur (float radius);
		void GaussianBlur (float red_radius, float green_radius, float blue_radius, float alpha_radius=0.0f);

		// Image copying functions (slow, but these don't require LOAD_TARGETABLE)
		void CopyTo (ImageX * dest);
		void CopyToAlpha (ImageX * dest);

		// Rotates image into a buffer image (slow; here for backwards compatibility)
		// Requires that the destination image is loaded with LOAD_TARGETABLE
		void Rotate (ImageX * dest, float ang, float scale=1.0f);

		// Specify specific part of the image to filter:
		void SetFilterRect(int xLeft, int yTop, int xRight, int yBottom) {m_fx1=xLeft; m_fy1=yTop; m_fx2=xRight; m_fy2=yBottom;}
		void ResetFilterRect(void) {m_fx1 = 0; m_fy1 = 0; m_fx2 = m_xres; m_fy2 = m_yres;}

		void ConvertRGBToLab(int r, int g, int b, int &L, int &A, int &B); // converts one RGB color to LAB
		void ConvertLabToRGB(int L, int A, int B, int &r, int &g, int &b); // converts one LAB color to RGB

 		// Functions that only ImageX, ImageExt, and WindowsDX should call:
		
		void Size (int xr, int yr, ImageOps ops = IMG_HIGHCOLOR);	
		inline void TurnOn (ImageOps opt);					// Turn on a PixelFormat option
		inline void TurnOff (ImageOps opt);					// Turn off a PixelFormat option
		inline int GetBytesPerPixel (ImageOps ops);			// Get Bytes-Per-Pixel
		inline int GetFormat (ImageOps ops);				// Get Format 
		inline int GetAlpha (ImageOps ops);					// Get Alpha
		void UseWith (ImageUsage use); // Used internally only; use ConvertUsageTo instead.
		void DeleteData (void);
		void Reset (void);
		void Restore (void);

		// Variables that only ImageX, ImageExt, and WindowsDX should access:

		LPDIRECTDRAWSURFACE7 m_surface;						// Color+Alpha Data
		int m_hassurface; // whether or not this image has a surface / texture at the moment
		int m_lost_counter; // how many times the surface has been restored
		int m_surf_width, m_surf_height, m_surf_bpp; // texture dimension for accurate mapping
		LPDIRECT3DDEVICE7 m_device; // D3D accelerator
		XBYTE * m_data;										// Color Data (gets copied into m_surface)
		XBYTE * m_alpha;									// Alpha Channel (gets copied into m_surface)
		int m_xres, m_yres, m_size;							// Resolution/Size info, size=xres*yres
		ImageUsage m_usage;									// Usage info
		ImageOps m_options;									// Pixel Format
		char status;										// Image Ready?
		char m_filename [MAX_PATH];							// Filename of ImageX
		int	m_ytable[4096+32]; // used for speed when drawing pixels into the image
		int m_fx1, m_fy1, m_fx2, m_fy2; // filter rectangle
		ImageX * m_internal_compatibility_img[3]; // used for certain video card workarounds
		float m_xmult, m_ymult; // resolution multipliers -- 0.5 means image is stored at half resolution, for example
		bool m_non_empty; // true if a file was loaded into this image or if its pixels were accessed by GameX.AccessPixels (which everything that can change the image calls), false otherwise

	private:

		// Functions that only ImageX should call:

		void DataToSurface (void);
///		void RotateImage (ImageX *dest, float ang, float scale, bool toVideo = false);
///		void RotateAlpha (ImageX *dest, float ang, float scale);
		inline void AddUsage (ImageUsage u) {m_usage |= u;}
		inline void RemoveUsage (ImageUsage u) {m_usage ^= u;}
		void ConvertFormat (ImageOps pf);
		void ConvertAlphaToMask (void);
		void ConvertMaskToAlpha (void);
//		void ConvertAlphaToBlend (void);
		void AllocateSurface (void);
		void CreateWithUse (int xr, int yr, ImageUsage use=0);
		void PreservedRecreateAs (int new_xres, int new_yres, ImageUsage new_use);
	};

	class ImageExt {
	public:
		ImageExt ();
		ImageExt (ImageX* pImg);
		~ImageExt ();

		// Loads an image from file into an ImageX object
		bool Load (char* filename, ImageX* img, bool alpha=false);

		// Saves an ImageX object into a file
		bool Save (char* filename, ImageX* img);

		// Saves an ImageX object into the clipboard
		bool CopyToClipboard (ImageX* img);

	private:
		bool LoadWithUse (char* filename, ImageX* img, ImageUsage use=0); // calls other loading functions as appropriate

		bool LoadTiff (char *filename, ImageX *img); // implemented in gamex-image-tiff.cpp
		bool SaveTiff (char *filename, ImageX *img); // implemented in gamex-image-tiff.cpp
		bool LoadBMP (char *filename, ImageX *img); // implemented in gamex-image-bmp.cpp
		bool SaveBmp (char *filename, HBITMAP hBitmap); // implemented in gamex-image-bmp.cpp
		bool LoadJPGorGIF (char *filename, ImageX *img); // currently implemented in gamex-image-bmp.cpp

		// helper methods for LoadTiff
		bool LoadTiffDirectory (File &tiff, Buffer &code);
		bool LoadTiffEntry (File &tiff, Buffer &code);
		bool LoadTiffStrips (File &tiff, Buffer &code);
		bool LoadTiffData (File &tiff, Buffer &code, unsigned long count, unsigned long offset, int row);

		// helper methods for SaveTiff:
		bool SaveTiffDirectory (File &tiff, Buffer &code);
		bool SaveTiffEntry (File &tiff, Buffer &code, unsigned int tag);
		bool SaveTiffExtras (File &tiff, Buffer &code, unsigned int tag);
		bool SaveTiffData (File &tiff, Buffer &code);
		
		// helper method for LoadBMP and LoadJPGorGIF:
		bool InterpretBits(HBITMAP hbmp);

		ImageX*				m_pImg;					// Pointer to an ImageX		
		char				m_filename[MAX_PATH];	// Filename of ImageX
		int					m_status;				// Status of ImageX
		
		#ifdef IMAGEX_SUPPORT_RAW					// RAW Format Info
			int				m_bpp;
			int				m_channel;
			int				m_header;
		#endif

		#ifdef IMAGEX_SUPPORT_TIFF					//**** TIFF Format Info
			int				m_xres, m_yres;			// TIFF Resolution
			int				m_ops;					// TIFF Pixel-Format Options for ImageX
			int				m_mode;					// TIFF Mode
			int				m_alpha;				// TIFF Alpha Channel Present?
			int				m_num_chan;				// TIFF Number of Channels
			unsigned long	m_bpcoff;				// TIFF 
			int				m_bpc[5];				// TIFF Bits per Channel
			int				m_compress;				// TIFF Compressed?
			int				m_photo;
			unsigned long	m_num_strips;			// TIFF Number of Strips
			unsigned long	m_strip_offsets;		// TIFF Strip Offsets
			unsigned long	m_strip_counts;			// TIFF Strip Counts
			int				m_rps;					// TIFF Rows per Strip
			int				m_bpp;					// TIFF Bits per Pixel
			int				m_bpr;					// TIFF Bits per Row
		#endif
	};
#endif
