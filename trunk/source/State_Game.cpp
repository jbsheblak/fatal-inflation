//---------------------------------------------------
// Name: Game : StateGame
// Desc:  the game
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "State_Game.h"

#include "Algorithms.h"
#include "Timer.h"
#include "GameXExt.h"
#include "GameConstants.h"
#include "ResourceCache.h"
#include <string>

#include "Util/Tuner.h"

namespace Game
{
	const char* kLevels [] = { "TestLevel",
							   "TestLevel2" };	

	const uint32_t kNumLevels = 2;


	State_Game::State_Game()
	{
		mCurLevel = 0;
	}

	void State_Game::Enter()
	{
		mBackground = NULL;	
		mpMusic		= NULL;
		mLevelEndTime = -1.0f;		

		// load the save file
		memset( &mSaveFile, 0, sizeof(GameSaveFile::SaveFile) );
		GameSaveFile::Import( kSaveFile, mSaveFile );

		// load our data
		LoadEntityDesc();

		LoadLevel( kLevels[mCurLevel] );		

		// start the arrow generation
		mEntityGen.StartGen();	

		int32_t pos [] = { kWindowWidth/2, kWindowHeight/2 };
		mPlayer.Init(pos);			

		mTimerImg = GetImage( "textures/timer.tga" );  
		sTimer.StartTimer();
	}

	void State_Game::Exit()
	{
		mEntityGen.ResetGen();
		mEntityGen.StopGen();
		mEntityGen.ClearGen();

		mActiveEntities.clear();
		DestroyEntityDescMap( &mEntityDescMap );

		// save the player settings
		if( mCurLevel > mSaveFile.mCompletedLevels )
			mSaveFile.mCompletedLevels = mCurLevel;

		GameSaveFile::Export( kSaveFile, mSaveFile );

#if PLAY_MUSIC
	
		if( mpMusic )
		{	
			//GameX.StopMusic( mpMusic );			

			delete mpMusic;
			mpMusic = NULL;
		}

#endif
	}

	void State_Game::Handle()
	{
		// handle input
		if( GameX.IsKeyDown( KEY_SPACE ) )
		{
			if( sTimer.IsPaused() )
				sTimer.UnpauseTimer();
			else
				sTimer.PauseTimer();
		}	

		if( GameX.IsKeyDown( KEY_BACKSPACE ) )
		{
			SMachine.RequestStateChange( "StartScreen" );
		}

		//reload tuners
		if( GameX.IsKeyDown( KEY_F5 ) )
		{
			gTuner.LoadTuners( "tuners.txt" );
		}

		if( GameX.IsKeyDown( KEY_UP ) )
		{
			const F32 kInflationRate = gTuner.GetFloat( "kInflationRate" );
			mPlayer.InflateBalloon( kInflationRate );
		}

		if( GameX.IsKeyDown( KEY_DOWN ) )
		{
			const F32 kInflationRate = gTuner.GetFloat( "kInflationRate" );
			mPlayer.InflateBalloon( -kInflationRate );
		}

		if( GameX.IsKeyDown( KEY_LEFT ) )
		{
			const int32_t kMoveAmt = gTuner.GetInt( "kMoveAmt" );
			mPlayer.MoveByDelta( -kMoveAmt, 0 );
		}

		if( GameX.IsKeyDown( KEY_RIGHT ) )
		{
			const int32_t kMoveAmt = gTuner.GetInt( "kMoveAmt" );
			mPlayer.MoveByDelta( kMoveAmt, 0 );		
		}

		// update things
		Entity* newEntity;
		while( (newEntity = mEntityGen.GenEntity()) != NULL )
		{
			// play the generation sound if it exists
			if( newEntity->GetBaseType() == kEntBase_Arrow )
				( (Arrow*)newEntity )->PlayGenSound();

			mActiveEntities.push_back(newEntity);		
		}
		
		// check for pauses
		sTimer.Tick();	

		//check for level end
		if( mLevelEndTime != -1.0f && sTimer.GetTimeElapsed() >= mLevelEndTime )
		{
			++mCurLevel;
			sTimer.ResetAndStopTimer();

			if( mCurLevel > kNumLevels )
				SMachine.RequestStateChange( "StartScreen" );
			else
				SMachine.RequestStateChange( "Game" );
		}

		GameX.ClearScreen();		

		// draw the background
		if( mBackground )
			GameX.DrawImage( mBackground, 0, 0 );

		// draw things
		std::list<Entity*>::iterator itr;	

		uint32_t yoffset = 50;

		for( itr = mActiveEntities.begin(); itr != mActiveEntities.end(); ++itr )
		{
			// update and draw it
			// TODO : We should probably remove arrows that are not alive...
			if( !sTimer.IsPaused() && (*itr)->IsActive() )
			{					
				(*itr)->Update( sTimer.GetTimeElapsed() );
				
				//if( mPlayer.Collide( *(*itr)->GetBBox() ) ) 
				//{
				//	mPlayer.Pop();
				//}
				//	SMachine.RequestStateChange( "StartScreen" );

				(*itr)->Draw();	
				
				if( (*itr)->GetBBox() )
				{
					//Draw Bounding Boxes
					(*itr)->GetBBox()->Draw();
				
					//// Draw Debug Status
					//BoundingBoxf* bbox = (*itr)->GetBBox();
					//char debugBuffer[256];

					//sprintf( debugBuffer, "( %i, %i )", (int32_t)bbox->mX, (int32_t)bbox->mY );
					//GameX.DrawText( 5, yoffset, debugBuffer, 255, 0, 0 );
					//yoffset += 20;				
				}
			}						
		}

		// draw the player
		mPlayer.Update( sTimer.GetTimeElapsed() );
		mPlayer.Draw();

		// draw the timer
#if _DEBUG
		char timer[64];
		sprintf( timer, "Time: %.3f", sTimer.GetTimeElapsed() );
		GameX.DrawText( 5, kWindowHeight - 20, timer, 255, 0, 0 );

		sprintf( timer, "Num Active: %i", (int32_t)mActiveEntities.size() );
		GameX.DrawText( 200, kWindowHeight - 20, timer, 255, 0, 0 );
#endif

		GameX.DrawImage( mTimerImg, (kWindowWidth - mTimerImg->GetWidth())/2, 10, 
			             (int32_t)( mTimerImg->GetWidth()  * sTimer.GetTimeElapsed() / mLevelEndTime ), 
						 mTimerImg->GetHeight() );			
	}

	// load the arrow description file
	bool State_Game::LoadEntityDesc()
	{
		PackFile::PackElement packedEntityDesc;

		if( !SPackFile.GetPackElement( "EntityDescriptions", packedEntityDesc ) )
			return false;

		if( !EntityDescFile::Import( (uint8_t*)packedEntityDesc.mData, packedEntityDesc.mSize, &mEntityDescMap ) )
			return false;	

		return true;
	}

	// load a level from the pak file
	bool State_Game::LoadLevel( const char* levelName )
	{
		char buffer[512];
		LevelFile::LevelEntry 		level;
		EntitySetFile::EntitySetList  arrowSet;

		// get the packed level
		PackFile::PackElement packedLevel;
		if( !SPackFile.GetPackElement( levelName, packedLevel ) )
			return false;

		// import the level
        if( !LevelFile::Import( (uint8_t*)packedLevel.mData, packedLevel.mSize, level ) )
			return false;

		// save the length time length
		mLevelEndTime = level.mTimeLength;

		// attempt to load the background				
		if( level.mBackground != "" )
		{
			if( mBackground )
			{
				delete mBackground;
			}			

			//load the background if possible
			mBackground = GetImage( (char*)( std::string( "textures/") + level.mBackground ).c_str() );
		}	

#if PLAY_MUSIC
		// load and play the music
		if( level.mMusic != "" )
		{				
			mpMusic = new MusicX();
			if( !mpMusic->Load( (char*)( std::string( "audio/" ) + level.mMusic ).c_str() ) )
			{
				delete mpMusic;
				mpMusic = NULL;
			}
			else
			{			
				GameX.PlayMusic( mpMusic );
			}
		}
#endif

		// clean up the arrow set name		
		jbsCommon::Algorithm::RemoveFileExtension( buffer, (char*)level.mEntitySet.c_str() );

		// get the packet arrow set
		PackFile::PackElement packedEntitySet;
		if( !SPackFile.GetPackElement( buffer, packedEntitySet ) )
			return false;

		// import the arrow set
		if( !EntitySetFile::Import( (uint8_t*)packedEntitySet.mData, packedEntitySet.mSize, arrowSet ) )
			return false;

		// create arrow creation entries to setup EntityGen
		CreateEntityGen( arrowSet );
		
		return true;
	}

	// create the arrow generation events from an arrow set
	bool State_Game::CreateEntityGen( EntitySetFile::EntitySetList& arrowSet )
	{
		EntitySetFile::EntitySetList::iterator itr;		
		for( itr = arrowSet.begin(); itr != arrowSet.end(); ++itr )
		{
			EntityDescMap::iterator mapItr;
			if( ( mapItr = mEntityDescMap.find( itr->mEntityName ) ) != mEntityDescMap.end() )
			{
				EntityDesc* desc = &mEntityDescMap[ itr->mEntityName ];
				EntityDescManager man(desc);

				// set the arrow position
				int32_t startX = itr->mStartX;
				int32_t startY = itr->mStartY;
				man.SetProperty2i( kArrowProp_StartPosition, startX, startY );				
				man.SetProperty1f( kArrowProp_Rotation, itr->mEntityRotation );

				// set the arrow image
				ImageX* img = GetImage( (char*)itr->mEntityName.c_str() );				
				if( img )
				{
					man.SetProperty( kEntProp_BaseImage, img, 0 );
				}

				// get arrow base override
				std::string entBase = "Default";
				EntityProperty baseEntityProp;
				if( man.GetProperty( kEntProp_BaseEntity, baseEntityProp ) )														
					entBase = std::string( (char*)baseEntityProp.mData );			

				
				EntityCreateEntry createEntry;
				createEntry.mEntBase   = entBase;
				createEntry.mGenTime   = itr->mEntityTime;
				createEntry.mEntDesc   = mEntityDescMap[ itr->mEntityName ];

				mEntityGen.AddEntity( createEntry );
			}
		}		

		return true;
	}

	


}; // end Game
