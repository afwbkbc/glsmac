#include <cmath>
#include <cstdint>

#include "Font.h"

#include "common/ObjectLink.h"

namespace types {

Font::Font( const std::string& name )
	: m_name( name ) {
	//
}

Font::~Font() {
	if ( m_graphics_object ) {
		m_graphics_object->Remove();
	}
	for ( uint8_t i = 32 ; i < 128 ; i++ ) {
		if ( m_symbols[ i ].data ) {
			free( m_symbols[ i ].data );
		}
	}
}

size_t Font::GetTextWidth( const char* text ) const {
	size_t width = 0;
	for ( const char* p = text ; *p ; p++ ) {
		unsigned char sym = (unsigned char)*p;
		auto* bitmap = &m_symbols[ sym ];
		width += bitmap->ax;
	}
	return width;
}

size_t Font::GetTextHeight( const char* text ) const {
	size_t height = 0;
	for ( const char* p = text ; *p ; p++ ) {
		unsigned char sym = (unsigned char)*p;
		auto* bitmap = &m_symbols[ sym ];
		const auto h = bitmap->height + bitmap->ay;
		if ( h > height ) {
			height = h;
		}
	}
	if ( height < m_dimensions.height ) {
		height = m_dimensions.height - 2; // TODO: why?
	}
	return height;
}

}
