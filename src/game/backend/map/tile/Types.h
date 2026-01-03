#pragma once

#include <vector>
#include <string>

#include "common/Common.h"

#include "types/Vec2.h"
#include "types/Vec3.h"
#include "types/Color.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

// map coordinates
// using SMAC coordinate system (increments by 2 horizontally and vertically, by 1 diagonally)
typedef types::Vec2< size_t > coords_t;

enum direction_t {
	D_NONE,
	D_W,
	D_NW,
	D_N,
	D_NE,
	D_E,
	D_SE,
	D_S,
	D_SW
};

// per-vertex, left, right and top linked to vertices of others tiles
typedef ssize_t elevation_t;
static constexpr elevation_t ELEVATION_MIN = -3500;
static constexpr elevation_t ELEVATION_MAX = 3500;
static constexpr elevation_t ELEVATION_LEVEL_COAST = 0;
static constexpr elevation_t ELEVATION_LEVEL_OCEAN = -1000;
static constexpr elevation_t ELEVATION_LEVEL_TRENCH = -2000;

// order is important (it defines rendering order)
enum tile_layer_type_t {
	LAYER_LAND, // land tiles, includes edge-smoothed moisture textures
	LAYER_WATER_SURFACE, // water tiles (alpha surface with gradients)
	LAYER_WATER_SURFACE_EXTRA, // extra textures/effects for water tiles
	LAYER_WATER, // water (for non-alpha objects on water)
	LAYER_MAX
};

struct tile_vertices_t {
	types::Vec3 center;
	types::Vec3 left;
	types::Vec3 top;
	types::Vec3 right;
	types::Vec3 bottom;
};

struct tile_tex_coords_t {
	types::Vec2< float > center;
	types::Vec2< float > left;
	types::Vec2< float > top;
	types::Vec2< float > right;
	types::Vec2< float > bottom;
};

struct tile_colors_t {
	types::Color center;
	types::Color left;
	types::Color top;
	types::Color right;
	types::Color bottom;
	void Set( const types::Color& color ) {
		center.Set( color );
		left.Set( color );
		top.Set( color );
		right.Set( color );
		bottom.Set( color );
	}
	void Set( const tile_colors_t& colors ) {
		center.Set( colors.center );
		left.Set( colors.left );
		top.Set( colors.top );
		right.Set( colors.right );
		bottom.Set( colors.bottom );
	}
};

// scalar
typedef uint8_t moisture_t;
static constexpr moisture_t MOISTURE_NONE = 0;
static constexpr moisture_t MOISTURE_ARID = 1;
static constexpr moisture_t MOISTURE_MOIST = 2;
static constexpr moisture_t MOISTURE_RAINY = 3;

// scalar
typedef uint8_t rockiness_t;
static constexpr rockiness_t ROCKINESS_NONE = 0;
static constexpr rockiness_t ROCKINESS_FLAT = 1;
static constexpr rockiness_t ROCKINESS_ROLLING = 2;
static constexpr rockiness_t ROCKINESS_ROCKY = 3;

// scalar
typedef uint8_t bonus_t;
static constexpr bonus_t BONUS_NONE = 0;
static constexpr bonus_t BONUS_NUTRIENT = 1;
static constexpr bonus_t BONUS_ENERGY = 2;
static constexpr bonus_t BONUS_MINERALS = 3;

// bitflags
typedef uint16_t feature_t;
static constexpr feature_t FEATURE_NONE = 0;
static constexpr feature_t FEATURE_RIVER = 1 << 0;
static constexpr feature_t FEATURE_MONOLITH = 1 << 1;
static constexpr feature_t FEATURE_XENOFUNGUS = 1 << 2;
static constexpr feature_t FEATURE_JUNGLE = 1 << 3;
static constexpr feature_t FEATURE_URANIUM = 1 << 4;
static constexpr feature_t FEATURE_GEOTHERMAL = 1 << 5;
static constexpr feature_t FEATURE_UNITY_POD = 1 << 6;
static constexpr feature_t FEATURE_VOLCANO = 1 << 7;
static constexpr feature_t FEATURE_SUNNY_MESA = 1 << 8;
static constexpr feature_t FEATURE_GARLAND_CRATER = 1 << 9;
static constexpr feature_t FEATURE_FOSSIL_FIELD_RIDGE = 1 << 10;
static constexpr feature_t FEATURE_UNITY_ENERGY = 1 << 11;
static constexpr feature_t FEATURE_UNITY_CHOPPER = 1 << 12;
static constexpr feature_t FEATURE_UNITY_RADAR = 1 << 13;
static constexpr feature_t FEATURE_DUNES = 1 << 14;

// bitflags
typedef uint16_t terraforming_t;
static constexpr terraforming_t TERRAFORMING_NONE = 0;
static constexpr terraforming_t TERRAFORMING_ROAD = 1 << 0;
static constexpr terraforming_t TERRAFORMING_MAG_TUBE = 1 << 1;
static constexpr terraforming_t TERRAFORMING_FOREST = 1 << 2;
static constexpr terraforming_t TERRAFORMING_FARM = 1 << 3;
static constexpr terraforming_t TERRAFORMING_SOIL_ENRICHER = 1 << 4;
static constexpr terraforming_t TERRAFORMING_SOLAR = 1 << 5;
static constexpr terraforming_t TERRAFORMING_MINE = 1 << 6;
static constexpr terraforming_t TERRAFORMING_CONDENSER = 1 << 7;
static constexpr terraforming_t TERRAFORMING_MIRROR = 1 << 8;
static constexpr terraforming_t TERRAFORMING_BOREHOLE = 1 << 9;
static constexpr terraforming_t TERRAFORMING_SENSOR = 1 << 10;
static constexpr terraforming_t TERRAFORMING_BUNKER = 1 << 11;
static constexpr terraforming_t TERRAFORMING_AIRBASE = 1 << 12;

typedef std::unordered_map< std::string, size_t > yields_t;

}
}
}
}
