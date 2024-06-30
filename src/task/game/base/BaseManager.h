#pragma once

#include <unordered_map>
#include <vector>

#include "common/Common.h"

#include "game/base/BaseData.h"

#include "game/unit/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"

namespace game::unit {
class Def;
}

namespace task {
namespace game {

class Game;
class Slot;

namespace sprite {
class InstancedSpriteManager;
}

namespace text {
class InstancedFont;
class InstancedText;
class InstancedTextManager;
}

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
		const ::types::Vec3& render_coords,
		const ::game::base::BaseData& data
	);
	// TODO void DespawnBase( const size_t base_id );

private:
	friend class base::Base;

private:

	Game* m_game;
	text::InstancedFont* m_name_font;

	std::unordered_map< size_t, base::Base* > m_bases = {};

};

}
}
}
