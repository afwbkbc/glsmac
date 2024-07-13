#pragma once

#include <string>

#include "game/backend/map/Types.h"

namespace scene::actor {
class Instanced;
}

namespace game {
namespace frontend {
namespace sprite {

class InstancedSprite {
public:
	std::string key;
	std::string name;
	backend::map::pcx_texture_coordinates_t xy;
	backend::map::pcx_texture_coordinates_t wh;
	backend::map::pcx_texture_coordinates_t cxy;
	scene::actor::Instanced* actor;
};

}
}
}
