//---------------------------------------------------
// Name: Game : StateEditMode
// Desc:  allows editing of arrows
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "State_EditMode.h"

#include <stdio.h>
#include "gamex.hpp"

#include "GameXExt.h"
#include "GameConstants.h"
#include "Timer.h"

#include "Arrow.h"
#include "Ball.h"
#include "Beam.h"

#include "ResourceCache.h"
#include "EntityFactory.h"

#include "MasterFile.h"

#include <algorithm>

namespace Game
{
	void GuiElement::Init( ImageX* img, int32_t x0, int32_t y0, int32_t x1, int32_t y1 )
	{
		mpImg = img;
		mBounds[0] = x0;
		mBounds[1] = y0;
		mBounds[2] = x1;
		mBounds[3] = y1;
	}

	bool GuiElement::CheckClick( int32_t x, int32_t y )
	{
		return x >= mBounds[0] && x < mBounds[2] && y >= mBounds[1] && y < mBounds[3];
	}
	
	bool GuiElement::Draw()
	{
		GameX.DrawImage( mpImg, mBounds[0], mBounds[1] );
		return true;
	}	


	// edit mode tuners
	const F32 State_EditMode::kImageHeight	     = 48.0f;
	const F32 State_EditMode::kArrowRotationInc  = 1.0f;

	void State_EditMode::Enter()
	{
		mTime = 0.0f;
		mArrowRotation = 0.0f;
		mMsg  = "";

		mDrawBBox = false;
		mShowHelp = false;	

		mSelectedEntity = NULL;

		// import our arrow descriptions
		PackFile::PackElement packEntityDesc;
		if( SPackFile.GetPackElement( "EntityDescriptions", packEntityDesc ) )
		{
			EntityDescFile::Import( (uint8_t*)packEntityDesc.mData, packEntityDesc.mSize, &mEntityDescMap );
		}		
		
		// load necessary images	
		mSelectedImage = NULL;				

		// load gui images
		mBack = GetImage( "textures/back.tga" );		
		mBackGui.Init( mBack, kWindowWidth - mBack->GetWidth(), 0, kWindowWidth, mBack->GetWidth() );		

		mEditBar = GetImage( "textures/editBar.tga" );		

		// start the timer up (for msgs, not elements)
		sTimer.StartTimer();
	}

	void State_EditMode::Exit()
	{
		Reset();
		DestroyEntityDescMap( &mEntityDescMap );		
		sTimer.StopTimer();
	}

	void State_EditMode::Handle()
	{
		//get input
		int32_t kMouseX = GameX.GetMouseX();
		int32_t kMouseY = GameX.GetMouseY();

		CheckInput();		
	

		// draw to screen
		GameX.ClearScreen();

		GameX.DrawImage( mEditBar, 0, 0 );
		DrawEntityChooser();		

		// draw entities
		
		EntitySetFile::EntitySetList::iterator entItr;
		for( entItr = mEntities.begin(); entItr != mEntities.end(); ++entItr )
		{
			Entity* ent = entItr->mEntity;
			ent->Update(mTime);
			ent->Draw();			

			if( mDrawBBox )
			{
				BoundingBoxf* bbox = ent->GetBBox();
				if( bbox )
					bbox->Draw();
			}
		}

		// draw selected image
		if( mSelectedImage )
		{
			GameX.DrawImage( mSelectedImage,
							 kMouseX - mSelectedImage->GetWidth()/2,
							 kMouseY - mSelectedImage->GetHeight()/2,
							 mArrowRotation, 1.0f);
		}

		// draw bbox of selected entity
		if( mSelectedEntity )
		{
			BoundingBoxf* bbox = mSelectedEntity->GetBBox();
			if( bbox )
			{	
				bbox->Draw();		
			}
		}


		// draw time
		char time[256];
	
		sprintf( time, "%s : v%.2f", kAppName, kVersion );
		GameX.DrawText( 5, kWindowHeight-90, time, 255, 0, 0 );		

		sprintf( time, "Entities: %i", (int32_t)mEntities.size() );
		GameX.DrawText( 5, kWindowHeight-70, time, 255, 0, 0 );

		sprintf( time, "Time: %.3f", mTime );
		GameX.DrawText( 5, kWindowHeight-50, time, 255, 0, 0 );				

		sprintf( time, "Press 'H' for more instructions" );
		GameX.DrawText( 5, kWindowHeight-30, time, 255, 0, 0 );

#define TextDraw( msg ) { GameX.DrawText( 5, yoffset, msg ); yoffset += 15; }

		if( mShowHelp )
		{
			uint32_t yoffset = 200;
			TextDraw( "SPACE : Save current entity set to file" );
			TextDraw( "DEL + LMouse : Delete entity from editor" );
			TextDraw( "R : Reset entity set" );
			TextDraw( "B : Toggle bounding boxes" );
			TextDraw( "H : Toggle this help" );
		}

#undef TextDraw		

		//draw msg

		if( mMsg != "" )
		{
			if( sTimer.GetTimeElapsed() > mMsgClearTime )
				mMsg = "";

			else
			   GameX.DrawText( 150, kWindowHeight-50, (char*)mMsg.c_str(), 255, 0, 0 );			
		}


		// draw gui
		mBackGui.Draw();		
	}

	// Draw the bar with shows all of the loaded arrow descriptions to chose from
	void State_EditMode::DrawEntityChooser()
	{
		uint32_t offsetX = 0;
		EntityDescMap::iterator itr;		
		for( itr = mEntityDescMap.begin(); itr != mEntityDescMap.end(); ++itr )
		{				
			ImageX* image = GetImage( itr->first.c_str() );			
			if( image )
			{
				// we want it to be kImageWidth pixels tall
				F32 scale = kImageHeight / image->GetHeight() ;

				GameX.DrawImage( image, offsetX, 0, (F32)0, (F32)scale );
				//GameX.DrawText( offsetX, (int32_t)kImageHeight, (char*)itr->first.c_str(), 255, 0, 0 );
				offsetX += (int32_t)(scale * image->GetWidth()) + 20;
			}
		}
	}

	// Check our input
	void State_EditMode::CheckInput()
	{
		//get input
		const bool kLMouseClicked = GameX.GetMouseClick( MOUSE_LEFT );
		if( kLMouseClicked )
		{		
			const int32_t x = GameX.GetMouseX();
			const int32_t y = GameX.GetMouseY();

			int32_t offsetX = 0;
			int32_t offsetY = 0;
			int32_t maxY = 0;
			bool    imageFound = false;			

			bool    entitySelected = false;

			// are we trying to click on something
			//if( deleteClick )
			{
				EntitySetFile::EntitySetList::iterator itr;
				for( itr = mEntities.begin(); itr != mEntities.end(); ++itr )
				{
					//check for selection
					if( itr->mEntity && itr->mEntity->GetBBox() &&
						itr->mEntity->GetBBox()->Collide( (F32)x, (F32)y ) )
					{	
						entitySelected = true;
						mSelectedEntity = itr->mEntity;
						break;						
					}
				}
			}

			if( !entitySelected )
				mSelectedEntity = NULL;

			// are we trying to select an image
			EntityDescMap::iterator itr;			
			for( itr = mEntityDescMap.begin(); itr != mEntityDescMap.end(); ++itr )
			{	
				ImageX* image = GetImage( itr->first.c_str() );				
				if( image )
				{	
					F32 scale = kImageHeight / image->GetHeight();
					int32_t width = (int32_t)( scale * image->GetWidth() );

					if( !imageFound &&
						x >= offsetX && x <= offsetX + width &&
						y >= offsetY && y <= offsetY + kImageHeight )
					{
						mSelectedImage = image;
						mSelectedEntityDesc = &itr->second;
						mSelectedEntityName = itr->first;
						imageFound = true;	
						
						char msg[256];
						sprintf( msg, "Selected %s", (char*)itr->first.c_str() );
						SetMessage(msg);
					}
					
					offsetX += width + 20;

					//if( image->GetHeight() > maxY )
					//	maxY = image->GetHeight();
					maxY = (int32_t)kImageHeight;
				}
			}

			// are we trying to place an entity
			if( y >= maxY && mSelectedImage )
			{
				EntityDescManager man( mSelectedEntityDesc );
				EntityProperty pos;
				EntityProperty img;			

				int32_t startX = x - mSelectedImage->GetWidth()/2;
				int32_t startY = y - mSelectedImage->GetHeight()/2;

				// set the starting position and image
				man.SetProperty2i( kArrowProp_StartPosition, startX, startY );								
				man.SetPropertyPtr( kEntProp_BaseImage, mSelectedImage );
				man.SetProperty1f( kArrowProp_Rotation, mArrowRotation );				

				Entity* pEnt = EntityFactory( mSelectedEntityName, mSelectedEntityDesc );				

				if( pEnt )
				{
					// place entity into the editor
					switch( pEnt->GetBaseType() )
					{
					case kEntBase_Arrow: PlaceArrow( (Arrow*)pEnt, startX, startY ); break;
					case kEntBase_Ball:  PlaceBall( (Ball*)pEnt, startX, startY ); break;
					case kEntBase_Beam:  PlaceBeam( (Beam*)pEnt, startX, startY ); break;
					}					
				}				
			}

			if( mBackGui.CheckClick( x, y ) )		
			{
				SMachine.RequestStateChange( "LoadGame" );
			}
		}

		const bool kRMouseClicked = GameX.GetMouseClick( MOUSE_RIGHT );
		if( kRMouseClicked )
		{
			mSelectedImage = NULL;
		}

		// key board input
		static const F32 kTimeInc = 0.02f;

		if( GameX.IsKeyDown( KEY_LEFT ) )
		{
			mTime -= kTimeInc;
			if( mTime < 0 )
				mTime = 0;
		}		

		if( GameX.IsKeyDown( KEY_RIGHT ) )		
			mTime += kTimeInc;
			

		if( GameX.IsKeyDown( KEY_SPACE ) )	
		{
			ExportEntityListToFile( "TEMP_EntitySet.as" );
			SetMessage( "Saved arrow set to TEMP_EntitySet.as" );			
		};

		if( GameX.IsKeyDown( KEY_BACKSPACE ) )	
		{
			SMachine.RequestStateChange( "LoadGame" );
		}

		// check for delete
		if( GameX.IsKeyDown( KEY_DELETE ) && mSelectedEntity )
		{
			EntitySetFile::EntitySetList::iterator itr;
			for( itr = mEntities.begin(); itr != mEntities.end(); ++itr )
			{
				if( itr->mEntity == mSelectedEntity )
				{		
					mSelectedEntity = NULL;
					mEntities.erase(itr);
					break;
				}
			}
		}

		static bool sBDown = false;
		static bool sHDown = false;

		if( GameX.IsKeyDown( KEY_R ) )
			Reset();

		if( GameX.IsKeyDown( KEY_B ) )
			sBDown = true;

		else if( sBDown )
		{
			mDrawBBox = !mDrawBBox;
			sBDown = false;
		}

		if( GameX.IsKeyDown( KEY_H ) )
			sHDown = true;

		else if( sHDown )
		{
			mShowHelp = !mShowHelp;
			sHDown = false;
		}

		if( GameX.IsKeyDown( KEY_PLUS ) )
			mArrowRotation -= kArrowRotationInc;

		if( GameX.IsKeyDown( KEY_MINUS ) )
			mArrowRotation += kArrowRotationInc;
	}

	void State_EditMode::Reset()
	{	
		// clear out arrows
		EntitySetFile::EntitySetList::iterator itr;
		for( itr = mEntities.begin(); itr != mEntities.end(); ++itr )
		{
			if( itr->mEntity )
				delete itr->mEntity;			
		}

		mEntities.clear();		

		// reset timer
		mTime = 0.0f;
		mSelectedEntity = NULL;
	}

	bool State_EditMode::EntitySetEntry_Sorter::operator() (const EntitySetFile::EntitySetEntry& lhs, 
														   const EntitySetFile::EntitySetEntry& rhs)
	{
		return lhs.mEntityTime < rhs.mEntityTime;
	}

	void State_EditMode::SetMessage( const char* msg )
	{
		mMsg = msg;
		mMsgClearTime = sTimer.GetTimeElapsed() + 5.0f;
	}

	void State_EditMode::PlaceArrow( Arrow* arrow, int32_t x, int32_t y )
	{
		if( !arrow )
			return;		

		// calculate how much must have past for the arrow to be here
		//F32 startTime = arrow->AdjustTimeToScreenLeft( mTime );
		F32 startTime = mTime;
		arrow->SetStartTime(startTime);

		int32_t xpos, ypos;

		//if more time is required than time has passed, we need to adjust
		//the timer and all the other arrows
		if( startTime < 0 )
		{
			// tell this arrow that it starts at time 0
			arrow->ModifyStartTime( -startTime );

			// adjust the timer so this arrow starts at time 0
			F32 timerMod = -startTime;
			mTime += -startTime;

			// have the other arrows re-adjust their starting times
			EntitySetFile::EntitySetList::iterator itr;
			for( itr = mEntities.begin(); itr != mEntities.end(); ++itr )
			{
				if( itr->mEntity->GetBaseType() == kEntBase_Arrow )
				{
					Arrow* arrow = (Arrow*)itr->mEntity;
					arrow->ModifyStartTime(timerMod);
					itr->mEntityTime += timerMod;
				}
			}

			// get the starting position of the arrow
			arrow->GetPosAtTime( 0, xpos, ypos );						

			// start time is zero
			startTime = 0;
		}
		else
		{
			// get the starting position of the arrow
			arrow->GetPosAtTime( startTime, xpos, ypos );
		}					

		// add the arrow to the field
		EntitySetFile::EntitySetEntry aee;
		aee.mEntity     = arrow;
		aee.mEntityName = mSelectedEntityName;
		aee.mEntityRotation = mArrowRotation;
		aee.mEntityTime = startTime;
		//aee.mStartX    = xpos;
		//aee.mStartY    = ypos;
		aee.mStartX = x;
		aee.mStartY = y;

		mEntities.push_back( aee );
	}

	void State_EditMode::PlaceBall( Ball* ball, int32_t x, int32_t y )
	{
		if( !ball )
			return;

		ball->SetStartTime(mTime);

		// add the arrow to the field
		EntitySetFile::EntitySetEntry aee;
		aee.mEntity     = ball;
		aee.mEntityName = mSelectedEntityName;
		aee.mEntityTime = mTime;
		aee.mEntityRotation = 0.0f;
		aee.mStartX    = x;
		aee.mStartY    = y;

		mEntities.push_back( aee );
	}

	void State_EditMode::PlaceBeam( Beam* beam, int32_t x, int32_t y )
	{
		if( !beam )
			return;

		beam->SetStartTime(0);

		// add the arrow to the field
		EntitySetFile::EntitySetEntry aee;
		aee.mEntity     = beam;
		aee.mEntityName = mSelectedEntityName;
		aee.mEntityRotation = 0.0f;
		aee.mEntityTime = 0;
		aee.mStartX    = x;
		aee.mStartY    = y;

		mEntities.push_back( aee );
	}

	bool State_EditMode::ExportEntityListToFile( const char* szFile )
	{
		// sort the arrows
		std::sort( mEntities.begin(), mEntities.end(), EntitySetEntry_Sorter() );

		// export to file
		if( !EntitySetFile::Export( szFile, mEntities ) )
			return false;

		return true;
	}
	
}; //end Game