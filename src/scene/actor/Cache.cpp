#include "Cache.h"

namespace scene {
namespace actor {

Cache::Cache( const std::string& name )
	: Actor( TYPE_CACHE, name ) {}

Cache::~Cache() {}

void Cache::AddCacheChild( Actor* actor ) {
	m_cache_children.push_back( actor );
}

const std::vector< Actor* >& Cache::GetCacheChildren() const {
	return m_cache_children;
}

}
}
