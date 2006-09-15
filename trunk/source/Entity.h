//---------------------------------------------------
// Name: Game : Entity
// Desc:  a thing on the field
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_ENTITY_H_
#define _GAME_ENTITY_H_

#include "Types.h"
#include "BoundingBox.h"
#include "gamex.hpp"
#include <vector>
#include <map>

namespace Game
{	
	// property flags
	enum EntityPropertyFlag
	{
		kEntProp_BaseEntity,	// string		
		kEntProp_BaseImage,		// base image ( 1 ptr )
		kEntProp_EndProp
	};

	// base types
	enum EntityBaseType
	{
		kEntBase_Arrow,
		kEntBase_Ball,
		kEntBase_Beam
	};

	//-----------------------------------------------------------
	// Name: EntityProperty
	// Desc:  a buffer of data used to describe the function of an entity
	//-----------------------------------------------------------
	struct EntityProperty
	{
		uint32_t				mFlag;
		uint8_t*				mData;
		uint32_t				mDataSize;

		EntityProperty() : mData(NULL) {}
	};

	typedef std::vector< EntityProperty > EntityDesc;
	typedef std::map< std::string, EntityDesc > EntityDescMap;

	//-----------------------------------------------------------
	// Name: EntityDescManager
	// Desc:  helper class to manage dynamic entity properties
	//-----------------------------------------------------------
	class EntityDescManager
	{
	public:
		EntityDescManager( EntityDesc* desc );

		bool  ContainsProperty( uint32_t flag );
		bool  GetProperty( uint32_t flag, EntityProperty& out );		
		bool  AddProperty( const EntityProperty& prop );
		bool  RemoveProperty( uint32_t flag );					
		bool  SetProperty( uint32_t flag, void* data, uint32_t dataSize );
		bool  SetProperty( const EntityProperty& prop );
		
		bool  SetProperty2i( uint32_t flag, int32_t e1, int32_t e2 );
		bool  SetProperty1f( uint32_t flag, F32 e1 );
		bool  SetProperty2f( uint32_t flag, F32 e1, F32 e2 );
		bool  SetPropertyPtr( uint32_t flag, void* ptr );

		bool  GetProperty2i( uint32_t flag, int32_t& e1, int32_t& e2 );

	private:
		EntityDesc*			mDesc; // a ptr to the properties we are modifying
	};


	//-----------------------------------------------------------
	// Name: Entity
	// Desc:  a base unit in the game
	//-----------------------------------------------------------
	class Entity
	{
	public:
		
		virtual ~Entity() {}

		// picks out useful properties for itself
		Entity( EntityDesc* desc );		

		// update and draw, children should override these
		virtual void Update( F32 curTime ) = 0;
		virtual void Draw() = 0;
		virtual bool IsActive() const = 0;

		// get the bounding box
		virtual BoundingBoxf* GetBBox() = 0;
		
		// manually adjust the starting time
		void SetStartTime( F32 time );

		// get the base type
		virtual uint32_t GetBaseType() const = 0;

		// play a generation sound
		virtual void PlayGenSound() {}

	protected:

		F32				mStartTime; // the time this entity was spawned
		ImageX*			mBaseImage; // our basic image to draw
	};

	// go through all the properties and delete the data
	// buffer allocated for the property
	void DestroyEntityDescMap( EntityDescMap* descMap );		


}; //end Game


#endif //end _GAME_ENTITY_H_
