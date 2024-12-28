#pragma once

#include <cstring>

namespace ui {

typedef float coord_t;

struct area_t {
	coord_t left;
	coord_t right;
	coord_t top;
	coord_t bottom;
	coord_t width;
	coord_t height;
	bool operator!=( const area_t& other ) const {
		return memcmp( this, &other, sizeof( other ) ) != 0;
	}
	const bool EnlargeTo( const area_t& other ) {
		bool changed = false;
		if ( top > other.top ) {
			top = other.top;
			changed = true;
		}
		if ( left > other.left ) {
			left = other.left;
			changed = true;
		}
		if ( bottom < other.bottom ) {
			bottom = other.bottom;
			changed = true;
		}
		if ( right < other.right ) {
			right = other.right;
			changed = true;
		}
		if ( changed ) {
			width = right - left;
			height = bottom - top;
		}
		return changed;
	}
};

}
