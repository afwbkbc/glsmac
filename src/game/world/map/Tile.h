#pragma once

#include <cstdint>
#include <functional>

#include "types/Serializable.h"

using namespace std;
using namespace types;

namespace game {
namespace world {
namespace map {

#define TH( ... ) [ __VA_ARGS__ ] ( Tile* tile ) -> void

// WARNING: for now everything is public (because it's not clear what should be hidden from what)
//   you can read any properties you need
//   but be careful modifying anything, some things are only to be modified within Tile::Update() to keep consistent state
// Some day this class will be refactored with access isolation and getters/setters
class Tile { // not deriving from anything because tiles are initialized with malloc (without new) so vtable would get screwed
	public:

	typedef function< void( Tile* ) > tile_cb_t;
	
	// map coordinates
	// using SMAC coordinate system (increments by 2 horizontally and vertically, by 1 diagonally)
	struct {
		size_t x;
		size_t y;
	} coord;

	// per-vertex, left, right and top linked to vertices of others tiles
	typedef ssize_t elevation_t;
	static const elevation_t ELEVATION_MIN = -3500;
	static const elevation_t ELEVATION_MAX = 3500;
	static const elevation_t ELEVATION_LEVEL_COAST = 0;
	static const elevation_t ELEVATION_LEVEL_OCEAN = -1000;
	static const elevation_t ELEVATION_LEVEL_TRENCH = -2000;
	
	// when reading or writing elevation - work only with values, make sure to not modify pointers themselves!
	// pointers are set only once by Tiles and are not to be changed after
	struct {
		elevation_t* center;
		elevation_t* left;
		elevation_t* top;
		elevation_t* right;
		elevation_t* bottom;
		vector<elevation_t*> corners; // for more convenient iteration, contains left, top, right and bottom pointers
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
	vector<Tile*> neighbours; // for more convenient iteration, contains all neighbouring tiles

	// dynamic parameters, do not modify them manually
	bool is_water_tile = false;
	
	// scalar
	typedef uint8_t moisture_t;
	static const moisture_t M_NONE = 0;
	static const moisture_t M_ARID = 1;
	static const moisture_t M_MOIST = 2;
	static const moisture_t M_RAINY = 3;
	moisture_t moisture;
	
	// scalar
	typedef uint8_t rockyness_t;
	static const rockyness_t R_NONE = 0;
	static const rockyness_t R_FLAT = 1;
	static const rockyness_t R_ROLLING = 2;
	static const rockyness_t R_ROCKY = 3;
	rockyness_t rockyness;
	
	// bitflags
	typedef uint16_t feature_t;
	static const feature_t F_NONE = 0;
	static const feature_t F_NUTRIENT_BONUS = 1 << 0;
	static const feature_t F_MINERALS_BONUS = 1 << 1;
	static const feature_t F_ENERGY_BONUS = 1 << 2;
	static const feature_t F_RIVER = 1 << 3;
	static const feature_t F_MONOLITH = 1 << 4;
	static const feature_t F_XENOFUNGUS = 1 << 5;
	static const feature_t F_JUNGLE = 1 << 6;
	static const feature_t F_SURVEY_POD = 1 << 7;
	static const feature_t F_VOLCANO = 1 << 8;
	static const feature_t F_SUNNY_MESA = 1 << 9;
	static const feature_t F_URANIUM_FLATS = 1 << 10;
	static const feature_t F_GARLAND_CRATER = 1 << 11;
	static const feature_t F_FOSSIL_FIELD_RIDGE = 1 << 12;
	static const feature_t F_UNITY_ENERGY = 1 << 13;
	static const feature_t F_UNITY_CHOPPER = 1 << 14;
	static const feature_t F_UNITY_RADAR = 1 << 15;
	feature_t features;
	
	// WARNING: make sure to call this after changing something in tile
	//   it recalculates dynamic properties and solves inconsistencies
	//   safe to call anytime
	void Update();
	
	const Buffer Serialize() const;
	void Unserialize( Buffer data );
};

}
}
}
