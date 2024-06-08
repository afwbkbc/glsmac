#pragma once

#include <unordered_map>
#include <vector>

#include "base/Base.h"

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

namespace ui {
class UnitsList;
}

namespace unit {

class Unit;
class UnitDef;
class BadgeDefs;
class SlotBadges;

CLASS( UnitManager, base::Base )

	UnitManager( Game* game );
	~UnitManager();

	void Iterate();

	Unit* GetUnitById( const size_t id ) const;

	void DefineUnit( const ::game::unit::Def* def );
	void SpawnUnit(
		const size_t unit_id,
		const std::string& unitdef_name,
		const size_t slot_index,
		const ::types::Vec2< size_t >& tile_coords,
		const ::types::Vec3& render_coords,
		const ::game::unit::movement_t movement,
		const ::game::unit::morale_t morale,
		const std::string& morale_string,
		const ::game::unit::health_t health
	);
	void DespawnUnit( const size_t unit_id );
	void RefreshUnit( Unit* unit );
	void MoveUnit( Unit* unit, tile::Tile* dst_tile, const size_t animation_id );
	void MoveUnit_deprecated( Unit* unit, tile::Tile* dst_tile, const types::Vec3& dst_render_coords );

	Unit* GetSelectedUnit() const;
	void SelectUnit( Unit* unit_data, const bool actually_select_unit );
	void DeselectUnit();
	const bool SelectNextUnitMaybe();
	void SelectNextUnitOrSwitchToTileSelection();

	SlotBadges* GetSlotBadges( const size_t slot_index ) const;
	void DefineSlotBadges( const size_t slot_index, const faction::Faction* faction );

	const types::Vec3 GetCloserCoords( const types::Vec3& coords, const types::Vec3& ref_coords ) const;

private:

	Game* m_game;
	InstancedSpriteManager* m_ism;

	const size_t m_slot_index;

	Unit* m_selected_unit = nullptr;

	std::unordered_map< std::string, UnitDef* > m_unitdefs = {};
	std::unordered_map< size_t, Unit* > m_units = {};
	std::vector< Unit* > m_selectable_units = {};

	BadgeDefs* m_badge_defs = nullptr;
	std::unordered_map< size_t, SlotBadges* > m_slot_badges = {};

	struct moving_unit_info_t {
		tile::Tile* tile;
		size_t animation_id;
	};
	std::unordered_map< Unit*, moving_unit_info_t > m_moving_units = {};

	void AddSelectable( Unit* unit );
	void RemoveSelectable( Unit* unit );
	void UpdateSelectable( Unit* unit );
	Unit* GetNextSelectable();

};

}
}
}
