//---------------------------------------------------
// Name: Game : StateMachine
// Desc:  controls game state
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_STATE_MACHINE_H_
#define _GAME_STATE_MACHINE_H_

#include <map>
#include <string>

namespace Game
{

	class State
	{
	public:
	       
		virtual ~State() {}      
	       
		virtual void Enter() = 0;
		virtual void Exit() = 0;
		virtual void Handle() = 0;
	};

	class StateMachine
	{
	public:
	       
		StateMachine();
		~StateMachine();    
		   
		bool RequestStateChange( std::string newState );
		void Handle();
		   
		void AddState( std::string name, State* addState );
		   
		static StateMachine* GetSM();
		   
	private:
		        
		static StateMachine*       sStateMachine;
		       
		std::map< std::string, State* > mStates;
		State*                          mNextState;
		State*                          mCurState;
		bool                            mStateChange;
	};

	#define SMachine (*StateMachine::GetSM())

}; //end Game

#endif // end _GAME_STATE_MACHINE_H_
    
