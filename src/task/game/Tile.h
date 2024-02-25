#pragma once

#include <cstddef>
#include <vector>
#include <unordered_map>

#include "types/Vec2.h"

namespace task {
namespace game {

class Unit;

class Tile {
public:
	types::Vec2< size_t > coords;
	struct {
		Unit* currently_rendered_unit = nullptr;
		std::vector< Unit* > currently_rendered_fake_badges = {};
	} render;
	std::unordered_map< size_t, Unit* > units = {};
};

}
}

#include "Unit.h"
