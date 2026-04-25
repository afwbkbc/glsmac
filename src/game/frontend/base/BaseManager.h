#pragma once

#include <unordered_map>
#include <vector>
#include <set>

#include "common/Common.h"

#include "game/backend/unit/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"

namespace game::backend {
namespace unit {
class Def;
}
namespace base {
class PopDef;
}
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

class PopDef;
class Base;
class SlotBadges;

CLASS( BaseManager, common::Class )

	BaseManager( Game* game );
	~BaseManager();

	base::Base* GetBaseById( const size_t id ) const;

	void DefinePop( const backend::base::PopDef* def );
	void UndefinePop( const std::string& id );

	const std::vector< std::string >& GetPopDefOrder() const;
	PopDef* GetPopDef( const std::string& id ) const;

	void SpawnBase(
		const size_t base_id,
		const size_t slot_index,
		const ::types::Vec2< size_t >& tile_coords,
		const ::types::Vec3& render_coords,
		const std::string& name
	);
	void DespawnBase( const size_t base_id );
	void RefreshBase( Base* base );

	SlotBadges* GetSlotBadges( const size_t slot_index ) const;
	void DefineSlotBadges( const size_t slot_index, const faction::Faction* faction );

	void SelectBase( Base* base );
	Base* GetBaseBefore( Base* base ) const;
	Base* GetBaseAfter( Base* base ) const;

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

	std::vector< std::string > m_popdefs_order = {};
	std::unordered_map< std::string, PopDef* > m_popdefs = {};

	std::unordered_map< size_t, base::Base* > m_bases = {};
	typedef std::set< size_t > ordered_base_ids_t;
	std::unordered_map< faction::Faction*, ordered_base_ids_t > m_faction_base_ids = {};

};

}
}
}
