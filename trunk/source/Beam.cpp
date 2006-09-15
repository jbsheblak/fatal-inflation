//---------------------------------------------------
// Name: Game : Beam
// Desc:  a beam that fires
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Beam.h"
#include "GameConstants.h"
#include <assert.h>

namespace Game
{	
	Beam::Beam( EntityDesc* desc ) : Entity(desc)
	{
		mActive = true;

		if( desc )
		{
			EntityDesc::iterator itr;			
			for( itr = desc->begin(); itr != desc->end(); ++itr )
			{
				switch( itr->mFlag )
				{			

				case kBeamProp_StartPosition:
					{
						memcpy( mStartPos, itr->mData, sizeof(int32_t) * 2 );
						break;
					}

				case kBeamProp_FireFrequency:
					{
						memcpy( &mFireFrequency, itr->mData, sizeof(F32) );						
						break;
					}

				case kBeamProp_FireDuration:
					{						
						memcpy( &mFireDuration, itr->mData, sizeof(F32) );						
						break;
					}
				}
			}
		}
	}

	void Beam::Update( F32 curTime )
	{		
		curTime -= mStartTime;		

		// are we firing?
		F32 mult = curTime / mFireFrequency;
		F32 fireTime = ( (int32_t)mult ) * mFireFrequency;
		mFiring = fabs( fireTime - curTime ) < mFireDuration;

		// update the bbox
		UpdateBBox( curTime );		
	}

	void Beam::Draw()
	{
		if( mBaseImage && mFiring )
		{
			uint32_t width = mBaseImage->GetWidth();
			uint32_t drawCount = kWindowWidth / width;

			for( uint32_t x = 0; x < kWindowWidth; x += width )
				GameX.DrawImage( mBaseImage, x, mStartPos[1] );
		}		
	}

	bool Beam::IsActive() const
	{
		return mActive;
	}

	BoundingBoxf* Beam::GetBBox()
	{
		return &mBBox;
	}

	void Beam::UpdateBBox( F32 time )
	{		
		if( !mFiring )
		{
			mBBox.mX = -1;
			mBBox.mY = -1;
			mBBox.mWidth = 0;
			mBBox.mHeight = 0;
		}
		else
		{
			mBBox.mX = 0;
			mBBox.mY = (F32)mStartPos[1];
			mBBox.mWidth = (F32)kWindowWidth;
			mBBox.mHeight = (F32)mBaseImage->GetHeight();
		}
	}
	
}; //end Game