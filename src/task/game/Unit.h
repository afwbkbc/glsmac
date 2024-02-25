#pragma once

#include <cstddef>

#include "UnitDef.h"
#include "Slot.h"
#include "Sprite.h"

#include "util/Timer.h"

namespace task {
namespace game {

class Tile;

class Unit {
public:
	size_t unit_id = 0;
	UnitDef* def = nullptr;
	Slot* slot = nullptr;
	Tile* tile = nullptr;
	struct {
		Vec3 coords = {};
		bool is_rendered = false;
		size_t instance_id = 0;
		struct {
			Sprite* def = nullptr;
			size_t instance_id = 0;
			struct {
				Sprite* def = nullptr;
				size_t instance_id = 0;
			} healthbar;
			struct {
				util::Timer timer;
			} blink;
		} badge;
		struct {
			bool is_rendered;
			size_t instance_id;
		} fake_badge;
	} render;
	bool is_active = false;
	::game::unit::Unit::movement_t movement = 0.0f;
	::game::unit::Unit::morale_t morale = 0;
	::game::unit::Unit::health_t health = 0;

};

}
}

#include "Tile.h"
