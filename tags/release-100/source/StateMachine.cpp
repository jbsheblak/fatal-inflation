//---------------------------------------------------
// Name: Game : StateMachine
// Desc:  controls game state
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "StateMachine.h"

namespace Game
{
	StateMachine* StateMachine::sStateMachine = NULL;

	StateMachine::StateMachine() :  mNextState(NULL)
								  , mCurState(NULL)
								  , mStateChange(false)
	{}	

	StateMachine::~StateMachine()
	{
		std::map< std::string, State*>::iterator itr;
		for( itr = mStates.begin(); itr != mStates.end(); ++itr )
		{
			delete itr->second;
		}
	}

	bool StateMachine::RequestStateChange( std::string newState )
	{  
		std::map< std::string, State*>::iterator itr;
		if( ( itr = mStates.find(newState) ) != mStates.end() )
		{
			mNextState = itr->second;
			mStateChange = true;
		}
		   
		else
		{   
			mStateChange = false;
		}
		   
		return mStateChange;
	}
	 
	void StateMachine::Handle()
	{
		if( mStateChange )
		{
			if( mCurState )
			{
				mCurState->Exit();
			}
	         
			mCurState = mNextState;
	         
			if( mCurState )
			{
				mCurState->Enter();
			}
	         
			mNextState = NULL;
			mStateChange = false;
		}
	     
		if( mCurState )
		{
			mCurState->Handle();
		}
	}

	void StateMachine::AddState( std::string name, State* addState )
	{
		std::map< std::string, State*>::iterator itr;
		if( (itr = mStates.find(name)) != mStates.end() )
		{         
			delete itr->second;
		}
	     
		mStates[name] = addState;
	}

	StateMachine* StateMachine::GetSM()
	{
		if( !sStateMachine )
		{
			sStateMachine = new StateMachine();
		}
		return sStateMachine;
	}   

}; // end Game
