#pragma once

#include <string>

#include "Types.h"

#include "types/Buffer.h"
#include "gse/Wrappable.h"

namespace game {

class Game;
namespace slot {
class Slot;
}
namespace map::tile {
class Tile;
}

namespace unit {

class Def;

class Unit : public gse::Wrappable {
public:

	static const size_t GetNextId();
	static const void SetNextId( const size_t id );

	Unit(
		Game* game,
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
	map::tile::Tile* m_tile = nullptr;

	movement_t m_movement;
	morale_t m_morale;
	health_t m_health;
	bool m_moved_this_turn;

	static const movement_t MINIMUM_MOVEMENT_TO_KEEP;
	static const movement_t MINIMUM_HEALTH_TO_KEEP;
	const bool HasMovesLeft() const;

	const std::string& GetMoraleString() const;

	static const types::Buffer Serialize( const Unit* unit );
	static Unit* Unserialize( types::Buffer& buf, Game* game );

	WRAPDEFS_DYNAMIC( Unit );

private:
	Game* const m_game;

};

}
}
