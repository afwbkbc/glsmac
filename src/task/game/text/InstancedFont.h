#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>

#include "game/map/Types.h"

namespace types {
class Font;
namespace texture {
class Texture;
}
}

namespace task {
namespace game {

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
	sprite::InstancedSprite* GetSymbolSprite( const unsigned char symbol ) const;
	const std::vector< types::Vec2< float > > GetSymbolOffsets( const std::string& text ) const;

private:

	struct symbol_pos_t {
		struct {
			::game::map::pcx_texture_coordinates_t top_left;
			::game::map::pcx_texture_coordinates_t width_height;
			::game::map::pcx_texture_coordinates_t center;
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

	types::texture::Texture* m_texture = nullptr;
};

}
}
}
