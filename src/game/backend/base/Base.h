#pragma once

#include <cstdint>
#include <unordered_set>
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
class Player;
namespace slot {
class Slot;
}
namespace faction {
class Faction;
}
namespace unit {
class Def;
}
namespace map::tile {
class Tile;
}

namespace base {

class FacilityDef;

class Base : public gse::Wrappable, public MapObject, public ResourceRelated {
public:

	static constexpr int64_t MAX_ACCUMULATED_MINERALS = 1000000000;
	static constexpr size_t MAX_PRODUCTION_QUEUE_SIZE = 8;

	enum production_kind_t : uint8_t {
		PK_UNIT = 0,
		PK_FACILITY = 1,
		PK_PROJECT = 2,
	};
	struct production_t {
		production_kind_t kind;
		std::string id;

		bool operator==( const production_t& other ) const {
			return kind == other.kind && id == other.id;
		}
	};
	typedef std::vector< production_t > production_queue_t;
	typedef std::unordered_set< std::string > facilities_t;

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
		const size_t next_pop_id = 1,
		const production_queue_t& production_queue = {},
		const int64_t accumulated_minerals = 0,
		const facilities_t& facilities = {}
	);
	virtual ~Base() = default;

	const Game* const GetGame() const;

	Pop* const AddPop( const Pop& pop );
	void SetOwner( GSE_CALLABLE, Player* owner );
	void RemovePop( GSE_CALLABLE, const size_t pop_id );
	void ChangePopType( GSE_CALLABLE, const size_t pop_id, const std::string& def_id );
	void WorkPopTile( GSE_CALLABLE, Pop* const pop, map::tile::Tile* const tile );
	void UnworkPopTile( GSE_CALLABLE, Pop* const pop, map::tile::Tile* const tile );
	const production_t* GetProduction() const;
	gse::Wrappable* GetProductionDef( const production_t& production ) const;
	bool CanProduceUnit( const unit::Def* def ) const;
	bool CanProduce( const production_t& production ) const;
	bool CanSetProduction( const production_t& production ) const;
	bool CanQueueProduction( const production_t& production ) const;
	void SetProduction( GSE_CALLABLE, const production_t& production );
	void EnqueueProduction( GSE_CALLABLE, const production_t& production );
	void RemoveProduction( GSE_CALLABLE, const size_t index );
	void SetProductionQueue( GSE_CALLABLE, const production_queue_t& production_queue );
	void ClearProduction();
	bool HasFacility( const std::string& id ) const;
	void AddFacility( GSE_CALLABLE, const std::string& id );
	void RemoveFacility( GSE_CALLABLE, const std::string& id );
	void SetAccumulatedMinerals( GSE_CALLABLE, const int64_t minerals );
	static bool ParseProductionKind( const std::string& value, production_kind_t& result );
	static const std::string GetProductionKindString( const production_kind_t kind );

	const size_t m_id;
	slot::Slot* m_owner;
	faction::Faction* m_faction;
	std::string m_name;
	pops_t m_pops;
	production_queue_t m_production_queue;
	int64_t m_accumulated_minerals;
	facilities_t m_facilities;

	static const types::Buffer Serialize( const Base* base );
	static Base* Deserialize( GSE_CALLABLE, types::Buffer& buf, Game* game );

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
	void RestoreWorkedTiles( GSE_CALLABLE );
	bool ValidateProductionQueue( const production_queue_t& production_queue, std::string& error ) const;
	bool ValidateFacilities( const facilities_t& facilities, std::string& error ) const;

	void TriggerUpdate();
};

}
}
}
