#pragma once

#include "../Mesh.h"

namespace types {
namespace mesh {

CLASS( Rectangle, Mesh )
	Rectangle ();
	Rectangle( const Vec2<Mesh::coord_t> &top_left, const Vec2<Mesh::coord_t> &bottom_right, const coord_t z );
	
	void SetCoords( const Vec2<Mesh::coord_t> &top_left, const Vec2<Mesh::coord_t> &bottom_right, const Vec2<size_t> &tx, const coord_t z );
	void SetCoords( const Vec2<Mesh::coord_t> &top_left, const Vec2<Mesh::coord_t> &bottom_right, const coord_t z );
};

}
}