//---------------------------------------------------
// Name: Game : Arrow
// Desc:  the game xfactor
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Arrow.h"
#include "Log.h"
#include "GameXExt.h"
#include "Algorithms.h"

#include "FileIO.h"
#include "ResourceCache.h"

#include <math.h>
#include <string>

namespace Game
{	
	//-----------------------------------------------------------
	// Name: Arrow
	// Desc:  Constructor - parses description
	//-----------------------------------------------------------
	Arrow::Arrow( EntityDesc* desc ) : Entity(desc)
	{
		mStartTime = 0;
		mActive = true;
		mpGenSound = NULL;				

		bool bStartPos = false,
			 bStartVel = false;			 

		if( desc )
		{
			EntityDesc::iterator itr;			
			for( itr = desc->begin(); itr != desc->end(); ++itr )
			{
				switch( itr->mFlag )
				{

				case kArrowProp_StartPosition:
					{
						memcpy( mStartPos, itr->mData, sizeof(int32_t)*2 );
						memcpy( mPos, itr->mData, sizeof(int32_t)*2 );
						bStartPos = true;
						break;
					}

				case kArrowProp_StartVelocity:
					{
						memcpy( mVel, itr->mData, sizeof(int32_t)*2 );
						bStartVel = true;
						break;
					}	

				case kArrowProp_GenSound:
					{
						if( mpGenSound = GetSound( (char*)itr->mData ) )						
						{
							// for some reason the arrow sound needs to play once
							// or the data does not get initialized...							
							GameX.PlaySound( mpGenSound, PLAY_REWIND, 0.0f, 0, 1.0f );
						}						
					}

				case kArrowProp_Rotation:
					{
						memcpy( &mBBox.mRotation, itr->mData, sizeof(F32) );
						break;
					}
				}
			}
		}

		//set defaults
		if( !bStartPos )
		{
			mStartPos[0] = 0;
			mStartPos[1] = 0;
			SLog->Print( "No start position specified. Defaulting" );
		}

		if( !bStartVel )
		{
			mVel[0] = 0;
			mVel[1] = 0;
			SLog->Print( "No start velocity specified. Defaulting" );
		}	

		// rotate velocities
		if( mBBox.mRotation != 0.0f )
		{
			// rotate our velocities
			F32 cos_val = cos( (F32)mBBox.mRotation * 3.14159f / 180.0f );
			F32 sin_val = sin( (F32)mBBox.mRotation * 3.14159f / 180.0f );

			int32_t vel [] = { mVel[0], mVel[1] };

			mVel[0] = (int32_t)(  vel[0] * cos_val + vel[1] * sin_val );
			mVel[1] = (int32_t)( -vel[0] * sin_val + vel[1] * cos_val );
		}
		
		UpdateBBox();
	}

	//-----------------------------------------------------------
	// Name: ~Arrow
	// Desc:   Deconstructor
	//-----------------------------------------------------------
	Arrow::~Arrow()
	{	
	}

	//-----------------------------------------------------------
	// Name: Update
	// Desc:  does an update based on time
	//-----------------------------------------------------------
	void Arrow::Update( F32 curTime )
	{
		GetPosAtTime( curTime, mPos[0], mPos[1] );		
		UpdateBBox();		
	}

	//-----------------------------------------------------------
	// Name: Draw
	// Desc:  draws the arrow base image
	//-----------------------------------------------------------
	void Arrow::Draw()
	{
		if( mBaseImage )
		{	
			GameX.DrawImage( mBaseImage, mPos[0], mPos[1], mBBox.mRotation, 1.0f );
		}
	}

	//-----------------------------------------------------------
	// Name: GetPosAtTime
	// Desc:  calculates where the arrow is at a given time
	//-----------------------------------------------------------
	bool Arrow::GetPosAtTime( F32 time, int32_t& x, int32_t& y )
	{
		// linear interpolation of position, change child classes to modify this
		x = (int32_t)( mStartPos[0] + mVel[0] * ( time - mStartTime ) );
		y = (int32_t)( mStartPos[1] + mVel[1] * ( time - mStartTime ) );
		return true;
	}

	//-----------------------------------------------------------
	// Name: AdjustTimeToScreenLeft
	// Desc:  calculates at what time this arrow would be screen left
	//-----------------------------------------------------------
	F32 Arrow::AdjustTimeToScreenLeft( F32 curTime )	
	{
		if( mVel[0] != 0 )
		{
			//old starting positions
			const int32_t oStartX = mStartPos[0];
			const int32_t oStartY = mStartPos[1];

			const int32_t kTargetX = mBaseImage == NULL ? 0 : -mBaseImage->GetWidth();
			const int32_t kDistToMoveX = mPos[0] - kTargetX;
			const F32 kTimeAdjustment = kDistToMoveX / (F32)mVel[0];

			GetPosAtTime( -kTimeAdjustment, mStartPos[0], mStartPos[1] );
			mStartTime = curTime - kTimeAdjustment;			

			return mStartTime;
		}

		return 0.0f;
	}

	//-----------------------------------------------------------
	// Name: ModifyStartTime
	// Desc:  
	//-----------------------------------------------------------
	void Arrow::ModifyStartTime( F32 modAmt )
	{
		mStartTime += modAmt;
	}
	
	//-----------------------------------------------------------
	// Name: Collide
	// Desc:  collide a circle with this arrow
	//-----------------------------------------------------------
	bool Arrow::Collide( F32 radius, F32* center )
	{
		return mBBox.Collide( radius, center );
	}	
	
	//-----------------------------------------------------------
	// Name: Kill
	// Desc:  deactivate this arrow
	//-----------------------------------------------------------
	bool Arrow::Kill()
	{
		mActive = false;
		return true;
	}

	//-----------------------------------------------------------
	// Name: IsActive
	// Desc:  returns whether this arrow is active
	//-----------------------------------------------------------
	bool Arrow::IsActive() const
	{
		return mActive;
	}

	//-----------------------------------------------------------
	// Name: PlayGenSound
	// Desc:  plays a generation sound if we have one
	//-----------------------------------------------------------
	void Arrow::PlayGenSound()
	{
		if( mpGenSound )
			GameX.PlaySound( mpGenSound );
	}		

	//-----------------------------------------------------------
	// Name: UpdateBBox
	// Desc:  updates the bbox based on new position
	//-----------------------------------------------------------
	void Arrow::UpdateBBox()
	{
		mBBox.mX = (F32)mPos[0];
		mBBox.mY = (F32)mPos[1];

		if( mBaseImage )
		{
			mBBox.mWidth = (F32)mBaseImage->GetWidth();
			mBBox.mHeight = (F32)mBaseImage->GetWidth();
		}
		else
		{
			mBBox.mWidth = 64;
			mBBox.mHeight = 64;
		}
	}
	
}; //end Game
