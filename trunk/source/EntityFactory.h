//---------------------------------------------------
// Name: Game : EntityFactory
// Desc:  generates entities
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_ENTITY_FACTORY_H_
#define _GAME_ENTITY_FACTORY_H_

#include <string>
#include "Entity.h"

namespace Game
{
	// Create an entity by way of the factory.
	// baseEnt should be the basic object you are trying to make such
	// as "arrow", "beam", etc. The variant description tells the factory
	// how this entity should be different
	Entity* EntityFactory( std::string baseEnt, EntityDesc* variantDesc );
};

#endif //end _GAME_ENTITY_FACTORY_H_
