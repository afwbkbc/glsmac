#include "Texture.h"

#include <cstring>
#include <cmath>

#include <iostream>

namespace types {

Texture::Texture( const std::string& name, const size_t width, const size_t height )
	: base::Base()
	, m_name( name )
{
	m_bpp = 4; // always RGBA format
	
	if (width > 0 && height > 0) {
		Resize( width, height );
	}
}

Texture::~Texture() {
	if ( m_bitmap ) {
		free( m_bitmap );
	}
	if ( m_renderer_object )
		m_renderer_object->Remove();
}

void Texture::Resize( const size_t width, const size_t height ) {
	
	if (m_width != width || m_height != height) {
		
		m_width = width;
		m_height = height;
	
		m_aspect_ratio = m_height / m_width;

		if ( m_bitmap ) {
			free( m_bitmap );
		}
		m_bitmap_size = this->m_width * this->m_height * this->m_bpp;
		m_bitmap = (unsigned char*) malloc( m_bitmap_size );
		memset( m_bitmap, 0, m_bitmap_size );
	}
}

void Texture::SetPixel( const size_t x, const size_t y, const Color::rgba_t& rgba ) {
	((Color::rgba_t*)m_bitmap)[ y * m_width + x ] = rgba;
}

void Texture::SetPixel( const size_t x, const size_t y, const Color& color ) {
	SetPixel( x, y, color.GetRGBA() );
}

void Texture::Rectangle( const size_t x1, const size_t y1, const size_t x2, const size_t y2, const Color color ) {
	
	auto rgba = color.GetRGBA();
	
	for (size_t x = x1 ; x < x2 ; x++) {
		for (size_t y = y1 ; y < y2 ; y++) {
			SetPixel(x, y, color);
		}
	}
}

void Texture::CopyFrom( const types::Texture* source, const size_t x1, const size_t y1, const size_t x2, const size_t y2 ) {
	
	// +1 because it's inclusive on both sides
	const size_t w = x2 - x1 + 1;
	const size_t h = y2 - y1 + 1;
	
	Resize( w, h );
	for (size_t y = 0 ; y < h ; y++) {
		for (size_t x = 0 ; x < w; x++) {
			memcpy(
				m_bitmap + ( y * m_width + x ) * m_bpp,
				source->m_bitmap + ( ( y + y1 ) * source->m_width + ( x + x1 ) ) * m_bpp,
				m_bpp
			);
		}
	}
}

void Texture::Rotate() {
	unsigned char *new_bitmap = (unsigned char*)malloc(m_bitmap_size);
	
	const size_t tmp = m_width;
	m_width = m_height;
	m_height = tmp;
	
	for (size_t y = 0 ; y < m_height ; y++) {
		for (size_t x = 0 ; x < m_width ; x++) {
			memcpy(
				new_bitmap + ( y * m_width + x ) * m_bpp,
				m_bitmap + ( x * m_height + y ) * m_bpp,
				m_bpp
			);
		}
	}
	
	free( m_bitmap );
	m_bitmap = new_bitmap;
}

void Texture::FlipV() {
	unsigned char *new_bitmap = (unsigned char*)malloc(m_bitmap_size);
	
	for (size_t y = 0 ; y < m_height ; y++) {
		for (size_t x = 0 ; x < m_width ; x++) {
			memcpy(
				new_bitmap + ( y * m_width + x ) * m_bpp,
				m_bitmap + ( ( m_height - y - 1 ) * m_width + x ) * m_bpp,
				m_bpp
			);
		}
	}
	
	free( m_bitmap );
	m_bitmap = new_bitmap;
}

void Texture::SetAlpha(const float alpha) {
	const uint8_t alpha_byte = alpha * 255;
	for (size_t y = 0 ; y < m_height ; y++) {
		for (size_t x = 0 ; x < m_width ; x++) {
			m_bitmap[ ( y * m_width + x ) * m_bpp + 3 ] = alpha_byte;
		}
	}
}

void Texture::SetContrast(const float contrast) {
	size_t i;
	for (size_t y = 0 ; y < m_height ; y++) {
		for (size_t x = 0 ; x < m_width ; x++) {
			for (size_t b = 0 ; b < 3 ; b++) {
				i = ( y * m_width + x ) * m_bpp + b;
				m_bitmap[ i ] = floor(std::fmin(255, (float)m_bitmap[i] * contrast));
			}
		}
	}
}

}
