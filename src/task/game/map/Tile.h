#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "types/Serializable.h"
#include "types/Vec2.h"

using namespace types;

namespace task {
namespace game {
namespace map {

#define TH( ... ) [ __VA_ARGS__ ] ( Tile* tile ) -> void

// WARNING: for now everything is public (because it's not clear what should be hidden from what)
//   you can read any properties you need
//   but be careful modifying anything, some things are only to be modified within Tile::Update() to keep consistent state
// Some day this class will be refactored with access isolation and getters/setters
class Tile { // not deriving from anything because tiles are initialized with malloc (without new) so vtable would get screwed
	public:

	typedef std::function< void( Tile* ) > tile_cb_t;
	
	// map coordinates
	// using SMAC coordinate system (increments by 2 horizontally and vertically, by 1 diagonally)
	Vec2< size_t > coord;

	// per-vertex, left, right and top linked to vertices of others tiles
	typedef ssize_t elevation_t;
	static constexpr elevation_t ELEVATION_MIN = -3500;
	static constexpr elevation_t ELEVATION_MAX = 3500;
	static constexpr elevation_t ELEVATION_LEVEL_COAST = 0;
	static constexpr elevation_t ELEVATION_LEVEL_OCEAN = -1000;
	static constexpr elevation_t ELEVATION_LEVEL_TRENCH = -2000;
	
	// when reading or writing elevation - work only with values, make sure to not modify pointers themselves!
	// pointers are set only once by Tiles and are not to be changed after
	struct {
		elevation_t* center;
		elevation_t* left;
		elevation_t* top;
		elevation_t* right;
		elevation_t* bottom;
		std::vector<elevation_t*> corners; // for more convenient iteration, contains left, top, right and bottom pointers
	} elevation;
	
	struct { // moved to separate structure to allow main structure to have only pointers, for consistency
		elevation_t bottom;
		elevation_t center;
	} elevation_data;
	
	// links to adjactent tiles ( west, north, east, south and combinations )
	// will be linked to itself if tile is at north or south map edge
	// never modify these pointers, work only with values
	Tile* W;
	Tile* NW;
	Tile* N;
	Tile* NE;
	Tile* E;
	Tile* SE;
	Tile* S;
	Tile* SW;
	std::vector<Tile*> neighbours; // for more convenient iteration, contains all neighbouring tiles

	// dynamic parameters, do not modify them manually
	bool is_water_tile = false;
	
	// scalar
	typedef uint8_t moisture_t;
	static constexpr moisture_t M_NONE = 0;
	static constexpr moisture_t M_ARID = 1;
	static constexpr moisture_t M_MOIST = 2;
	static constexpr moisture_t M_RAINY = 3;
	moisture_t moisture;
	
	// scalar
	typedef uint8_t rockiness_t;
	static constexpr rockiness_t R_NONE = 0;
	static constexpr rockiness_t R_FLAT = 1;
	static constexpr rockiness_t R_ROLLING = 2;
	static constexpr rockiness_t R_ROCKY = 3;
	rockiness_t rockiness;
	
	// scalar
	typedef uint8_t bonus_t;
	static constexpr bonus_t B_NONE = 0;
	static constexpr bonus_t B_NUTRIENT = 1;
	static constexpr bonus_t B_ENERGY = 2;
	static constexpr bonus_t B_MINERALS = 3;
	bonus_t bonus;
	
	// bitflags
	typedef uint16_t feature_t;
	static constexpr feature_t F_NONE = 0;
	static constexpr feature_t F_RIVER = 1 << 0;
	static constexpr feature_t F_MONOLITH = 1 << 1;
	static constexpr feature_t F_XENOFUNGUS = 1 << 2;
	static constexpr feature_t F_JUNGLE = 1 << 3;
	static constexpr feature_t F_URANIUM = 1 << 4;
	static constexpr feature_t F_GEOTHERMAL = 1 << 5;
	static constexpr feature_t F_UNITY_POD = 1 << 6;
	static constexpr feature_t F_VOLCANO = 1 << 7;
	static constexpr feature_t F_SUNNY_MESA = 1 << 8;
	static constexpr feature_t F_GARLAND_CRATER = 1 << 9;
	static constexpr feature_t F_FOSSIL_FIELD_RIDGE = 1 << 10;
	static constexpr feature_t F_UNITY_ENERGY = 1 << 11;
	static constexpr feature_t F_UNITY_CHOPPER = 1 << 12;
	static constexpr feature_t F_UNITY_RADAR = 1 << 13;
	static constexpr feature_t F_DUNES = 1 << 14;
	feature_t features;
	
	// bitflags
	typedef uint16_t terraforming_t;
	static constexpr terraforming_t T_NONE = 0;
	static constexpr terraforming_t T_ROAD = 1 << 0;
	static constexpr terraforming_t T_MAG_TUBE = 1 << 1;
	static constexpr terraforming_t T_FOREST = 1 << 2;
	static constexpr terraforming_t T_FARM = 1 << 3;
	static constexpr terraforming_t T_SOIL_ENRICHER = 1 << 4;
	static constexpr terraforming_t T_SOLAR = 1 << 5;
	static constexpr terraforming_t T_MINE = 1 << 6;
	static constexpr terraforming_t T_CONDENSER = 1 << 7;
	static constexpr terraforming_t T_MIRROR = 1 << 8;
	static constexpr terraforming_t T_BOREHOLE = 1 << 9;
	static constexpr terraforming_t T_SENSOR = 1 << 10;
	static constexpr terraforming_t T_BUNKER = 1 << 11;
	static constexpr terraforming_t T_AIRBASE = 1 << 12;
	terraforming_t terraforming;
	
	// WARNING: make sure to call this after changing something in tile
	//   it recalculates dynamic properties and solves inconsistencies
	//   safe to call anytime
	void Update();
	
	// reset to empty state
	void Clear();
	
	const Buffer Serialize() const;
	void Unserialize( Buffer data );
};

}
}
}
