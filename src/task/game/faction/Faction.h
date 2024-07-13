#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "game/backend/rules/Types.h"

#include "types/Color.h"
#include "task/game/sprite/Sprite.h"

namespace types {
namespace texture {
class Texture;
}
}

namespace game::backend::rules {
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
	Faction( const ::game::backend::rules::Faction* def, sprite::InstancedSpriteManager* ism );

	sprite::Sprite* GetBaseSprite( const bool is_water, const uint8_t size, const uint8_t perimeter_level );

	const std::string m_id;

	struct {
		const types::Color text = {};
		const types::Color text_shadow = {};
		const types::Color border = {};
	} m_colors = {};

	struct {
		const std::vector< std::string > land = {};
		const std::vector< std::string > water = {};
	} m_base_names = {};

	const bool m_is_progenitor;

private:

	sprite::InstancedSpriteManager* m_ism = nullptr;

	struct {
		::game::backend::rules::bases_render_info_t bases_render;
	} m_render = {};

	types::texture::Texture* m_base_grid_texture = nullptr;
	types::texture::Texture* GetBaseGridTexture();

	std::unordered_map< uint8_t, sprite::Sprite > m_base_grid_sprites = {};
	sprite::Sprite m_base_sprites[6][4] = {};

};

}
}
}
