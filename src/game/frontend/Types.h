#pragma once

#include <vector>
#include <unordered_map>

namespace game {
namespace frontend {

namespace sprite {
class InstancedSprite;
}

typedef std::vector< sprite::InstancedSprite* > instanced_sprites_t;

enum z_level_t {
	ZL_TERRAIN,
	ZL_TERRAIN_TEXT,
	ZL_BASES,
	ZL_UNITS,
	ZL_ANIMATIONS,
};
static const float MAX_ZINDEX_ADJUSTMENT = 0.05f;

static const std::unordered_map< z_level_t, float > s_zlevel_map = {
	{
		ZL_TERRAIN,
		0.4f
	},
	{
		ZL_TERRAIN_TEXT,
		0.8f
	},
	{
		ZL_BASES,
		0.5f
	},
	{
		ZL_UNITS,
		0.6f
	},
	{
		ZL_ANIMATIONS,
		0.7f
	},
};

}
}
