#include "InstancedFont.h"

#include <algorithm>

#include "types/Font.h"
#include "types/texture/Texture.h"
#include "task/game/sprite/InstancedSpriteManager.h"
#include "engine/Engine.h"
#include "graphics/Graphics.h"

namespace task {
namespace game {
namespace text {

InstancedFont::InstancedFont( sprite::InstancedSpriteManager* ism, const types::Font* font )
	: m_ism( ism )
	, m_font( font )
	, m_name( font->m_name ) {

	// load font into texture

	unsigned int w = 0;
	unsigned int h = 0;
	for ( uint8_t i = 32 ; i < 128 ; i++ ) {
		const auto& sym = m_font->m_symbols[ i ];
		w += sym.width;
		h = std::max( h, sym.top + sym.height + 1 );
	}

	NEW( m_texture, types::texture::Texture, "InstancedFont_" + m_name, w, h );

	float cx = 0;
	float cy = 0;
	unsigned int sym_x = 0;
	for ( uint8_t i = 32 ; i < 128 ; i++ ) {
		const auto& sym = m_font->m_symbols[ i ];
		const int sym_y = i == 'a' || i == 'b'
			? 1 + cy + sym.top
			: 0;//1 + sym.top;
		for ( size_t y = 0 ; y < sym.height ; y++ ) {
			for ( size_t x = 0 ; x < sym.width ; x++ ) {
				m_texture->SetPixel(
					sym_x + sym.left + x,
					sym_y + y,
					types::Color{
						1.0f,
						1.0f,
						1.0f,
						(float)( sym.data[ y * sym.width + x ] ) / 256.0f
					}
				);
			}
		}
		cx += sym.ax;
		cy += sym.ay;
		sym_x += sym.left + sym.width;
	}

	//m_texture->SetPixel()

	//m_texture->Fill( 0, 0, m_texture->m_width - 1, m_texture->m_height - 1, types::Color( 1.0f, 0.0f, 0.0f, 1.0f ) );

}

InstancedFont::~InstancedFont() {

	DELETE( m_texture );
}

const std::string& InstancedFont::GetFontName() const {
	return m_name;
}

sprite::InstancedSprite* InstancedFont::GetSymbolSprite( const char symbol ) const {
	return m_ism->GetInstancedSprite(
		"InstancedFont_" + m_name + "_sym_" + std::to_string( symbol ),
		m_texture,
		{
			0,
			0
		},
		{
			(uint32_t)m_texture->m_width,
			(uint32_t)m_texture->m_height,
		},
		{
			25,
			25
		},
		{
			12.0f,
			1.0f
		},
		ZL_TERRAIN_TEXT
	);
}

}
}
}
