#pragma once

#include <vector>

#include "types/Vec3.h"

namespace scene {

enum scene_type_t {
	SCENE_TYPE_NONE,
	SCENE_TYPE_SIMPLE2D,
	SCENE_TYPE_ORTHO,
	SCENE_TYPE_ORTHO_UI, // same as ortho, but is drawn after (and on top of) overlay
	SCENE_TYPE_PERSP,
	SCENE_TYPE_UI,
};

typedef std::vector< types::Vec3 > instance_positions_t;

}
