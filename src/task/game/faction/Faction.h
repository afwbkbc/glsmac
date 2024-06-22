#pragma once

#include <unordered_map>

#include "game/rules/Types.h"

#include "types/Color.h"
#include "task/game/sprite/Sprite.h"

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

namespace sprite {
class InstancedSpriteManager;
class InstancedSprite;
}

namespace faction {

class Faction {
public:
	Faction( const ::game::rules::Faction* def, sprite::InstancedSpriteManager* ism );

	sprite::Sprite* GetBaseSprite( const bool is_water, const uint8_t size, const uint8_t perimeter_level );

	const std::string m_id;

	struct {
		const types::Color text = {};
		const types::Color border = {};
	} m_colors = {};

	const bool m_is_progenitor;

private:

	sprite::InstancedSpriteManager* m_ism = nullptr;

	struct {
		::game::rules::bases_render_info_t bases_render;
	} m_render = {};

	types::texture::Texture* m_base_grid_texture = nullptr;
	types::texture::Texture* GetBaseGridTexture();

	std::unordered_map< uint8_t, sprite::Sprite > m_base_grid_sprites = {};
	sprite::Sprite m_base_sprites[6][4] = {};

};

}
}
}
