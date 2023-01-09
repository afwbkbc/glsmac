#pragma once

#include "../Vec2Mesh.h"

namespace scene {
namespace mesh {
namespace vec2 {

CHILD_CLASS( Rectangle, Vec2Mesh )
	Rectangle(){};
	Rectangle ( const vec2_t &top_left, const vec2_t &bottom_right ) {
		SetCoords( top_left, bottom_right );
	}
	void SetCoords( const vec2_t &top_left, const vec2_t &bottom_right ) {
		SetCoords( top_left.x, top_left.y, bottom_right.x, bottom_right.y );
	}
	void SetCoords( const coord_t left, const coord_t top, const coord_t right, const coord_t bottom ) {
		Clear();
		auto v_top_left = AddVertex( vec2_t( left, top ) );
		auto v_top_right = AddVertex( vec2_t( right, top ) );
		auto v_bottom_right = AddVertex( vec2_t( right, bottom ) );
		auto v_bottom_left = AddVertex( vec2_t( left, bottom ) );
		AddSurface( v_top_left, v_top_right, v_bottom_right );
		AddSurface( v_top_left, v_bottom_left, v_bottom_right );
		Update();
	}
};

} /* namespace vec2 */
} /* namespace mesh */
} /* namespace scene */
