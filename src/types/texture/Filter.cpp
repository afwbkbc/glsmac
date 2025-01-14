#include "Filter.h"

#include "Texture.h"

namespace types {
namespace texture {

Filter::Filter( types::texture::Texture* const source_texture )
	: m_source_texture( source_texture )
	, m_width( source_texture->m_width )
	, m_height( source_texture->m_height ) {
	ASSERT_NOLOG( m_source_texture, "texture not set" );
}

Filter::~Filter() {
	if ( m_colormap ) {
		free( m_colormap );
	}
}

const size_t Filter::GetWidth() const {
	return m_width;
}

const size_t Filter::GetHeight() const {
	return m_height;
}

const Filter::texture_data_t Filter::GetTexture() {
	if ( m_colormap ) {
		auto* texture = new Texture( m_width, m_height );

		// export color map to texture
		const auto* in = m_colormap;
		auto* out = (types::Color::rgba_t*)texture->m_bitmap;
		for ( auto y = 0 ; y < m_height ; y++ ) {
			const auto yw = y * m_width;
			for ( auto x = 0 ; x < m_width ; x++ ) {
				types::Color::Convert( in[ yw + x ], out[ yw + x ] );
			}
		}

		return {
			texture,
			true,
		};
	}
	else {
		return {
			m_source_texture,
			false,
		};
	}
}

void Filter::Tint( const types::Color& color, const float intensity ) {
	ASSERT_NOLOG( intensity >= 0.0f && intensity <= 1.0f, "intensity overflow" );
	Prepare();

	const float a = 1.0f - intensity;
	const auto b = color.value * intensity;
	for ( auto y = 0 ; y < m_height ; y++ ) {
		const auto yw = y * m_width;
		for ( auto x = 0 ; x < m_width ; x++ ) {
			auto& c = m_colormap[ yw + x ];
			c = c * a + b;
		}
	}
}

void Filter::Crop( const size_t x1, const size_t y1, const size_t x2, const size_t y2 ) {
	ASSERT_NOLOG( x2 >= x1, "crop x2 less than x1" );
	ASSERT_NOLOG( y2 >= y1, "crop x2 less than x1" );
	ASSERT_NOLOG( x2 < m_width, "crop x2 overflow" );
	ASSERT_NOLOG( y2 < m_height, "crop y2 overflow" );

	const auto w = x2 - x1 + 1;
	const auto h = y2 - y1 + 1;

	Prepare();

	auto* colormap = (types::Color::color_t*)malloc( h * w * sizeof( types::Color::color_t ) );

	for ( auto y = 0 ; y < h ; y++ ) {
		memcpy( colormap + y * w, m_colormap + ( y1 + y ) * w, w * sizeof( types::Color::color_t ) );
	}

	free( m_colormap );
	m_colormap = colormap;
	m_width = w;
	m_height = h;
}

void Filter::Prepare() {
	if ( !m_colormap ) {
		m_colormap = (types::Color::color_t*)malloc( sizeof( types::Color::color_t ) * m_height * m_width );

		// export texture to color map
		const auto* in = (types::Color::rgba_t*)m_source_texture->m_bitmap;
		auto* out = m_colormap;
		for ( auto y = 0 ; y < m_height ; y++ ) {
			const auto yw = y * m_width;
			for ( auto x = 0 ; x < m_width ; x++ ) {
				types::Color::Convert( in[ yw + x ], out[ yw + x ] );
			}
		}

	}
}

}
}
