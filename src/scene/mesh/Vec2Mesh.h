#pragma once

#include "Mesh.h"

#include "types/Vec2.h"
#include "Vec3Mesh.h"

namespace scene {
namespace mesh {

CHILD_CLASS( Vec2Mesh, Vec3Mesh )

	Vec2Mesh() : Vec3Mesh() {};
	index_t AddVertex( const vec2_t &coord, const vec2_t &tex_coord = { 0, 0 } ) {
		AddCoord( coord.x );
		AddCoord( coord.y );
		AddCoord( 0 );
		AddCoord( tex_coord.x );
		index_t ret = AddCoord( tex_coord.y );
		return ret;
	}
};

} /* namespace mesh */
} /* namespace scene */
