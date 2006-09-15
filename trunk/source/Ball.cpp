//---------------------------------------------------
// Name: Game : Ball
// Desc:  ball that bounces off the sides
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Ball.h"
#include "GameConstants.h"
#include <assert.h>

namespace Game
{	
	Ball::Ball( EntityDesc* desc ) : Entity(desc)
	{
		srand( (uint32_t)time(NULL) );

		int32_t randNum = rand();

		mStartRotation = 0.0f;
		mRotationPerturb = (F32)( (randNum % 1000) * 20 );
		mActive = true;

		if( desc )
		{
			EntityDesc::iterator itr;			
			for( itr = desc->begin(); itr != desc->end(); ++itr )
			{
				switch( itr->mFlag )
				{			

				case kBallProp_StartPosition:
					{
						memcpy( mPos, itr->mData, sizeof(int32_t) * 2 );
						memcpy( mStartPos, itr->mData, sizeof(int32_t) * 2 );					

						break;
					}

				case kBallProp_StartVelocity:
					{
						memcpy( mStartVel, itr->mData, sizeof(int32_t) * 2 );
						memcpy( mVel, itr->mData, sizeof(int32_t) * 2 );
						break;
					}

				case kBallProp_Rotation:
					{
						memcpy( &mStartRotation, itr->mData, sizeof(F32) );
						break;
					}
				}
			}
		}
	}

	void Ball::Update( F32 curTime )
	{		
		curTime -= mStartTime;

		GetPosAtTime( curTime, mPos[0], mPos[1] );
		UpdateBBox();

		mRotation = mRotationPerturb + 360 * mStartRotation * curTime;
	}

	void Ball::Draw()
	{
		if( mBaseImage )
		{
			GameX.DrawImage( mBaseImage, mPos[0], mPos[1], mRotation, 1.0f );
		}
	}

	bool Ball::IsActive() const
	{
		return mActive;
	}

	BoundingBoxf* Ball::GetBBox()
	{
		return &mBBox;
	}	

	// This function is for edit mode only...
	void Ball::GetPosAtTime( F32 time, int32_t& x, int32_t& y )
	{
		//Integrate our way to the solution...slowly.

		const F32 kTimeInc = 0.01f;

		memcpy( mVel, mStartVel, sizeof(uint32_t) * 2 );
		
		F32 fPos[2];
		fPos[0] = (F32)mStartPos[0];
		fPos[1] = (F32)mStartPos[1];

		int32_t width = 0;
		int32_t height = 0;
		if( mBaseImage )
		{
			width  = mBaseImage->GetWidth();
			height = mBaseImage->GetHeight();
		}
		
		for( F32 intgTime = 0.0f; intgTime < time; intgTime += kTimeInc )
		{
			GetPosRuntime( kTimeInc, fPos[0], fPos[1], fPos[0], fPos[1] );
			mPos[0] = (int32_t)fPos[0];
			mPos[1] = (int32_t)fPos[1];
			UpdateBBox();

			if( OutOfBounds() )
			{			
				if( fPos[0] < 0 )
				{
					fPos[0] = 0;
					mVel[0] = -mVel[0];
				}
				else if( fPos[0] + mBBox.mWidth > kWindowWidth )
				{
					fPos[0] = kWindowWidth - ( mBBox.mWidth );
					mVel[0] = -mVel[0];
				}

				if( fPos[1] < 0 )
				{
					fPos[1] = 0;
					mVel[1] = -mVel[1];
				}
				else if( fPos[1] + mBBox.mHeight > kWindowHeight )
				{
					fPos[1] = kWindowHeight - ( mBBox.mHeight );
					mVel[1] = -mVel[1];
				}
			}

			if( intgTime >= time - kTimeInc )
			{
				int sdfsdf = 10;
			}
		}

		x = (int32_t)fPos[0];
		y = (int32_t)fPos[1];	

	}

	void Ball::GetPosRuntime( F32 dt, F32 oldPosX, F32 oldPosY, F32 &x, F32& y )
	{
		x = oldPosX + mVel[0] * dt;
		y = oldPosY + mVel[1] * dt;		
	}

	bool Ball::OutOfBounds()
	{
		return  ( ( mBBox.mX < 0 ) || ( mBBox.mX + mBBox.mWidth > kWindowWidth ) ||
				  ( mBBox.mY < 0 ) || ( mBBox.mY + mBBox.mHeight > kWindowHeight ) );
	}

	void Ball::UpdateBBox()
	{
		if( mBaseImage )
		{
			mBBox.mX = (F32)(mPos[0]);
			mBBox.mY = (F32)(mPos[1]);
			mBBox.mWidth  = (F32)(mBaseImage->GetWidth());
			mBBox.mHeight = (F32)(mBaseImage->GetHeight());
		}
	}
	
}; //end Game