#pragma once

#include <unordered_map>

#include "game/rules/Types.h"

#include "types/Color.h"
#include "task/game/Sprite.h"

namespace types {
namespace texture {
class Texture;
}
}

namespace game::rules {
class Faction;
}

namespace task {
namespace game {

class InstancedSpriteManager;
class InstancedSprite;

namespace faction {

class Faction {
public:
	Faction( const ::game::rules::Faction* def, InstancedSpriteManager* ism );

	Sprite* GetBaseSprite( const bool is_water, const uint8_t size, const uint8_t perimeter_level );

	const std::string m_id;
	const types::Color m_border_color;
	const bool m_is_progenitor;

private:

	InstancedSpriteManager* m_ism = nullptr;

	struct {
		::game::rules::bases_render_info_t bases_render;
	} m_render = {};

	types::texture::Texture* m_base_grid_texture = nullptr;
	types::texture::Texture* GetBaseGridTexture();

	std::unordered_map< uint8_t, Sprite > m_base_grid_sprites = {};
	Sprite m_base_sprites[6][4] = {};

};

}
}
}
