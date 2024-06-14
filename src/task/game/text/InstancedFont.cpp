#include "InstancedFont.h"

#include "types/Font.h"
#include "types/texture/Texture.h"

#include "task/game/sprite/Sprite.h"

namespace task {
namespace game {
namespace text {

InstancedFont::InstancedFont( const types::Font* font )
	: m_name( font->m_name ) {

	// load font into texture
	NEW( m_texture, types::texture::Texture, "InstancedFont_" + m_name, font->m_dimensions.width, font->m_dimensions.height );
	m_texture->Fill( 0, 0, m_texture->m_width - 1, m_texture->m_height - 1, types::Color( 1.0f, 0.0f, 0.0f, 1.0f ) );

}

InstancedFont::~InstancedFont() {

	DELETE( m_texture );
}

const std::string& InstancedFont::GetFontName() const {
	return m_name;
}

}
}
}
