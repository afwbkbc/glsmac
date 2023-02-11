#include "TextureLoader.h"

namespace loader {
namespace texture {

Texture* TextureLoader::LoadTextureTC( const std::string &name, const Color::rgba_t transparent_color ) {
	const Color::rgba_t color_old = m_transparent_color;
	m_transparent_color = transparent_color;
	Texture* result = LoadTexture( name );
	m_transparent_color = color_old;
	return result;
}

Texture* TextureLoader::LoadTextureTC( const std::string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const Color::rgba_t transparent_color, const uint8_t flags, const float value ) {
	const Color::rgba_t color_old = m_transparent_color;
	m_transparent_color = transparent_color;
	Texture* result = LoadTexture( name, x1, y1, x2, y2, flags, value );
	m_transparent_color = color_old;
	return result;
}

void TextureLoader::SetTransparentColor( const Color::rgba_t rgba ) {
	m_transparent_color = rgba;
	m_is_transparent_color_set = true;
}


}
}
