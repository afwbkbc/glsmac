#pragma once

#include <string>

#include "game/map/Types.h"

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
	::game::map::pcx_texture_coordinates_t xy;
	::game::map::pcx_texture_coordinates_t wh;
	::game::map::pcx_texture_coordinates_t cxy;
	scene::actor::Instanced* actor;
};

}
}
}
