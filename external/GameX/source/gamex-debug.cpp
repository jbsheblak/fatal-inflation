//
// GameX - Debug Class Code 
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

//*** Global Debugging (for ALL modules)
// The following define tells the debug include file that
// THIS module (Debug.cpp) should recieve the 
// global "debug" object to make debugging available to 
// all modules automatically. All other modules (except the 
// Debug module) will receive an external reference to this 
// global variable. (See debug.hpp)

#define DEBUG_GLOBAL

//*** Debugging Include

#include "gamex-debug.hpp"

//*** Debugging Class Code

Debugging::Debugging (void)
{
	m_output_file = NULL;
	Open ("debug.txt");
	fprintf (m_output_file, "* Debugging Output *\n");
	nonEmpty = false;
}

Debugging::~Debugging (void)
{
	Close ();
	m_output_file = NULL;
}

void Debugging::Open (char *filename)
{
	if (m_output_file!=NULL) fclose (m_output_file);
	m_output_file = fopen (filename, "w+t");
	if (m_output_file==NULL) {
		printf ("DEBUGGER ERROR: Unable to open debug log file.\n");
	}
}

void Debugging::Close (void)
{
	if (m_output_file!=NULL) {
		fclose (m_output_file);
		if(!nonEmpty) remove("debug.txt");
	}
}

void Debugging::Output (char *msg1, char *msg2)
{
	fprintf (m_output_file, "%s%s", msg1, msg2);
	fflush (m_output_file);
	nonEmpty = true;
}