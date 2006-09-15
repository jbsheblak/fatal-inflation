//---------------------------------------------------
// Name: Game : ResourceCache
// Desc:  caches resources
// Author: John Sheblak
// Contact: jbsheblak@mail.utexas.edu
//---------------------------------------------------

#include "ResourceCache.h"

namespace Game
{
	//-----------------------------------------------------------
	// Name: Resource
	// Desc:  constructor
	//-----------------------------------------------------------
	Resource::Resource() : mResType( 0 )
						 , mResHandle( 0 )						 
	{}

	Resource::~Resource()
	{
		int x = 0;
	}

	//-----------------------------------------------------------
	// Name: GetResType
	// Desc:  returns the type of this resource
	//-----------------------------------------------------------
	Resource::ResType Resource::GetResType()
	{
		return mResType;
	}

	//-----------------------------------------------------------
	// Name: GetResHandle
	// Desc:  gets the handle associated with this resource
	//-----------------------------------------------------------
	Resource::ResHandle Resource::GetResHandle()
	{
		return mResHandle;
	}	

	//-----------------------------------------------------------
	// Name: GetResData
	// Desc:  get data associated with this resource
	//-----------------------------------------------------------
	void* Resource::GetResData()
	{
		return mpData;
	}

	//-----------------------------------------------------------
	// Name: GetResCache
	// Desc:  singleton pattern
	//-----------------------------------------------------------
	ResourceCache* ResourceCache::GetResCache()
	{
		static ResourceCache* pResCache = new ResourceCache;
		return pResCache;
	}

	//-----------------------------------------------------------
	// Name: MakeHandle
	// Desc:  generate a ResHandle from a name
	//-----------------------------------------------------------
	Resource::ResHandle ResourceCache::MakeHandle( const char* name )
	{
		if( !name )
			return Resource::kInvalidHandle;
		
		return (Resource::ResHandle)DJBHash( name );
	}

	//-----------------------------------------------------------
	// Name: AddRes
	// Desc:  add a resource to the cache using a name as a key
	//-----------------------------------------------------------	
	Resource::ResHandle ResourceCache::AddRes( Resource::ResHandle handle, Resource* pResource)
	{
		if( handle == Resource::kInvalidHandle || !pResource )
			return Resource::kInvalidHandle;	

		HashHandleMap::iterator itr;
		if( ( itr = mHandleMap.find(handle) ) != mHandleMap.end() )
		{
			uint32_t idx = itr->second;

			// on conflict, delete old entry and overwrite with new
			if( mData[idx] )
			{				
				delete mData[idx];
			}			

			mHandleMap[handle] = idx;
			mData[idx] = pResource;
		}		
		else
		{
			mHandleMap[handle] = (uint32_t)mData.size();
			mData.push_back(pResource);
		}

		return handle;
	}	

	//-----------------------------------------------------------
	// Name: AddRes
	// Desc:  add a resource by hashed name
	//-----------------------------------------------------------
	Resource::ResHandle ResourceCache::AddRes( Resource* pResource )
	{
		if( !pResource )
			return Resource::kInvalidHandle;

		return AddRes( pResource->GetResHandle(), pResource );
	}

	//-----------------------------------------------------------
	// Name: GetResource
	// Desc:  get the resource with a name key (not intended to be used frequently)
	// Note:  it is more efficient to call GetHandle() once, save the handle
	//        and then used GetRes( handle ) to get resources
	//-----------------------------------------------------------
	Resource* ResourceCache::GetResource( const char* name, Resource::ResType type )
	{		
		Resource* pRes = GetResource( MakeHandle( name ) );
		if( !pRes || pRes->GetResType() != type )
			return NULL;

		return pRes;
	}

	//-----------------------------------------------------------
	// Name: GetResource
	// Desc:  gets the resource with a handle key
	//-----------------------------------------------------------
	Resource* ResourceCache::GetResource( Resource::ResHandle handle )
	{
		if( mHandleMap.find(handle) == mHandleMap.end() )
			return NULL;

		uint32_t idx = mHandleMap[handle];
		return mData[idx];
	}

	//-----------------------------------------------------------
	// Name: Flush
	// Desc:  throw away all resources in cache
	//-----------------------------------------------------------
	void ResourceCache::Flush()
	{
		DataStorage::iterator itr;
		for( itr = mData.begin(); itr != mData.end(); ++itr )
		{
			delete *itr;			
		}

		mData.clear();
		mHandleMap.clear();
	}


	//-----------------------------------------------------------
	// Name: DJBHash
	// Desc:  hash a string into a numeric value
	//-----------------------------------------------------------
	uint32_t ResourceCache::DJBHash( const char* str )
	{		
		uint32_t hash = 5381;

		const uint32_t length = (uint32_t)strlen(str);

		for(uint32_t i = 0; i < length; i++)
		{
			hash = ((hash << 5) + hash) + str[i];
		}

		return (hash & 0x7FFFFFFF);
	}
	
}; //end Game
