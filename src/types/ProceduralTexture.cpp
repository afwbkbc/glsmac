#include "ProceduralTexture.h"

#include <cstring>

namespace types {

ProceduralTexture::ProceduralTexture( const std::string& name, const size_t width, const size_t height )
	: Texture( name )
{
	m_width = width;
	m_height = height;
	m_bpp = 4; // always RGBA format
	m_aspect_ratio = m_height / m_width;
	
	const size_t sz = this->m_width * this->m_height * this->m_bpp;
	m_bitmap = (unsigned char*) malloc( sz );
	memset( m_bitmap, 0, sz );
}

void ProceduralTexture::SetPixel( const size_t x, const size_t y, const Color color ) {
	((Color::rgba_t*)m_bitmap)[ y * m_width + x ] = color.GetRGBA();
}

void ProceduralTexture::Rectangle( const size_t x1, const size_t y1, const size_t x2, const size_t y2, const Color color ) {
	
	auto rgba = color.GetRGBA();
	
	for (size_t x = x1 ; x < x2 ; x++) {
		for (size_t y = y1 ; y < y2 ; y++) {
			SetPixel(x, y, color);
		}
	}
}

}