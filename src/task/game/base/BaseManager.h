#pragma once

#include <unordered_map>
#include <vector>

#include "common/Common.h"

#include "game/unit/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"

namespace game::unit {
class Def;
}

namespace task {
namespace game {

class Game;
class InstancedSpriteManager;

namespace tile {
class Tile;
}

namespace faction {
class Faction;
}

namespace base {

class Base;

CLASS( BaseManager, common::Class )

	BaseManager( Game* game );
	~BaseManager();

	base::Base* GetBaseById( const size_t id ) const;

	void SpawnBase(
		const size_t base_id,
		const size_t slot_index,
		const ::types::Vec2< size_t >& tile_coords,
		const ::types::Vec3& render_coords
	);
	// TODO void DespawnBase( const size_t base_id );

private:

	Game* m_game;
	InstancedSpriteManager* m_ism;

	const size_t m_slot_index;

	std::unordered_map< size_t, base::Base* > m_bases = {};

};

}
}
}
