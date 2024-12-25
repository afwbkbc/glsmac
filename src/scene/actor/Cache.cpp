#include "Cache.h"

namespace scene {
namespace actor {

Cache::Cache( const std::string& name )
	: Actor( TYPE_CACHE, name ) {

}

Cache::~Cache() {
	if ( !m_cache_children.empty() ) {
		Log( "WARNING: some cache children still exist" );
	}
}

void Cache::AddCacheChild( Actor* actor ) {
	ASSERT( m_cache_children.find( actor ) == m_cache_children.end(), "duplicate cache child" );
	m_cache_children.insert( actor );
}

void Cache::RemoveCacheChild( Actor* actor ) {
	ASSERT( m_cache_children.find( actor ) != m_cache_children.end(), "cache child not found" );
	m_cache_children.erase( actor );
}

const std::unordered_set< Actor* >& Cache::GetCacheChildren() const {
	return m_cache_children;
}

}
}
