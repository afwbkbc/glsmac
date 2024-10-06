#pragma once

#include <cstddef>

namespace game {
namespace frontend {
namespace sprite {

class InstancedSprite;

class Sprite {
public:
	InstancedSprite* instanced_sprite = nullptr;
	size_t next_instance_id = 1;
};

}
}
}
