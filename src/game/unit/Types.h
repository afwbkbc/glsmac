#pragma once

#include <string>
#include <cstdint>

namespace game {
namespace unit {

enum def_type_t {
	DT_STATIC,
};

enum movement_type_t {
	MT_IMMOVABLE,
	MT_LAND,
	MT_WATER,
	MT_AIR
};

typedef float movement_t;

typedef float health_t;

// must be hardcoded for now because unit badge sprites are
typedef uint8_t morale_t;
static constexpr morale_t MORALE_MIN = 0;
static constexpr morale_t MORALE_MAX = 6;

struct sprite_render_info_t {
	std::string file;
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
	uint32_t cx;
	uint32_t cy;
	uint32_t morale_based_xshift;
};

}
}
