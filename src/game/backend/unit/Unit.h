#pragma once

#include <string>

#include "gse/Wrappable.h"
#include "game/backend/MapObject.h"

#include "Types.h"

#include "types/Buffer.h"

namespace game {
namespace backend {

namespace slot {
class Slot;
}
namespace map::tile {
class Tile;
}

namespace unit {

class Def;
class UnitManager;

class Unit : public gse::Wrappable, public MapObject {
public:

	static const size_t GetNextId();
	static const void SetNextId( const size_t id );

	Unit(
		GSE_CALLABLE,
		UnitManager* um,
		const size_t id,
		Def* def,
		slot::Slot* owner,
		map::tile::Tile* tile,
		const movement_t movement,
		const morale_t morale,
		const health_t health,
		const bool moved_this_turn
	);
	virtual ~Unit() = default;

	const size_t m_id;
	Def* m_def;
	slot::Slot* m_owner;

	movement_t m_movement;
	morale_t m_morale;
	health_t m_health;
	bool m_moved_this_turn;

	size_t m_animation_id = 0;

	static const movement_t MINIMUM_MOVEMENT_TO_KEEP;
	static const movement_t MINIMUM_HEALTH_TO_KEEP;
	const bool HasMovesLeft() const;

	const std::string& GetMoraleString() const;

	void SetTile( GSE_CALLABLE, map::tile::Tile* tile );

	static const types::Buffer Serialize( const Unit* unit );
	static Unit* Unserialize( GSE_CALLABLE, types::Buffer& buf, UnitManager* um );

	WRAPDEFS_DYNAMIC( Unit );

private:
	UnitManager* const m_um;

};

}
}
}
