//---------------------------------------------------
// Name: Game : Arrow
// Desc:  the game xfactor
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_ARROW_H_
#define _GAME_ARROW_H_

#include "Types.h"
#include "Entity.h"

#include <vector>
#include <map>

namespace Game
{
	// properties describing the arrow
	enum ArrowPropertyFlag
	{
        kArrowProp_StartPosition = kEntProp_EndProp,	// starting position ( 2 ints )
		kArrowProp_StartVelocity,					    // starting velocity ( 2 ints )
		kArrowProp_StartAlpha,							// starting alpha ( 1 float )
		kArrowProp_Lifetime,							// lifetime ( 1 float )
		kArrowProp_GenSound,							// 1 string
		kArrowProp_Rotation,							// 1 float
		kArrowProp_EndProp
	};

	//-----------------------------------------------------------
	// Name: Arrow
	// Desc:  a projectile in the game
	//-----------------------------------------------------------
	class Arrow : public Entity
	{	
	public:

		// goes through the arrow properties and grabs what it can use
		Arrow( EntityDesc* desc );

		virtual ~Arrow();

		// update and draw arrow, children should override these
		virtual void Update( F32 curTime );
		virtual void Draw();		

		// get the position at a given time, children should override thsi
		virtual bool GetPosAtTime( F32 time, int32_t& x, int32_t& y );

		// time manipulation methods to put arrows offscreen
		F32 AdjustTimeToScreenLeft( F32 curTime );
		void ModifyStartTime( F32 modAmt );

		// collision methods, maybe override these
		virtual bool Collide( F32 radius, F32* center );
		
		// deactivate an arrow
		virtual bool Kill();

		// check if the arrow is active
		bool IsActive() const;		

		BoundingBoxf* GetBBox() { return &mBBox; }

		// get the base type
		uint32_t GetBaseType() const { return kEntBase_Arrow; }

		// play a generation sound;
		void PlayGenSound();

	protected:

		// update the position of the bounding box
		void UpdateBBox();

		BoundingBoxf		mBBox;			// our bounding box
		int32_t				mPos[2];		// current position
		int32_t				mStartPos[2];   // position when arrow was spawned
		int32_t				mVel[2];        // velocity		

		bool				mActive;		// activeness
		
		SoundX*				mpGenSound;
	};	
	
}; //end Game

#endif // end _GAME_ARROW_H_
    
