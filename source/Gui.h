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

		// deletes its children
		virtual ~GuiElement();

		// ctor
		GuiElement();

		// set the images
		void SetImages( ImageX* normal, ImageX* mouseOver = NULL, ImageX* mouseClicked = NULL );

		// set the parent of this element
		void SetParent( GuiElement* elem );	

		// set the position
		void SetPosition( int32_t x, int32_t y );

		// set the extent
		void SetExtent( int32_t eX, int32_t eY );
		void SetExtentFromImages();

		// set the alignment
		void SetAlignment( Alignment align );

		// set visibility
		void SetVisible( bool visible );        

		//render
		virtual bool Draw();

		// add child
		void AddChild( GuiElement* child );		

		// get the state
		State GetState();

		// location
		int32_t	GetX();
		int32_t	GetY();
		int32_t GetExtentX();
		int32_t GetExtentY();

		// Action Methods

		//check if a click affects you or your children	
		virtual bool CheckMouse( int32_t x, int32_t y, bool lClick, bool rClick );		

		// callbacks
		virtual void OnMouseOver();
		virtual void OnMouseOut();
		virtual void OnClick();
		virtual void OnClickRelease();

	protected:
				
		void	BaseBoundsDraw();
		ImageX* GetStateImage();
		bool	MouseInBounds( int32_t x, int32_t y );

	protected:

		GuiElement* mpParent;

		ImageX*		mpImageNormal;
		ImageX*		mpImageOver;
		ImageX*		mpImageDepressed;	

		int32_t		mPos[2];
		int32_t		mExtent[2];

		State		mState;
		Alignment   mAlignX;
		Alignment	mAlignY;
		bool		mVisible;

		std::vector<GuiElement*>	mChildren;
	};

	// gui that scrolls icons across the window
	class GuiIconScroller : public GuiElement
	{
	public:

		//TODO : Use this.
		enum Orientation
		{
			kOrientation_Vert,
			kOrientation_Horiz
		};

		enum ScrollDirection
		{
			kSD_Left,
			kSD_Right,
			kSD_Up,
			kSD_Down
		};

		// a scroll button in the container
		class ScrollButton : public GuiElement
		{
		public:

			ScrollButton( GuiIconScroller* parent, ScrollDirection d );
			void OnClick();

		private:

			ScrollDirection    mDirection;
			GuiIconScroller*   mpParent;
		};

	public:

		void left() { ++mIconOffset; }
		void right() {--mIconOffset; }

		virtual ~GuiIconScroller();
		GuiIconScroller();

		//do not add these as children as well
		void SetScrollButtons( ScrollButton* e1, ScrollButton* e2 );

		//check if a click affects you or your children	
		virtual bool CheckMouse( int32_t x, int32_t y, bool lClick, bool rClick );

		// draw
		virtual bool Draw();

	protected:

		GuiElement*	mScroller0;
		GuiElement* mScroller1;		

		int32_t mIconOffset;
	};
	
}; //end Game

#endif // end _GAME_GUI_H_
    
