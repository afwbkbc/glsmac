#pragma once

#include <vector>

#include "Actor.h"

namespace scene {
namespace actor {

class Cache : public Actor {
public:
	Cache( const std::string& name );
	~Cache();

	void AddCacheChild( Actor* actor );
	const std::vector< Actor* >& GetCacheChildren() const;

private:
	std::vector< Actor* > m_cache_children = {};
};

}
}
