//---------------------------------------------------
// Name: Game : MasterFile
// Desc:  a mastering file
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_MASTER_H_
#define _GAME_MASTER_H_

namespace Game
{

// version control
extern const char* kAppName;
extern const float kVersion;

// file controls
#define COMPILE_FILES 1		// set to zero to disable file compilation
#define PLAY_MUSIC	  1		// set to zero to disable music
#define ENABLE_EDIT   1		// set to zero to disable edit mode

}; //end Game

#endif // end _GAME_MASTER_H_
    
