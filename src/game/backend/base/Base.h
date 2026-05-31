#pragma once

#include <vector>
#include <string>

#include "gse/Wrappable.h"
#include "game/backend/MapObject.h"
#include "game/backend/ResourceRelated.h"

#include "types/Buffer.h"

#include "Pop.h"

namespace gse::value {
class Object;
class Array;
}

namespace game {
namespace backend {

class Game;
namespace slot {
class Slot;
}
namespace faction {
class Faction;
}
namespace map::tile {
class Tile;
}

namespace base {

class Base : public gse::Wrappable, public MapObject, public ResourceRelated {
public:

	static const size_t GetNextId();
	static const void SetNextId( const size_t id );

	typedef std::map< size_t, Pop > pops_t;

	Base(
		Game* game,
		const size_t id,
		slot::Slot* owner,
		faction::Faction* faction, // faction may differ from owner's faction in some cases, i.e. after being conquered
		map::tile::Tile* tile,
		const std::string& name,
		const pops_t& pops,
		const size_t next_pop_id = 1
	);
	virtual ~Base() = default;

	const Game* const GetGame() const;

	Pop* const AddPop( const Pop& pop );
	void RemovePop( const size_t pop_id );
	void ChangePopType( GSE_CALLABLE, const size_t pop_id, const std::string& def_id );

	const size_t m_id;
	slot::Slot* m_owner;
	faction::Faction* m_faction;
	std::string m_name;
	pops_t m_pops;

	static const types::Buffer Serialize( const Base* base );
	static Base* Deserialize( types::Buffer& buf, Game* game );

	WRAPDEFS_DYNAMIC( Base );

	WRAPDEF_SERIALIZABLE;

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

private:
	Game* const m_game;

	std::unordered_set< map::tile::Tile* > m_worked_tiles = {};

	size_t m_next_pop_id = 1;

	const PopDef* const GetPopDef( GSE_CALLABLE, const std::string& id ) const;

	gse::value::Array* const GetWorkableTiles( GSE_CALLABLE );
	gse::value::Array* const GetWorkedTiles( GSE_CALLABLE );
	gse::value::Array* const GetUnworkedTiles( GSE_CALLABLE );
	gse::value::Object* const GetIntake( GSE_CALLABLE );
	gse::value::Object* const GetConsumption( GSE_CALLABLE );

	void TriggerUpdate();
};

}
}
}
