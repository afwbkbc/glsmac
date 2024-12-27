#pragma once

#include <unordered_set>

#include "Actor.h"

#include "types/mesh/Types.h"

namespace scene {
namespace actor {

CLASS( Cache, Actor )

	Cache( const std::string& name );
	~Cache();

	void AddCacheChild( Actor* actor );
	void RemoveCacheChild( Actor* actor );

	void SetEffectiveArea( const types::Vec2< types::mesh::coord_t >& top_left, const types::Vec2< types::mesh::coord_t >& bottom_right, const types::mesh::coord_t z );
	const bool GetEffectiveAreaIfChanged( types::Vec2< types::mesh::coord_t >& top_left, types::Vec2< types::mesh::coord_t >& bottom_right, types::mesh::coord_t& z );

private:
	std::unordered_set< Actor* > m_cache_children = {};

	bool m_is_changed = false;
	types::Vec2< types::mesh::coord_t > m_top_left = {};
	types::Vec2< types::mesh::coord_t > m_bottom_right = {};
	types::mesh::coord_t m_z = 0;
	
};

}
}
