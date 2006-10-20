//---------------------------------------------------
// Name: Game : Gui
// Desc:  interface
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_GUI_H_
#define _GAME_GUI_H_

#include "Types.h"
#include <vector>
#include <map>
#include "Vector.h"
#include "BoundingBox.h"

class ImageX;

namespace Game
{
	// A Base element in the Gui system
	class GuiElement
	{
	public:

		// the mouse state of the element
		enum State
		{
			kState_Normal,
			kState_MouseOver,
			kState_Depressed
		};

		// alignment with respect to parent container
		enum Alignment
		{
			kAlign_Top,
			kAlign_Bottom,			
			kAlign_RelativeTop,
			kAlign_RelativeBottom,
			kAlign_VertCenter,
			kAlign_Left,
			kAlign_Right,
			kAlign_RelativeLeft,
			kAlign_RelativeRight,
			kAlign_HorizCenter
		};	

	public:		
		
		virtual ~GuiElement() {}

		// ctor
		GuiElement();		

		// set the parent of this element
		void SetParent( GuiElement* elem );	

		// set the position
		void SetPosition( const jbsCommon::Vec2i& pos );
		void SetExtent( const jbsCommon::Vec2i& extent );
		void SetAlignment( const jbsCommon::Vec2<Alignment>& align );		

		// set visibility
		void SetVisible( bool visible );        

		//render
		virtual bool Draw();
		virtual bool DrawBounds();
		
		State GetState();

		jbsCommon::Vec2i GetPosition();
		jbsCommon::Vec2i GetParentOffset();
		jbsCommon::Vec2i GetExtent();

		virtual BoundingBoxi GetBounds();

		bool IsVisible();

		// action
		virtual void OnMouseOver( const jbsCommon::Vec2i& pos );
		virtual void OnMouseOut( const jbsCommon::Vec2i& pos );
		virtual void OnClick( const jbsCommon::Vec2i& pos );
		virtual void OnClickRelease( const jbsCommon::Vec2i& pos );

	public:

		// do not delete elements of this, they are just references
		static std::map< std::string, GuiElement* > sGuiRegistry;

	protected:		

		jbsCommon::Vec2i mPosition;
		jbsCommon::Vec2i mExtent;

	private:

		GuiElement* mpParent;
		State		mState;
		
		jbsCommon::Vec2<Alignment> mAlign;		

		bool		mVisible;		
	};

	class GuiContainer : public GuiElement
	{
	public:

		virtual ~GuiContainer();

		virtual bool Draw();
		virtual bool DrawBounds();
		
		virtual void AddChild( GuiElement* child );	
		virtual void RemoveChild( GuiElement* child );

		virtual void OnMouseOver( const jbsCommon::Vec2i& pos );
		virtual void OnMouseOut( const jbsCommon::Vec2i& pos );
		virtual void OnClick( const jbsCommon::Vec2i& pos );
		virtual void OnClickRelease( const jbsCommon::Vec2i& pos );

	protected:

		std::vector<GuiElement*>	mChildren;

	};

	class GuiListContainer : public GuiContainer
	{
	public:

		GuiListContainer();

		virtual void AddChild( GuiElement* child );
		virtual void RemoveChild( GuiElement* child );

		void		SetSpacing( uint32_t spacing );
		uint32_t	GetSpacing();

	private:

		uint32_t			mSpacing;	///< Spacing between elements		
	};
	
	class GuiControl : public GuiElement
	{		
	};

	class GuiImageControl : public GuiControl
	{
	public:

		GuiImageControl();
		virtual ~GuiImageControl() {}

		virtual bool Draw();

		void SetExtentFromImages();
		void SetImage( ImageX* img );		

		ImageX* GetImage();

	private:

		ImageX*		mpImage;
	};

	class GuiButtonControl : public GuiImageControl
	{
	public:

		GuiButtonControl();

		virtual bool Draw();

		void SetImages( ImageX* normal, ImageX* depressed, ImageX* mouseOver );
		void SetExtentFromImages();

	private:

		ImageX*		mpNormalImage;
		ImageX*		mpDepressedImage;
		ImageX*		mpOverImage;
	};

	
}; //end Game

#endif // end _GAME_GUI_H_
    
