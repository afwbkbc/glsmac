#pragma once

#include "common/Common.h"

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

CLASS2( AnimationManager, common::Class, gse::GCWrappable )
public:
	AnimationManager( Game* game );
	~AnimationManager();

	typedef std::function< void() > cb_oncomplete;

	void Clear();
	void DefineAnimation( Def* def );
	void UndefineAnimation( const std::string& id );
	const std::string* ShowAnimation( const std::string& animation_id, const map::tile::Tile* tile, const cb_oncomplete& on_complete );
	const size_t AddAnimationCallback( const cb_oncomplete& on_complete );
	void FinishAnimation( const size_t animation_id );

	WRAPDEFS_PTR( AnimationManager )

	void Serialize( types::Buffer& buf ) const;
	void Unserialize( types::Buffer& buf );

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override;
#endif

private:
	Game* m_game = nullptr;

	std::unordered_map< std::string, animation::Def* > m_animation_defs = {};
	size_t m_next_running_animation_id = 1;
	std::unordered_map< size_t, cb_oncomplete > m_running_animations_callbacks = {};

};

}
}
}
