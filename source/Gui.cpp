//---------------------------------------------------
// Name: Game : Gui
// Desc:  interface
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Gui.h"
#include "gamex.hpp"
#include "GameConstants.h"

#include <assert.h>

namespace Game
{
	GuiElement::~GuiElement()
	{
		for( uint32_t i = 0; i < mChildren.size(); ++i )
			delete mChildren[i];
	}

	GuiElement::GuiElement() : mpParent(NULL)
						     , mpImageNormal(NULL)
							 , mpImageOver(NULL)
							 , mpImageDepressed(NULL)
							 , mVisible(false)
							 , mState( kState_Normal )
							 , mAlignX( kAlign_RelativeLeft )
							 , mAlignY( kAlign_RelativeTop )
	{
		memset( mPos, 0, sizeof(int32_t)*2 );
	}
	
	void GuiElement::SetImages( ImageX* normal, ImageX* mouseOver, ImageX* mouseClicked )
	{
		mpImageNormal			= normal;
		mpImageOver				= mouseOver;
		mpImageDepressed		= mouseClicked;
	}

	void GuiElement::SetParent( GuiElement* elem )
	{
		mpParent = elem;
	}

	void GuiElement::SetPosition( int32_t x, int32_t y )
	{
		mPos[0] = x;
		mPos[1] = y;
	}

	void GuiElement::SetExtent( int32_t eX, int32_t eY )
	{
		mExtent[0] = eX;
		mExtent[1] = eY;
	}

	void GuiElement::SetExtentFromImages()
	{
		if( mpImageNormal )
		{
			SetExtent( mpImageNormal->GetWidth(), mpImageNormal->GetHeight() );
		}
	}

	void GuiElement::SetAlignment( Alignment align )
	{
		if( align <= kAlign_VertCenter )
			mAlignY = align;

		else
			mAlignX = align;
	}

	void GuiElement::SetVisible( bool visible )
	{
		mVisible = visible;
	}

	bool GuiElement::Draw()
	{
		// are we visible?
		if( !mVisible )
			return true;

		//draw ourself, then draw children
		BaseBoundsDraw();

		std::vector<GuiElement*>::iterator itr;
		for( itr = mChildren.begin(); itr != mChildren.end(); ++itr )
		{
			(*itr)->Draw();
		}

		return true;
	}

	void GuiElement::AddChild( GuiElement* child )
	{
		mChildren.push_back(child);
	}

	GuiElement::State GuiElement::GetState()
	{
		return mState;
	}

	// gets the screenspace x
	int32_t	GuiElement::GetX()
	{
		int32_t px  = 0;		
		int32_t peX = kWindowWidth;		

		if( mpParent )
		{
			px  = mpParent->GetX();			
			peX = mpParent->GetExtentX();			
		}

		switch( mAlignX )
		{		
		case kAlign_Left:		   return px;
		case kAlign_Right:		   return px + peX - GetExtentX();
		case kAlign_RelativeLeft:  return px + mPos[0];
		case kAlign_RelativeRight: return px + peX - GetExtentX() - mPos[0];
		case kAlign_HorizCenter:   return px + (peX - GetExtentX())/2;
		default: assert(false); return -1;
		}		
	}

	// gets the screenspace y
	int32_t	GuiElement::GetY()
	{
		int32_t py  = 0;		
		int32_t peY = kWindowHeight;

		if( mpParent )
		{
			py  = mpParent->GetY();			
			peY = mpParent->GetExtentY();
		}

		switch( mAlignY )
		{		
		case kAlign_Top:				return py;
		case kAlign_Bottom:				return py + peY - GetExtentY();
		case kAlign_RelativeTop:		return py + mPos[1];
		case kAlign_RelativeBottom:		return py + peY - GetExtentY() - mPos[1];
		case kAlign_VertCenter:			return py + (peY - GetExtentY())/2;
		default: assert(false); return -1;
		}	
	}

	int32_t GuiElement::GetExtentX()
	{
		return mExtent[0];
	}

	int32_t GuiElement::GetExtentY()
	{
		return mExtent[1];
	}	

	void GuiElement::OnMouseOver()
	{
		mState = kState_MouseOver;
	}

	bool GuiElement::CheckMouse( int32_t x, int32_t y, bool lClick, bool rClick )
	{
		//is the mouse in our bounds?
		if( MouseInBounds(x,y) )
		{
			//we are in an L-Click
			if( mState == kState_Normal && lClick )
			{				
				OnClick();
			}

			//we are depressed and no L-Click
			else if( mState == kState_Depressed && !lClick )
			{
				mState = kState_MouseOver;				
				OnClickRelease();
			}

			//mouse just entered and no L-Click
			else if( mState == kState_Normal && !lClick )
			{				
				OnMouseOver();
			}

			return true;
		}

		//no it isn't
		else
		{
			if( mState == kState_MouseOver )
			{				
				OnMouseOut();
			}

			else if( mState == kState_Depressed )
			{
				mState = kState_Normal;
				OnClickRelease();
			}

			return false;
		}
	}

	void GuiElement::OnMouseOut()
	{
		mState = kState_Normal;
	}

	void GuiElement::OnClick()
	{
		mState = kState_Depressed;
	}

	void GuiElement::OnClickRelease()
	{
		//mState = kState_Normal;
	}

	void GuiElement::BaseBoundsDraw()
	{
		ImageX* img;
		if( (img = GetStateImage()) == NULL )
			img = mpImageNormal;

		if( img )
		{
			GameX.DrawImage( img,GetX(), GetY() );
		}
	}

	ImageX* GuiElement::GetStateImage()
	{
		switch( mState )
		{
		case kState_Normal:		return mpImageNormal;
		case kState_MouseOver:	return mpImageOver;
		case kState_Depressed:	return mpImageDepressed;
		}

		return NULL;
	}

	bool GuiElement::MouseInBounds( int32_t x, int32_t y )
	{
		int32_t wx0 = GetX();
		int32_t wy0 = GetY();		

		int wx1 = wx0 + GetExtentX();
		int wy1 = wy0 + GetExtentY();

		return x >= wx0 && x < wx1 && y >= wy0 && y < wy1;
	}

////////////////////////////////////////////////////////////////////////////////////

	GuiIconScroller::ScrollButton::ScrollButton( GuiIconScroller* parent, ScrollDirection d )
	{
		mDirection = d;
		mpParent   = parent;
	}

	void GuiIconScroller::ScrollButton::OnClick()
	{
		GuiElement::OnClick();

		switch( mDirection )
		{
		case kSD_Left:   mpParent->left(); break;
		case kSD_Right:  mpParent->right(); break;
		}
	}	

	GuiIconScroller::~GuiIconScroller()
	{
		if( mScroller0 )
			delete mScroller0;

		if( mScroller1 )
			delete mScroller1;
	}

	GuiIconScroller::GuiIconScroller() : mScroller0(NULL)
									   , mScroller1(NULL)
									   , mIconOffset(0)
	{}

	void GuiIconScroller::SetScrollButtons( ScrollButton* e1, ScrollButton* e2 )
	{
		mIconOffset = 0;
		mScroller0 = e1;
		mScroller1 = e2;

		if( mScroller0 )
		{
			mScroller0->SetVisible(true);
			mScroller0->SetAlignment( GuiElement::kAlign_Top );
		}

		if( mScroller1 )
		{
			mScroller1->SetVisible(true);
			mScroller1->SetAlignment( GuiElement::kAlign_Top );
		}
	}

	bool GuiIconScroller::CheckMouse( int32_t x, int32_t y, bool lClick, bool rClick )
	{
		if( GuiElement::CheckMouse( x, y, lClick, rClick ) )
		{		
			// Check the scroller icons
			if( mScroller0 )
			{
				mScroller0->CheckMouse( x, y, lClick, rClick );
			}

			if( mScroller1 )
			{
				mScroller1->CheckMouse( x, y, lClick, rClick );
			}

			return true;
		}

		return false;
	}

	bool GuiIconScroller::Draw()
	{
		// draw base
		BaseBoundsDraw();

		int32_t scroller0w = mScroller0 ? mScroller0->GetExtentX() : 0;
		int32_t scroller1w = mScroller1 ? mScroller1->GetExtentX() : 0;			

		int32_t oldX    = mPos[0];
		int32_t extent  = GetExtentX();

		if( mScroller0 )
		{
			mScroller0->Draw();
			mPos[0] += mScroller0->GetExtentX();
		}

		// draw children
		if( mIconOffset < 0 )
			mIconOffset = 0;

		for( uint32_t i = mIconOffset; i < mChildren.size(); ++i )
		{
			if( mPos[0] + mChildren[i]->GetExtentX() < extent )
			{
				mChildren[i]->Draw();
			}

			mPos[0] += mChildren[i]->GetExtentX();
		}

		if( mScroller1 )
		{
			mPos[0] = extent - mScroller1->GetExtentX();
			mScroller1->Draw();			
		}

		mPos[0] = oldX;
	
		return true;
	}


	
}; //end Game
