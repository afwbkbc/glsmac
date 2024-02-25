#pragma once

#include <string>

#include "game/map/Map.h"

namespace task {
namespace game {

class InstancedSprite {
public:
	std::string key;
	std::string name;
	::game::map::Consts::pcx_texture_coordinates_t xy;
	::game::map::Consts::pcx_texture_coordinates_t wh;
	::game::map::Consts::pcx_texture_coordinates_t cxy;
	scene::actor::Instanced* actor;
};

}
}
