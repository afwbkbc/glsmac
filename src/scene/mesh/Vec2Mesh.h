#pragma once

#include "Mesh.h"

#include "types/Vec2.h"

namespace scene {
namespace mesh {

MINOR_CLASS( Vec2, Mesh )
	typedef types::Vec2<Mesh::coord_t> vec2_t;

	Vec2Mesh() : Mesh( 2 ) {};
	index_t AddVertex( const vec2_t &coord ) {
		AddCoord( coord.x );
		return AddCoord( coord.y );
	}
};

} /* namespace mesh */
} /* namespace scene */
