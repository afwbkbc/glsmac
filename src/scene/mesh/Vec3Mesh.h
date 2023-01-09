#pragma once

#include "Mesh.h"

#include "types/Vec3.h"

namespace scene {
namespace mesh {

MINOR_CLASS( Vec3, Mesh )
	typedef types::Vec3 vec3_t;

	Vec3Mesh() : Mesh( 3 ) {};
	index_t AddVertex( const vec3_t &coord ) {
		AddCoord( coord.x );
		AddCoord( coord.y );
		return AddCoord( coord.z );
	}
};

} /* namespace mesh */
} /* namespace scene */
