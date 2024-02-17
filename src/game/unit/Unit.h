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

	Unit( const size_t id, const Def* def, Slot* owner, map::Tile* tile );
	virtual ~Unit() = default;

	const size_t m_id;
	const Def* m_def;
	Slot* m_owner;
	map::Tile* m_tile = nullptr;

	static const types::Buffer Serialize( const Unit* unit );
	static Unit* Unserialize( types::Buffer& buf, const Game* game );

	WRAPDEFS_PTR( Unit );
};

}
}
