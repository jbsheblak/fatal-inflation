//
// GameX - ImageX Class Code 
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
// 

#include <math.h>
#include <string.h>

#include "gamex-image.hpp"

ImageX::ImageX (void)
{
	m_data = NULL;
	m_alpha = NULL;
	m_surface = NULL;
	for(int i=0;i<3;i++) m_internal_compatibility_img[i] = NULL;
	Reset();
}

ImageX::~ImageX (void)
{
	Destroy();
}

void ImageX::Reset (void)
{
	m_xres = 0;
	m_yres = 0;
	m_size = 0;
	m_options = IMG_UNDEF;
	DeleteData(); // delete the non-surface data
	m_usage = DRAW_UNDEF;
	status = IMG_NOTREADY;
	m_hassurface = 0;
	sprintf(m_filename, "Uninitialized ImageX");
	m_surface = NULL;
	m_device = NULL;
	ResetFilterRect();
	m_non_empty = false;
}

// Loads an image from file into this ImageX
bool ImageX::Load (char * filename, bool alpha)
{
	ImageExt ie;
	return ie.Load(filename,this,alpha);
}

bool ImageX::Load (char * filename, char * alphafilename)
{
	bool ok1 = Load(filename,true);
	if(!ok1) {
		bool ok3 = Load(alphafilename,true);
		if(!ok3) return false;
		else ChangeBrightness(-255,0);
	}
	bool ok2 = LoadIntoAlpha(alphafilename);
	return (ok1 && ok2);
}

bool ImageX::LoadIntoAlpha (char * filename)
{
	GameX.SetStatus(GAMEX_CREATING_TEST_SURFACE); // prevent GameX from registering as user-created
	ImageX temp_img;
	bool ok = temp_img.Load(filename);
	if(ok) temp_img.CopyToAlpha(this);
	GameX.SetStatus(GAMEX_READY);
	return ok;
}

// Creates a blank image, to be drawn after you draw into it
void ImageX::Create (int xr, int yr, bool alpha)
{
	ImageUsage use = (alpha) ? (LOAD_ALPHA) : 0;
	CreateWithUse(xr,yr,use);
}

void ImageX::CreateWithUse (int xr, int yr, ImageUsage use)
{
	Destroy();
	Reset();

	strcpy(m_filename,"Created ImageX");

	#ifdef WINDX_DEBUG
		if(GameX.GetStatus() == GAMEX_READY)
			debug.Output("Created Image:                                  ",""); 
	#endif
	switch (use) {
		case LOAD_ALPHA: Size (xr, yr, IMG_TRUECOLORX | IMG_MERGEALPHA); break;	
		default: Size (xr, yr, IMG_HIGHCOLOR); break;
	}
	UseWith(use);
	Fill(0,0,0,0);
	m_non_empty = false;
}

void ImageX::Destroy (void)
{
	if(m_surface != NULL) {
		if(GameX.GetDD()) { // if we haven't already released all of DirectDraw
			GameX.MemoryRecordChange(-m_surf_width*m_surf_height*(m_surf_bpp/8)/1024);
			m_surface->Release(); // release the surface data
		}
		m_surface = NULL;
	}
	for(int i=0;i<3;i++)
	if(m_internal_compatibility_img[i] != NULL) {
		m_internal_compatibility_img[i]->Destroy();
		m_internal_compatibility_img[i] = NULL;
	}
	if(m_device != NULL) {
		if(GameX.GetDD()) { // if we haven't already released all of DirectDraw
			m_device->Release();
		}
		m_device = NULL;
	}
	Reset();
}

void ImageX::DeleteData (void)
{
	if(m_data!=NULL) delete [] m_data, m_data = NULL;
	if(m_alpha!=NULL) delete [] m_alpha, m_alpha = NULL;
}

bool ImageX::Save (char * filename)
{
	ImageExt ie;
	return ie.Save(filename,this);
}

void ImageX::ConvertUsageTo (ImageUsage use)
{
	if(use == m_usage) return;
	PreservedRecreateAs(m_xres, m_yres, use);
}

void ImageX::Resize (int new_xres, int new_yres)
{
	if(new_xres == m_xres && new_yres == m_yres) return;
	PreservedRecreateAs(new_xres, new_yres, m_usage);
}

void ImageX::PreservedRecreateAs (int new_xres, int new_yres, ImageUsage new_use)
{
	GameX.SetStatus(GAMEX_CREATING_TEST_SURFACE); // (prevent GameX from registering as user-created)
	if(m_non_empty) {
		ImageX temp_img;
		temp_img.CreateWithUse(new_xres, new_yres, new_use);
		CopyTo(&temp_img);
		CreateWithUse(temp_img.m_xres, temp_img.m_yres, temp_img.m_usage);
		temp_img.CopyTo(this);
	} else {
		CreateWithUse(new_xres, new_yres, new_use);
	}
	GameX.SetStatus(GAMEX_READY);
}

void ImageX::AllocateSurface ()
{
	bool textureSurface = !(m_usage & LOAD_NON3D);
	int alphaSurface = ((m_usage & LOAD_ALPHA) && (m_options & IMG_ALPHA) && !(m_usage & LOAD_MASKED)) ? 2 : 0;
	bool targetable = (m_usage & LOAD_TARGETABLE) ? true : false;
	m_surface = GameX.CreateImageSurface(m_xres, m_yres, textureSurface, alphaSurface, targetable);

	if(m_surface != NULL) {

		DDPIXELFORMAT ddpf;
		ZeroMemory (&ddpf, sizeof(DDPIXELFORMAT)) ;
		ddpf.dwSize = sizeof(ddpf);
		m_surface->GetPixelFormat(&ddpf); // get the pixel format DirectDraw made the surface in,
		m_surf_bpp = ddpf.dwRGBBitCount; // usually this is win_screenbpp or 32 for alpha, but it's not guaranteed

		if(targetable && textureSurface) {
			// this will probably get the best SOFTWARE 3D rendering we have,
			// since the hardware device is likely to be tied up by the back buffer,
			// This is not the way to enable fastest drawing to surfaces, but in DX7 it seems to be the easiest and most compatible way
			if(GameX.CreateDeviceForSurface(m_device, m_surface)) {

				m_device->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP); // we don't want texture wrapping
				m_device->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP); // that doesn't work right on most image sizes anyway
				m_device->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_MODULATE);
				m_device->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_LINEAR);
				m_device->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_LINEAR);

				m_device->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE); // we have no D3D light sources, for now
				m_device->SetRenderState(D3DRENDERSTATE_COLORVERTEX, TRUE); // we want to allow per-vertex coloring
				m_device->SetRenderState(D3DRENDERSTATE_ANTIALIAS, FALSE); // set antialiasing off				
				m_device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE); // turn on alpha blending
				m_device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE); // and dithering

			}
		}

		m_hassurface = (textureSurface) ? 2 : 1; // success: 1 for normal, 2 for 3D accelerated

		DDSURFACEDESC2 ddsd;	
		ZeroMemory (&ddsd, sizeof (ddsd)) ;
		ddsd.dwSize = sizeof (ddsd) ;
		m_surface->GetSurfaceDesc(&ddsd);
		m_surf_width = ddsd.dwWidth;
		m_surf_height = ddsd.dwHeight;

		if(m_surf_width < m_xres)
			m_xmult = (float)m_surf_width / (float)m_xres;
		if(m_surf_height < m_yres)
			m_ymult = (float)m_surf_height / (float)m_yres;

		int bpp = GetBytesPerPixel(m_options);
		if(alphaSurface==2)
			bpp = 4; // may not have converted it yet (such aas masked to 32-bit alpha yet)
		int adjusted_pitch = ddsd.lPitch / bpp;
		for (int y=0; y<=max(m_yres,m_surf_height); y++)
			m_ytable[y] = (int)((float)y * m_ymult) * adjusted_pitch;

		RECT boundary[1] = {0,0,m_xres,m_yres};
		LPDIRECTDRAWCLIPPER clipper;
		LPRGNDATA rgn_data;
		if (FAILED(GameX.GetDD()->CreateClipper(0,&clipper,NULL))) {
			return;
		}
		int rectSize = sizeof(RECT);
		int rgnHeaderSize = sizeof(RGNDATAHEADER);
		rgn_data = (LPRGNDATA)malloc(rgnHeaderSize+rectSize);
		memcpy(rgn_data->Buffer, boundary, rectSize);
		rgn_data->rdh.dwSize = rgnHeaderSize;
		rgn_data->rdh.nRgnSize = rectSize;
		rgn_data->rdh.nCount = 1;
		rgn_data->rdh.iType = RDH_RECTANGLES;
		rgn_data->rdh.rcBound = boundary[0];
		if (FAILED(clipper->SetClipList(rgn_data,0)) || FAILED(m_surface->SetClipper(clipper))) {
		 	free(rgn_data);
			return;
		}
		free(rgn_data);
	} else {
		m_hassurface = 0; // failure
	}
}

void ImageX::DataToSurface (void)
{
	// copy the pixels onto the surface, so that the surface can be blitted for us later
	if(m_hassurface == NULL || m_data == NULL) {
		return;
	}

	DDSURFACEDESC2 ddsd;	
	ZeroMemory (&ddsd, sizeof (ddsd)) ;
	ddsd.dwSize = sizeof (ddsd) ;

 	HRESULT status = m_surface->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL); 	// lock drawing surface
	if(status != DD_OK) {
		GameX.DirectDrawError(status);
		return;
	}

	int xres = m_xres;
	int yres = m_yres;

	if(m_xmult != 1.0f || m_ymult != 1.0f) {

		// Scale the image's pixel data to reduce image resolution:

		int bytes_per_pix = GetBytesPerPixel(m_options);
		int bytes_per_alp = (m_alpha != NULL) ? 1 : 0;

		XBYTE *pData  = (XBYTE *) m_data; // pointer to data write pixel
		XBYTE *pDataO  = (XBYTE *) m_data; // pointer to data read pixel
		XBYTE *pAlpha = (XBYTE *) m_alpha; // pointer to alpha write pixel
		XBYTE *pAlphaO = (XBYTE *) m_alpha; // pointer to alpha read pixel

		int adj_pix_pitch = m_xres * bytes_per_pix;
		int adj_alp_pitch = m_xres;

		float yConvCounter = 0.0f;
		for(int sy = 0, dy = 0 ; sy < m_yres ; sy++) {
			float xConvCounter = 1.0f;
			if(yConvCounter >= 1.0f) {
				yConvCounter -= 1.0f;
				pData += adj_pix_pitch;
				pAlpha += adj_alp_pitch;
				dy++;
			}
			for(int sx = 0, dx = 0 ; sx < m_xres ; sx++) {
				if(xConvCounter >= 1.0f) {
					xConvCounter -= 1.0f;
					//copy
					for(int i = 0 ; i < bytes_per_pix ; i++)
						pData[dx*bytes_per_pix+i] = pDataO[sx*bytes_per_pix+i];
					for(int j = 0 ; j < bytes_per_alp ; j++)
						pAlpha[dx*bytes_per_alp+j] = pAlphaO[sx*bytes_per_alp+j];
					dx++;
				}
				xConvCounter += m_xmult;
			}
			yConvCounter += m_ymult;
			pDataO += adj_pix_pitch;
			pAlphaO += adj_alp_pitch;
		}

		xres = (int)((float)xres * m_xmult);
		yres = (int)((float)yres * m_ymult);
	}

	if(m_options & IMG_INDEXED) {
		MessageBox(GameX.GetWindow(), "Error: Unsupported transfer operation (8 bit image to surface).", "GameX Error",MB_OK|MB_ICONSTOP) ;
		// not supported yet
	}
	else
	if(m_options & IMG_HIGHCOLOR) {
		if(m_options & IMG_ALPHA) {
			// if this image has a separate alpha channel,
			// combine the alpha and color data into a 32 bit surface texture for D3D to draw
			// (D3D's conversion from 32 bit to 16 bit on each draw can't be helped)
			XBYTE2 *pDataOld;
			XBYTE *pAlphaOld;
			XBYTE4 *pDataNew;
			pDataOld = (XBYTE2 *) m_data; 
			pAlphaOld = (XBYTE *) m_alpha; 
			pDataNew = (XBYTE4 *) ddsd.lpSurface;
			int adjusted_pitch = ddsd.lPitch / 4; // /4 for 32 bits
			int offset = 0;

			for(int y = 0 ; y < m_yres ; y++) {
				offset = y * adjusted_pitch;
				for(int x = 0 ; x < m_xres ; x++) {
					if(x < xres && y < yres) {
						int clr = *pDataOld;
						int blue  = (clr & GameX.win_maskb) >> GameX.win_shiftb << 3; // decode blue
						int green = (clr & GameX.win_maskg) >> GameX.win_shiftg << 2; // decode green
						int red   = (clr & GameX.win_maskr) >> GameX.win_shiftr << 3; // decode red
						int alpha;
						if(m_alpha == NULL) alpha = 0;
						else alpha = *pAlphaOld;
						int newColor = RGBA_MAKE(red,green,blue,alpha);
						*(pDataNew + offset) = newColor;

						// clamp by one pixel to prevent oddities at scaled edges
						if(x == xres-1 && m_surf_width > xres) *(pDataNew + offset + 1) = newColor;
						if(y == yres-1 && m_surf_height > yres) {
							*(pDataNew + offset + adjusted_pitch) = newColor;
							if(x == xres-1 && m_surf_width > xres) *(pDataNew + offset + adjusted_pitch + 1) = newColor;
						}
						if(x == 0 && m_surf_width > xres) *(pDataNew + offset + m_surf_width-1) = newColor;
						if(y == 0 && m_surf_height > yres) *(pDataNew + offset + adjusted_pitch*(m_surf_height-1)) = newColor;

						offset++;
					}
					pDataOld++;
					pAlphaOld++;
				}
			}

		} else {

			XBYTE2 * buf_data = (XBYTE2 *) ddsd.lpSurface;
			int bpp = ddsd.ddpfPixelFormat.dwRGBBitCount;
		//	int adjusted_pitch = ddsd.lPitch >> (GetBytesPerPixel(m_options)-1); // pitch adjusted for bit depth (>> 1 for 16 bit, etc.)
			int adjusted_pitch = ddsd.lPitch / 2; // /2 for 16 bits
			XBYTE2 *pData = (XBYTE2*) m_data;

			int offset = 0;

			for(int y = 0 ; y < m_yres ; y++) {
				for(int x = 0 ; x < m_xres ; x++) {
					if(x < xres && y < yres) {
						int newColor = *pData;
						*(buf_data + offset) = newColor;

						// clamp by one pixel to prevent oddities at scaled edges
						if(x == xres-1 && m_surf_width > xres) *(buf_data + offset + 1) = newColor;
						if(y == yres-1 && m_surf_height > yres) {
							*(buf_data + offset + adjusted_pitch) = newColor;
							if(x == xres-1 && m_surf_width > xres) *(buf_data + offset + adjusted_pitch + 1) = newColor;
						}
						if(x == 0 && m_surf_width > xres) *(buf_data + offset + m_surf_width-1) = newColor;
						if(y == 0 && m_surf_height > yres) *(buf_data + offset + adjusted_pitch*(m_surf_height-1)) = newColor;

						offset++;
					}

					pData++;
				}
				offset += adjusted_pitch - xres;
			}
		}
	}
	else
	if(m_options & IMG_TRUECOLOR) {
		if(m_options & IMG_ALPHA) {
			// if this image has a separate alpha channel,
			// combine the alpha and color data into a 32 bit surface texture for D3D to draw
			XBYTE *pDataOld;
			XBYTE *pAlphaOld;
			XBYTE4 *pDataNew;
			pDataOld = (XBYTE *) m_data; 
			pAlphaOld = (XBYTE *) m_alpha; 
			pDataNew = (XBYTE4 *) ddsd.lpSurface;
			int adjusted_pitch = ddsd.lPitch / 4; // /4 for 32 bits
			int offset = 0;

			for(int y = 0 ; y < m_yres ; y++) {
				offset = y * adjusted_pitch;
				for(int x = 0 ; x < m_xres ; x++) {
					if(x < xres && y < yres) {
						int clr = ((*pDataOld)<<16) | ((*(pDataOld+1))<<8) | ((*(pDataOld+2))<<0);
						int alpha;
						if(m_alpha == NULL) alpha = 0;
						else alpha = *pAlphaOld;
						int newColor = clr + (alpha << 24);
						*(pDataNew + offset) = newColor;

						// clamp by one pixel to prevent oddities at scaled edges
						if(x == xres-1 && m_surf_width > xres) *(pDataNew + offset + 1) = newColor;
						if(y == yres-1 && m_surf_height > yres) {
							*(pDataNew + offset + adjusted_pitch) = newColor;
							if(x == xres-1 && m_surf_width > xres) *(pDataNew + offset + adjusted_pitch + 1) = newColor;
						}
						if(x == 0 && m_surf_width > xres) *(pDataNew + offset + m_surf_width-1) = newColor;
						if(y == 0 && m_surf_height > yres) *(pDataNew + offset + adjusted_pitch*(m_surf_height-1)) = newColor;

						offset++;
					}
					pDataOld+=3;
					pAlphaOld++;
				}
			}

		} else {
			if(m_surf_bpp == 24) {
				XBYTE * buf_data = (XBYTE *) ddsd.lpSurface;
				int bpp = ddsd.ddpfPixelFormat.dwRGBBitCount;
				int adjusted_pitch = ddsd.lPitch / 3; // /3 for 24 bits
				XBYTE *pData = (XBYTE*) m_data;
				int offset = 0;

				for(int y = 0 ; y < m_yres ; y++) {
					for(int x = 0 ; x < m_xres ; x++) {
						if(x < xres && y < yres) {
							int red = *pData;
							int green = *(pData+1);
							int blue = *(pData+2);
							*(buf_data + offset) = red;
							*(buf_data + offset+1) = green;
							*(buf_data + offset+2) = blue;

							// clamp by one pixel to prevent oddities at scaled edges
							if(x == xres-1 && m_surf_width > xres) {
								*(buf_data + offset + 3) = red;
								*(buf_data + offset + 4) = green;
								*(buf_data + offset + 5) = blue;
							}
							if(y == yres-1 && m_surf_height > yres) {
								*(buf_data + offset + adjusted_pitch  ) = red;
								*(buf_data + offset + adjusted_pitch+1) = green;
								*(buf_data + offset + adjusted_pitch+2) = blue;
								if(x == xres-1 && m_surf_width > xres) {
									*(buf_data + offset + adjusted_pitch+3) = red;
									*(buf_data + offset + adjusted_pitch+4) = green;
									*(buf_data + offset + adjusted_pitch+5) = blue;
								}
							}
							if(x == 0 && m_surf_width > xres) {
								*(buf_data + offset + (m_surf_width-1)*3  ) = red;
								*(buf_data + offset + (m_surf_width-1)*3+1) = green;
								*(buf_data + offset + (m_surf_width-1)*3+2) = blue;
							}
							if(y == 0 && m_surf_height > yres) {
								*(buf_data + offset + adjusted_pitch*(m_surf_height-1)  ) = red;
								*(buf_data + offset + adjusted_pitch*(m_surf_height-1)+1) = green;
								*(buf_data + offset + adjusted_pitch*(m_surf_height-1)+2) = blue;
							}

							offset+=3;
						}

						pData+=3;
					}
					offset += adjusted_pitch - xres;
				}
			}
			else
			if(m_surf_bpp == 32) {
				XBYTE4 * buf_data = (XBYTE4 *) ddsd.lpSurface;
				int bpp = ddsd.ddpfPixelFormat.dwRGBBitCount;
				int adjusted_pitch = ddsd.lPitch / 4; // /4 for 32 bits
				XBYTE *pData = (XBYTE*) m_data;
				int offset = 0;

				for(int y = 0 ; y < m_yres ; y++) {
					for(int x = 0 ; x < m_xres ; x++) {
						if(x < xres && y < yres) {
							int clr = ((*pData)<<16) | ((*(pData+1))<<8) | ((*(pData+2))<<0);
							*(buf_data + offset) = clr;

							// clamp by one pixel to prevent oddities at scaled edges
							if(x == xres-1 && m_surf_width > xres) {
								*(buf_data + offset + 1) = clr;
							}
							if(y == yres-1 && m_surf_height > yres) {
								*(buf_data + offset + adjusted_pitch) = clr;
								if(x == xres-1 && m_surf_width > xres) {
									*(buf_data + offset + adjusted_pitch+1) = clr;
								}
							}
							if(x == 0 && m_surf_width > xres) *(buf_data + offset + m_surf_width-1) = clr;
							if(y == 0 && m_surf_height > yres) *(buf_data + offset + adjusted_pitch*(m_surf_height-1)) = clr;

							offset++;
						}

						pData+=3;
					}
					offset += adjusted_pitch - xres;
				}
			}
		}
	}
	else
	if(m_options & IMG_TRUECOLORX) {
		if(m_options & IMG_ALPHA) {
			// if this image has a separate alpha channel,
			// combine the alpha and color data into a 32 bit surface texture for D3D to draw
			XBYTE4 *pDataOld;
			XBYTE *pAlphaOld;
			XBYTE4 *pDataNew;
			pDataOld = (XBYTE4 *) m_data; 
			pAlphaOld = (XBYTE *) m_alpha; 
			pDataNew = (XBYTE4 *) ddsd.lpSurface;
			int adjusted_pitch = ddsd.lPitch / 4; // /4 for 32 bits
			int offset = 0;

			for(int y = 0 ; y < m_yres ; y++) {
				offset = y * adjusted_pitch;
				for(int x = 0 ; x < m_xres ; x++) {
					if(x < xres && y < yres) {
						int clr = *pDataOld;
						int alpha;
						if(m_alpha == NULL) alpha = 0; // this should be 0 if the alpha is already in the color
						else alpha = *pAlphaOld;
						int newColor = clr + (alpha << 24);
						*(pDataNew + offset) = newColor;

						// clamp by one pixel to prevent oddities at scaled edges
						if(x == xres-1 && m_surf_width > xres) *(pDataNew + offset + 1) = newColor;
						if(y == yres-1 && m_surf_height > yres) {
							*(pDataNew + offset + adjusted_pitch) = newColor;
							if(x == xres-1 && m_surf_width > xres) *(pDataNew + offset + adjusted_pitch + 1) = newColor;
						}
						if(x == 0 && m_surf_width > xres) *(pDataNew + offset + m_surf_width-1) = newColor;
						if(y == 0 && m_surf_height > yres) *(pDataNew + offset + adjusted_pitch*(m_surf_height-1)) = newColor;

						offset++;
					}

					pDataOld++;
					pAlphaOld++;
				}
			}
		} else {
			XBYTE4 * buf_data = (XBYTE4 *) ddsd.lpSurface;
			int bpp = ddsd.ddpfPixelFormat.dwRGBBitCount;
			int adjusted_pitch = ddsd.lPitch / 4; // /4 for 32 bits
			XBYTE4 *pData = (XBYTE4*) m_data;
			int offset = 0;

			for(int y = 0 ; y < m_yres ; y++) {
				for(int x = 0 ; x < m_xres ; x++) {
					if(x < xres && y < yres) {
						int newColor = *pData;
						*(buf_data + offset) = newColor;

						// clamp by one pixel to prevent oddities at scaled edges
						if(x == xres-1 && m_surf_width > xres) *(buf_data + offset + 1) = newColor;
						if(y == yres-1 && m_surf_height > yres) {
							*(buf_data + offset + adjusted_pitch) = newColor;
							if(x == xres-1 && m_surf_width > xres) *(buf_data + offset + adjusted_pitch + 1) = newColor;
						}
						if(x == 0 && m_surf_width > xres) *(buf_data + offset + m_surf_width-1) = newColor;
						if(y == 0 && m_surf_height > yres) *(buf_data + offset + adjusted_pitch*(m_surf_height-1)) = newColor;

						offset++;
					}

					pData++;
				}
				offset += adjusted_pitch - xres;
			}
		}
	}

 	status = m_surface->Unlock(NULL); // unlock drawing surface
	if(status != DD_OK){ 	
		GameX.DirectDrawError(status);
		return;
	}

	if(GameX.CanDeleteMData() && m_data != NULL) {
		if(!(m_usage & LOAD_NON3D) || !((m_usage & LOAD_ALPHA && !(m_usage & LOAD_MASKED)))) {
			DeleteData();
		}
	}
}

inline int ImageX::GetBytesPerPixel (ImageOps ops)
{												// Pixel Format
	if (ops & IMG_MERGEALPHA) {					// I R G B A
		if (ops & IMG_INDEXED) return 2;		// 8-0-0-0-8
		if (ops & IMG_LOWCOLOR) return 2;		// 0-4-4-4-4
		if (ops & IMG_HIGHCOLOR) return 3;		// 0-6-5-5-8
		if (ops & IMG_TRUECOLOR) return 4;		// 0-8-8-8-8
		if (ops & IMG_TRUECOLORX) return 4;		// 0-8-8-8-8
	} else {
		if (ops & IMG_INDEXED) return 1;		// 8-0-0-0-0
		if (ops & IMG_LOWCOLOR) return IMG_UNDEF;// undefined
		if (ops & IMG_HIGHCOLOR) return 2;		// 0-6-5-5-0
		if (ops & IMG_TRUECOLOR) return 3;		// 0-8-8-8-0
		if (ops & IMG_TRUECOLORX) return 4;		// 0-8-8-8-8
	}
	return IMG_UNDEF;
}

inline int ImageX::GetFormat (ImageOps ops)
{
	if (ops & IMG_INDEXED) return IMG_INDEXED;
	if (ops & IMG_LOWCOLOR) return IMG_LOWCOLOR;
	if (ops & IMG_HIGHCOLOR) return IMG_HIGHCOLOR;
	if (ops & IMG_TRUECOLOR) return IMG_TRUECOLOR;
	if (ops & IMG_TRUECOLORX) return IMG_TRUECOLORX;
	return IMG_UNDEF;
}

inline int ImageX::GetAlpha (ImageOps ops)
{	
	if (ops & IMG_ALPHA) return IMG_ALPHA;
	return IMG_UNDEF;
}

inline void ImageX::TurnOff (ImageOps opt)
{	
	if(m_options & opt)
		m_options ^= opt;
}

inline void ImageX::TurnOn (ImageOps opt)
{	
	m_options |= opt;
}

void ImageX::Size (int xr, int yr, ImageOps ops)
{
	if(GetFormat(ops) == IMG_UNDEF) {
#ifdef _DEBUG
		MessageBox(GameX.GetWindow(), "Error: Image options passed to ImageX::Size() must include a format such as IMG_HIGHCOLOR", "GameX Error",MB_OK|MB_ICONSTOP) ;
#endif
		return;
	}

	int bpp = GetBytesPerPixel (ops);
	
	if (bpp != -1) {
		if (m_data!=NULL) {delete [] m_data; m_data = NULL;}	
		m_data = new XBYTE[xr * yr * bpp];
	}
	if (ops & IMG_ALPHA) {
		if (m_alpha!=NULL) {delete [] m_alpha; m_alpha = NULL;}
		m_alpha = new XBYTE[xr * yr];
	}	
	m_xres = xr; m_yres = yr; m_size = xr * yr;
	m_options = ops;
}

void ImageX::Fill (int r, int g, int b, int a) // r, g, b, a are out of 255
{
	if(m_data != NULL) {
		XBYTE2 * pData = (XBYTE2*) m_data;
		int size = m_size * GetBytesPerPixel(m_options) / sizeof(XBYTE2);
		for (int n=0; n < size; n++) {
			*pData++ = (((r >> 3) << GameX.win_shiftr) + ((g >> 2) << GameX.win_shiftg) + ((b >> 3) << GameX.win_shiftb));
		}
	}

	if(m_alpha != NULL) {
		XBYTE * pAlpha = (XBYTE*) m_alpha;
		for (int n=0; n < m_size; n++) {
			*pAlpha++ = a;
		}
	}

	if(m_surface != NULL) {
		GameX.AccessPixels(this);
		for(int y = m_fy1 ; y < m_fy2 ; y++)
			for(int x = m_fx1 ; x < m_fx2 ; x++)
				GameX.DrawPixel(x,y,r,g,b,a);
		GameX.EndPixelAccess();
	}
	ResetFilterRect();
}

void ImageX::SwapColors (ColorSwapType type) // rotates or swaps the image's color values
{
	if(type < 0) return;
	type %= 20;
	switch(type) { // if it's a complicated LAB color changes:
		case COLOR_SWAP_BA:   ChangeColors(COLOR_SET_B,COLOR_SET_A); return;
		case COLOR_SWAP_NBA:  ChangeColors(COLOR_SET_B,COLOR_SET_A,-1.0f,1.0f); return;
		case COLOR_SWAP_BNA:  ChangeColors(COLOR_SET_B,COLOR_SET_A,1.0f,-1.0f); return;
		case COLOR_SWAP_NBNA: ChangeColors(COLOR_SET_B,COLOR_SET_A,-1.0f,-1.0f); return;
		case COLOR_SWAP_ANB:  ChangeColors(COLOR_SET_A,COLOR_SET_B,1.0f,-1.0f); return;
		case COLOR_SWAP_NAB:  ChangeColors(COLOR_SET_A,COLOR_SET_B,-1.0f,1.0f); return;
		case COLOR_SWAP_NANB: ChangeColors(COLOR_SET_A,COLOR_SET_B,-1.0f,-1.0f); return;
		case COLOR_SWAP_BB:   ChangeColors(COLOR_SET_B,COLOR_SET_B); return;
		case COLOR_SWAP_NBB:  ChangeColors(COLOR_SET_B,COLOR_SET_B, -1.0f,1.0f); return;
		case COLOR_SWAP_BNB:  ChangeColors(COLOR_SET_B,COLOR_SET_B, 1.0f,-1.0f); return;
		case COLOR_SWAP_NBNB: ChangeColors(COLOR_SET_B,COLOR_SET_B, -1.0f,-1.0f); return;
		case COLOR_SWAP_AA:   ChangeColors(COLOR_SET_A,COLOR_SET_A); return;
		case COLOR_SWAP_NAA:  ChangeColors(COLOR_SET_A,COLOR_SET_A, -1.0f,1.0f); return;
		case COLOR_SWAP_ANA:  ChangeColors(COLOR_SET_A,COLOR_SET_A, 1.0f,-1.0f); return;
		case COLOR_SWAP_NANA: ChangeColors(COLOR_SET_A,COLOR_SET_A, -1.0f,-1.0f); return;
	}

	// otherwise, plain color component swapping:
	
	GameX.AccessPixels(this);
	for(int y = m_fy1 ; y < m_fy2 ; y++) {
		if(m_ymult != 1.0f && y != m_fy1 && (int)(m_ymult * (float)(y-1)) == (int)(m_ymult * (float)(y))) continue;
		for(int x = m_fx1 ; x < m_fx2 ; x++) {
			if(m_xmult != 1.0f && x != m_fx1 && (int)(m_xmult * (float)(x-1)) == (int)(m_xmult * (float)(x))) continue;
			int r,g,b,a;
			GameX.ReadPixel(x,y,r,g,b,a);
			if(r == 0 && g == 0 && b == 0) continue;
			switch(type) {
				case COLOR_SWAP_ROTATE120: GameX.DrawPixel(x,y,b,r,g,a); break; // rotate 120 degrees
				case COLOR_SWAP_ROTATE240: GameX.DrawPixel(x,y,g,b,r,a); break; // rotate 240 degrees
				case COLOR_SWAP_REDGREEN:  GameX.DrawPixel(x,y,g,r,b,a); break; // swap red and green
				case COLOR_SWAP_REDBLUE:   GameX.DrawPixel(x,y,b,g,r,a); break; // swap red and blue
				case COLOR_SWAP_GREENBLUE: GameX.DrawPixel(x,y,r,b,g,a); break; // swap green and blue
			}
		}
	}
	GameX.EndPixelAccess();
	ResetFilterRect();
}


void ImageX::ChangeColors(ColorSetType a_settype, ColorSetType b_settype, float a_set_mult, float b_set_mult, float a_app_alpha, float b_app_alpha, ColorSetCondition a_acond, ColorSetCondition a_bcond, ColorSetCondition b_acond, ColorSetCondition b_bcond) {
 	int types [2], conditions [4];
	float mult [2], alpha [2];
	types[0] = a_settype;
	types[1] = b_settype;
	mult[0] = a_set_mult;
	mult[1] = b_set_mult;
	alpha[0] = a_app_alpha;
	alpha[1] = b_app_alpha;
	conditions[0] = a_acond;
	conditions[1] = a_bcond;
	conditions[2] = b_acond;
	conditions[3] = b_bcond;
	GameX.AccessPixels(this);
	for(int y = m_fy1 ; y < m_fy2 ; y++) {
		if(m_ymult != 1.0f && y != m_fy1 && (int)(m_ymult * (float)(y-1)) == (int)(m_ymult * (float)(y))) continue;
		for(int x = m_fx1 ; x < m_fx2 ; x++) {
			if(m_xmult != 1.0f && x != m_fx1 && (int)(m_xmult * (float)(x-1)) == (int)(m_xmult * (float)(x))) continue;
			int r,g,b,a;
			GameX.ReadPixel(x,y,r,g,b,a);
			if(r == 0 && g == 0 && b == 0) continue;
			int L, c[2], o[2], match [2]; // c for color, o for original
			ConvertRGBToLab(r,g,b,L,o[0],o[1]);
			c[0] = o[0];
			c[1] = o[1];
			match[0] = 0;
			match[1] = 0;
			for(int s = 0 ; s < 2 ; s++) { // setting match
				for(int t = 0 ; t < 2 ; t++) { // being tested
					switch(conditions[s + t+t]) {
						case COLOR_COND_NEGATIVE: if(o[s] < 0) match[t]++; break; // < 0
						case COLOR_COND_POSITIVE: if(o[s] > 0) match[t]++; break; // > 0
						case COLOR_COND_ANYTHING: match[t]++; break; // anything
						case COLOR_COND_NEUTRAL:  if(o[s] > -50 && o[s] < 50) match[t]++; break; // -50 to 50
						case COLOR_COND_GREATER: if(o[s] > o[1-s]) match[t]++; break; // > other
						case COLOR_COND_LESS: if(o[s] < o[1-s]) match[t]++; break; // < other
						case COLOR_COND_NEAR: if(o[s] > o[1-s]-50 && o[s] < o[1-s]+50) match[t]++; break; // other-50 to other+50
					}
				}
			}
			if(match[0] < 2 && match[1] < 2) continue;
			for(int t = 0 ; t < 2 ; t++) {
				if(match[t] < 2) continue;
				switch(types[t]) {
					case COLOR_SET_CONSTANT: c[t] = 50; break;
					case COLOR_SET_A: c[t] = o[0]; break;
					case COLOR_SET_B: c[t] = o[1]; break;
					case COLOR_SET_ABS_A: c[t] = abs(o[0]); break;
					case COLOR_SET_ABS_B: c[t] = abs(o[1]); break;
					case COLOR_SET_MAX: c[t] = max(o[0],o[1]); break;
					case COLOR_SET_MIN: c[t] = min(o[0],o[1]); break;
					case COLOR_SET_A_PLUS_B: c[t] = o[0]+o[1]; break;
					case COLOR_SET_A_MINUS_B: c[t] = o[0]-o[1]; break;
					case COLOR_SET_ABS_A_PLUS_ABS_B: c[t] = abs(o[0])+abs(o[1]); break;
					case COLOR_SET_ABS_A_MINUS_ABS_B: c[t] = abs(o[0])-abs(o[1]); break;
					case COLOR_SET_L: c[t] = (int)((float)(L*L*L)/(100.0f*100.0f)); break;
					case COLOR_SET_L_SMOOTH: c[t] = (int)(((float)(L*L*L)/(100.0f*100.0f)-50)*2); break;
					case COLOR_SET_SIN_A: c[t] = (int)(sinf((float)o[0]*((360.0f/100.0f)*DEGtoRAD))*100.0f); break;
					case COLOR_SET_SIN_B: c[t] = (int)(sinf((float)o[1]*((360.0f/100.0f)*DEGtoRAD))*100.0f); break;
					case COLOR_SET_COS_A: c[t] = (int)(cosf((float)o[0]*((360.0f/100.0f)*DEGtoRAD))*100.0f); break;
					case COLOR_SET_COS_B: c[t] = (int)(cosf((float)o[1]*((360.0f/100.0f)*DEGtoRAD))*100.0f); break;
					case COLOR_SET_SIN_L: c[t] = (int)(sinf((float)(L*L*L)/(100.0f*100.0f)*((360.0f/100.0f)*DEGtoRAD))*100.0f); break;
					case COLOR_SET_COS_L: c[t] = (int)(cosf((float)(L*L*L)/(100.0f*100.0f)*((360.0f/100.0f)*DEGtoRAD))*100.0f); break;
				}
				if(mult[t] != 1.0f) {
					if(mult[t] == 0.0f)
						c[t] = 0;
					else if(mult[t] == -1.0f)
						c[t] = -c[t];
					else
						c[t] = (int)((float)c[t] * mult[t]);
				}
				if(alpha[t] != 1.0f) {
					if(alpha[t] == 0.0f)
						c[t] = o[t];
					else
						c[t] = (int)((float)c[t]*alpha[t] + (float)o[t]*(1.0f-alpha[t]));
				}
			}
			ConvertLabToRGB(L,c[0],c[1],r,g,b);
			GameX.DrawPixel(x,y,r,g,b,a);
		}
	}
	GameX.EndPixelAccess();
	ResetFilterRect();
}

void ImageX::Invert (bool preserve_color) // inverts the image
{
	int rmin = GameX.win_maskminr;
	int gmin = GameX.win_maskming;
	int bmin = GameX.win_maskminb;
	GameX.AccessPixels(this);
	int maxl = 1000;
	for(int y = m_fy1 ; y < m_fy2 ; y++)
		for(int x = m_fx1 ; x < m_fx2 ; x++) {
			int r,g,b,a;
			GameX.ReadPixel(x,y,r,g,b,a);

			if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin)
				continue;

			if(m_usage & LOAD_MASKED && r>255-rmin && g>255-gmin && b>255-bmin) {
				r = 255-rmin;
				g = 255-gmin;
				b = 255-bmin;
			}
			if(preserve_color) {
				int L,A,B;
				ConvertRGBToLab(r,g,b,L,A,B);
				ConvertLabToRGB(150-L,A,B,r,g,b);
				GameX.DrawPixel(x,y,r,g,b,a);
			} else
				GameX.DrawPixel(x,y,255-r,255-g,255-b,a);
		}
	GameX.EndPixelAccess();
	ResetFilterRect();
}

void ImageX::ChangeBrightness (int brightness, float contrast)
{
	ChangeBrightnesses(brightness,brightness,brightness,0,contrast,contrast,contrast,1.0f);
}

void ImageX::ChangeBrightnesses (int brightness_r, int brightness_g, int brightness_b, int brightness_a, float contrast_r, float contrast_g, float contrast_b, float contrast_a) // changes brightness and contrast
{
	int offset_r = brightness_r + (contrast_r ? (int)(255.0f*(1.0f - contrast_r)/2.0f) : 0);
	int offset_g = brightness_g + (contrast_g ? (int)(255.0f*(1.0f - contrast_g)/2.0f) : 0);
	int offset_b = brightness_b + (contrast_b ? (int)(255.0f*(1.0f - contrast_b)/2.0f) : 0);
	int offset_a = brightness_a + (contrast_a ? (int)(255.0f*(1.0f - contrast_a)/2.0f) : 0);
	int rmin = GameX.win_maskminr;
	int gmin = GameX.win_maskming;
	int bmin = GameX.win_maskminb;
	GameX.AccessPixels(this);
	for(int y = m_fy1 ; y < m_fy2 ; y++) {
		if(m_ymult != 1.0f && y != m_fy1 && (int)(m_ymult * (float)(y-1)) == (int)(m_ymult * (float)(y))) continue;
		for(int x = m_fx1 ; x < m_fx2 ; x++) {
			if(m_xmult != 1.0f && x != m_fx1 && (int)(m_xmult * (float)(x-1)) == (int)(m_xmult * (float)(x))) continue;
			int r,g,b,a;
			GameX.ReadPixel(x,y,r,g,b,a);

			if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin)
				continue;

			if(contrast_r != 1.0f) r = (int)((float)r * contrast_r);
			if(contrast_g != 1.0f) g = (int)((float)g * contrast_g);
			if(contrast_b != 1.0f) b = (int)((float)b * contrast_b);
			if(contrast_a != 1.0f) a = (int)((float)a * contrast_a);
			r += offset_r;
			g += offset_g;
			b += offset_b;
			a += offset_a;

			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;
			if(a > 255) a = 255;
			if(r < 0) r = 0;
			if(g < 0) g = 0;
			if(b < 0) b = 0;
			if(a < 0) a = 0;

			if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin)
				b = bmin;

			GameX.DrawPixel(x,y,r,g,b,a);
		}
	}
	GameX.EndPixelAccess();
	ResetFilterRect();
}

void ImageX::ChangeSaturation (float sat)
{
	ChangeSaturations(sat,sat,sat);
}

void ImageX::ChangeSaturations (float sat_r, float sat_g, float sat_b) // reduces or enhances color while keeping brightness the same
{
	int rmin = GameX.win_maskminr;
	int gmin = GameX.win_maskming;
	int bmin = GameX.win_maskminb;
	GameX.AccessPixels(this);
	for(int y = m_fy1 ; y < m_fy2 ; y++) {
		if(m_ymult != 1.0f && y != m_fy1 && (int)(m_ymult * (float)(y-1)) == (int)(m_ymult * (float)(y))) continue;
		for(int x = m_fx1 ; x < m_fx2 ; x++) {
			if(m_xmult != 1.0f && x != m_fx1 && (int)(m_xmult * (float)(x-1)) == (int)(m_xmult * (float)(x))) continue;
			int r,g,b,a;
			GameX.ReadPixel(x,y,r,g,b,a);

			if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin)
				continue;
			
			float red, green, blue;
			red = (float)r/255.0f;
			green = (float)g/255.0f;
			blue = (float)b/255.0f;
			float gray = red*RED_LUMINANCE + green*GREEN_LUMINANCE + blue*BLUE_LUMINANCE;
			
			red = (red*sat_r + gray*(1.0f-sat_r));
			green = (green*sat_g + gray*(1.0f-sat_g));
			blue = (blue*sat_b + gray*(1.0f-sat_b));
			
			r = (int)(red*255.0f);
			g = (int)(green*255.0f);
			b = (int)(blue*255.0f);

			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;
			if(r < 0) r = 0;
			if(g < 0) g = 0;
			if(b < 0) b = 0;

			if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin) {
				r = rmin;
				g = gmin;
				b = bmin;
			}

			GameX.DrawPixel(x,y,r,g,b,a);
		}
	}
	GameX.EndPixelAccess();
	ResetFilterRect();
}

void ImageX::ConvertToGrayscale (void)
{
	ChangeSaturation(0.0f);
}

void ImageX::SetChannel (int channel, int value) // sets all of one channel to one value
{
	channel %= 4;
	if(value < 0) value = 0;
	if(value > 255) value = 255;
	int rmin = GameX.win_maskminr;
	int gmin = GameX.win_maskming;
	int bmin = GameX.win_maskminb;
	GameX.AccessPixels(this);
	for(int y = m_fy1 ; y < m_fy2 ; y++)
	for(int x = m_fx1 ; x < m_fx2 ; x++) {
		int r,g,b,a;
		GameX.ReadPixel(x,y,r,g,b,a);
		
		if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin)
			continue;
		
		switch(channel) {
			case 0: r = value; break;
			case 1: g = value; break;
			case 2: b = value; break;
			case 3: a = value; break;
		}
		
		if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin) {
			if(channel == 0) r = rmin;
			if(channel == 1) g = gmin;
			if(channel == 2) b = bmin;
		}
		
		GameX.DrawPixel(x,y,r,g,b,a);
	}
	GameX.EndPixelAccess();
	ResetFilterRect();
}

void ImageX::AddNoise (int noise)
{
	AddNoise(noise,noise,noise,0);
}

void ImageX::AddNoise (int noise_r, int noise_g, int noise_b, int noise_a) // adds random noise to the image
{
	int rmin = GameX.win_maskminr;
	int gmin = GameX.win_maskming;
	int bmin = GameX.win_maskminb;

	GameX.AccessPixels(this);
	for(int y = m_fy1 ; y < m_fy2 ; y++) {
		if(m_ymult != 1.0f && y != m_fy1 && (int)(m_ymult * (float)(y-1)) == (int)(m_ymult * (float)(y))) continue;
		for(int x = m_fx1 ; x < m_fx2 ; x++) {
			if(m_xmult != 1.0f && x != m_fx1 && (int)(m_xmult * (float)(x-1)) == (int)(m_xmult * (float)(x))) continue;
			int r,g,b,a;
			GameX.ReadPixel(x,y,r,g,b,a);

			if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin)
				continue;

			if(noise_r < 0) r += GameX.GetRandomInt(noise_r,0);
			else if(noise_r > 0) r += GameX.GetRandomInt(-noise_r,noise_r);
			if(noise_g < 0) g += GameX.GetRandomInt(noise_g,0);
			else if(noise_r > 0) g += GameX.GetRandomInt(-noise_g,noise_g);
			if(noise_b < 0) b += GameX.GetRandomInt(noise_b,0);
			else if(noise_r > 0) b += GameX.GetRandomInt(-noise_b,noise_b);
			if(noise_a < 0) a += GameX.GetRandomInt(noise_a,0);
			else if(noise_a > 0) a += GameX.GetRandomInt(-noise_a,noise_a);

			if(r > 255) r = 255;
			if(g > 255) g = 255;
			if(b > 255) b = 255;
			if(a > 255) a = 255;
			if(r < 0) r = 0;
			if(g < 0) g = 0;
			if(b < 0) b = 0;
			if(a < 0) a = 0;

			if(m_usage & LOAD_MASKED && r<rmin && g<gmin && b<bmin)
				b = bmin;

			GameX.DrawPixel(x,y,r,g,b,a);
		}
	}
	GameX.EndPixelAccess();
	ResetFilterRect();
}

void ImageX::GaussianBlur (float radius)
{
	GaussianBlur(radius,radius,radius,radius);
}

void ImageX::GaussianBlur (float red_radius, float green_radius, float blue_radius, float alpha_radius)
{
	int xWidth = m_fx2-m_fx1;
	int size = (xWidth)*(m_fy2-m_fy1)*4;
	XBYTE * data = new XBYTE[size];

	GameX.AccessPixels(this);
	for(int y1 = m_fy1 ; y1 < m_fy2 ; y1++) {
		if(m_ymult != 1.0f && y1 != m_fy1 && (int)(m_ymult * (float)(y1-1)) == (int)(m_ymult * (float)(y1))) continue;
		for(int x1 = m_fx1 ; x1 < m_fx2 ; x1++) {
			if(m_xmult != 1.0f && x1 != m_fx1 && (int)(m_xmult * (float)(x1-1)) == (int)(m_xmult * (float)(x1))) continue;
			int c[4];
			GameX.ReadPixel(x1,y1,c[0],c[1],c[2],c[3]);
			for(int i = 0 ; i < 4 ; i++)
				data[i+4*(x1-m_fx1+(xWidth)*(y1-m_fy1))] = (XBYTE)c[i];
		}
	}
	
	float rMaxDistSq = red_radius*red_radius*4.0f;
	float gMaxDistSq = green_radius*green_radius*4.0f;
	float bMaxDistSq = blue_radius*blue_radius*4.0f;
	float aMaxDistSq = alpha_radius*alpha_radius*4.0f;
	float maxDistSq = max(rMaxDistSq,max(gMaxDistSq,max(bMaxDistSq,aMaxDistSq)));
	int radInt = (int)ceil(sqrtf(maxDistSq));

	float est = (float)(radInt)*(float)(radInt)*(float)size;

	if(est > 32000000.0f) { // cap the blurring time at estimated 32x10^6 loops
		est = 32000000.0f/est;
		rMaxDistSq *= est;
		gMaxDistSq *= est;
		bMaxDistSq *= est;
		aMaxDistSq *= est;
		maxDistSq = max(rMaxDistSq,max(gMaxDistSq,max(bMaxDistSq,aMaxDistSq)));
		radInt = (int)ceil(sqrtf(maxDistSq));
	}

	int rmin = GameX.win_maskminr;
	int gmin = GameX.win_maskming;
	int bmin = GameX.win_maskminb;

	for(int y = m_fy1 ; y < m_fy2 ; y++) {
		if(m_ymult != 1.0f && y != m_fy1 && (int)(m_ymult * (float)(y-1)) == (int)(m_ymult * (float)(y))) continue;
		for(int x = m_fx1 ; x < m_fx2 ; x++) {
			if(m_xmult != 1.0f && x != m_fx1 && (int)(m_xmult * (float)(x-1)) == (int)(m_xmult * (float)(x))) continue;
			int offset = 4*((x-m_fx1)+(xWidth)*(y-m_fy1));
			float rSum=0, gSum=0, bSum=0, aSum=0; // sum of color values read times current amount
			float rTot=0, gTot=0, bTot=0, aTot=0; // sum of current amounts
			for(int y2 = y-radInt ; y2 <= y+radInt ; y2++) {
				for(int x2 = x-radInt ; x2 <= x+radInt ; x2++) {
					if(x2<m_fx1||y2<m_fy1||x2>=m_fx2||y2>=m_fy2)
						continue;
					float xdiff = (float)(x2-x);
					float ydiff = (float)(y2-y);
					float distSq = xdiff*xdiff+ydiff*ydiff;
					if(distSq > maxDistSq+0.001f)
						continue;
					if(distSq < 0.005f)
						distSq = 1.0f;
					int r,g,b,a;
					GameX.ReadPixel(x2,y2,r,g,b,a);
					if(a < gmin || (m_usage&LOAD_MASKED && (r<rmin && g<gmin && b<bmin))) {
						r = (int)data[0+offset];
						g = (int)data[1+offset];
						b = (int)data[2+offset];
					}
					if(distSq < rMaxDistSq+0.001f) {
						float r_amount = (float)red_radius / distSq;
						rTot += r_amount;
						if(r>=rmin) rSum += (float)r * r_amount;
					}
					if(distSq < gMaxDistSq+0.001f) {
						float g_amount = (float)green_radius / distSq;
						gTot += g_amount;
						if(g>=gmin) gSum += (float)g * g_amount;
					}
					if(distSq < bMaxDistSq+0.001f) {
						float b_amount = (float)blue_radius / distSq;
						bTot += b_amount;
						if(b>=bmin) bSum += (float)b * b_amount;
					}
					if(distSq < aMaxDistSq+0.001f) {
						float a_amount = (float)alpha_radius / distSq;
						aTot += a_amount;
						if(a>=gmin) aSum += (float)a * a_amount;
					}
				}
			}
			if(rTot) data[0+offset] = (XBYTE)(rSum/rTot);
			if(gTot) data[1+offset] = (XBYTE)(gSum/gTot);
			if(bTot) data[2+offset] = (XBYTE)(bSum/bTot);
			if(aTot) data[3+offset] = (XBYTE)(aSum/aTot);
		}
	}
	for(int y3 = m_fy1 ; y3 < m_fy2 ; y3++) {
		for(int x3 = m_fx1 ; x3 < m_fx2 ; x3++) {
			int offset = 4*(x3-m_fx1+xWidth*(y3-m_fy1));
			GameX.DrawPixel(x3,y3,(int)data[0+offset],(int)data[1+offset],(int)data[2+offset],(int)data[3+offset]);
		}
	}
	GameX.EndPixelAccess();

	delete [] data; data = NULL;

	ResetFilterRect();
}

void ImageX::CopyTo (ImageX * dest)
{
	if(dest == NULL || dest == VIEWPORT || status != IMG_OK) return;
	if(dest->status != IMG_OK) // if copying to image that hasn't been loaded or created yet,
		dest->CreateWithUse(m_xres,m_yres,m_usage); // create it before copying into it

	int sy1 = m_fy1;
	int sy2 = m_fy2;
	int sx1 = m_fx1;
	int sx2 = m_fx2;
	int dy1 = dest->m_fy1;
	int dy2 = dest->m_fy2;
	int dx1 = dest->m_fx1;
	int dx2 = dest->m_fx2;
///	int x2 = (m_fx2 >= dest->m_xres) ? dest->m_surf_width : m_fx2;

	int sxWidth = sx2-sx1;
	int syHeight = sy2-sy1;

	// allocate enough data to hold source pixels matching (in size) the destination:
	int dxWidth = dx2-dx1;
	int dyHeight = dy2-dy1;
	int size = (dxWidth)*(dyHeight)*4;
	XBYTE * data = new XBYTE[size];

	// make a temporary copy of this image, scaled to destination size if necessary:
	GameX.AccessPixels(this);
	int c[4];
	if(sxWidth == dxWidth && syHeight == dyHeight) { // if no scaling needed:
		for(int y = dy1 ; y < dy2 ; y++)
		for(int x = dx1 ; x < dx2 ; x++) {
			GameX.ReadPixel(x,y,c[0],c[1],c[2],c[3]);
			for(int i = 0 ; i < 4 ; i++)
				data[i+4*(x-dx1+(dxWidth)*(y-dy1))] = (XBYTE)c[i];
		}
	} else { // otherwise scale with nearest-neighbor method:
		double xmult = (double)sxWidth/(double)dxWidth;
		double ymult = (double)syHeight/(double)dyHeight;
		for(int y = dy1 ; y < dy2 ; y++) {
			int gy = (int)((double)y*ymult);
			if(m_ymult != 1.0f && gy != m_fy1 && (int)(m_ymult * (float)(gy-1)) == (int)(m_ymult * (float)(gy))) continue;
			for(int x = dx1 ; x < dx2 ; x++) {
				int gx = (int)((double)x*xmult);
				if(m_xmult != 1.0f && gx != m_fx1 && (int)(m_xmult * (float)(gx-1)) == (int)(m_xmult * (float)(gx))) continue;
				GameX.ReadPixel(gx,gy,c[0],c[1],c[2],c[3]);
				for(int i = 0 ; i < 4 ; i++)
					data[i+4*(x-dx1+(dxWidth)*(y-dy1))] = (XBYTE)c[i];
			}
		}
	}
	GameX.EndPixelAccess();

	// now transfer the data copy of this image to the destination image:
	GameX.AccessPixels(dest);
	for(int y = dy1 ; y < dy2 ; y++) {
		if(dest->m_ymult != 1.0f && y != dest->m_fy1 && (int)(dest->m_ymult * (float)(y-1)) == (int)(dest->m_ymult * (float)(y))) continue;
		for(int x = dx1 ; x < dx2 ; x++) {
			if(dest->m_xmult != 1.0f && x != dest->m_fx1 && (int)(dest->m_xmult * (float)(x-1)) == (int)(dest->m_xmult * (float)(x))) continue;
			int offset = 4*(x-dx1+dxWidth*(y-dy1));
			GameX.DrawPixel(x,y,(int)data[0+offset],(int)data[1+offset],(int)data[2+offset],(int)data[3+offset]);
		}
	}
	GameX.EndPixelAccess();

	// finally, deallocate the temporary copy memory
	delete [] data; data = NULL;
}

void ImageX::CopyToAlpha (ImageX * dest)
{
	if(dest == NULL || dest == VIEWPORT || status != IMG_OK) return;

	if(dest->status != IMG_OK) // if copying to image that hasn't been loaded or created yet,
		dest->CreateWithUse(m_xres,m_yres,m_usage|LOAD_ALPHA); // create it before copying into it

	if(!(dest->m_usage & LOAD_ALPHA)) { // if destination has no alpha channel, convert it to have one:
		ImageUsage use = dest->m_usage|LOAD_ALPHA;
		if(use & LOAD_MASKED) use ^= LOAD_MASKED;
		dest->ConvertUsageTo(use);
	}

	int rmin = GameX.win_maskminr;
	int gmin = GameX.win_maskming;
	int bmin = GameX.win_maskminb;

	int sy1 = m_fy1;
	int sy2 = m_fy2;
	int sx1 = m_fx1;
	int sx2 = m_fx2;
	int dy1 = dest->m_fy1;
	int dy2 = dest->m_fy2;
	int dx1 = dest->m_fx1;
	int dx2 = dest->m_fx2;
///	int x2 = (m_fx2 >= dest->m_xres) ? dest->m_surf_width : m_fx2;

	int sxWidth = sx2-sx1;
	int syHeight = sy2-sy1;
	
	// allocate enough data to hold source pixels matching (in size) the destination:
	int dxWidth = dx2-dx1;
	int dyHeight = dy2-dy1;
	int size = (dxWidth)*(dyHeight);
	XBYTE * data = new XBYTE[size];

	// make a temporary grayscale copy of this image, scaled to destination size if necessary:
	GameX.AccessPixels(this);
	int c[4];
	if(sxWidth == dxWidth && syHeight == dyHeight) { // if no scaling needed:
		for(int y = dy1 ; y < dy2 ; y++)
		for(int x = dx1 ; x < dx2 ; x++) {
			GameX.ReadPixel(x,y,c[0],c[1],c[2],c[3]);
			data[(x-dx1+(dxWidth)*(y-dy1))] = (XBYTE)((int)(c[0]*RED_LUMINANCE + c[1]*GREEN_LUMINANCE + c[2]*BLUE_LUMINANCE) * c[3] / 255);
		}
	} else { // otherwise scale with nearest-neighbor method:
		double xmult = (double)sxWidth/(double)dxWidth;
		double ymult = (double)syHeight/(double)dyHeight;
		for(int y = dy1 ; y < dy2 ; y++) {
			int gy = (int)((double)y*ymult);
			if(m_ymult != 1.0f && gy != m_fy1 && (int)(m_ymult * (float)(gy-1)) == (int)(m_ymult * (float)(gy))) continue;
			for(int x = dx1 ; x < dx2 ; x++) {
				int gx = (int)((double)x*xmult);
				if(m_xmult != 1.0f && gx != m_fx1 && (int)(m_xmult * (float)(gx-1)) == (int)(m_xmult * (float)(gx))) continue;
				GameX.ReadPixel(gx,gy,c[0],c[1],c[2],c[3]);
				data[(x-dx1+(dxWidth)*(y-dy1))] = (XBYTE)((int)(c[0]*RED_LUMINANCE + c[1]*GREEN_LUMINANCE + c[2]*BLUE_LUMINANCE) * c[3] / 255);
			}
		}
	}
	GameX.EndPixelAccess();

	// now transfer the data copy of this image to the destination image:
	GameX.AccessPixels(dest);
	for(int y = dy1 ; y < dy2 ; y++) {
		if(dest->m_ymult != 1.0f && y != dest->m_fy1 && (int)(dest->m_ymult * (float)(y-1)) == (int)(dest->m_ymult * (float)(y))) continue;
		for(int x = dx1 ; x < dx2 ; x++) {
			if(dest->m_xmult != 1.0f && x != dest->m_fx1 && (int)(dest->m_xmult * (float)(x-1)) == (int)(dest->m_xmult * (float)(x))) continue;
			int offset = (x-dx1+dxWidth*(y-dy1));
			GameX.ReadPixel(x,y,c[0],c[1],c[2]);
			c[3] = (int)(data[offset]);
			if(dest->m_usage & LOAD_MASKED) {
				if(c[3]>=gmin && c[0]<rmin && c[1]<gmin && c[2]<bmin)
					{c[0] = rmin; c[1] = gmin; c[2] = bmin; c[3] = 255;}
				else if(c[3]<gmin && (c[0]>=rmin || c[1]>=gmin || c[2]>=bmin))
					{c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0;}
			}
			GameX.DrawPixel(x,y,c[0],c[1],c[2],c[3]);
		}
	}
	GameX.EndPixelAccess();

	// finally, deallocate the temporary copy memory
	delete [] data; data = NULL;
}

bool ImageX::CopyToClipboard (void)
{
	ImageExt ie;
	return ie.CopyToClipboard(this);
}


void ImageX::UseWith (ImageUsage use)
{
	m_usage = use;
	if(!(m_options & IMG_MERGEALPHA)) {
		switch(GameX.GetBPP()) {
		case 8:
			ConvertFormat(IMG_INDEXED);
			break;
		case 16:
			ConvertFormat(IMG_HIGHCOLOR);
			break;
		case 24:
			ConvertFormat(IMG_TRUECOLOR);
			break;
		case 32:
			ConvertFormat(IMG_TRUECOLORX);
			break;
		}
	}
	m_size = m_xres*m_yres;
	m_xmult = 1.0f;
	m_ymult = 1.0f;

	bool makeSurface = true;
	bool saveData = !GameX.CanDeleteMData();

	if(m_options & IMG_ALPHA) {
		if(use & LOAD_MASKED)
			ConvertAlphaToMask();					   			
	} else {
		if(m_options & IMG_MERGEALPHA)
			m_options |= IMG_ALPHA;
		else
		if(use & LOAD_ALPHA)
			if(!(use & LOAD_MASKED))
				ConvertMaskToAlpha();
	}
	if(!((m_usage & LOAD_ALPHA) && (m_options & IMG_ALPHA) && !(m_usage & LOAD_MASKED))) {
		if(m_options & IMG_ALPHA) {
			if(m_options & IMG_MERGEALPHA) {
				TurnOff(IMG_MERGEALPHA);
				if(GameX.GetBPP() == 8) ConvertFormat(IMG_INDEXED);
				if(GameX.GetBPP() == 16) ConvertFormat(IMG_HIGHCOLOR);
				if(GameX.GetBPP() == 24) ConvertFormat(IMG_TRUECOLOR);
			}
			TurnOff(IMG_ALPHA);
		}
		if (m_alpha!=NULL) {delete [] m_alpha; m_alpha = NULL;}
	}
	if(makeSurface)
		AllocateSurface();

	if(m_hassurface) {
		DataToSurface();
		if(!saveData)
			DeleteData();
	} else {
		int adjusted_pitch = m_xres;
		for (int y=0; y<=m_surf_height; y++)
			m_ytable[y] = y * adjusted_pitch;
	}
	
	status = IMG_OK;

	ResetFilterRect();
}

void ImageX::ConvertMaskToAlpha (void)
{
	// main reason for doing this:
	// D3D renders color key edges in a very ugly way,
	// but it handles alpha channels just fine (esp. with rotation).

	if (GetAlpha(m_options)!=IMG_ALPHA) {
		if (m_alpha!=NULL) {delete [] m_alpha; m_alpha = NULL;}
		m_alpha = new XBYTE[m_xres * m_yres];
		XBYTE *pAlphaData;
		XBYTE2 *pData;
		pData = (XBYTE2*) m_data;
		pAlphaData = m_alpha;
		for (int n=0; n<m_size; n++) {
			if(*pData == 0) *pAlphaData = 0;
			else			*pAlphaData = 255;
			*pAlphaData++;
			*pData++;
		}
		m_options |= IMG_ALPHA;
	}
}

void ImageX::SetPixel (int x, int y, int r, int g, int b, int a)
{
	if(x < 0 || y < 0 || x >= min(m_xres+1,m_surf_width) || y >= min(m_yres+1,m_surf_height))
		return;

	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;
	if(a < 0) a = 0;
	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
	if(a > 255) a = 255;

	GameX.AccessPixels(this);
	GameX.DrawPixel(x,y,r,g,b,a);
	GameX.EndPixelAccess();
}

void ImageX::GetPixel (int x, int y, int &r, int &g, int &b, int &a)
{
	if(x < 0 || y < 0 || x >= m_xres || y >= m_yres) {
		r = 0; g = 0; b = 0; a = 0;
		return;
	}
	GameX.AccessPixels(this);
	GameX.ReadPixel(x,y,r,g,b,a);
	GameX.EndPixelAccess();
}

void ImageX::ConvertFormat (ImageOps ops)
{	
	XBYTE * newdata;
	
	switch (GetFormat(m_options)) {			// Source Format:
		case IMG_INDEXED:					//   Indexed Color (8 bit)
			switch (GetFormat(ops))	{	// Target Format:
				case IMG_HIGHCOLOR:	//   High Color (16 bit)
					GameX.ErrorDialog("Unsupported color conversion operation (8 to 16 bit).") ;
					break;
				case IMG_TRUECOLOR:	//   True Color (24 bit)
					GameX.ErrorDialog("Unsupported color conversion operation (8 to 24 bit).") ;
					break;
				case IMG_TRUECOLORX:	//   True Color with X channel (32 bit)
					GameX.ErrorDialog("Unsupported color conversion operation (8 to 32 bit).") ;
					break;
			}
			break;
		case IMG_HIGHCOLOR:					//   High Color (16 bit)
			switch (GetFormat(ops))	{	// Target Format:
				case IMG_INDEXED:	//   High Color (8 bit)
					MessageBox(GameX.GetWindow(), "Error: Unsupported color conversion operation (16 to 8 bit).", "GameX Error",MB_OK|MB_ICONSTOP) ;
					break; // not yet supported
				case IMG_TRUECOLOR:	{ //   True Color (24 bit)
					XBYTE2 *pDataOld;
					XBYTE *pDataNew;
					newdata = new XBYTE[m_size * GetBytesPerPixel(ops)];			
					pDataOld = (XBYTE2 *) m_data; 
					pDataNew = (XBYTE *) newdata;

					if(!GameX.win_shiftr)
						GameX.ReportProblem(GAMEX_NO_INIT) ;

					for (int n=0; n<m_size; n++) {
						int clr = *pDataOld++;
						int blue  = (((clr & 0x001F) >> 0 ) << 3);
						int green = (((clr & 0x07E0) >> 5 ) << 2);
						int red   = (((clr & 0xF800) >> 11) << 3);
						*pDataNew++ = red;
						*pDataNew++ = green;
						*pDataNew++ = blue;
					}
					delete [] m_data; m_data = NULL;
					m_data = newdata; newdata = NULL;		
					break;
				}
				case IMG_TRUECOLORX: {	//   True Color with X channel (32 bit)
					XBYTE2 *pDataOld;
					XBYTE4 *pDataNew;
					newdata = new XBYTE[m_size * GetBytesPerPixel(ops)];			
					pDataOld = (XBYTE2 *) m_data; 
					pDataNew = (XBYTE4 *) newdata;

					if(!GameX.win_shiftr)
						GameX.ReportProblem(GAMEX_NO_INIT) ;

					for (int n=0; n<m_size; n++) {
						int clr = *pDataOld++;
						int blue  = (((clr & 0x001F) >> 0 ) << 3);
						int green = (((clr & 0x07E0) >> 5 ) << 2);
						int red   = (((clr & 0xF800) >> 11) << 3);
						*pDataNew++ = (red << GameX.win_shiftr) + (green << GameX.win_shiftg) + (blue << GameX.win_shiftb);
					}
					delete [] m_data; m_data = NULL;
					m_data = newdata; newdata = NULL;
					break;
				}
			}
			break;							// Source Format:
		case IMG_TRUECOLOR:					//   True Color (24 bit)
			switch (GetFormat(ops))	{			// Target Format:
				case IMG_INDEXED:			//   Indexed Color (8 bit)
					MessageBox(GameX.GetWindow(), "Error: Unsupported color conversion operation (24 to 8 bit).", "GameX Error",MB_OK|MB_ICONSTOP) ;
					break;
				case IMG_HIGHCOLOR:	{			//   High Color (16 bit)
					XBYTE *pDataOld;
					XBYTE2 *pDataNew;
					newdata = new XBYTE[m_size * GetBytesPerPixel(ops)];			
					pDataOld = (XBYTE *) m_data; 
					pDataNew = (XBYTE2 *) newdata;

					if(!GameX.win_shiftr)
						GameX.ReportProblem(GAMEX_NO_INIT) ;

					for (int n=0; n<m_size; n++) {
						*pDataNew = (*pDataOld++ >> 3) << GameX.win_shiftr;
						*pDataNew |= (*pDataOld++ >> 2) << GameX.win_shiftg;
						*pDataNew++ |= (*pDataOld++ >> 3) << GameX.win_shiftb;
					}
					delete [] m_data; m_data = NULL;
					m_data = newdata; newdata = NULL;
					break;
				}
				case IMG_TRUECOLORX: {	//   True Color with X channel (32 bit)
					XBYTE *pDataOld;
					XBYTE4 *pDataNew;
					newdata = new XBYTE[m_size * GetBytesPerPixel(ops)];			
					pDataOld = (XBYTE *) m_data; 
					pDataNew = (XBYTE4 *) newdata;

					if(!GameX.win_shiftr)
						GameX.ReportProblem(GAMEX_NO_INIT) ;

					for (int n=0; n<m_size; n++) {
						int newPixel = 0;
						newPixel |= *pDataOld++ << GameX.win_shiftr;
						newPixel |= *pDataOld++ << GameX.win_shiftg;
						newPixel |= *pDataOld++ << GameX.win_shiftb;
						*pDataNew++ = newPixel;
					}
					delete [] m_data; m_data = NULL;
					m_data = newdata; newdata = NULL;
					break;
				}
			}
			break;							// Source Format:
		case IMG_TRUECOLORX:				//   True Color with X channel (32 bit)
			switch (GetFormat(ops))	{		// Target Format:
				case IMG_INDEXED:			//   Indexed Color (8 bit)
					MessageBox(GameX.GetWindow(), "Error: Unsupported color conversion operation (32 to 8 bit).", "GameX Error",MB_OK|MB_ICONSTOP) ;
					break;
				case IMG_HIGHCOLOR: {			//   High Color (16 bit)
					XBYTE4 *pDataOld;
					XBYTE2 *pDataNew;
					newdata = new XBYTE[m_size * GetBytesPerPixel(ops)];			
					pDataOld = (XBYTE4 *) m_data; 
					pDataNew = (XBYTE2 *) newdata;

					if(!GameX.win_shiftr)
						GameX.ReportProblem(GAMEX_NO_INIT) ;

					for (int n=0; n<m_size; n++) {
						int oldPixel = *pDataOld++;
						int red = ((oldPixel & 0x00FF0000) >> 16);
						int green = ((oldPixel & 0x0000FF00) >> 8);
						int blue = ((oldPixel & 0x000000FF) >> 0);
						int newPixel = ((red >> 3) << 11) | ((green >> 2) << 5) | ((blue >> 3) << 0);
						*pDataNew++ = newPixel;
					}
					delete [] m_data; m_data = NULL;
					m_data = newdata; newdata = NULL;
					break;
				}
				case IMG_TRUECOLOR: {			//   True Color (24 bit)
					XBYTE4 *pDataOld;
					XBYTE *pDataNew;
					newdata = new XBYTE[m_size * GetBytesPerPixel(ops)];			
					pDataOld = (XBYTE4 *) m_data; 
					pDataNew = (XBYTE *) newdata;

					if(!GameX.win_shiftr)
						GameX.ReportProblem(GAMEX_NO_INIT) ;

					for (int n=0; n<m_size; n++) {
						int oldPixel = *pDataOld++;
						int red = ((oldPixel & 0x00FF0000) >> 16);
						int green = ((oldPixel & 0x0000FF00) >> 8);
						int blue = ((oldPixel & 0x000000FF) >> 0);
						*pDataNew++ = red;
						*pDataNew++ = green;
						*pDataNew++ = blue;
					}
					delete [] m_data; m_data = NULL;
					m_data = newdata; newdata = NULL;
					break;
				}
			}
			break;
	}
	TurnOff (GetFormat(m_options));
	TurnOn (ops);
}

void ImageX::ConvertAlphaToMask (void)
{
	if (GetAlpha(m_options)==IMG_ALPHA) {
		switch (GetFormat(m_options)) {
		case IMG_HIGHCOLOR: {
			XBYTE2 *pData;
			XBYTE *pAlphaData;
			pData = (XBYTE2*) m_data;
			pAlphaData = m_alpha;
			for (int n=0; n<m_size; n++) {
				if (*pData==0) *pData = 1;
				if (*pAlphaData < 16) *pData = 0;
				*pData++;
				*pAlphaData++;
			}
		} break;
		}

		TurnOff (IMG_ALPHA);
		TurnOn (IMG_MASKED);
	}
}

// Image rotation function for backward compatibility
// (DrawImage or DrawImageEx should be used for rotation now)
void ImageX::Rotate (ImageX *dest, float ang, float scale)
{
	// Fill the destination with pure black, like Rotate used to do.
	GameX.ResetDrawStates();
	GameX.SetDrawDestination(dest);
	GameX.FillScreen(ColorX(0,0,0,0));

	// Now draw the rotated image into the center of the destination.
	// Note that this requires destination to be loaded with LOAD_TARGETABLE.
	GameX.SetDrawDestination(dest);
	GameX.SetDrawMode(DRAW_PLAIN|DRAWOP_CENTERED);
	GameX.DrawImage(this, dest->m_xres/2,dest->m_yres/2, ang,scale);
	GameX.ResetDrawStates();
}

/*
		
void ImageX::RotateImage (ImageX *dest, float ang, float scale, bool toVideo)
{
	// NOTE: ASSUMES BOTH IMAGES ARE IMG_HIGHCOLOR

	if (dest!=NULL && m_data!=NULL) {
		XBYTE2 *src_pix;
		XBYTE2 *dest_pix;
		XBYTE2 *dest_start;
		float delt_x, delt_y;
		float start_x, start_y;
		float off_x, off_y;
		float pix_x, pix_y, h;
		int y, cnt;
		int rowSize;

		h = (float) sqrt(dest->m_xres * dest->m_xres + dest->m_yres * dest->m_yres);
		start_x = (m_xres/2.0f) + cosf((135-ang)*DEGtoRAD)*(h/2.0f)/scale;
		start_y = (m_yres/2.0f) - sinf((135-ang)*DEGtoRAD)*(h/2.0f)*(m_xres/m_yres)/scale+1; // the +1 fixes a vertical off-by-one bug
		delt_x = cosf(ang*DEGtoRAD)/scale;
		delt_y = sinf(ang*DEGtoRAD)*(m_xres/m_yres)/scale;
		off_x = delt_x * dest->m_xres;
		off_y = delt_y * dest->m_xres;
		if(toVideo) {
			DDSURFACEDESC2 ddsd;	
			ZeroMemory (&ddsd, sizeof (ddsd)) ;
			ddsd.dwSize = sizeof (ddsd) ;

 			HRESULT status = dest->m_surface->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);	// lock drawing surface
			if(status != DD_OK) {
				GameX.DirectDrawError(status);
				return;
			}
			dest_start = (XBYTE2 *) ddsd.lpSurface;
			rowSize = ddsd.lPitch >> 1; // >> 1 for 16 bits
		}
		else {
			dest_start = (XBYTE2*) dest->m_data;
			rowSize = dest->m_xres;
		}
		for (y=0; y<dest->m_yres; y++) {	
			cnt = dest->m_xres;						// Start with full pixel set in line
			pix_x = start_x + off_x-1; //-1 fixes off-by-1 bug				// Goto end of line and scan backwards
			pix_y = start_y + off_y;
			dest_pix = dest_start + cnt-1;
			while ((pix_x<0 || pix_x>=m_xres || pix_y<0 || pix_y>=m_yres) && cnt>0) {
				pix_x -= delt_x;					// Scan until end of span found
				pix_y -= delt_y;
				cnt--;
				*dest_pix-- = 0;
			}	
			dest_pix = dest_start;
			pix_x = start_x;						// Goto beginning of line and scan forward
			pix_y = start_y;
			while ((pix_x<0 || pix_x>=m_xres || pix_y<0 || pix_y>=m_yres) && cnt>0) {
				pix_x += delt_x;					// Scan until start of span found
				pix_y += delt_y;
				cnt--;
				*dest_pix++ = 0;
			}
			while (cnt>0) {
				src_pix = ((XBYTE2*) m_data) + (int) pix_y*m_xres + (int) pix_x;
				*dest_pix++ = *src_pix;			
				pix_x += delt_x; pix_y += delt_y;
				cnt--;
			}
			dest_start += rowSize;
			start_x -= delt_y;
			start_y += delt_x;
		}
		if(toVideo) {
 			status = dest->m_surface->Unlock(NULL); 	// unlock drawing surface
			if(status != DD_OK){
				GameX.DirectDrawError(status);
				return;
			}
		}
	}
}

void ImageX::RotateAlpha (ImageX *dest, float ang, float scale)
{
	if (dest->m_options & IMG_ALPHA && m_alpha!=NULL && dest->m_alpha!=NULL) {
		XBYTE *src_pix;
		XBYTE *dest_pix;
		XBYTE *dest_start;
		float delt_x, delt_y;
		float start_x, start_y;
		float off_x, off_y;
		float pix_x, pix_y, h;
		int y, cnt;

		h = (float) sqrt(dest->m_xres * dest->m_xres + dest->m_yres * dest->m_yres);
		start_x = (m_xres/2.0f) + cosf((135-ang)*DEGtoRAD)*(h/2.0f)/scale;
		start_y = (m_yres/2.0f) - sinf((135-ang)*DEGtoRAD)*(h/2.0f)/scale+1; // the +1 is to work around an off-by-one bug
		delt_x = cosf(ang*DEGtoRAD)/scale;
		delt_y = sinf(ang*DEGtoRAD)/scale;
		off_x = delt_x * dest->m_xres;
		off_y = delt_y * dest->m_xres;	
		dest_start = (XBYTE*) dest->m_alpha;
		for (y=0; y<dest->m_yres; y++) {	
			cnt = dest->m_xres;						// Start with full pixel set in line
			pix_x = start_x + off_x-1; // -1 avoids off-by-one bug		// Goto end of line and scan backwards
			pix_y = start_y + off_y;
			dest_pix = dest_start + cnt-1;
			while ((pix_x<0 || pix_x>=m_xres || pix_y<0 || pix_y>=m_yres) && cnt>0) {
				pix_x -= delt_x;					// Scan until end of span found
				pix_y -= delt_y;
				cnt--;
				*dest_pix-- = 255;
			}	
			dest_pix = dest_start;
			pix_x = start_x;						// Goto beginning of line and scan forward
			pix_y = start_y;
			while ((pix_x<0 || pix_x>=m_xres || pix_y<0 || pix_y>=m_yres) && cnt>0) {
				pix_x += delt_x;					// Scan until start of span found
				pix_y += delt_y;
				cnt--;
				*dest_pix++ = 255;
			}
			while (cnt>0) {
				src_pix = ((XBYTE*) m_alpha) + (int) pix_y*m_xres + (int) pix_x;
				*dest_pix++ = *src_pix;			
				pix_x += delt_x; pix_y += delt_y;
				cnt--;
			}
			dest_start += dest->m_xres;
			start_x -= delt_y;
			start_y += delt_x;
		}
	}
}
*/

ImageExt::ImageExt ()
{
	m_pImg = NULL;
}

ImageExt::ImageExt (ImageX* pImg)
{
	m_pImg = pImg;
}

ImageExt::~ImageExt ()
{
	m_pImg = NULL;
}

bool ImageExt::Load (char * filename, ImageX * img, bool alpha)
{
	ImageUsage use = (alpha) ? (LOAD_ALPHA) : 0;
	return LoadWithUse(filename,img,use);
}

bool ImageExt::LoadWithUse (char * filename, ImageX * img, ImageUsage use)
{
	img->Destroy();
	img->Reset();

	strcpy(img->m_filename,filename);

	#ifdef WINDX_DEBUG
		if(GameX.GetStatus() == GAMEX_READY) {
			int z;
			char debug_message [MAX_PATH];
			for(z = 0 ; filename[z] ; z++) {
				debug_message[z] = filename[z];
			}
			debug_message[z++] = ':';
			while(z < 48) {
				debug_message[z++] = ' ';
			}
			debug_message[z++] = (char)0;
			debug.Output(debug_message,""); 
		}
	#endif

	bool success = false;


	char * extension = strrchr(filename, '.'); // string of text from the last dot onward

	if(extension && (stricmp(extension, ".TIFF") == 0 || stricmp(extension, ".TIF") == 0)) { // case insensitive match
		success = LoadTiff (filename, img);
	}

	if(!success) {
		if(extension && (stricmp(extension, ".BMP") == 0 || stricmp(extension, ".DIB") == 0))
			success = LoadBMP (filename, img);
		else
			success = LoadJPGorGIF (filename, img);
	}

	if(!success) {
		// something went wrong, either the file wasn't found
		// or it's in a nonstandard format such as [A4 R4 G4 B4] or [R5 G6 B5]
		char disp [500];
		sprintf (disp, "The image in file %s did not load. \n\nMake sure the image is in that location and that this game's executable is in the correct location to find it.", filename); 
		img->Size(64,64,IMG_HIGHCOLOR); // arbitrary size
		img->Fill(0,0,0,0); // black or invisible

		MessageBox (NULL, disp, "GameX Error", MB_OK|MB_ICONSTOP);
	}

	img->UseWith (use);

	if(success) img->m_non_empty = true;

	return success;
}

bool ImageExt::Save (char * filename, ImageX * img)
{
	bool ok = false;
	char * extension = strrchr(filename, '.'); // string of text from the last dot onward
	if(extension && (stricmp(extension, ".TIF") == 0 || stricmp(extension, ".TIFF") == 0)) { // case insensitive match
		ok = SaveTiff(filename, img);
	} else { // since the only other format we can save to is BMP, assume that and warn if it isn't
		LPDIRECTDRAWSURFACE7 surface;
		if(img == VIEWPORT) {
			surface = GameX.GetBackSurf();
		} else {
			if(img->status != IMG_OK)
				return false;
			surface = img->m_surface;
		}

		HDC hdc, hmemdc;
		HBITMAP hbitmap, hprevbitmap;
		DDSURFACEDESC2 ddsd;
		surface->GetDC(&hdc);
		hmemdc = CreateCompatibleDC(hdc); 
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		surface->GetSurfaceDesc(&ddsd);
		hbitmap = CreateCompatibleBitmap(hdc, ddsd.dwWidth, ddsd.dwHeight);
		hprevbitmap = (HBITMAP) SelectObject(hmemdc, hbitmap);
		BitBlt(hmemdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdc, 0, 0, SRCCOPY);
			
		ok = SaveBmp(filename,hbitmap);

		SelectObject(hmemdc,hprevbitmap);
		DeleteDC(hmemdc);
		surface->ReleaseDC(hdc);

		if (!(extension && (stricmp(extension, ".BMP") == 0 || stricmp(extension, ".DIB") == 0))) {
			char errstr [256], fixstr [64];
			if(extension == NULL) sprintf(fixstr, "rename it to \"%s.bmp\".", filename);
			else				  sprintf(fixstr, "replace \"%s\" with \".bmp\".", extension);
			sprintf(errstr,"GameX has been requested to save an ImageX to \"%s\", but GameX only supports saving to .BMP or .TIFF files.\nTherefore, to open \"%s\", you will need to %s",filename,filename,fixstr);
			MessageBox (NULL, errstr, "GameX Warning", MB_OK|MB_ICONWARNING);
		}
	}

	return ok;
}

bool ImageExt::CopyToClipboard(ImageX * img)
{
	return GameX.CopyToClipboard(img);
}

void ImageX::ConvertRGBToLab(int r, int g, int b, int & L, int & A, int & B)
{
	float x = (0.412453f*r + 0.357580f*g + 0.180423f*b) * (1.0f / (255.0f * 0.950456f));
	float y = (0.212671f*r + 0.715160f*g + 0.072169f*b) * (1.0f / 255.0f);
	float z = (0.019334f*r + 0.119193f*g + 0.950227f*b) * (1.0f / (255.0f * 1.088754f));

	float fx, fy, fz;

	if(y > 0.008856f) {
		fy = powf(y, 0.333333333f);
		L = (int)(116.0f*fy + (-16.0f + 0.5f));
	} else {
		fy = 7.787f*y + (16.0f/116.0f);
		L = (int)(903.3f*y + 0.5f);
	}

	if(x > 0.008856f)
		fx = powf(x, 0.333333333f);
	else
		fx = 7.787f*x + (16.0f/116.0f);

	A = (int)(500.0f*(fx - fy) + 0.5f);

	if(z > 0.008856f)
		fz = powf(z, 0.333333333f);
	else
		fz = 7.787f*z + (16.0f/116.0f);

	B = (int)(200.0f*(fy - fz) + 0.5f);
}

void ImageX::ConvertLabToRGB(int L, int A, int B, int &r, int &g, int &b)
{
	float y;
	float fy = (L + 16.0f) * (1.0f / 116.0f);
	fy = fy*fy*fy;
	if(fy < 0.008856f) {
		fy = L * (1.0f/903.3f);
		y = fy;
		fy = 7.787f*fy + (16.0f/116.0f);
	} else {
		y = fy;
		fy = powf(fy, 0.333333333f);
	}

	float x, fx = A * (1.0f/500.0f) + fy;
	float z, fz = fy - B * (1.0f/200.0f);

	if(fx > 0.206893f)
		x = fx*fx*fx;
	else
		x = (fx - (16.0f/116.0f)) * (1.0f / 7.787f);

	if(fz > 0.206893f)
		z = fz*fz*fz;
	else
		z = (fz - (16.0f/116.0f)) * (1.0f / 7.787f);

	x *= (0.950456f * 255.0f);
	y *= (255.0f);
	z *= (1.088754f * 255.0f);

	r = (int)(3.240479f*x - 1.537150f*y - 0.498535f*z + 0.5f);
	g = (int)(-0.969256f*x + 1.875992f*y + 0.041556f*z + 0.5f);
	b = (int)(0.055648f*x - 0.204043f*y + 1.057311f*z + 0.5f);

	if(r<0) r=0; if(r>255) r=255;
	if(g<0) g=0; if(g>255) g=255;
	if(b<0) b=0; if(b>255) b=255;
}
