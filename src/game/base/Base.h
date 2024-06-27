#pragma once

#include <string>

#include "gse/Wrappable.h"
#include "game/MapObject.h"

#include "types/Buffer.h"

namespace game {

class Game;
namespace slot {
class Slot;
}
namespace map::tile {
class Tile;
}

namespace base {

class Base : public gse::Wrappable, public MapObject {
public:

	static const size_t GetNextId();
	static const void SetNextId( const size_t id );

	Base(
		Game* game,
		const size_t id,
		slot::Slot* owner,
		map::tile::Tile* tile,
		const std::string& name
	);
	virtual ~Base() = default;

	const size_t m_id;
	slot::Slot* m_owner;
	std::string m_name;

	static const types::Buffer Serialize( const Base* base );
	static Base* Unserialize( types::Buffer& buf, Game* game );

	WRAPDEFS_DYNAMIC( Base );

private:
	Game* const m_game;

};

}
}
