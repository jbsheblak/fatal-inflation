//---------------------------------------------------
// Name: Game : StartScreen
// Desc:  the game start screen
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "State_StartScreen.h"

#include "GameXExt.h"
#include "GameConstants.h"
#include "Gui.h"
#include "Action.h"

namespace Game
{
	void State_StartScreen::Enter()
	{
		// load button images
		ImageX* logo  = GetImage( "textures/logo.tga" );		
		ImageX* start = GetImage( "textures/start.tga" );
		ImageX* edit  = GetImage( "textures/edit.tga" );
		ImageX* cont  = GetImage( "textures/continue.tga" );
		ImageX* options = GetImage( "textures/options.tga" );
		ImageX* quit = GetImage( "textures/quit.tga" );

		// setup gui

		// options gui
		GuiListContainer*	options_container = new GuiListContainer; 
		options_container->SetPosition( jbsCommon::Vec2i(300,300) );
		options_container->SetVisible(true);
		options_container->SetExtent( jbsCommon::Vec2i( 200, 200 ) );
		options_container->SetSpacing(10);

		GuiImageControl* options_image = new GuiImageControl;
		options_image->SetPosition( jbsCommon::Vec2i( 0, 0 ) );
		options_image->SetImage( logo );
		options_image->SetVisible( true );
		options_image->SetExtentFromImages();

		options_container->AddChild(options_image);

		//main container
		GuiContainer* mainContainer = new GuiContainer;
		mainContainer->SetPosition( jbsCommon::Vec2i(0,0) );
		mainContainer->SetVisible(true);
		mainContainer->SetExtent( jbsCommon::Vec2i( kWindowWidth, kWindowHeight ) );

		GuiListContainer*	gc = new GuiListContainer; 
		gc->SetPosition( jbsCommon::Vec2i(0,0) );
		gc->SetVisible(true);
		gc->SetExtent( jbsCommon::Vec2i( kWindowWidth, kWindowHeight ) );
		gc->SetSpacing(10);

		GuiImageControl* guiLogo = new GuiImageControl;
		guiLogo->SetImage( logo );
		guiLogo->SetPosition( jbsCommon::Vec2i( 0, 50 ) );
		guiLogo->SetExtentFromImages();
		guiLogo->SetVisible(true);		
		guiLogo->SetAlignment( jbsCommon::Vec2<GuiElement::Alignment>( GuiElement::kAlign_HorizCenter,
																  GuiElement::kAlign_RelativeTop ) );

		GuiButtonControl* sb = new ActionButton( new StateChangeAction("Game") );
		sb->SetImages( start, start, start );
		sb->SetPosition(  jbsCommon::Vec2i(0,0) );
		sb->SetExtentFromImages();
		sb->SetVisible(true);
		sb->SetAlignment( jbsCommon::Vec2<GuiElement::Alignment>( GuiElement::kAlign_HorizCenter,
																  GuiElement::kAlign_RelativeTop ) );

		GuiButtonControl* eb = new ActionButton( new StateChangeAction("EditMode") );
		eb->SetImages( edit, edit, edit );
		eb->SetPosition( jbsCommon::Vec2i(0,0) );
		eb->SetExtentFromImages();
		eb->SetVisible(true);
		eb->SetAlignment( jbsCommon::Vec2<GuiElement::Alignment>( GuiElement::kAlign_HorizCenter,
																  GuiElement::kAlign_RelativeTop ) );

		GuiButtonControl* ob = new ActionButton( new ToggleVisible(options_container) );
		ob->SetImages( options, options, options );
		ob->SetPosition( jbsCommon::Vec2i(0,0) );
		ob->SetExtentFromImages();
		ob->SetVisible(true);
		ob->SetAlignment( jbsCommon::Vec2<GuiElement::Alignment>( GuiElement::kAlign_HorizCenter,
																  GuiElement::kAlign_RelativeTop ) );

		GuiButtonControl* ct = new ActionButton( new StateChangeAction("EditMode") );
		ct->SetImages( cont, cont, cont );
		ct->SetPosition( jbsCommon::Vec2i(0,0) );
		ct->SetExtentFromImages();
		ct->SetVisible(true);
		ct->SetAlignment( jbsCommon::Vec2<GuiElement::Alignment>( GuiElement::kAlign_HorizCenter,
																  GuiElement::kAlign_RelativeTop ) );

		GuiButtonControl* qt = new ActionButton( new ActionQuit );
		qt->SetImages( quit, quit, quit );
		qt->SetPosition( jbsCommon::Vec2i(0,0) );
		qt->SetExtentFromImages();
		qt->SetVisible(true);
		qt->SetAlignment( jbsCommon::Vec2<GuiElement::Alignment>( GuiElement::kAlign_HorizCenter,
																  GuiElement::kAlign_RelativeTop ) );

		gc->AddChild(guiLogo);
		gc->AddChild(sb);
		gc->AddChild(eb);
		gc->AddChild(ob);
		gc->AddChild(ct);
		gc->AddChild(qt);

		mainContainer->AddChild(gc);
		mainContainer->AddChild(options_container);

		mGui = mainContainer;

		// load save file to see how many levels have been completed
		memset( &mSaveFile, 0, sizeof(GameSaveFile::SaveFile) );
		GameSaveFile::Import( kSaveFile, mSaveFile );
	}

	void State_StartScreen::Exit()
	{	
		delete mGui;
	}

	void State_StartScreen::Handle()
	{
		const int32_t kButtonX = 100;
		const int32_t kButtonY = 100;

		//get input
		const bool kLMouseClicked = GameX.GetMouseClick( MOUSE_LEFT );

		if( kLMouseClicked )
		{
			const int32_t x = GameX.GetMouseX();
			const int32_t y = GameX.GetMouseY();

			jbsCommon::Vec2i pos( x, y );

			mGui->OnClick(pos);
		}

		GameX.ClearScreen();

		// draw gui
		mGui->Draw();
		mGui->DrawBounds();

		// draw text
		char buffer[256];
		sprintf( buffer, "Levels Completed: %i", (int32_t)mSaveFile.mCompletedLevels );
		GameX.DrawText( 20, 20, buffer, 255, 0, 0 );
	}

}; // end Game
