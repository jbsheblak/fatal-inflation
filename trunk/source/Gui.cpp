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
	std::map< std::string, GuiElement* > GuiElement::sGuiRegistry;

	GuiElement::GuiElement() : mpParent(NULL)						     
							 , mVisible(false)
							 , mState( kState_Normal )							 
	{
		mAlign.x = kAlign_RelativeLeft;
		mAlign.y = kAlign_RelativeTop;
	}	
	
	void GuiElement::SetParent( GuiElement* elem )
	{
		mpParent = elem;
	}

	void GuiElement::SetPosition( const jbsCommon::Vec2i& pos )
	{
		mPosition = pos;
	}

	void GuiElement::SetExtent( const jbsCommon::Vec2i& extent )
	{
		mExtent = extent;		
	}

	void GuiElement::SetAlignment( const jbsCommon::Vec2<Alignment>& align )
	{
		mAlign = align;		
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

		return true;
	}

	bool GuiElement::DrawBounds()
	{
		if( IsVisible() )
		{
			BoundingBoxi bbox = GetBounds();
			GameX.DrawLine( ColorX(255,0,0), bbox.mX, bbox.mY, bbox.mX + bbox.mWidth, bbox.mY );
			GameX.DrawLine( ColorX(255,0,0), bbox.mX + bbox.mWidth, bbox.mY, bbox.mX + bbox.mWidth, bbox.mY + bbox.mHeight );
			GameX.DrawLine( ColorX(255,0,0), bbox.mX + bbox.mWidth, bbox.mY + bbox.mHeight, bbox.mX, bbox.mY + bbox.mHeight );
			GameX.DrawLine( ColorX(255,0,0), bbox.mX, bbox.mY + bbox.mHeight, bbox.mX, bbox.mY );
		}
		return true;
	}

	GuiElement::State GuiElement::GetState()
	{
		return mState;
	}

	// gets the screenspace position	
	jbsCommon::Vec2i GuiElement::GetPosition()
	{
		jbsCommon::Vec2i pos;
		jbsCommon::Vec2i ext = GetExtent();
		jbsCommon::Vec2i parentPos( 0, 0 );
		jbsCommon::Vec2i parentExt( kWindowWidth, kWindowHeight );		

		if( mpParent )
		{
			parentPos = mpParent->GetPosition();
			parentExt = mpParent->GetExtent();			
		}

		switch( mAlign.x )
		{		
		case kAlign_Left:		   pos.x = parentPos.x; break;
		case kAlign_Right:		   pos.x = parentPos.x + parentExt.x - ext.x; break;
		case kAlign_RelativeLeft:  pos.x = parentPos.x + mPosition.x; break;
		case kAlign_RelativeRight: pos.x = parentPos.x + parentExt.x - ext.x - mPosition.x; break;
		case kAlign_HorizCenter:   pos.x = parentPos.x + (parentExt.x - ext.x)/2; break;
		default: assert(false);
		}

		switch( mAlign.y )
		{		
		case kAlign_Top:				pos.y = parentPos.y; break;
		case kAlign_Bottom:				pos.y = parentPos.y + parentExt.y - ext.y; break;
		case kAlign_RelativeTop:		pos.y = parentPos.y + mPosition.y; break;
		case kAlign_RelativeBottom:		pos.y = parentPos.y + parentExt.y - ext.y - mPosition.y; break;
		case kAlign_VertCenter:			pos.y = parentPos.y + (parentExt.y - ext.y)/2; break;
		default: assert(false);
		}

		return pos;
	}

	jbsCommon::Vec2i GuiElement::GetParentOffset()
	{
		return mPosition;
	}

	// get the screen space extent
	jbsCommon::Vec2i GuiElement::GetExtent()
	{
		return mExtent;
	}	

	BoundingBoxi GuiElement::GetBounds()
	{
		BoundingBoxi bb;
			
		jbsCommon::Vec2i pos = GetPosition();
		jbsCommon::Vec2i ext = GetExtent();

		bb.mX = pos.x;
		bb.mY = pos.y;
		bb.mWidth  = ext.x;
		bb.mHeight = ext.y;
		bb.mRotation = 0;		
		return bb;
	}

	bool GuiElement::IsVisible()
	{
		return mVisible;
	}

	void GuiElement::OnMouseOver( const jbsCommon::Vec2i& pos )
	{
		mState = kState_MouseOver;
	}
/*
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
	}*/

	void GuiElement::OnMouseOut( const jbsCommon::Vec2i& pos )
	{
		mState = kState_Normal;
	}

	void GuiElement::OnClick( const jbsCommon::Vec2i& pos )
	{
		mState = kState_Depressed;
	}

	void GuiElement::OnClickRelease( const jbsCommon::Vec2i& pos )
	{
		//mState = kState_Normal;
	}

/*
	void GuiElement::BaseBoundsDraw()
	{
		ImageX* img;
		if( (img = GetStateImage()) == NULL )
			img = mpImageNormal;

		if( img )
		{
			GameX.DrawImage( img,GetX(), GetY() );
		}
	}*/
/*
	ImageX* GuiElement::GetStateImage()
	{
		switch( mState )
		{
		case kState_Normal:		return mpImageNormal;
		case kState_MouseOver:	return mpImageOver;
		case kState_Depressed:	return mpImageDepressed;
		}

		return NULL;
	}*/
/*
	bool GuiElement::MouseInBounds( int32_t x, int32_t y )
	{
		int32_t wx0 = GetX();
		int32_t wy0 = GetY();		

		int wx1 = wx0 + GetExtentX();
		int wy1 = wy0 + GetExtentY();

		return x >= wx0 && x < wx1 && y >= wy0 && y < wy1;
	}*/


///////////////////////////////////////////////////////////////////////////

	GuiContainer::~GuiContainer()
	{
		for( uint32_t i = 0; i < mChildren.size(); ++i )
		{
			if( mChildren[i] )
			{
				delete mChildren[i];
			}
		}
	}

	bool GuiContainer::Draw()
	{
		// don't do anything if we are not visible
		if( !IsVisible() )
			return true;

		// draw the things we contain
		std::vector<GuiElement*>::iterator itr = mChildren.begin();
		std::vector<GuiElement*>::iterator end = mChildren.end();
		for( ; itr != end; ++itr )
		{
			GuiButtonControl* bc = (GuiButtonControl*)(*itr);
			(*itr)->Draw();
		}
		return true;
	}	

	bool GuiContainer::DrawBounds()
	{
		if( !IsVisible() )
			return true;

		// draw the things we contain
		std::vector<GuiElement*>::iterator itr = mChildren.begin();
		std::vector<GuiElement*>::iterator end = mChildren.end();
		for( ; itr != end; ++itr )
		{
			GuiButtonControl* bc = (GuiButtonControl*)(*itr);
			(*itr)->DrawBounds();
		}

		GuiElement::DrawBounds();
		return true;
	}

	void GuiContainer::AddChild( GuiElement* child )
	{
		if( child )
		{
			child->SetParent(this);
			mChildren.push_back(child);
		}
	}

	void GuiContainer::RemoveChild( GuiElement* child )
	{
		std::vector<GuiElement*>::iterator itr = mChildren.begin();
		std::vector<GuiElement*>::iterator end = mChildren.end();
		for( ; itr != end; ++itr )
		{
			if( (*itr) == child )
			{
				mChildren.erase( itr );
				break;
			}
		}
	}

	void GuiContainer::OnMouseOver( const jbsCommon::Vec2i& pos )
	{
		GuiElement::OnMouseOver(pos);

		std::vector<GuiElement*>::iterator itr = mChildren.begin();		
		for( ; itr != mChildren.end(); ++itr )
		{
			if( (*itr)->IsVisible() && (*itr)->GetBounds().Collide(pos.x, pos.y) )
			{
				(*itr)->OnMouseOver(pos);
			}			
		}
	}    

	void GuiContainer::OnMouseOut( const jbsCommon::Vec2i& pos )
	{
		GuiElement::OnMouseOut( pos );

		std::vector<GuiElement*>::iterator itr = mChildren.begin();		
		for( ; itr != mChildren.end(); ++itr )
		{
			if( (*itr)->IsVisible() )
			{
				(*itr)->OnMouseOut(pos);
			}			
		}
	}

	void GuiContainer::OnClick( const jbsCommon::Vec2i& pos )
	{
		GuiElement::OnClick(pos);

		std::vector<GuiElement*>::iterator itr = mChildren.begin();		
		for( ; itr != mChildren.end(); ++itr )
		{
			if( (*itr)->IsVisible() && (*itr)->GetBounds().Collide(pos.x, pos.y) )
			{
				(*itr)->OnClick(pos);
			}			
		}
	}

	void GuiContainer::OnClickRelease( const jbsCommon::Vec2i& pos )
	{
		GuiElement::OnClickRelease(pos);

		std::vector<GuiElement*>::iterator itr = mChildren.begin();		
		for( ; itr != mChildren.end(); ++itr )
		{
			if( (*itr)->IsVisible() && (*itr)->GetBounds().Collide(pos.x, pos.y) )
			{
				(*itr)->OnClickRelease(pos);
			}			
		}
	}		

/////////////////////////////////////////////////////////////////////////////////

	GuiListContainer::GuiListContainer() : mSpacing(5)
	{}		

	void GuiListContainer::AddChild( GuiElement* child )
	{
		if( mChildren.size() > 0 )
		{
			jbsCommon::Vec2i backPos  = mChildren.back()->GetParentOffset();
			jbsCommon::Vec2i childPos = child->GetParentOffset();

			childPos.y = backPos.y + mChildren.back()->GetExtent().y + mSpacing;
			child->SetPosition(childPos);
		}
		else
		{
			jbsCommon::Vec2i childPos = child->GetParentOffset();
			child->SetPosition( jbsCommon::Vec2i( childPos.x, 0 ) );
		}			

		GuiContainer::AddChild(child);		
	}

	///
	/// Basically we want to find the child in the list.
	/// When we do that, we need to get it's height and 
	/// default spacing from every element below it to
	/// remove the elements up
	///
	void GuiListContainer::RemoveChild( GuiElement* child )
	{
		bool found = false;
		int32_t foundHeight = 0;

		std::vector<GuiElement*>::iterator itrFound;
		std::vector<GuiElement*>::iterator itr = mChildren.begin();
		std::vector<GuiElement*>::iterator end = mChildren.end();

		for( ; itr != end; ++itr )
		{
			if( *itr == child )
			{
				found = true;
				foundHeight = (*itr)->GetExtent().y;
				itrFound = itr;
			}

			if(found)
			{
				jbsCommon::Vec2i pos = (*itr)->GetParentOffset();
				pos.y -= foundHeight + mSpacing;
				(*itr)->SetPosition(pos);
			}
		}

		if(found)
		{
			mChildren.erase(itrFound);
		}
	}
    
	void GuiListContainer::SetSpacing( uint32_t spacing )
	{
		mSpacing = spacing;
	}
	
	uint32_t GuiListContainer::GetSpacing()
	{
		return mSpacing;
	}

/////////////////////////////////////////////////////////////////////////////////

	GuiImageControl::GuiImageControl() : mpImage(NULL)
	{}	

	bool GuiImageControl::Draw()
	{
		jbsCommon::Vec2i pos = GetPosition();
		GameX.DrawImage( mpImage, pos.x, pos.y );
		return true;
	}

	void GuiImageControl::SetImage( ImageX* img )
	{
		mpImage = img;
	}

	void GuiImageControl::SetExtentFromImages()
	{
		if( mpImage )
		{
			jbsCommon::Vec2i ext( mpImage->GetWidth(), mpImage->GetHeight() );
			SetExtent(ext);
		}
	}

	ImageX* GuiImageControl::GetImage()
	{
		return mpImage;
	}

///////////////////////////////////////////////////////////////////////////////

	GuiButtonControl::GuiButtonControl() : mpNormalImage(NULL),
										   mpDepressedImage(NULL),
										   mpOverImage(NULL)
	{}

	bool GuiButtonControl::Draw()
	{
		ImageX* curImage = NULL;
		switch( GetState() )
		{
		case GuiElement::kState_Normal:    curImage = mpNormalImage; break;
		case GuiElement::kState_Depressed: curImage = mpDepressedImage; break;
		case GuiElement::kState_MouseOver: curImage = mpOverImage; break;
		}

		if( curImage )
		{
			jbsCommon::Vec2i pos = GetPosition();
			GameX.DrawImage( curImage, pos.x, pos.y );
		}
		return true;
	}

	void GuiButtonControl::SetImages( ImageX* normal, ImageX* depressed, ImageX* mouseOver )
	{
		mpNormalImage = normal;
		mpDepressedImage = depressed;
		mpOverImage = mouseOver;
	}

	void GuiButtonControl::SetExtentFromImages()
	{
		if( mpNormalImage )
		{
			jbsCommon::Vec2i ext( mpNormalImage->GetWidth(), mpNormalImage->GetHeight() );
			SetExtent(ext);
		}
	}
	
}; //end Game
