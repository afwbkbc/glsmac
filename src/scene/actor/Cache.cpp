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

void Cache::SetEffectiveArea( const ui::area_t& area ) {
	m_area = area;
}

const ui::area_t& Cache::GetEffectiveArea() {
	return m_area;
}

}
}
