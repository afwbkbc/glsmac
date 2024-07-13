#pragma once

#include <string>

#include "game/backend/map/Types.h"

namespace scene::actor {
class Instanced;
}

namespace task {
namespace game {
namespace sprite {

class InstancedSprite {
public:
	std::string key;
	std::string name;
	::game::backend::map::pcx_texture_coordinates_t xy;
	::game::backend::map::pcx_texture_coordinates_t wh;
	::game::backend::map::pcx_texture_coordinates_t cxy;
	scene::actor::Instanced* actor;
};

}
}
}
