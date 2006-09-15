//---------------------------------------------------
// Name: Game : ResourceCache
// Desc:  caches resources
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#ifndef _GAME_RESOURCE_CACHE_H_
#define _GAME_RESOURCE_CACHE_H_

#include "Types.h"

#include <map>
#include <vector>

#ifndef SAFE_DELETE
#define SAFE_DELETE( x ) { if(x) { delete (x); (x) = NULL; } }
#endif
										 

namespace Game
{
	//-----------------------------------------------------------
	// Name: Resource
	// Desc:  an individual data chunk in our cache
	//-----------------------------------------------------------
	class Resource
	{	
	public:

		typedef uint32_t ResHandle;
		typedef uint32_t ResType;

		static const ResHandle kInvalidHandle = 0xffffffff;

	public:

		// ctor / dtor
		Resource();	
		virtual ~Resource();

		// accessors
		ResType 		GetResType();
		ResHandle		GetResHandle();				
		void*			GetResData();

	protected:

		uint32_t		mResType;		// Resource Type (image, audio, etc)
		ResHandle		mResHandle;		// Handle associated with cache (for lookups)		
		void*			mpData;			// data
	};

	//-----------------------------------------------------------
	// Name: TypedResource
	// Desc:  a resource that inherits from a base class
	//-----------------------------------------------------------
	template < typename T >
	class TypedResource : public Resource
	{
	public:

		TypedResource( ResType type, ResHandle handle, T* data )
		{
			Resource::mResType	  = type;
			Resource::mResHandle  = handle;
			Resource::mpData      = data;
		}	

		~TypedResource()
		{
			T* pData = (T*)mpData;
			if( pData )
			{
				delete pData;
				pData = NULL;
			}
		}
	};

	//-----------------------------------------------------------
	// Name: ResourceCache
	// Desc:  stores resources used frequently in the application
	//-----------------------------------------------------------
	class ResourceCache
	{
	public:		

		// singleton access
		static ResourceCache* GetResCache();

	public:		

		// generate a handle from a name and a type (hash of "name"")
		Resource::ResHandle MakeHandle( const char* name );

		// add a resource using a given handle
		Resource::ResHandle AddRes( Resource::ResHandle handle, Resource* pResource );

		// add a resource using the handle given by pResource->GetResHandle()
		Resource::ResHandle AddRes( Resource* pResource );

		// get a resource from the cache with a name and type
		Resource* GetResource( const char* name, Resource::ResType type );

		// get a resource from the cache with a handle
		Resource* GetResource( Resource::ResHandle handle );			

		// clear out all resource entries in cache
		void Flush();

	public:

		// hash a name
		static uint32_t DJBHash( const char* str );

	private:

		typedef std::map< Resource::ResHandle, uint32_t >	HashHandleMap;
		typedef std::vector< Resource* >					DataStorage;

		HashHandleMap				mHandleMap;
		DataStorage					mData;
	};

#define ResCache (*ResourceCache::GetResCache())
	
}; //end Game

#endif // end _GAME_RESOURCE_CACHE_H_
    
