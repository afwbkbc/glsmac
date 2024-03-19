#pragma once

#include <cstddef>

#include "Def.h"

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

	typedef uint8_t morale_t;
	static const morale_t MORALE_MIN;
	static const morale_t MORALE_MAX;

	typedef float health_t;
	static const health_t HEALTH_MIN;
	static const health_t HEALTH_MAX;
	static const health_t HEALTH_PER_TURN;

	Unit(
		const size_t id,
		Def* def,
		Slot* owner,
		map::Tile* tile,
		const movement_t movement,
		const morale_t morale,
		const health_t health
	);
	virtual ~Unit() = default;

	const size_t m_id;
	Def* m_def;
	Slot* m_owner;
	map::Tile* m_tile = nullptr;

	movement_t m_movement;
	morale_t m_morale;
	health_t m_health;

	enum movement_result_t {
		MR_MOVED,
		MR_TRIED_BUT_FAILED,
		MR_COULD_NOT_MOVE,
	};
	static const movement_t MINIMUM_MOVEMENT_TO_KEEP;
	const bool HasMovesLeft() const;
	void UpdateMoves( Game* game, const map::Tile* dst_tile );

	void OnTurn();

	static const std::string& GetMoraleString( const morale_t morale );

	static const types::Buffer Serialize( const Unit* unit );
	static Unit* Unserialize( types::Buffer& buf, const Game* game );

	WRAPDEFS_DYNAMIC( Unit );
};

}
}
