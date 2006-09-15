//
// GameX - SoundX Class Header 
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
	#include <windows.h>
	#include <direct.h>

	#include "gamex-file.hpp"
	#include "gamex-buffer.hpp"
	#include "gamex-win-dx.hpp"

#ifndef SOUNDX_DEF
	#define SOUNDX_DEF

	class SoundX {
	public:
		SoundX ();
		~SoundX ();
		bool LoadWav (char *filename);
		inline bool Load (char *filename) {return LoadWav(filename);}
		void InvalidWav (void);
		bool ReadName (File& wav, Buffer &code, char *match_name);
		bool ReadSize (File& wav, Buffer &code, unsigned int &size);
		void CutLength (float new_length);
		bool NeedUpdate (void);
		inline unsigned short	GetBPS (void)			{return bits_per_sample;}
		inline unsigned short	GetNumChannels (void)	{return num_channels;}
		inline int				GetNumSamples (void)	{return num_samples;}
		inline int				GetSamplesPerSec (void)	{return samples_per_sec;}
		inline int				GetNumBytes (void)		{return num_bytes;}
//		inline int				GetIndex (void)			{return dx_index;}
//		inline void				SetIndex (int n)		{dx_index = n;}
		inline char*			GetData (void)			{return data;}
		inline bool				GetRefresh (void)		{return dx_refresh;}
		inline void				SetRefresh (bool r)		{dx_refresh = r;}
		inline LPDIRECTSOUNDBUFFER	GetDSBuffer (void)	{return ds_sound_buffer;}
		void PrepareDSBuffer (void);
	private:
		unsigned int	samples_per_sec;	// Quality (44100, 22050, 11025, 8000, 6000, etc.) 
		unsigned short	bits_per_sample;	// Resolution (8-bit, 16-bit, etc.)
		unsigned short	num_channels;		// Number of channels
		float			length;				// Length of sound (seconds)
		int				num_samples;		// Num Samples = samples_per_sec * length
		int				num_bytes;			// Total Bytes = (bits_per_sample/8) * num_channels * num_samples
		char*			data;
		LPDIRECTSOUNDBUFFER ds_sound_buffer;
//		int				dx_index;			// Direct Sound index
		bool			dx_refresh;			// Direct Sound refresh
		char			m_filename [MAX_PATH];
	};
	typedef	SoundX*		SoundXPtr;

	static int now_playing;
	static int merge_id;

	class MusicX {
	public:
		MusicX ();
		~MusicX ();

		// The game's code should call this function to load a music object before playing it,
		// i.e. call music.Load("musicfile.mid"); before calling GameX.Play(&music);
		bool Load (char *filename);

		// Getter methods:
		inline IDirectMusicSegment * GetSegment(void) {return segment;}
		inline IDirectMusicSegmentState * GetSegmentState(void) {return segment_state;}
		inline float GetVolume(void) {return vol;} 
		inline int GetPlayState(void) {return playstate;} 
		void SetPlayState(int set) {playstate = set;} // to be called by GameX only
		void RememberVol(float set) {vol = set;} // to be called by GameX only

		// called by GameX when uninitializing DirectX
		// necessary because the deconstructor can be called AFTER DirectX is uninitialized
		// but music objects must be unloaded BEFORE it's uninitialized or there's a runtime error
		void Release (void);

		// Runs the music through a volume/pitch/? curve
		// Used by GameX.VolumeFadeMusic() and GameX.PitchBendMusic(), not really intended to be called directly
		void Curve(long time, int target, int type, int shape, int start = -1);

		int format; // MUSIC_TYPE_MIDI or MUSIC_TYPE_MP3
		int playing_loops_left; // for MP3 looping
		IGraphBuilder *	dsh_gb;
		IMediaControl * dsh_mc;
		IMediaPosition * dsh_mp;
		IBasicAudio * dsh_ba;
		IMediaEventEx * dsh_me;
	private:
		bool LoadMidi (char *filename);
		bool LoadMP3 (char *filename);
		bool SetUpFilterGraph (void);

		IDirectMusicSegment *		segment;		// Music segment data
		IDirectMusicSegmentState *	segment_state;	// Music segment state
		DMUS_TEMPO_PARAM			original_tempo;
		bool						registered;
		float						vol;
		int							playstate;
	};

#endif
