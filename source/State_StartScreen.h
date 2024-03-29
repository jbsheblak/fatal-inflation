//---------------------------------------------------
// Name: Game : StartScreen
// Desc:  the game start screen
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_STATE_STARTSCREEN_H_
#define _GAME_STATE_STARTSCREEN_H_

#include "StateMachine.h"
#include "Types.h"
#include "gamex.hpp"
#include "MasterFile.h"
#include "FileIO.h"
#include "Gui.h"
namespace Game
{
	class State_StartScreen : public State
	{
	public:
		void Enter();
		void Exit();
		void Handle();

	private:

		class StartButton : public GuiButtonControl
		{
		public:
			void OnClick( const jbsCommon::Vec2i& pos );
		};

		class EditButton : public GuiButtonControl
		{
		public:
			void OnClick( const jbsCommon::Vec2i& pos );
		};



	private:
		
		GuiElement*			mGui;
		GameSaveFile::SaveFile mSaveFile;

	};	
	
}; //end Game

#endif // end _GAME_STATE_STARTSCREEN_H_
    
