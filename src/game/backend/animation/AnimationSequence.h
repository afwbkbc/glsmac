#pragma once

#include "gc/Object.h"
#include "gse/Value.h"

#include <unordered_set>
#include <vector>
#include <unordered_map>

namespace game {
namespace backend {

namespace map::tile {
class Tile;
}

namespace animation {

class Animation;
class AnimationManager;

class AnimationSequence : public gc::Object {
public:
	AnimationSequence( gc::Space* const gc_space, AnimationManager* const animation_manager, const size_t sequence_id, const size_t size );

	void AddAnimation( Animation* const animation );
	void Run( GSE_CALLABLE );

	void Abort();

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override;
#endif

private:
	friend class Animation;
	AnimationManager* const m_am;
	void Finish();
	void RunNextMaybe( GSE_CALLABLE );

private:
	friend class AnimationManager;
	const size_t m_sequence_id;

private:

	bool m_is_running = false;

	const size_t m_expected_size;
	std::unordered_set< map::tile::Tile* > m_locked_tiles = {};
	std::vector< Animation* > m_animations = {};
	size_t m_current_animation_idx = 0;

};

}
}
}
