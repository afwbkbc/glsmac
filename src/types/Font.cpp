#include "Font.h"

namespace types {

Font::~Font() {
	if ( m_graphics_object )
		m_graphics_object->Remove();
	for (uint8_t i = 32; i < 128 ; i++) {
		if ( m_symbols[i].data ) {
			free( m_symbols[i].data );
		}
	}
}

size_t Font::GetTextWidth(const char* text) const {
	size_t width = 0;
	for ( const char *p = text; *p; p++ ) {
		unsigned char sym = (unsigned char)*p;
		auto *bitmap = &m_symbols[sym];
		width += bitmap->width + 1; // not sure why + 1 is needed but it aligns correctly with that
	}
	return width;
}

size_t Font::GetTextHeight(const char* text) const {
	size_t height = 0;
	for ( const char *p = text; *p; p++ ) {
		unsigned char sym = (unsigned char)*p;
		auto *bitmap = &m_symbols[sym];
		if (bitmap->height > height) {
			height = bitmap->height;
		}
	}
	height++; // same
	if ( height < m_dimensions.height - 2 ) {
		height = m_dimensions.height - 2;
	}
	return height;
}

} /* namespace types */
