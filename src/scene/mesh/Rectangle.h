#pragma once

#include "Mesh.h"

using namespace types;

namespace scene {
namespace mesh {

CHILD_CLASS( Rectangle, Mesh )
	Rectangle ();
	Rectangle( const Vec2<Mesh::coord_t> &top_left, const Vec2<Mesh::coord_t> &bottom_right );
	void SetCoords( const Vec2<Mesh::coord_t> &top_left, const Vec2<Mesh::coord_t> &bottom_right );
};

} /* namespace mesh */
} /* namespace scene */
