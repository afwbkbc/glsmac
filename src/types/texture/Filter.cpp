#include "Filter.h"

#include "Texture.h"

namespace types {
namespace texture {

Filter::Filter( types::texture::Texture* const source_texture )
	: m_source_texture( source_texture )
	, m_width( source_texture->GetWidth() )
	, m_height( source_texture->GetHeight() ) {
	ASSERT( m_source_texture, "texture not set" );
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
		auto* out = (types::Color::rgba_t*)texture->GetBitmap();
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
	ASSERT( intensity >= 0.0f && intensity <= 1.0f, "intensity overflow" );
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
	ASSERT( x2 >= x1, "crop x2 less than x1" );
	ASSERT( y2 >= y1, "crop x2 less than x1" );
	ASSERT( x2 < m_width, "crop x2 overflow" );
	ASSERT( y2 < m_height, "crop y2 overflow" );

	const auto w = x2 - x1 + 1;
	const auto h = y2 - y1 + 1;

	Prepare();

	auto* colormap = (types::Color::color_t*)malloc( h * w * sizeof( types::Color::color_t ) );
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = 0 ; x < w ; x++ ) {
			colormap[ y * w + x ] = m_colormap[ ( y1 + y ) * m_width + ( x1 + x ) ];
		}
	}

	free( m_colormap );
	m_colormap = colormap;
	m_width = w;
	m_height = h;
}

void Filter::Rotate() {
	Prepare();

	auto* colormap = (types::Color::color_t*)malloc( m_height * m_width * sizeof( types::Color::color_t ) );
	for ( auto y = 0 ; y < m_height ; y++ ) {
		const auto yw = y * m_width;
		for ( auto x = 0 ; x < m_width ; x++ ) {
			colormap[ x * m_height + y ] = m_colormap[ yw + x ];
		}
	}

	free( m_colormap );
	m_colormap = colormap;
	const auto newh = m_width;
	m_width = m_height;
	m_height = newh;
}

void Filter::FlipV() {
	Prepare();

	auto* colormap = (types::Color::color_t*)malloc( m_height * m_width * sizeof( types::Color::color_t ) );
	for ( auto y = 0 ; y < m_height ; y++ ) {
		memcpy( colormap + y * m_width, m_colormap + ( m_height - y - 1 ) * m_width, m_width * sizeof( types::Color::color_t ) );
	}

	free( m_colormap );
	m_colormap = colormap;
}

void Filter::FlipH() {
	Prepare();

	auto* colormap = (types::Color::color_t*)malloc( m_height * m_width * sizeof( types::Color::color_t ) );
	for ( auto y = 0 ; y < m_height ; y++ ) {
		const auto yw = y * m_width;
		for ( auto x = 0 ; x < m_width ; x++ ) {
			colormap[ yw + x ] = m_colormap[ yw + ( m_width - x - 1 ) ];
		}
	}

	free( m_colormap );
	m_colormap = colormap;
}

void Filter::Contrast( const float value ) {
	ASSERT( value >= 0.0f && value <= 10.0f, "contrast overflow" );

	Prepare();

	auto* colormap = (types::Color::color_t*)malloc( m_height * m_width * sizeof( types::Color::color_t ) );
	for ( auto y = 0 ; y < m_height ; y++ ) {
		const auto yw = y * m_width;
		for ( auto x = 0 ; x < m_width ; x++ ) {
			const auto xy = yw + x;
			const auto& c = m_colormap[ xy ];
			colormap[ xy ] = types::Color::color_t{
				c.red * value,
				c.green * value,
				c.blue * value,
				c.alpha,
			};
		}
	}

	free( m_colormap );
	m_colormap = colormap;
}

void Filter::Prepare() {
	if ( !m_colormap ) {
		m_colormap = (types::Color::color_t*)malloc( sizeof( types::Color::color_t ) * m_height * m_width );

		// export texture to color map
		const auto* in = (types::Color::rgba_t*)m_source_texture->GetBitmap();
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
