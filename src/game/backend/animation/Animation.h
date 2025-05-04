#pragma once

#include "gc/Object.h"
#include "gse/Value.h"

#include <string>

namespace gse::value {
class Callable;
}

namespace game {
namespace backend {

namespace map::tile {
class Tile;
}

namespace animation {

class AnimationSequence;

class Animation : public gc::Object {
public:
	Animation(
		gc::Space* const gc_space,
		map::tile::Tile* const tile,
		const std::string animdef_id,
		gse::value::Callable* const oncomplete
	);

	void Run( GSE_CALLABLE );
	void Abort();

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override;
#endif

private:
	friend class AnimationSequence;

	AnimationSequence* m_sequence = nullptr;

	map::tile::Tile* const m_tile;
	const std::string m_animdef_id;
	gse::value::Callable* const m_oncomplete;

	size_t m_animation_id = 0;
	bool m_is_finished = false;
};

}
}
}
