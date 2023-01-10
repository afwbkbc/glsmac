#include "Texture.h"

#include <cstring>

namespace types {

Texture::Texture( const std::string& name, const size_t width, const size_t height )
	: m_name( name )
{
	m_width = width;
	m_height = height;
	m_bpp = 4; // always RGBA format
	
	if (m_width > 0 && m_height > 0) {
		Resize( m_width, m_height );
	}
}

Texture::~Texture() {
	if ( m_bitmap )
		delete m_bitmap;
	if ( m_renderer_object )
		m_renderer_object->Remove();
}

void Texture::Resize( const size_t width, const size_t height ) {
	
	if (m_width != width || m_height != height) {
		
		m_width = width;
		m_height = height;
	
		m_aspect_ratio = m_height / m_width;

		if ( m_bitmap ) {
			delete m_bitmap;
		}
		m_bitmap_size = this->m_width * this->m_height * this->m_bpp;
		m_bitmap = (unsigned char*) malloc( m_bitmap_size );
		memset( m_bitmap, 0, m_bitmap_size );
	}
}

void Texture::SetPixel( const size_t x, const size_t y, const Color& color ) {
	((Color::rgba_t*)m_bitmap)[ y * m_width + x ] = color.GetRGBA();
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
	const size_t w = x2 - x1;
	const size_t h = y2 - y1;
	
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
	
	delete m_bitmap;
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
	
	delete m_bitmap;
	m_bitmap = new_bitmap;
}

}

