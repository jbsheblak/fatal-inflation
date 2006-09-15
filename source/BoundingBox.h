//---------------------------------------------------
// Name: Game : BoundingBox
// Desc:  a bounding ... box.
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_BBOX_H_
#define _GAME_BBOX_H_

#include "Types.h"
#include <math.h>
#include <assert.h>

#include "gamex.hpp"

namespace Game
{
	template <typename T>
	class BoundingBox
	{
	private:
		
		//utility functions

		//-------------------------------------------------------------
		// Name: Normalize
		// Desc:  normalizes a directional vector
		//-------------------------------------------------------------
		void Normalize( T* vec ) const
		{
			T len = vec[0] * vec[0] + vec[1] * vec[1];
			if( len != (T)0 )
			{
				T sqrt_len = (T)sqrt( (double)len );
				vec[0] /= sqrt_len;
				vec[1] /= sqrt_len;
			}
		}

		//-------------------------------------------------------------
		// Name: SideCheck
		// Desc:  performs an intersection check of one side of bbox with circle
		//-------------------------------------------------------------
		bool SideCheck( T radius, T centerX, T centerY,
						T x0, T y0, T x1, T y1 ) const
		{
			T dir [] = { x1-x0, y1-y0 };
			Normalize(dir);

			// rotate directional vector by bbox amt
			F32 cos_val = cos( (F32)-mRotation );
			F32 sin_val = sin( (F32)-mRotation );

            dir[0] =  dir[0] * cos_val + dir[0] * sin_val;
			dir[1] = -dir[1] * sin_val + dir[1] * cos_val;			

			float v[] = { dir[0], dir[1] };
			float c[] = { centerX, centerY };
			float x[] = { x0, y0 };			

			float A = v[0]*v[0] + v[1]*v[1];
			if( A == 0.0f )
				return false;

			float B = 2*v[0]*(x[0]-c[0]) + 2*v[1]*(x[1]-c[1]);
			float C = (x[0]-c[0])*(x[0]-c[0]) + (x[1]-c[1])*(x[1]-c[1]) - radius*radius;

			float discrimSquared = B*B-4*A*C;
			if( discrimSquared < 0 )
				return false;

			float discrim = (float)sqrt( discrimSquared );

			float t1 = (-B + discrim)/ ( 2.0f * A );
			float t2 = (-B - discrim)/ ( 2.0f * A );
			
			return ( (t1 >= 0.0f && t1 <= 1.0f) ||
				     (t2 >= 0.0f && t2 <= 1.0f ) );			
		}

		//-------------------------------------------------------------
		// Name: RotateVector
		// Desc:  rotate a vector by deg degress
		//-------------------------------------------------------------
		void RotateVector( T* vec, F32 deg ) const
		{
			T oldVec[] = { vec[0], vec[1] };

			F32 cos_val = cos( (F32)deg * 3.14159f/180.0f  );
			F32 sin_val = sin( (F32)deg * 3.14159f/180.0f );

			vec[0] = (T)(  oldVec[0] * cos_val - oldVec[1] * sin_val );
			vec[1] = (T)(  oldVec[0] * sin_val + oldVec[1] * cos_val );			
		}		
		
	public:

		//public variables
		T		mX, mY;				// top left corner of bbox
		T		mWidth, mHeight;	// dimensions
		F32		mRotation;			// rotation of the bbox

		//-------------------------------------------------------------
		// Name: BoundingBox
		// Desc:  constructor
		//-------------------------------------------------------------
		BoundingBox() : mX(0), mY(0), mWidth(0), mHeight(0), mRotation(0)
		{}			

		//-------------------------------------------------------------
		// Name: Collide
		// Desc:  collides a point with this bounding box
		//-------------------------------------------------------------
		bool Collide( T x, T y )
		{
			T halfWidth  = mWidth/2;
			T halfHeight = mHeight/2;

			// put point in coordinate system relative to center of our box
			T c[] = { mX + halfWidth, mY + halfHeight };
			T p[] = { x - c[0], y - c[1] };
			RotateVector( p, mRotation );		

			return ( (p[0] >= -halfWidth  && p[0] <= halfWidth ) &&
					 (p[1] >= -halfHeight && p[1] <= halfHeight) );			
		}

		//-------------------------------------------------------------
		// Name: Collide
		// Desc:  collides a circle with this bbox
		//-------------------------------------------------------------
		bool Collide( T radius, T* center ) const
		{
			// rotate our bounding box
			T dirX [] = { mWidth/2, 0 };
			T dirY [] = { 0, mHeight/2 };

			RotateVector( dirX, -mRotation );
			RotateVector( dirY, -mRotation );

			T c[] = { mX + mWidth/2, mY + mHeight/2 };

			F32 tl[] = { c[0] - dirX[0] - dirY[0], c[1] - dirX[1] - dirY[1] };
			F32 bl[] = { c[0] - dirX[0] + dirY[0], c[1] - dirX[1] + dirY[1] };
			F32 tr[] = { c[0] + dirX[0] - dirY[0], c[1] + dirX[1] - dirY[1] };
			F32 br[] = { c[0] + dirX[0] + dirY[0], c[1] + dirX[1] + dirY[1] };	

			// do a ray / circle collision check with each side
			if( SideCheck( radius, center[0], center[1], tl[0], tl[1], tr[0], tr[1] ) ||
				SideCheck( radius, center[0], center[1], tr[0], tr[1], br[0], br[1] ) ||
				SideCheck( radius, center[0], center[1], br[0], br[1], bl[0], bl[1] ) ||
				SideCheck( radius, center[0], center[1], bl[0], bl[1], tl[0], tl[1] ) )
				return true;			

			return false;
		}			

		//-------------------------------------------------------------
		// Name: Draw
		// Desc:  draw the bounding box
		//-------------------------------------------------------------
		void Draw( int32_t red = 255, int32_t green = 255, int32_t blue = 255 )
		{		
			ColorX clr;
			clr.SetRed  ( red );
			clr.SetGreen( green );
			clr.SetBlue ( blue );

			T dirX [] = { mWidth/2, 0 };
			T dirY [] = { 0, mHeight/2 };

			RotateVector( dirX, -mRotation );
			RotateVector( dirY, -mRotation );

			T c[] = { mX + mWidth/2, mY + mHeight/2 };

			F32 tl[] = { c[0] - dirX[0] - dirY[0], c[1] - dirX[1] - dirY[1] };
			F32 bl[] = { c[0] - dirX[0] + dirY[0], c[1] - dirX[1] + dirY[1] };
			F32 tr[] = { c[0] + dirX[0] - dirY[0], c[1] + dirX[1] - dirY[1] };
			F32 br[] = { c[0] + dirX[0] + dirY[0], c[1] + dirX[1] + dirY[1] };						

			GameX.DrawLine( clr, (int32_t)tl[0], (int32_t)tl[1], (int32_t)tr[0], (int32_t)tr[1] );
			GameX.DrawLine( clr, (int32_t)tr[0], (int32_t)tr[1], (int32_t)br[0], (int32_t)br[1] );
			GameX.DrawLine( clr, (int32_t)br[0], (int32_t)br[1], (int32_t)bl[0], (int32_t)bl[1] );
			GameX.DrawLine( clr, (int32_t)bl[0], (int32_t)bl[1], (int32_t)tl[0], (int32_t)tl[1] );			
		}
	};

	typedef BoundingBox<F32>		BoundingBoxf;
	typedef BoundingBox<int32_t>	BoundingBoxi;
	typedef BoundingBox<uint32_t>	BoundingBoxu;	
	
}; //end Game

#endif // end _GAME_BBOX_H_
    
