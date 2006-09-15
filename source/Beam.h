//---------------------------------------------------
// Name: Game : Beam
// Desc:  a beam that fires
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_BEAM_H_
#define _GAME_BEAM_H_

#include "Types.h"
#include "Entity.h"

namespace Game
{
	// properties describing a beam
	enum BeamPropertyFlag
	{
		kBeamProp_StartPosition = kEntProp_EndProp, // 2 ints
		kBeamProp_FireFrequency,					// 1 float
		kBeamProp_FireDuration,						// 1 float
		kBeamProp_EndProp
	};	
	
	//-----------------------------------------------------------
	// Name: Beam
	// Desc:  a beam
	//-----------------------------------------------------------
	class Beam : public Entity
	{
	public:

		virtual ~Beam() {}
		Beam( EntityDesc* desc );

		// update and draw, children should override these
		virtual void Update( F32 curTime );
		virtual void Draw();
		bool IsActive() const;

		// get the bounding box
		virtual BoundingBoxf* GetBBox();		

		// get the base type
		virtual uint32_t GetBaseType() const { return kEntBase_Beam; }

	protected:

		void GetPosAtTime( F32 time, int32_t& x, int32_t& y );
		void GetPosRuntime( F32 dt, F32 oldPosX, F32 oldPosY, F32 &x, F32& y );
		bool OutOfBounds();
		void UpdateBBox( F32 time );

	protected:

		bool				mFiring;
		bool				mActive;
		int32_t				mStartPos[2];
		BoundingBoxf		mBBox;

		F32					mFireFrequency;
		F32					mFireDuration;
	};

	
	
}; //end Game

#endif // end _GAME_BEAM_H_
    
