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

	void SetEffectiveArea( const types::Vec2< size_t >& top_left, const types::Vec2< size_t >& bottom_right, const types::mesh::coord_t z );
	void GetEffectiveArea( types::Vec2< size_t >& top_left, types::Vec2< size_t >& bottom_right ) const;

private:
	std::unordered_set< Actor* > m_cache_children = {};

	types::Vec2< size_t > m_top_left = {};
	types::Vec2< size_t > m_bottom_right = {};

};

}
}
