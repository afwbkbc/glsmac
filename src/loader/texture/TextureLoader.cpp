#include "TextureLoader.h"

namespace loader {
namespace texture {

Texture* TextureLoader::LoadTexture( const std::string &name, const Color::rgba_t transparent_color ) {
	const Color::rgba_t color_old = m_transparent_color;
	m_transparent_color = transparent_color;
	Texture* result = LoadTexture( name );
	m_transparent_color = color_old;
	return result;
}

void TextureLoader::SetTransparentColor( const Color::rgba_t rgba ) {
	m_transparent_color = rgba;
	m_is_transparent_color_set = true;
}


}
}
