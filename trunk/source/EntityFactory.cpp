//---------------------------------------------------
// Name: Game : EntityFactory
// Desc:  generates entities
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "EntityFactory.h"

#include "Arrow.h"
#include "Ball.h"
#include "Beam.h"
#include "AllArrows.h"

namespace Game
{	

// a utility macros to make adding factory elements less tedious
#define FACTORY_ELEMENT( token, entClass ) { if( !strcmp( token, baseEnt.c_str() ) ) { return new entClass(variantDesc); } }


	// Factory
	Entity* EntityFactory( std::string baseEnt, EntityDesc* variantDesc )
	{
		//try to match up with a specified factory element
		FACTORY_ELEMENT( "Default", Arrow );
		FACTORY_ELEMENT( "Arrow", Arrow );
		FACTORY_ELEMENT( "Ball", Ball );
		FACTORY_ELEMENT( "Beam", Beam );
		FACTORY_ELEMENT( "TimedArrow", TimedArrow );

		// otherwise just go with an arrow
		return new Arrow(variantDesc);
	}

#undef FACTORY_ELEMENT
};