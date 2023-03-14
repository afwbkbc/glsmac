#include "TextureLoader.h"

namespace loader {
namespace texture {

Texture* TextureLoader::LoadTextureTCS( const std::string &name, const transparent_colors_t transparent_colors ) {
	const transparent_colors_t colors_old = m_transparent_colors;
	m_transparent_colors = transparent_colors;
	Texture* result = LoadTexture( name );
	m_transparent_colors = colors_old;
	return result;
}

Texture* TextureLoader::LoadTextureTC( const std::string &name, const Color::rgba_t transparent_color ) {
	return LoadTextureTCS( name, { transparent_color } );
}

Texture* TextureLoader::LoadTextureTCS( const std::string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const transparent_colors_t transparent_colors, const uint8_t flags, const float value ) {
	const transparent_colors_t colors_old = m_transparent_colors;
	m_transparent_colors = transparent_colors;
	Texture* result = LoadTexture( name, x1, y1, x2, y2, flags, value );
	m_transparent_colors = colors_old;
	return result;
}

Texture* TextureLoader::LoadTextureTC( const std::string &name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const Color::rgba_t transparent_color, const uint8_t flags, const float value ) {
	return LoadTextureTCS( name, x1, y1, x2, y2, { transparent_color }, flags, value );
}

const Texture* TextureLoader::GetColorTexture( const Color& color ) {
	const Color::rgba_t c = color.GetRGBA();
	const auto& it = m_color_textures.find( c );
	if ( it != m_color_textures.end() ) {
		return it->second;
	}
	const auto* texture = Texture::FromColor( color );
	m_color_textures[ c ] = texture;
	return texture;
}

void TextureLoader::SetTransparentColor( const Color::rgba_t rgba ) {
	m_transparent_colors = { rgba };
	m_is_transparent_color_set = true;
}


}
}
