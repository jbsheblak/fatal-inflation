//---------------------------------------------------
// Name: Game : StartScreen
// Desc:  the game start screen
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "State_StartScreen.h"

#include "GameXExt.h"
#include "GameConstants.h"

namespace Game
{
	static int32_t gStartRect[4];
	static int32_t gEditRect[4];
	static int32_t gLogoRect[4];

	void State_StartScreen::Enter()
	{
		// load button images
		mLogo			= GetImage( "textures/logo.tga" );		
		mStartButton	= GetImage( "textures/start.tga" );
		mEditButton		= GetImage( "textures/edit.tga" );		

		gLogoRect[0] = (kWindowWidth - mLogo->GetWidth())/2;
		gLogoRect[1] = 50;
		gLogoRect[2] = gLogoRect[0] + mLogo->GetWidth();
		gLogoRect[3] = gLogoRect[1] + mLogo->GetHeight();

		gStartRect[0] = (kWindowWidth - mStartButton->GetWidth())/2;
		gStartRect[1] = gLogoRect[3] + 10;
		gStartRect[2] = gStartRect[0] + mStartButton->GetWidth();
		gStartRect[3] = gStartRect[1] + mStartButton->GetHeight();

		gEditRect[0] = (kWindowWidth - mStartButton->GetWidth())/2;
		gEditRect[1] = gStartRect[3] + 10;
		gEditRect[2] = gEditRect[0] + mEditButton->GetWidth();
		gEditRect[3] = gEditRect[1] + mEditButton->GetHeight();
	}

	void State_StartScreen::Exit()
	{		
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

			if( x >= gStartRect[0] && x <= gStartRect[2] &&
				y >= gStartRect[1] && y <= gStartRect[3] )
			{			
				//change state
				SMachine.RequestStateChange( "Game" );
			}

#if ENABLE_EDIT
			if( x >= gEditRect[0] && x <= gEditRect[2] &&
				y >= gEditRect[1] && y <= gEditRect[3] )
			{			
				//change state
				SMachine.RequestStateChange( "EditMode" );
			}
#endif
		}

		GameX.ClearScreen();

		//draw button		
		GameX.DrawImage( mLogo, gLogoRect[0], gLogoRect[1] );
		GameX.DrawImage( mStartButton, gStartRect[0], gStartRect[1] );
		
#if ENABLE_EDIT
		GameX.DrawImage( mEditButton, gEditRect[0], gEditRect[1] );
#endif
	}

}; // end Game
