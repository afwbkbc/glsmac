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

void Cache::SetEffectiveArea( const types::Vec2< types::mesh::coord_t >& top_left, const types::Vec2< types::mesh::coord_t >& bottom_right, const types::mesh::coord_t z ) {
	if ( m_top_left != top_left || m_bottom_right != bottom_right || m_z != z ) {
		m_top_left = top_left;
		m_bottom_right = bottom_right;
		m_z = z;
		m_is_changed = true;
	}
}

const bool Cache::GetEffectiveAreaIfChanged( types::Vec2< types::mesh::coord_t >& top_left, types::Vec2< types::mesh::coord_t >& bottom_right, types::mesh::coord_t& z ) {
	if ( m_is_changed ) {
		m_is_changed = false;
		top_left = m_top_left;
		bottom_right = m_bottom_right;
		z = m_z;
		return true;
	}
	else {
		return false;
	}
}

}
}
