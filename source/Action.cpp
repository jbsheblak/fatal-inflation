//---------------------------------------------------
// Name: Game : Action
// Desc:  performs actions
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Action.h"
#include "StateMachine.h"

namespace Game
{	
	EASY_ACTION_IMPL( ActionQuit )
	{
		GameX.Quit();
	}

	StateChangeAction::StateChangeAction( std::string newState )
	{
		mNewState = newState;
	}

	ToggleVisible::ToggleVisible( GuiElement* elem ) : mElem(elem)
	{}

	void ToggleVisible::Act()
	{
		if( mElem )
			mElem->SetVisible( !mElem->IsVisible() );
	}	

	void StateChangeAction::Act()
	{
		SMachine.RequestStateChange( mNewState );
	}	

	
	ActionButton::ActionButton( Action* action )
	{
		mAction = action;
	}

	ActionButton::~ActionButton()
	{
		delete mAction;
	}

	void ActionButton::OnClick( const jbsCommon::Vec2i& pos )
	{
		mAction->Act();
	}

}; //end Game
