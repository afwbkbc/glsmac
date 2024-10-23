#pragma once

#include <vector>
#include <string>

#include "gse/Wrappable.h"
#include "game/backend/MapObject.h"

#include "types/Buffer.h"

#include "Pop.h"

namespace game {
namespace backend {

class Game;
namespace slot {
class Slot;
}
namespace rules {
class Faction;
}
namespace map::tile {
class Tile;
}

namespace base {

class Base : public gse::Wrappable, public MapObject {
public:

	static const size_t GetNextId();
	static const void SetNextId( const size_t id );

	typedef std::vector< Pop > pops_t;

	Base(
		Game* game,
		const size_t id,
		slot::Slot* owner,
		rules::Faction* faction, // faction may differ from owner's faction in some cases, i.e. after being conquered
		map::tile::Tile* tile,
		const std::string& name,
		const pops_t& pops
	);
	virtual ~Base() = default;

	void AddPop( const Pop& pop );

	const size_t m_id;
	slot::Slot* m_owner;
	rules::Faction* m_faction;
	std::string m_name;
	pops_t m_pops;

	static const types::Buffer Serialize( const Base* base );
	static Base* Unserialize( types::Buffer& buf, Game* game );

	WRAPDEFS_DYNAMIC( Base );

private:
	Game* const m_game;

};

}
}
}
