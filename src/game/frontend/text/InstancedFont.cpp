#include "InstancedFont.h"

#include <algorithm>

#include "types/Font.h"
#include "types/texture/Texture.h"
#include "game/frontend/sprite/InstancedSpriteManager.h"

namespace game {
namespace frontend {
namespace text {

static const types::Vec2< float > s_font_scale = {
	0.003f,
	0.004f,
};

const std::vector< float > s_shadow_alpha_levels = {
	1.0f,
	1.0f,
	0.5f,
	0.25f,
};

InstancedFont::InstancedFont( sprite::InstancedSpriteManager* ism, const types::Font* font )
	: m_ism( ism )
	, m_font( font )
	, m_name( font->m_name ) {

	// load font into texture

	const uint8_t sym_offset = s_shadow_alpha_levels.size() * 2 + 1; // to keep antialiasing working
	const uint8_t shadow_offset = s_shadow_alpha_levels.size();

	unsigned int w = sym_offset + shadow_offset;
	unsigned int h = 0;
	for ( uint8_t i = 32 ; i < 128 ; i++ ) {
		const auto& sym = m_font->m_symbols[ i ];
		w += sym.width + sym_offset;
		h = std::max( h, sym.top + sym.height + sym_offset );
	}

	NEW(
		m_base_texture,
		types::texture::Texture,
		"InstancedFont_" + m_name + "_BASE",
		w + sym_offset + shadow_offset,
		h + ( sym_offset + shadow_offset ) * 2
	);
	const auto f_paint_base_texture = [ this, sym_offset ]( const types::Vec2< uint8_t >& offsets, const types::Color& multiplier ) -> void {
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
	for ( uint8_t i = 0 ; i < shadow_offset ; i++ ) {
		const auto& c = types::Color{
			0.0f,
			0.0f,
			0.0f,
			s_shadow_alpha_levels.at( i )
		};
		const uint8_t before = shadow_offset - i - 1;
		const uint8_t after = shadow_offset + i + 1;
		std::vector< types::Vec2< uint8_t > > offsets = {
			{
				before,
				shadow_offset,
			},
			{
				shadow_offset,
				before,
			},
			{
				after,
				shadow_offset,
			},
			{
				shadow_offset,
				after,
			},
			{
				before,
				before,
			},
			{
				after,
				before,
			},
			{
				after,
				after,
			},
			{
				before,
				after,
			},
		};
		for ( const auto& o : offsets ) {
			f_paint_base_texture( o, c );
		}
	}
	f_paint_base_texture(
		{
			shadow_offset,
			shadow_offset,
		}, types::Color{
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
							sym.width + sym_offset,
							sym.height + sym_offset
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

const std::vector< sprite::InstancedSprite* > InstancedFont::GetSymbolSprites( const std::string& text, const types::Color& color, const types::Color& shadow_color ) {
	std::vector< sprite::InstancedSprite* > sprites = {};
	auto* texture = GetColorizedTexture( color, shadow_color );
	for ( const auto symbol : text ) {
		ASSERT( m_symbol_positions.find( symbol ) != m_symbol_positions.end(), "invalid/unsupported symbol: " + std::to_string( symbol ) );
		const auto& pos = m_symbol_positions.find( symbol )->second;
		sprites.push_back(
			m_ism->GetInstancedSprite(
				"InstancedFont_" + m_name + "_sym_" + std::to_string( symbol ),
				texture,
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
		ASSERT( m_symbol_positions.find( symbol ) != m_symbol_positions.end(), "invalid/unsupported symbol: " + std::to_string( symbol ) );
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

static size_t s_text_texture_id = 1;
types::texture::Texture* InstancedFont::GetTextTexture(
	const std::string& text,
	const types::Color& background,
	const types::Color& foreground,
	const types::Color& shadow,
	const uint8_t margin
) {

	uint32_t w = 0;
	uint32_t h = 0;
	for ( const auto symbol : text ) {
		ASSERT( m_symbol_positions.find( symbol ) != m_symbol_positions.end(), "invalid/unsupported symbol: " + std::to_string( symbol ) );
		const auto& pos = m_symbol_positions.find( symbol )->second;
		w += pos.src.width_height.x;
		h = std::max( h, pos.src.width_height.y );
	}
	w += margin * 2 + 1; // some symbols like '1' or '4' look shifted to the left without + 1 // TODO: investigate
	h += margin * 2;

	NEWV( texture, types::texture::Texture, "Texture_" + m_name + "_" + std::to_string( s_text_texture_id++ ), w, h );

	texture->Fill( 0, 0, texture->GetWidth() - 1, texture->GetHeight() - 1, background );

	uint32_t x = margin;
	uint32_t y = margin;
	const auto* source_texture = GetColorizedTexture( foreground, shadow );
	for ( const auto symbol : text ) {
		const auto& pos = m_symbol_positions.find( symbol )->second;
		texture->AddFrom( source_texture, types::texture::AM_MERGE, pos.src.top_left.x, pos.src.top_left.y, pos.src.top_left.x + pos.src.width_height.x - 1, pos.src.top_left.y + pos.src.width_height.y - 1, x, y );
		x += pos.src.width_height.x;
	}

	return texture;
}

types::texture::Texture* InstancedFont::GetColorizedTexture( const types::Color& color, const types::Color& shadow_color ) {
	const auto texture_key = color.GetRGBA();
	auto texture_it = m_color_textures.find( texture_key );
	if ( texture_it == m_color_textures.end() ) {
		NEWV(
			texture,
			types::texture::Texture,
			"InstancedFont_" + m_name + "_" + std::to_string( texture_key ),
			m_base_texture->GetWidth(),
			m_base_texture->GetHeight()
		);
		texture->ColorizeFrom( m_base_texture, color, shadow_color );
		texture_it = m_color_textures.insert(
			{
				texture_key,
				texture,
			}
		).first;
	}
	return texture_it->second;
}

}
}
}
