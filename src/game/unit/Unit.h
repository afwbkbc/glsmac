#pragma once

#include <cstddef>

#include "Def.h"
#include "Morale.h"

#include "gse/Value.h"

#include "game/map/Tile.h"
#include "game/Slot.h"

namespace game {

class Game;

class Slot;

namespace unit {

class Unit {
public:

	static const size_t GetNextId();
	static const void SetNextId( const size_t id );

	typedef float movement_t;
	typedef float health_t;

	Unit(
		const size_t id,
		Def* def,
		Slot* owner,
		map::Tile* tile,
		const movement_t movement,
		const Morale::morale_t morale,
		const health_t health,
		const bool moved_this_turn
	);
	virtual ~Unit() = default;

	const size_t m_id;
	Def* m_def;
	Slot* m_owner;
	map::Tile* m_tile = nullptr;

	movement_t m_movement;
	Morale::morale_t m_morale;
	health_t m_health;
	bool m_moved_this_turn;

	enum movement_result_t {
		MR_MOVED,
		MR_TRIED_BUT_FAILED,
		MR_COULD_NOT_MOVE,
	};
	static const movement_t MINIMUM_MOVEMENT_TO_KEEP;
	static const movement_t MINIMUM_HEALTH_TO_KEEP;
	const bool HasMovesLeft() const;

	const std::string& GetMoraleString();

	static const types::Buffer Serialize( const Unit* unit );
	static Unit* Unserialize( types::Buffer& buf, const Game* game );

	WRAPDEFS_DYNAMIC( Unit );
};

}
}
