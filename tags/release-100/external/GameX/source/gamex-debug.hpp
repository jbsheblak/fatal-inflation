//
// GameX - Debug Class Header
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

#ifndef DEBUGGING_DEF
	#define DEBUGGING_DEF
										//**** Debug Switches
	//#define DEBUG_PRINTF				// Send output to console	
	#define DEBUG_FILE					// Send output to file
	
	#define DEBUG_MAX			50

	class Debugging {
	public:
		Debugging ();
		~Debugging ();	
		void Open (char *filename);
		void Close (void);
		void Output (char *msg1, char *msg2);
		void Output (float value) {char str[64]; sprintf(str,"%f",value); Output(str,"\n");}
		void Output (char *message) {Output(message,"\n");}
	private:
		FILE *m_output_file;
		bool nonEmpty;
	};	

	#ifdef DEBUG_GLOBAL
		Debugging			debug;	
	#else
		extern Debugging	debug;
	#endif
#endif


