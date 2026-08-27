#pragma once

#include <string>

#include "gse/Wrappable.h"
#include "game/backend/MapObject.h"

#include "Types.h"
#include "game/backend/map/tile/Types.h"

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
		const bool moved_this_turn,
		const map::tile::terraforming_t terraforming,
		const uint16_t terraforming_turns_remaining,
		const size_t home_base_id,
		const uint16_t fuel,
		const size_t transport_id,
		const bool native_capture_attempted = false
	);
	virtual ~Unit();

	const size_t m_id;
	Def* m_def;
	slot::Slot* m_owner;

	movement_t m_movement;
	morale_t m_morale;
	health_t m_health;
	bool m_moved_this_turn;
	map::tile::terraforming_t m_terraforming;
	uint16_t m_terraforming_turns_remaining;
	size_t m_home_base_id;
	uint16_t m_fuel;
	size_t m_transport_id;
	bool m_native_capture_attempted;
	bool m_is_registered = false;
	static constexpr uint16_t MAX_TERRAFORMING_TURNS = 255;

	size_t m_animation_id = 0;

	static const movement_t MINIMUM_MOVEMENT_TO_KEEP;
	static const movement_t MINIMUM_HEALTH_TO_KEEP;
	const bool HasMovesLeft() const;

	const std::string& GetMoraleString() const;

	void SetTile( GSE_CALLABLE, map::tile::Tile* tile );
	void SetTerraformingOrder(
		GSE_CALLABLE,
		const map::tile::terraforming_t terraforming,
		const uint16_t turns_remaining
	);
	void SetFuel( GSE_CALLABLE, const uint16_t fuel );
	void SetTransportId( const size_t transport_id );

	static const types::Buffer Serialize( const Unit* unit );
	static Unit* Deserialize( GSE_CALLABLE, types::Buffer& buf, UnitManager* um );

	WRAPDEFS_DYNAMIC( Unit );

	WRAPDEF_SERIALIZABLE;

private:
	UnitManager* const m_um;

};

}
}
}
