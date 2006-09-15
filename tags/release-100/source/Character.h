//---------------------------------------------------
// Name: Game : Character
// Desc:  a character on the field
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_CHARACTER_H_
#define _GAME_CHARACTER_H_

#include "BoundingBox.h"
#include "gamex.hpp"

namespace Game
{
	class Character
	{
	public:

		enum State
		{
			kState_Alive,
			kState_Popped,
			kState_Dead
		};

	public:		
		
		bool	Init( int32_t* pos );

		void	MoveToPosition( int32_t x, int32_t y );
		void	MoveByDelta( int32_t dx, int32_t dy );
		void    InflateBalloon( F32 amt );

		void	Pop();

		void	Update( F32 curTime );
		void	Draw();

		BoundingBoxf*	GetBBox();
		State			GetState();

		bool	Collide( const BoundingBoxf& bbox );

	protected:

		void UpdateBBox();

	protected:

		BoundingBoxf		mBBox;
		int32_t				mPos[2];
		F32					mBalloonInflation;
		State				mState;

		ImageX*				mBalloonImg;
		ImageX*				mBodyImg;

		ImageX*				mDieFadeImg;
		float				mDieFadeAlpha;
	};

}; //end Game

#endif //end _GAME_CHARACTER_H_
