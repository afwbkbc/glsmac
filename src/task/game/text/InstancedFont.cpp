#include "InstancedFont.h"

#include <algorithm>

#include "types/Font.h"
#include "types/texture/Texture.h"
#include "task/game/sprite/InstancedSpriteManager.h"

namespace task {
namespace game {
namespace text {

InstancedFont::InstancedFont( sprite::InstancedSpriteManager* ism, const types::Font* font )
	: m_ism( ism )
	, m_font( font )
	, m_name( font->m_name ) {

	// load font into texture

	uint32_t sym_offset = 1; // to keep antialiasing working

	unsigned int w = sym_offset;
	unsigned int h = 0;
	for ( uint8_t i = 32 ; i < 128 ; i++ ) {
		const auto& sym = m_font->m_symbols[ i ];
		w += sym.width + sym_offset;
		h = std::max( h, sym.top + sym.height + 1 );
	}

	NEW( m_texture, types::texture::Texture, "InstancedFont_" + m_name, w, sym_offset + h + sym_offset );

	unsigned int sym_x = sym_offset;
	for ( uint8_t i = 32 ; i < 128 ; i++ ) {
		const auto& sym = m_font->m_symbols[ i ];
		for ( size_t y = 0 ; y < sym.height ; y++ ) {
			for ( size_t x = 0 ; x < sym.width ; x++ ) {
				m_texture->SetPixel(
					sym_x + x,
					sym_offset + y,
					types::Color{
						1.0f,
						1.0f,
						1.0f,
						(float)( sym.data[ y * sym.width + x ] ) / 256.0f
					}
				);
			}
		}
		if ( i == 'i' || i == 'j' ) {
			int a = 5;
			a++;
		}
		m_symbol_positions.insert(
			{
				i,
				{
					{
						{
							sym_x,
							sym_offset
						},
						{
							sym.width,
							sym.height
						},
						{
							sym_x + sym.width / 2,
							sym_offset + sym.height / 2
						}
					},
					{
						{
							sym.left - (int)sym.width / 2,
							sym.top - (int)sym.height / 2
						},
						{
							sym.ax,
							sym.ay
						}
					}
				}
			}
		);
		sym_x += sym.width + sym_offset;
	}

}

InstancedFont::~InstancedFont() {

	DELETE( m_texture );
}

const std::string& InstancedFont::GetFontName() const {
	return m_name;
}

sprite::InstancedSprite* InstancedFont::GetSymbolSprite( const unsigned char symbol ) const {
	ASSERT_NOLOG( m_symbol_positions.find( symbol ) != m_symbol_positions.end(), "invalid/unsupported symbol: " + std::to_string( symbol ) );
	const auto& pos = m_symbol_positions.find( symbol )->second;
	return m_ism->GetInstancedSprite(
		"InstancedFont_" + m_name + "_sym_" + std::to_string( symbol ),
		m_texture,
		pos.src.top_left,
		pos.src.width_height,
		pos.src.center,
		{
			(float)pos.src.width_height.x * 0.01f,
			(float)pos.src.width_height.y * 0.01f
		},
		ZL_TERRAIN_TEXT
	);
}

const std::vector< types::Vec2< float > > InstancedFont::GetSymbolOffsets( const std::string& text ) const {
	std::vector< types::Vec2< float > > offsets = {};
	float total_offset = 0.0f;
	float x_center = 0.0f;
	const size_t middle = text.size() / 2 - 1;
	for ( size_t i = 0 ; i < text.size() ; i++ ) {
		const auto symbol = text.at( i );
		ASSERT_NOLOG( m_symbol_positions.find( symbol ) != m_symbol_positions.end(), "invalid/unsupported symbol: " + std::to_string( symbol ) );
		const auto& pos = m_symbol_positions.find( symbol )->second;
		offsets.push_back(
			{
				total_offset + ( pos.dst.top_left.x + pos.dst.advance.x ) * 0.01f,
				( pos.dst.top_left.y ) * 0.01f
			}
		);
		const float offset = pos.dst.advance.x * 0.01f;
		total_offset += offset;
		if ( i < middle ) {
			x_center += offset;
		}
		else if ( i == middle ) {
			x_center += offset / 2.0f;
		}
	}
	x_center = total_offset / 2.0f;
	for ( auto& offset : offsets ) {
		offset.x -= x_center;
	}
	return offsets;

/*	ASSERT_NOLOG( m_symbol_positions.find( symbol ) != m_symbol_positions.end(), "invalid/unsupported symbol: " + std::to_string( symbol ) );
	const auto& pos = m_symbol_positions.find( symbol )->second;
	return pos.top_left.x;*/
}

}
}
}
