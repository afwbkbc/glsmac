#pragma once

#include <string>

#include "base/ObjectLink.h"

namespace types {

class Font {
public:
	~Font() {
		if ( m_renderer_object )
			m_renderer_object->Remove();
		for (uint8_t i = 32; i < 128 ; i++) {
			delete m_symbols[i].data;
		}
	}

	typedef struct dimensions_t {
		float width;
		float height;
	} dimensions_t;

	typedef struct bitmap_t {
		int ax; // advance.x
		int ay; // advance.y
		unsigned int width; // bitmap.width;
		unsigned int height; // bitmap.rows;
		int left; // bitmap_left;
		int top; // bitmap_top;
		unsigned char *data;
	} bitmap_t;

	std::string m_name;
	bitmap_t m_symbols[128];
	dimensions_t m_dimensions;

	base::ObjectLink *m_renderer_object = NULL;
	
	size_t GetTextWidth(const char* text) const {
		size_t width = 0;
		for ( const char *p = text; *p; p++ ) {
			unsigned char sym = (unsigned char)*p;
			auto *bitmap = &m_symbols[sym];
			width += bitmap->width;
		}
		return width;
	}
	size_t GetTextHeight(const char* text) const {
		size_t height = 0;
		for ( const char *p = text; *p; p++ ) {
			unsigned char sym = (unsigned char)*p;
			auto *bitmap = &m_symbols[sym];
			if (bitmap->height > height) {
				height = bitmap->height;
			}
		}
		return height;
	}

};

} /* namespace types */
