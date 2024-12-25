#pragma once

#include <unordered_set>

#include "Actor.h"

namespace scene {
namespace actor {

CLASS( Cache, Actor )

	Cache( const std::string& name );
	~Cache();

	void AddCacheChild( Actor* actor );
	void RemoveCacheChild( Actor* actor );
	const std::unordered_set< Actor* >& GetCacheChildren() const;

private:
	std::unordered_set< Actor* > m_cache_children = {};

};

}
}
