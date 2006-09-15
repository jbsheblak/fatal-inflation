//---------------------------------------------------
// Name: Game : BaseArrow
// Desc:  fills in properties, flies straight
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "AllArrows.h"

namespace Game
{	
	TimedArrow::TimedArrow( EntityDesc* desc ) : Arrow(desc)
	{
		mLifetime = -1.0f;

		if( desc )
		{
			EntityDesc::iterator itr;
			for( itr = desc->begin(); itr != desc->end(); ++itr )
			{
				switch( itr->mFlag )
				{

				case kArrowProp_Lifetime:
					{
						memcpy( &mLifetime, itr->mData, sizeof(F32) );
						break;
					}
				}
			}
		}

		UpdateBBox();		
	}

	void TimedArrow::Update( F32 curTime )
	{
		Arrow::Update(curTime);		

		if( mLifetime != -1.0f )
		{
			if( curTime >= mStartTime + mLifetime )
				Kill();	
		}
	}
	

}; //end Game
