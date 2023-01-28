#pragma once

#include "base/Base.h"

#include <functional>

using namespace std;

namespace game {
namespace world {
namespace map {

#define TH( ... ) [ __VA_ARGS__ ] ( Tile* tile ) -> void

CLASS( Tile, base::Base )

	typedef function< void( Tile* ) > tile_cb_t;
	
	// per-vertex, left, right and top linked to vertices of others tiles
	typedef ssize_t elevation_t;
	static const elevation_t ELEVATION_MIN = -3500;
	static const elevation_t ELEVATION_MAX = 3500;
	struct {
		elevation_t* left;
		elevation_t* top;
		elevation_t* right;
		elevation_t bottom;
		elevation_t center;
	} elevation;
	
	// links to adjactent tiles ( west, north, east, south and combinations )
	// will be linked to itself if tile is at north or south map edge
	Tile* W;
	Tile* NW;
	Tile* N;
	Tile* NE;
	Tile* E;
	Tile* SE;
	Tile* S;
	Tile* SW;

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
	
	// helpers
	Tile* Around( tile_cb_t cb, bool need_all = false );
	Tile* SelfAndAround( tile_cb_t cb );
	
};

}
}
}
