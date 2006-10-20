//---------------------------------------------------
// Name: Game : Action
// Desc:  performs actions
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_ACTION_H_
#define _GAME_ACTION_H_

#include "Types.h"
#include "Gui.h"

namespace Game
{	

//easy action macros...
#define EASY_ACTION_DECL( name ) class name : public Action { public: void Act(); }
#define EASY_ACTION_IMPL( name ) void name::Act()

	class Action
	{
	public:
		virtual void Act() = 0;
	};

	EASY_ACTION_DECL( ActionQuit );	

	class StateChangeAction : public Action
	{
	public:

		StateChangeAction( std::string newState );
		void Act();

	private:
		std::string mNewState;
	};

	class ToggleVisible : public Action
	{
	public:
		ToggleVisible( GuiElement* elem );
		void Act();
	private:
		GuiElement* mElem;
	};


	class ActionButton : public GuiButtonControl
	{
	public:
		ActionButton( Action* action );		
		~ActionButton();

		void OnClick( const jbsCommon::Vec2i& pos );

	private:
		Action* mAction;
	};	
	
}; //end Game

#endif // end _GAME_ACTION_H_
    
