//---------------------------------------------------
// Name: Game : Entity
// Desc:  a thing on the field
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "Entity.h"

#include <assert.h>
#include "Algorithms.h"
#include "ResourceCache.h"
#include "FileIO.h"
#include "GameXExt.h"

namespace Game
{
//----------------------------------------------------------------------------
// Name: EntityDescManager
// Desc: manages an entities properties
//----------------------------------------------------------------------------

	//-----------------------------------------------------------
	// Name: EntityDescManager
	// Desc:  constructor
	//-----------------------------------------------------------
	EntityDescManager::EntityDescManager( EntityDesc* desc )
	{
		mDesc = desc;
	}

	//-----------------------------------------------------------
	// Name: ContainsProperty
	// Desc:  checks if this arrow's desc contains this prop
	//-----------------------------------------------------------
	bool EntityDescManager::ContainsProperty( uint32_t flag )
	{
		EntityDesc::iterator itr;
		for( itr = mDesc->begin(); itr != mDesc->end(); ++itr )
		{
			if( itr->mFlag == flag )
			{
				return true;
			}		
		}
		return false;
	}

	//-----------------------------------------------------------
	// Name: GetProperty
	// Desc:  attempts to get the property with givne flag
	//-----------------------------------------------------------
	bool EntityDescManager::GetProperty( uint32_t flag, EntityProperty& out )
	{
		EntityDesc::iterator itr;
		for( itr = mDesc->begin(); itr != mDesc->end(); ++itr )
		{
			if( itr->mFlag == flag )
			{
				out.mData	  = itr->mData;
				out.mDataSize = itr->mDataSize;
				out.mFlag     = itr->mFlag;
				return true;
			}		
		}

		return false;
	}	
	
	//-----------------------------------------------------------
	// Name: AddProperty
	// Desc:  adds a property to the description (does not check for existance)
	//-----------------------------------------------------------
	bool EntityDescManager::AddProperty( const EntityProperty& prop )
	{
		mDesc->push_back(prop);
		return true;
	}
		
	//-----------------------------------------------------------
	// Name: RemoveProperty
	// Desc:  removes a property from a description
	//-----------------------------------------------------------
	bool EntityDescManager::RemoveProperty( uint32_t flag )
	{		
		EntityDesc::iterator itr;
		for( itr = mDesc->begin(); itr != mDesc->end(); ++itr )
		{
			if( itr->mFlag == flag )
			{				
				mDesc->erase(itr);
				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------
	// Name: SetPropertyFromArrayne
	// Desc:  sets or adds a property in a description
	//-----------------------------------------------------------
	template <typename T>
	bool SetPropertyFromArray2e( EntityDescManager* man, uint32_t flag, T e1, T e2 )
	{
		T elem [] = { e1, e2 };
		return man->SetProperty( flag, (void*)elem, sizeof(T) * 2 );
	}

	//-----------------------------------------------------------
	// Name: SetPropertyFromArrayne
	// Desc:  sets or adds a property in a description
	//-----------------------------------------------------------
	template <typename T>
	bool SetPropertyFromArray1e( EntityDescManager* man, uint32_t flag, T e1 )
	{
		T elem [] = { e1 };
		return man->SetProperty( flag, (void*)elem, sizeof(T) * 1 );
	}

	//-----------------------------------------------------------
	// Name: SetProperty
	// Desc:  sets or adds a property in a description
	//-----------------------------------------------------------
	bool EntityDescManager::SetProperty( uint32_t flag, void* data, uint32_t dataSize )
	{
		EntityProperty prop;
		prop.mFlag = flag;
		prop.mData = (uint8_t*)data;
		prop.mDataSize = dataSize;
		return SetProperty(prop);
	}

	//-----------------------------------------------------------
	// Name: SetProperty
	// Desc:  sets or adds a property in a description
	//-----------------------------------------------------------
	bool EntityDescManager::SetProperty( const EntityProperty& prop )
	{
		// if prop has a data size of zero, it indicates that we
		// are just trying to pass a ptr (ie: passing the base image ptr)

		EntityProperty internal;
		if( GetProperty( prop.mFlag, internal ) )
		{
			assert( internal.mData );
			if( !internal.mDataSize )
			{
				internal.mData = prop.mData;
			}
			else
			{
				memcpy( internal.mData, prop.mData, internal.mDataSize );
			}
		}
		else
		{
			EntityProperty newProp;
			newProp.mFlag     = prop.mFlag;
			newProp.mDataSize = prop.mDataSize;

			if( newProp.mDataSize )
			{
				newProp.mData = new uint8_t[prop.mDataSize];
				memcpy( newProp.mData, prop.mData, prop.mDataSize );
			}
			else
			{
				newProp.mData = prop.mData;
			}
			AddProperty(newProp);
		}
		return true;
	}

	bool EntityDescManager::SetProperty2i( uint32_t flag, int32_t e1, int32_t e2 )
	{
		return SetPropertyFromArray2e<int32_t>( this, flag, e1, e2 );
	}

	bool EntityDescManager::SetProperty1f( uint32_t flag, F32 e1 )
	{
		return SetPropertyFromArray1e<F32>( this, flag, e1 );
	}

	bool EntityDescManager::SetProperty2f( uint32_t flag, F32 e1, F32 e2 )
	{
		return SetPropertyFromArray2e<F32>( this, flag, e1, e2 );
	}	

	bool EntityDescManager::SetPropertyPtr( uint32_t flag, void* ptr )
	{
		return SetProperty( flag, ptr, 0 );
	}	

	bool EntityDescManager::GetProperty2i( uint32_t flag, int32_t& e1, int32_t& e2 )
	{
		EntityProperty prop;
		if( !GetProperty(flag,prop) )
			return false;

		int32_t* data = (int32_t*)prop.mData;
		e1 = data[0];
		e2 = data[1];
		return true;
	}		

//----------------------------------------------------------------------------
// Name: Entity
// Desc: A basic unit on the field
//----------------------------------------------------------------------------

	//-----------------------------------------------------------
	// Name: Entity
	// Desc:  constructor
	//-----------------------------------------------------------
	Entity::Entity( EntityDesc* desc ) : mStartTime(-1.0f)
		                               , mBaseImage(NULL)
	{
		if( desc )
		{
			EntityDesc::iterator itr;			
			for( itr = desc->begin(); itr != desc->end(); ++itr )
			{
				switch( itr->mFlag )
				{			

				case kEntProp_BaseImage:
					{
						mBaseImage = (ImageX*)itr->mData;												
						break;
					}
				}
			}
		}			
	}

	//-----------------------------------------------------------
	// Name: SetStartTime
	// Desc:  set the start (spawn) time for entity
	//-----------------------------------------------------------			
	void Entity::SetStartTime( F32 time )
	{
		mStartTime = time;
	}
	
	//-----------------------------------------------------------
	// Name: DestroyEntityDescMap
	// Desc:  delete entity property data
	//-----------------------------------------------------------
	void DestroyEntityDescMap( EntityDescMap* descMap )
	{
		if( !descMap )
			return;

		EntityDescMap::iterator itr;
		for( itr = descMap->begin(); itr != descMap->end(); ++itr )
		{
			EntityDesc::iterator propItr;
			for( propItr = itr->second.begin(); propItr != itr->second.end(); ++propItr )
			{
				if( propItr->mData && propItr->mDataSize )
					delete [] propItr->mData;
			}
		}
	}

}; //end Game
