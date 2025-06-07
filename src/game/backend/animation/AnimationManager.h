#pragma once

#include "gse/GCWrappable.h"

#include "gse/value/Object.h"

namespace game {
namespace backend {

class Game;

namespace map::tile {
class Tile;
}

namespace animation {

class Def;
class AnimationSequence;

CLASS( AnimationManager, gse::GCWrappable )
public:
	AnimationManager( Game* game );
	~AnimationManager();

	typedef std::function< void( const size_t& animation_id ) > cb_oncomplete;

	void Clear();
	void DefineAnimation( Def* def );
	void UndefineAnimation( const std::string& id );
	const size_t ShowAnimation( GSE_CALLABLE, const std::string& animation_id, const map::tile::Tile* tile, const cb_oncomplete& on_complete );
	const size_t AddAnimationCallback( const cb_oncomplete& on_complete );
	void FinishAnimation( const size_t animation_id );
	void AbortAnimation( const size_t animation_id );

	WRAPDEFS_PTR( AnimationManager )

	void Serialize( types::Buffer& buf ) const;
	void Unserialize( types::Buffer& buf );

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

private:
	friend class AnimationSequence;
	Game* m_game = nullptr;
	void RemoveAnimationSequence( const size_t sequence_id );

private:
	std::unordered_map< std::string, animation::Def* > m_animation_defs = {};
	size_t m_next_running_animation_id = 1;
	std::unordered_map< size_t, cb_oncomplete > m_running_animations_callbacks = {};

	std::unordered_map< size_t, AnimationSequence* > m_animation_sequences = {};
	size_t m_next_animation_sequence_id = 1;

};

}
}
}
