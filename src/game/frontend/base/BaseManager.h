#pragma once

#include <unordered_map>
#include <vector>

#include "common/Common.h"

#include "game/backend/base/BaseData.h"

#include "game/backend/unit/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"

namespace game::backend::unit {
class Def;
}

namespace game {
namespace frontend {

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
class SlotBadges;

CLASS( BaseManager, common::Class )

	BaseManager( Game* game );
	~BaseManager();

	base::Base* GetBaseById( const size_t id ) const;

	void SpawnBase(
		const size_t base_id,
		const size_t slot_index,
		const ::types::Vec2< size_t >& tile_coords,
		const ::types::Vec3& render_coords,
		const backend::base::BaseData& data
	);
	// TODO void DespawnBase( const size_t base_id );

	SlotBadges* GetSlotBadges( const size_t slot_index ) const;
	void DefineSlotBadges( const size_t slot_index, const faction::Faction* faction );

	void SelectBase( Base* base );

private:
	friend class Base;

private:
	friend class SlotBadges;
	text::InstancedFont* GetBadgeFont() const;

private:

	Game* m_game;
	sprite::InstancedSpriteManager* m_ism;

	text::InstancedFont* m_name_font;
	text::InstancedFont* m_badge_font;

	std::unordered_map< size_t, SlotBadges* > m_slot_badges = {};

	std::unordered_map< size_t, base::Base* > m_bases = {};

};

}
}
}
