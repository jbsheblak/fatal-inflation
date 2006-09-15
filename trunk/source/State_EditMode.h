//---------------------------------------------------
// Name: Game : StateEditMode
// Desc:  allows editing of arrows
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_STATE_EDIT_MODE_H_
#define _GAME_STATE_EDIT_MODE_H_

#include "StateMachine.h"
#include "Types.h"

#include <vector>

#include "FileIO.h"

class ImageX;
class EntityDesc;

namespace Game
{
	class Ball;
	class Beam;

	class GuiElement
	{
	public:

		void Init( ImageX* img, int32_t x0, int32_t y0, int32_t x1, int32_t y1 );
		bool CheckClick( int32_t x, int32_t y );
		bool Draw();

	private:

		ImageX*		mpImg;
		int32_t		mBounds[4];
	};

	class State_EditMode : public State
	{
	public:		
	       
		void Enter();
		void Exit();
		void Handle();

	private:

		// static tuner constants
		static const F32 kImageHeight;			// the height of the images in the select bar
		static const F32 kArrowRotationInc;     // the rate at which we change arrow rotation

	private:		
		
		void DrawEntityChooser();
		void CheckInput();	
		void Reset();

		bool ExportEntityListToFile( const char* szFile );

		void SetMessage( const char* msg );

		void PlaceArrow( Arrow* arrow, int32_t x, int32_t y );
		void PlaceBall( Ball* ball, int32_t x, int32_t y );
		void PlaceBeam( Beam* beam, int32_t x, int32_t y );

	private:

		struct EntitySetEntry_Sorter
		{
			bool operator() (const EntitySetFile::EntitySetEntry& lhs, 
							 const EntitySetFile::EntitySetEntry& rhs);
		};				

	private:		

		F32				   mTime;
		F32				   mArrowRotation;

		GuiElement		   mBackGui;
		Entity*			   mSelectedEntity;

		ImageX*			   mSelectedImage;
		ImageX*			   mBack;
		ImageX*			   mEditBar;
		EntityDesc*		   mSelectedEntityDesc;
		std::string		   mSelectedEntityName;

		EntityDescMap	   mEntityDescMap;

		std::string		   mMsg;
		F32				   mMsgClearTime;

		EntitySetFile::EntitySetList mEntities;

		bool			   mDrawBBox;
		bool			   mShowHelp;
	};
	
}; //end Game

#endif // end _GAME_STATE_EDIT_MODE_H_
    
