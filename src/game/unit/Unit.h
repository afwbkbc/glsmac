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

	typedef uint8_t morale_t;
	static const morale_t MORALE_MIN;
	static const morale_t MORALE_MAX;

	typedef float health_t;
	static const health_t HEALTH_MIN;
	static const health_t HEALTH_MAX;

	Unit(
		const size_t id,
		const Def* def,
		Slot* owner,
		map::Tile* tile,
		const morale_t morale,
		const health_t health
	);
	virtual ~Unit() = default;

	const size_t m_id;
	const Def* m_def;
	Slot* m_owner;
	map::Tile* m_tile = nullptr;

	morale_t m_morale;
	health_t m_health;
	const std::string& GetMoraleString() const;

	static const types::Buffer Serialize( const Unit* unit );
	static Unit* Unserialize( types::Buffer& buf, const Game* game );

	WRAPDEFS_PTR( Unit );
};

}
}
