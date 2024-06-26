#include "InstancedFont.h"

#include <algorithm>

#include "types/Font.h"
#include "types/texture/Texture.h"
#include "task/game/sprite/InstancedSpriteManager.h"

namespace task {
namespace game {
namespace text {

static const types::Vec2< float > s_font_scale = {
	0.004f,
	0.005f,
};

static const types::Vec2< uint8_t > s_shadow_offsets = {
	0,
	0
};

static const types::Vec2< uint8_t > s_normal_offsets = {
	1,
	1
};

InstancedFont::InstancedFont( sprite::InstancedSpriteManager* ism, const types::Font* font )
	: m_ism( ism )
	, m_font( font )
	, m_name( font->m_name ) {

	// load font into texture

	const uint32_t sym_offset = 1; // to keep antialiasing working

	unsigned int w = sym_offset;
	unsigned int h = 0;
	for ( uint8_t i = 32 ; i < 128 ; i++ ) {
		const auto& sym = m_font->m_symbols[ i ];
		w += sym.width + sym_offset;
		h = std::max( h, sym.top + sym.height + 1 );
	}

	NEW(
		m_base_texture,
		types::texture::Texture,
		"InstancedFont_" + m_name + "_BASE",
		w + std::max( s_shadow_offsets.x, s_normal_offsets.x ),
		sym_offset + h + sym_offset + std::max( s_shadow_offsets.y, s_normal_offsets.y )
	);
	const auto f_paint_base_texture = [ this ]( const types::Vec2< uint8_t >& offsets, const types::Color& multiplier ) -> void {
		unsigned int sym_x = sym_offset;
		for ( uint8_t i = 32 ; i < 128 ; i++ ) {
			const auto& sym = m_font->m_symbols[ i ];
			for ( size_t y = 0 ; y < sym.height ; y++ ) {
				for ( size_t x = 0 ; x < sym.width ; x++ ) {
					const auto alpha = sym.data[ y * sym.width + x ];
					if ( alpha ) {
						const auto px = sym_x + x + offsets.x;
						const auto py = sym_offset + y + offsets.y;
						const auto c = types::Color::FromRGBA( m_base_texture->GetPixel( px, py ) );
						m_base_texture->SetPixel(
							px,
							py,
							types::Color{
								std::fmax( c.value.red, multiplier.value.red ),
								std::fmax( c.value.green, multiplier.value.green ),
								std::fmax( c.value.blue, multiplier.value.blue ),
								std::fmax( c.value.alpha, (float)alpha / 256.0f * multiplier.value.alpha )
							}
						);
					}
				}
			}
			sym_x += sym.width + sym_offset;
		}
	};
	f_paint_base_texture(
		s_shadow_offsets, types::Color{
			0.0f,
			0.0f,
			0.0f,
			1.0f
		}
	);
	f_paint_base_texture(
		s_normal_offsets, types::Color{
			1.0f,
			1.0f,
			1.0f,
			1.0f
		}
	);
	unsigned int sym_x = sym_offset;
	for ( uint8_t i = 32 ; i < 128 ; i++ ) {
		const auto& sym = m_font->m_symbols[ i ];
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
							sym.width + s_normal_offsets.x,
							sym.height + s_normal_offsets.y
						},
						{
							sym_x,// + sym.width / 2,
							sym_offset,// + sym.height / 2
						}
					},
					{
						{
							sym.left,
							sym.top
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
	for ( const auto& it : m_color_textures ) {
		DELETE( it.second );
	}
	DELETE( m_base_texture );
}

const std::string& InstancedFont::GetFontName() const {
	return m_name;
}

const std::vector< sprite::InstancedSprite* > InstancedFont::GetSymbolSprites( const std::string& text, const types::Color& color ) {
	std::vector< sprite::InstancedSprite* > sprites = {};
	const auto texture_key = color.GetRGBA();
	auto texture_it = m_color_textures.find( texture_key );
	if ( texture_it == m_color_textures.end() ) {
		NEWV(
			texture,
			types::texture::Texture,
			"InstancedFont_" + m_name + "_" + std::to_string( texture_key ),
			m_base_texture->m_width,
			m_base_texture->m_height
		);
		texture->ColorizeFrom( m_base_texture, color );
		texture_it = m_color_textures.insert(
			{
				texture_key,
				texture,
			}
		).first;
	}
	for ( const auto symbol : text ) {
		ASSERT_NOLOG( m_symbol_positions.find( symbol ) != m_symbol_positions.end(), "invalid/unsupported symbol: " + std::to_string( symbol ) );
		const auto& pos = m_symbol_positions.find( symbol )->second;
		sprites.push_back(
			m_ism->GetInstancedSprite(
				"InstancedFont_" + m_name + "_sym_" + std::to_string( symbol ),
				texture_it->second,
				pos.src.top_left,
				pos.src.width_height,
				pos.src.center,
				{
					(float)pos.src.width_height.x * s_font_scale.x,
					(float)pos.src.width_height.y * s_font_scale.y
				},
				ZL_TERRAIN_TEXT
			)
		);
	}
	return sprites;
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

		const float ox = (float)pos.src.width_height.x + pos.dst.top_left.x;
		const float oy = pos.dst.top_left.y;// - (float)pos.src.width_height.y / 2.0f;

		offsets.push_back(
			{
				total_offset + ox * s_font_scale.x,
				oy * s_font_scale.y
			}
		);
		const float offset = ( pos.dst.advance.x + pos.dst.top_left.x ) * s_font_scale.x;
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
}

}
}
}
