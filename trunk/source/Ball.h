//---------------------------------------------------
// Name: Game : Ball
// Desc:  ball that bounces off the sides
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_BALL_H_
#define _GAME_BALL_H_

#include "Types.h"
#include "Entity.h"

namespace Game
{
	// properties describing a ball
	enum BallPropertyFlag
	{
		kBallProp_StartPosition	= kEntProp_EndProp,	// 2 ints
		kBallProp_StartVelocity,					// 2 ints
		kBallProp_Rotation,							// 1 float
		kBallProp_EndProp
	};
	
	//-----------------------------------------------------------
	// Name: Ball
	// Desc:  a bouncy ball
	//-----------------------------------------------------------
	class Ball : public Entity
	{
	public:

		virtual ~Ball() {}
		Ball( EntityDesc* desc );

		// update and draw, children should override these
		virtual void Update( F32 curTime );
		virtual void Draw();
		bool IsActive() const;

		// get the bounding box
		virtual BoundingBoxf* GetBBox();		

		// get the base type
		virtual uint32_t GetBaseType() const { return kEntBase_Ball; }

	protected:

		void GetPosAtTime( F32 time, int32_t& x, int32_t& y );
		void GetPosRuntime( F32 dt, F32 oldPosX, F32 oldPosY, F32 &x, F32& y );
		bool OutOfBounds();
		void UpdateBBox();

	protected:

		bool				mActive;
		int32_t				mStartPos[2];
		int32_t				mPos[2];
		int32_t				mStartVel[2];
		int32_t				mVel[2];	
		F32					mStartRotation;
		F32					mRotation;
		F32					mRotationPerturb;

		BoundingBoxf		mBBox;
	};

	
	
}; //end Game

#endif // end _GAME_ARROW_H_
    
