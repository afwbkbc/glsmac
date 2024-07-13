#pragma once

#include <cstdint>

#include "types/Vec2.h"

namespace game {
namespace backend {
namespace map {

// coordinates of textures (x1 and y1) in pcx file
typedef types::Vec2< uint32_t > pcx_texture_coordinates_t;

struct sprite_actor_t {
	std::string name = "";
	pcx_texture_coordinates_t tex_coords = {};
	float z_index = 0.5f;
};

}
}
}
