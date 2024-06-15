#pragma once

#include <string>

#include "common/Common.h"

namespace common {
class ObjectLink;
}

namespace types {

CLASS( Font, common::Class )
	virtual ~Font();

	struct dimensions_t {
		float width;
		float height;
	};

	struct bitmap_t {
		int ax; // advance.x
		int ay; // advance.y
		unsigned int width; // bitmap.width;
		unsigned int height; // bitmap.rows;
		int left; // bitmap_left;
		int top; // bitmap_top;
		unsigned char* data;
	};

	std::string m_name = "";
	bitmap_t m_symbols[128] = {};
	dimensions_t m_dimensions = {
		0.0,
		0.0
	};

	common::ObjectLink* m_graphics_object = nullptr;

	size_t GetTextWidth( const char* text ) const;
	size_t GetTextHeight( const char* text ) const;

};

}
