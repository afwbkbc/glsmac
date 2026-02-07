#pragma once

#include <vector>

#include "types/Vec3.h"

namespace scene {

enum scene_type_t {
	SCENE_TYPE_NONE,
	SCENE_TYPE_WORLD,
	SCENE_TYPE_UI,
};

typedef std::vector< types::Vec3 > instance_positions_t;

}
