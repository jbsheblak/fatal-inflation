//---------------------------------------------------
// Name: Game : AllArrows
// Desc:  all of the game arrows
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_ALL_ARROWS_H_
#define _GAME_ALL_ARROWS_H_

#include "Arrow.h"

namespace Game
{	
	class TimedArrow : public Arrow
	{	
	public:

		virtual ~TimedArrow() {}

		TimedArrow( EntityDesc* desc );

		virtual void Update( F32 curTime );
		
	protected:				

		F32		mLifetime;			
	};	
	
}; //end Game

#endif // end _GAME_ALL_ARROWS_H_
    
