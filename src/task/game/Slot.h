#pragma once

#include <cstddef>

#include "Sprite.h"
#include "SlotBadges.h"

#include "types/Color.h"

namespace task {
namespace game {

class Slot {
public:
	size_t slot_index = 0;
	types::Color color = {};
	std::unordered_map< ::game::unit::Unit::morale_t, SlotBadges > badges = {};
	Sprite fake_badge = {};
};

}
}
