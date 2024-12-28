#pragma once

#include <unordered_set>

#include "Actor.h"

#include "types/mesh/Types.h"
#include "ui/Types.h"

namespace scene {
namespace actor {

CLASS( Cache, Actor )

	Cache( const std::string& name );
	~Cache();

	void AddCacheChild( Actor* actor );
	void RemoveCacheChild( Actor* actor );

	void SetEffectiveArea( const ui::area_t& area );
	const ui::area_t& GetEffectiveArea();

private:
	std::unordered_set< Actor* > m_cache_children = {};

	bool m_is_changed = false;
	ui::area_t m_area = {};

};

}
}
