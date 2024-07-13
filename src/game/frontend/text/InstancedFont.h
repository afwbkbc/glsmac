#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>

#include "game/backend/map/Types.h"

#include "types/Color.h"

namespace types {
class Font;
namespace texture {
class Texture;
}
}

namespace game {
namespace frontend {

namespace sprite {
class InstancedSpriteManager;
class InstancedSprite;
}

namespace text {

class InstancedFont {
public:
	InstancedFont( sprite::InstancedSpriteManager* ism, const types::Font* font );
	~InstancedFont();

	const std::string& GetFontName() const;
	const std::vector< sprite::InstancedSprite* > GetSymbolSprites( const std::string& text, const types::Color& color, const types::Color& shadow_color );
	const std::vector< types::Vec2< float > > GetSymbolOffsets( const std::string& text ) const;

private:

	struct symbol_pos_t {
		struct {
			backend::map::pcx_texture_coordinates_t top_left;
			backend::map::pcx_texture_coordinates_t width_height;
			backend::map::pcx_texture_coordinates_t center;
		} src;
		struct {
			types::Vec2< int > top_left;
			types::Vec2< int > advance;
		} dst;
	};
	std::unordered_map< uint8_t, symbol_pos_t > m_symbol_positions = {};

	sprite::InstancedSpriteManager* m_ism = nullptr;
	const types::Font* m_font = nullptr;
	const std::string m_name = "";

	types::texture::Texture* m_base_texture = nullptr;
	std::unordered_map< types::Color::rgba_t, types::texture::Texture* > m_color_textures = {};
};

}
}
}
