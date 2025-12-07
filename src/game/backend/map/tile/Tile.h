#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <unordered_map>

#include "gse/Wrappable.h"

#include "Types.h"

#include "types/Buffer.h"

namespace game {
namespace backend {

namespace unit {
class Unit;
}

namespace base {
class Base;
}

namespace map {
namespace tile {

// WARNING: for now everything is public (because it's not clear what should be hidden from what)
//   you can read any properties you need
//   but be careful modifying anything, some things are only to be modified within Tile::Update() to keep consistent state
// Some day this class will be refactored with access isolation and getters/setters
class Tile : public gse::Wrappable {
public:

	coords_t coord;

	// when reading or writing elevation - work only with values, make sure to not modify pointers themselves!
	// pointers are set only once by Tiles and are not to be changed after
	struct {
		elevation_t* center = nullptr;
		elevation_t* left = nullptr;
		elevation_t* top = nullptr;
		elevation_t* right = nullptr;
		elevation_t* bottom = nullptr;
		std::vector< elevation_t* > corners = {}; // for more convenient iteration, contains left, top, right and bottom pointers
	} elevation;

	struct { // moved to separate structure to allow main structure to have only pointers, for consistency
		elevation_t bottom;
		elevation_t center;
	} elevation_data;

	// links to adjactent tiles ( west, north, east, south and combinations )
	// will be linked to itself if tile is at north or south map edge
	// never modify these pointers, work only with values
	Tile* W = nullptr;
	Tile* NW = nullptr;
	Tile* N = nullptr;
	Tile* NE = nullptr;
	Tile* E = nullptr;
	Tile* SE = nullptr;
	Tile* S = nullptr;
	Tile* SW = nullptr;
	std::vector< Tile* > neighbours = {}; // for more convenient iteration, contains all neighbouring tiles

	static const std::unordered_map< direction_t, std::string > s_direction_str;
	static const std::string& GetDirectionString( const direction_t direction );
	Tile* GetNeighbour( const direction_t direction );

	// dynamic parameters, do not modify them manually
	bool is_water_tile = false;

	moisture_t moisture;
	rockiness_t rockiness;
	bonus_t bonus;
	feature_t features;
	terraforming_t terraforming;

	yields_t yields = {};

	// units (id -> unit)
	std::map< size_t, unit::Unit* > units = {};

	base::Base* base = nullptr;

	// WARNING: make sure to call this after changing something in tile
	//   it recalculates dynamic properties and solves inconsistencies
	//   safe to call anytime
	void Update();

	// reset to empty state
	void Clear();

	const bool IsAdjactentTo( const Tile* other ) const;

	const types::Buffer Serialize() const;
	void Deserialize( types::Buffer data );

	const std::string ToString() const;

	WRAPDEFS_PTR( Tile );

	WRAPDEF_SERIALIZABLE;

	void Lock( const size_t initiator_slot );
	void Unlock();
	const bool IsLocked() const;
	const bool IsLockedBy( const size_t initiator_slot ) const;

private:
	bool m_is_locked = false;
	size_t m_lock_initiator_slot = 0;
};

}
}
}
}
