//---------------------------------------------------
// Name: Game : EntityGen
// Desc:  generates the entities at given times
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_ENTITYGEN_H_
#define _GAME_ENTITYGEN_H_

#include "Types.h"
#include <vector>

#include "Entity.h"

namespace Game
{	
	// structure that tells the EntityFactory how to create the entity of base type
	struct EntityCreateEntry
	{
		F32							mGenTime;	// when to generate the arrow
		std::string					mEntBase;   // the base arrow for this variant
		EntityDesc					mEntDesc;   // arrow description
	};

	typedef std::vector< EntityCreateEntry > EntityCreateList;

	// structure that holds on to the created entity until the time is right to release it
	struct EntityGenEntry
	{
		F32			mGenTime;	// when to generate the ent
		Entity*		mEnt;		// the ent to create		
	};

	typedef std::vector< EntityGenEntry > EntityGenList;
    
	// class that generates the arrows at the given times
	class EntityGen
	{
	public:

		EntityGen() {}
		EntityGen( const EntityCreateList& list );

		bool AddEntity( const EntityCreateEntry& entry );

		Entity* GenEntity();

		void StartGen();
		void StopGen();
		void ResetGen();
		void PauseGen();

		void ClearGen();

	private:		

		EntityGenList			mEntityGenList;
		uint32_t				mEntityGenIdx;
	};
	
}; //end Game

#endif // end _GAME_ENTITYGEN_H_
    
