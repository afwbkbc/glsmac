#pragma once

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

}
}
