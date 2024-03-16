#pragma once

#include <cstdint>

#include "InstancedSprite.h"

namespace task {
namespace game {

class Sprite {
public:
	InstancedSprite* instanced_sprite = nullptr;
	size_t next_instance_id = 1;
};

}
}
