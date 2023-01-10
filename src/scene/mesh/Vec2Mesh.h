#pragma once

#include "Mesh.h"

#include "types/Vec2.h"
#include "Vec3Mesh.h"

namespace scene {
namespace mesh {

CHILD_CLASS( Vec2Mesh, Vec3Mesh )

	Vec2Mesh() : Vec3Mesh() {};
	
	index_t AddVertex( const vec2_t &coord ) {
		return Vec3Mesh::AddVertex( vec3_t( coord.x, coord.y, 0.0 ) ); // no texture, only tint
	}
	
	index_t AddVertex( const vec2_t &coord, const vec2_t &tex_coord ) {
		return Vec3Mesh::AddVertex( vec3_t( coord.x, coord.y, 0.0 ), tex_coord ); // only texture, no tint
	}
	
};

} /* namespace mesh */
} /* namespace scene */
