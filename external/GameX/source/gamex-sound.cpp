//
// GameX (Release 4 - SoundX)
// Sound Class Code 
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
//

#include "gamex-sound.hpp"

SoundX::SoundX()
{
	samples_per_sec = 0;
	bits_per_sample = 0;
	length = 0;
	num_bytes = 0;
	num_samples = 0;	
	data = NULL;
//	dx_index = -1;
	dx_refresh = true;
	ds_sound_buffer = NULL;
}

SoundX::~SoundX()
{
	if(data!=NULL) {
		delete [] data; data = NULL;
		GameX.MemoryRecordChange(-(num_bytes+1023)/1024);
	}
	if(GameX.GetDS()) { // if we haven't already released all of DirectSound
		if(GameX.IsSoundPlaying(this)) {
			GameX.StopSound(this);
			#ifdef _DEBUG
				GameX.WarningDialog("A SoundX object was deleted while it was still playing.\nTo avoid this, it is recommended that you make all SoundX objects global variables or members of ones,\nor use SoundX pointers allocated with the 'new' keyword.");
			#endif
		}
		if(ds_sound_buffer != NULL) {
			ds_sound_buffer->Release(), ds_sound_buffer = NULL;
			GameX.MemoryRecordChange(-(num_bytes+1023)/1024);
		}
	}
}

bool SoundX::ReadName (File& wav, Buffer &code, char *match_name)
{
	char buf[5];
	wav.ReadC (4, buf);
	if (strncmp (buf, match_name, 4)==0)	return true;
	else									return false;
}

bool SoundX::ReadSize (File& wav, Buffer &code, unsigned int &size)
{
	XBYTE4 fourbytes;
	wav.Read4 (fourbytes); 
	size = code.ReadL (fourbytes);
	return true;
}

bool SoundX::LoadWav (char *filename)
{
	XBYTE2 twobytes;
	XBYTE4 fourbytes;
	XBYTE2 format;
	unsigned int size;
	int source_bps;
	char buf[100];
	ZeroMemory(buf,100);
	int n;

	File wav;
	Buffer code;

	if (wav.Open (filename, FILE_READ | FILE_RANDOM) != FILE_EXIST_NO) {
		code.SetOrder (BUFFER_ORDER_LBF);
		
		if (ReadName(wav, code, "RIFF")==false) {InvalidWav(); return false;}
		if (ReadSize(wav, code, size)==false)	{InvalidWav(); return false;}
		if (ReadName(wav, code, "WAVE")==false)	{InvalidWav(); return false;}
		if (ReadName(wav, code, "fmt ")==false)	{InvalidWav(); return false;}
		if (ReadSize(wav, code, size)==false)	{InvalidWav(); return false;}
		// Read WAV Format Header (THIS READS 16 BYTES)
		wav.Read2 (twobytes); format = code.ReadI (twobytes);				// WAV Format Tag (1 = PCM) (2 = MS ADPCM)
		switch(format) {
		case 1: {
			wav.Read2 (twobytes); num_channels = code.ReadI (twobytes);			// WAV Number of Channels
			wav.Read4 (fourbytes); samples_per_sec = code.ReadL (fourbytes);	// WAV Samples per Second
			wav.Read4 (fourbytes);												// WAV Avergae Bytes per Second (IGNORED)
			wav.Read2 (twobytes); int block_align = code.ReadI (twobytes);		// WAV Block Align (IGNORED)
			wav.Read2 (twobytes); bits_per_sample = code.ReadI (twobytes);		// WAV Bits Per Sample			
			source_bps = bits_per_sample;
			bits_per_sample = min(16,source_bps);
			wav.ReadC (size-16, buf);
			while(wav.EndOfFile()==0) {
				int stat = wav.ReadC (1, buf);
				if(stat == FILE_STATUS_EOF) break;
				if(buf[0] == 'd')
					wav.ReadC (3, buf+1);
				else if(buf[0] == 'f')
					wav.ReadC (3, buf+1);
				else continue;
				if (strncmp (buf, "data", 4)==0) {
					if (ReadSize(wav, code, size)==false)	{InvalidWav(); return false;}
					// Read WAV Data
					num_bytes = size;								// Load number of bytes in data (from file)				
					num_samples = num_bytes / ((bits_per_sample/8) * num_channels);	// Calculate number of samples
					length = (float) num_samples / (float) samples_per_sec;		// Calculate length of sound (in seconds)
					if (data!=NULL) {delete [] data; data = NULL;}
					data = new char[num_bytes];
					ZeroMemory(data, num_bytes ) ;
	//				for (n = 0; n < num_bytes; n++) 
	//					*(data + n) = 0;

					int source_byte_ps = source_bps/8;
					int dest_byte_ps = bits_per_sample/8;
					int skip_channels = (source_byte_ps-dest_byte_ps);
					int read_channels = dest_byte_ps;

					for (n = 0; n < num_bytes; n+=dest_byte_ps) {
						if(skip_channels) {
							char skip [8];
							wav.ReadC (skip_channels, skip);
						}
						wav.ReadC (read_channels, data + n);
					}
				} else if (strncmp (buf, "fact", 4)==0) {
					wav.Read4 (fourbytes);
					wav.Read4 (fourbytes);
				}
			}
		}	break;
		default:
			char errstr [128];
			sprintf(errstr, "The WAV \"%s\" was not saved in PCM format. Please re-save it in any standard PCM format so GameX can load it.", filename);
			debug.Output("Error loading sound:",errstr);
			GameX.ErrorDialog(errstr);
			wav.Close();
			return false;
		}
				
//		dx_index = -1; // fixes bug that can make playing any sound fail if a different sound has been loaded already
		dx_refresh = true;
		wav.Close ();
	} else {
		char disp[500];
		sprintf (disp, ".WAV File Not Found: %s\n\nMake sure paths and names of WAV files are correct. If you started the game outside of Visual C++, make sure the executable is in the correct location to find sounds.", filename); 
		debug.Output(disp,"\n");
#ifdef _DEBUG
		MessageBox (GameX.GetWindow(), disp, "GameX Error", MB_OK);
#endif
		return false;
	}

	sprintf(m_filename, filename);

	#ifdef WINDX_DEBUG
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
//		sprintf(debug_message,"%s:", filename);
		debug.Output(debug_message,""); 

		char sizeStr [8];
		sprintf(sizeStr, "%05d", (num_bytes+1023)/1024);
		for(z = 0 ; sizeStr[z] == '0' ; z++) sizeStr[z] = ' ';

		char str[128];
		sprintf(str,"%s KB, %dx%dx%d",sizeStr,num_samples,num_channels,bits_per_sample);
		debug.Output(str);
	#endif
	GameX.MemoryRecordChange((num_bytes+1023)/1024);


	return true;
}

void SoundX::InvalidWav (void)
{
	char disp[500];
	sprintf (disp, ".WAV Format Invalid: \n\nInvalid WAV Format. Not all types of WAV files are supported. Try saving your WAV file with a different program, or with 'extra wav information' turned off."); 
	debug.Output(disp,"\n");
#ifdef _DEBUG
	MessageBox (GameX.GetWindow(), disp, "GameX Error", MB_OK);
#endif
}

void SoundX::CutLength (float new_length)
{
	int new_samples, new_bytes;
	char* new_data;

	if (data!=NULL && length > new_length) {
		new_samples = (int) (samples_per_sec * new_length);
		new_bytes = (bits_per_sample/8) * num_channels * new_samples;	
		new_data = new char[new_bytes];
		memcpy (new_data, data, new_bytes);
		if (data!=NULL) {delete [] data; data = NULL;}
		data = new_data;
		if (new_data!=NULL) {delete [] new_data; new_data = NULL;}
		num_bytes = new_bytes;
		num_samples = new_samples;
		length = new_length;
		dx_refresh = true;
	}
}

bool SoundX::NeedUpdate (void)
{
	if (/*dx_index==-1 ||*/ dx_refresh==true) return true;
	return false;
}


void SoundX::PrepareDSBuffer (void)
{
	WAVEFORMATEX format; 
	ZeroMemory(&format,sizeof(format)) ;		
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = num_channels;
	format.nSamplesPerSec = samples_per_sec;
	format.wBitsPerSample = bits_per_sample;
	format.nBlockAlign = (format.wBitsPerSample / 8) * format.nChannels; // /8 converts bits to bytes
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;	
	
	DSBUFFERDESC bufferDesc; 
	ZeroMemory(&bufferDesc,sizeof(bufferDesc)) ;  
	bufferDesc.dwSize = sizeof(bufferDesc) ; 
	bufferDesc.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME; 
	bufferDesc.dwBufferBytes = num_samples * format.nBlockAlign;
	bufferDesc.lpwfxFormat = (LPWAVEFORMATEX) &format;

	LPDIRECTSOUND ds_main = GameX.GetDS();

	if(ds_sound_buffer != NULL) {
		ds_sound_buffer->Release(), ds_sound_buffer = NULL;
		GameX.MemoryRecordChange(-(num_bytes+1023)/1024);
	}

	HRESULT status = ds_main->CreateSoundBuffer(&bufferDesc, &ds_sound_buffer, NULL);
	if (FAILED (status)) {
		switch(status) {
 		case DSERR_ALLOCATED: debug.Output("DSERR_ALLOCATED"); break;
		case DSERR_BADFORMAT: debug.Output("DSERR_BADFORMAT"); break;
		case DSERR_BUFFERTOOSMALL: debug.Output("DSERR_BUFFERTOOSMALL"); break;
		case DSERR_CONTROLUNAVAIL: debug.Output("DSERR_CONTROLUNAVAIL"); break;
		case DSERR_DS8_REQUIRED: debug.Output("DSERR_DS8_REQUIRED"); break;
		case DSERR_INVALIDCALL: debug.Output("DSERR_INVALIDCALL"); break;
		case DSERR_INVALIDPARAM: debug.Output("DSERR_INVALIDPARAM"); break;
		case DSERR_NOAGGREGATION: debug.Output("DSERR_NOAGGREGATION"); break;
		case DSERR_OUTOFMEMORY: debug.Output("DSERR_OUTOFMEMORY"); break;
		case DSERR_UNINITIALIZED: debug.Output("DSERR_UNINITIALIZED"); break;
		case DSERR_UNSUPPORTED: debug.Output("DSERR_UNSUPPORTED"); break;
		}
		GameX.ReportProblem (GAMEX_SOUNDCREATE_FAILED) ;
	}
	
	return;
}


MusicX::MusicX()
{
	segment = NULL;
	segment_state = NULL;
	registered = false;
	vol = 0;
	playstate = 0;
	dsh_gb = NULL;
	dsh_mc = NULL;
	dsh_mp = NULL;
	dsh_ba = NULL;
	dsh_me = NULL;
	Release();
}

MusicX::~MusicX()
{
	if(GameX.GetDMP()) { // if we haven't already released all of DirectMusic
		if(GameX.IsMusicPlaying(this)) {
			GameX.PlayMusic(NULL); // stops music and sets current music to NULL
			#ifdef _DEBUG
				GameX.WarningDialog("A MusicX object was deleted while it was still playing. Consider replacing code like:\n\n  MusicX m;\n  m.Load(\"file.mp3\");\n  GameX.PlayMusic(&m);\n\nwith code like:\n\n  MusicX* m = new MusicX;\n  m->Load(\"file.mp3\");\n  GameX.PlayMusic(m);");
			#endif
		}
		Release();
	}
}

void MusicX::Release (void)
{
	if(segment != NULL) {
		segment->SetParam(GUID_Unload, -1, 0, 0,(void*)GameX.GetDMP());
		segment->Release(), segment = NULL;
	}
	if(segment_state != NULL) segment_state->Release(), segment_state = NULL;

	// DirectShow
	if (dsh_gb) 		dsh_gb->Release(),			 dsh_gb = NULL;
	if (dsh_mc) 		dsh_mc->Release(),			 dsh_mc = NULL;
	if (dsh_mp) 		dsh_mp->Release(),			 dsh_mp = NULL;
	if (dsh_ba) 		dsh_ba->Release(),			 dsh_ba = NULL;
	if (dsh_me) 		dsh_me->Release(),			 dsh_me = NULL;

	playing_loops_left = 0;
	format = MUSIC_TYPE_NONE;
}

// loads a MIDI or MP3 file of a given filename into this MusicX object
bool MusicX::Load (char * filename)
{
	char * extension = strrchr(filename, '.'); // string of text from the last dot onward
	if(extension && (stricmp(extension, ".MID") == 0 || stricmp(extension, ".MIDI") == 0)) // case insensitive match
		return LoadMidi (filename);
	else
	if(extension && (stricmp(extension, ".MP3") == 0) || extension && (stricmp(extension, ".WMA") == 0))
		return LoadMP3 (filename);
	else {
#ifdef _DEBUG
		char str [256];
		sprintf(str,"Did not load %s because it does not appear to be in a supported music format. If it is a MIDI or MP3 file, make sure it has the proper .mid or .mp3 filename extension.",filename);
		debug.Output(str);
		GameX.ErrorDialog(str);
#endif
		return false;
	}
}

bool MusicX::LoadMidi (char *filename)
{
	// this function loads a midi segment
	DMUS_OBJECTDESC obj_desc;

	Release();

	format = MUSIC_TYPE_MIDI;

	// get current working directory
	char dir [_MAX_PATH];
	WCHAR w_dir [_MAX_PATH];
	if(_getcwd(dir, _MAX_PATH) == NULL)
		return false;

	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,dir,-1,w_dir,_MAX_PATH);

	IDirectMusicLoader * dml = GameX.GetDML();

	// tell the loader were to look for files
	if(dml == NULL || FAILED(dml->SetSearchDirectory(GUID_DirectMusicAllTypes, w_dir, FALSE)))
		return false;

	// convert filename to wide string
	WCHAR w_filename[_MAX_PATH];
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,w_filename,_MAX_PATH);

	// setup object description
	ZeroMemory(&obj_desc, sizeof(obj_desc));
	obj_desc.dwSize = sizeof(obj_desc);
	obj_desc.guidClass = CLSID_DirectMusicSegment;
	wcscpy(obj_desc.wszFileName, w_filename);
	obj_desc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;

	if(FAILED(GameX.GetDML()->GetObject(&obj_desc,IID_IDirectMusicSegment, (void**) &segment))
	|| FAILED(segment->SetParam(GUID_StandardMIDIFile,-1, 0, 0, (void*)GameX.GetDMP()))
	|| FAILED(segment->SetParam(GUID_Download, -1, 0, 0, (void*)GameX.GetDMP()))) {
		Release();
		return false;
	}

	segment->GetParam(GUID_TempoParam, 0xFFFF, 0, 0, NULL, &original_tempo);

	if(!registered) {
		GameX.RegisterMusicX(this);
		registered = true;
	}

	return true;
}

bool MusicX::LoadMP3 (char *filename)
{
	if(!GameX.CanPlayMP3()) return false;

	bool success = SetUpFilterGraph();
	if(!success) return false;

	format = MUSIC_TYPE_MP3;

//	GameX.WarningDialog("MP3 not yet implemented.");
//	return false;
//	GameX.GetFGB()->RenderFile(L"test.avi",NULL);

	// convert filename to wide string
	WCHAR w_filename[_MAX_PATH];
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename,-1,w_filename,_MAX_PATH);

	HRESULT status;
	status = dsh_gb->RenderFile(w_filename,NULL);

	if(FAILED(status)) {
		char str [_MAX_PATH+128];
		sprintf(str,"MusicX: Failed to render (load) the file %s.",filename);
		debug.Output(str);
		return false;
	} else {
		return true;
	}
}

bool MusicX::SetUpFilterGraph (void)
{
	Release();

	HRESULT status;

	status = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&dsh_gb);
	if(FAILED(status)) {
		debug.Output("WinDX: Failed to create graph builder.");
		return false;
	}

	status = dsh_gb->QueryInterface(IID_IMediaControl, (void **)&dsh_mc);
	if(FAILED(status)) {
		debug.Output("WinDX: Failed to create media control.");
		return false;
	}

	status = dsh_gb->QueryInterface(IID_IMediaPosition,(void**)&dsh_mp);
	if(FAILED(status)) {
		debug.Output("WinDX: Failed to create media position.");
		return false;
	}

	status = dsh_gb->QueryInterface(IID_IMediaEventEx,(void**)&dsh_me);
	if(FAILED(status)) {
		debug.Output("WinDX: Failed to create media event extender.");
		return false;
	}

	status = dsh_gb->QueryInterface(IID_IBasicAudio,(void**)&dsh_ba);
	if(FAILED(status)) {
		debug.Output("WinDX: Failed to create basic audio modifier.");
		return false;
	}

	status = dsh_me->SetNotifyWindow((OAHWND)GameX.GetWindow(), WM_DIRECTSHOW_EVENT, 0);
	if(FAILED(status)) {
		debug.Output("WinDX: Failed to set music notify callback.");
		return false;
	}

	return true;
}

// sends a curve message (to alter volume or pitch over time)
void MusicX::Curve(long time, int target, int type, int shape, int start)
{
	if(format != MUSIC_TYPE_MIDI) return; // not supported for MP3s (must be a DirectShow filter for it though)

	DMUS_CURVE_PMSG *pCurveMsg;
	HRESULT hr;

	if (NULL == GameX.GetDMP()) return;
	hr = GameX.GetDMP()->AllocPMsg(sizeof(DMUS_CURVE_PMSG), (DMUS_PMSG**) &pCurveMsg);
	if (SUCCEEDED(hr))
	{
		ZeroMemory(pCurveMsg, sizeof(DMUS_CURVE_PMSG));
		pCurveMsg->dwSize = sizeof(DMUS_CURVE_PMSG);
		pCurveMsg->rtTime = 0;
		pCurveMsg->dwFlags = DMUS_PMSGF_REFTIME | DMUS_PMSGF_LOCKTOREFTIME;
		pCurveMsg->dwPChannel = DMUS_PCHANNEL_BROADCAST_PERFORMANCE;
		pCurveMsg->dwType = DMUS_PMSGT_CURVE;
		pCurveMsg->dwGroupID = 0xFFFFFFF;
		pCurveMsg->mtDuration = time;
		pCurveMsg->nEndValue = target;
		pCurveMsg->bCurveShape = shape;
		pCurveMsg->bCCData = 7;
		if(start == -1) {
			pCurveMsg->bFlags = DMUS_CURVE_START_FROM_CURRENT;
		} else {
			pCurveMsg->bFlags = 0;
			pCurveMsg->nStartValue = start;
		}
		pCurveMsg->bType = type ;
		pCurveMsg->wMergeIndex = merge_id++; // curves attempt to merge unless they have identical merge indexes
		hr = GameX.GetDMP()->SendPMsg((DMUS_PMSG*) pCurveMsg);
	}
}


