//---------------------------------------------------
// Name: Game : Character
// Desc:  a character on the field
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Character.h"

#include "GameXExt.h"
#include "GameConstants.h"

#include "Util/Tuner.h"

namespace Game
{
	bool Character::Init( int32_t* pos )
	{
		//load the images	
		if( !( mBalloonImg = GetImage( "textures/balloon.tga" ) ) )
			return false;

		if( !( mBodyImg = GetImage( "textures/body.tga" ) ) )
			return false;		

		mPos[0] = pos[0];
		mPos[1] = pos[1];
		mBalloonInflation = 1.0f;

		mState = kState_Alive;

		UpdateBBox();
		return true;
	}
	
	void Character::MoveToPosition( int32_t x, int32_t y )
	{
		if( mState != kState_Alive )
			return; 

		mPos[0] = x;
		mPos[1] = y;
		UpdateBBox();
	}

	void Character::MoveByDelta( int32_t dx, int32_t dy )
	{
		//if( mState != kState_Alive )
		//	return; 

		mPos[0] += dx;
		mPos[1] += dy;

		if( mPos[0] < ( mBalloonImg->GetWidth()/2 ) )
			mPos[0] = mBalloonImg->GetWidth()/2;

		if( mPos[0] > (int32_t)( kWindowWidth - mBalloonImg->GetWidth()/2 ) )
			mPos[0] = (int32_t)( kWindowWidth - mBalloonImg->GetWidth()/2 );

		UpdateBBox();
	}

	void Character::InflateBalloon( F32 amt )
	{
		if( mState != kState_Alive )
			return; 

		const F32 kInflationMin = gTuner.GetFloat( "kInflationMin" );
		const F32 kInflationMax = gTuner.GetFloat( "kInflationMax" );

		mBalloonInflation += amt;

		if( mBalloonInflation < kInflationMin )
			mBalloonInflation = kInflationMin;

		if( mBalloonInflation > kInflationMax )
			mBalloonInflation = kInflationMax;

		UpdateBBox();
	}

	void Character::Pop()
	{
		mState = kState_Popped;
		mDieFadeAlpha = 1.0f;
		mDieFadeImg = GetImage( "textures/dieFade.tga" );
	}

	void Character::Update( F32 curTime )
	{
		if( mState == kState_Alive )
		{
			F32 kGravity = 10.0f;		

			const F32 kBuoyancy = gTuner.GetFloat( "kBuoyancy" );

			int32_t dy = (int32_t)(kGravity + mBalloonInflation * kBuoyancy );
			if( !dy ) dy += 2;
			MoveByDelta( 0, dy );
		}
	}
	
	void Character::Draw()
	{	
		int32_t width  = (int32_t)(mBalloonImg->GetWidth()  * mBalloonInflation);
		int32_t height = (int32_t)(mBalloonImg->GetHeight() * mBalloonInflation);

		GameX.DrawImage( mBalloonImg, mPos[0] - width/2, mPos[1] - height/2, 0.0f, mBalloonInflation );

		int32_t bodyX = mPos[0] - mBodyImg->GetWidth()/2;
		int32_t bodyY = mPos[1] + height/2;

		GameX.DrawImage( mBodyImg, bodyX, bodyY );

		if( mState == kState_Popped )
		{
			if( mDieFadeAlpha <= 0 )
			{
				mState = kState_Dead;			
			}
			else if( mDieFadeImg )
			{	
				GameX.DrawImage( mDieFadeImg, mPos[0] - mDieFadeImg->GetWidth()/2,
											  mPos[1] - mDieFadeImg->GetHeight()/2 );				
			}
		}
	}

	BoundingBoxf* Character::GetBBox()
	{
		return &mBBox;
	}

	Character::State Character::GetState()
	{
		return mState;
	}

	bool Character::Collide( const BoundingBoxf& bbox )
	{
		F32 pos [] = { (F32)mPos[0], (F32)mPos[1] };
		return bbox.Collide( mBalloonImg->GetWidth() * mBalloonInflation * 0.5f, pos );
	}

	void Character::UpdateBBox()
	{
		mBBox.mX = mPos[0] - ( mBalloonImg->GetWidth()  * mBalloonInflation ) / 2;
		mBBox.mY = mPos[1] - ( mBalloonImg->GetHeight() * mBalloonInflation ) / 2;
		mBBox.mWidth  = mBalloonImg->GetWidth()  * mBalloonInflation;
		mBBox.mHeight = mBalloonImg->GetHeight() * mBalloonInflation;
	}

}; //end Game
