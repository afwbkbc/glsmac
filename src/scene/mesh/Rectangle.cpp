#include "Rectangle.h"

namespace scene {
namespace mesh {

Rectangle::Rectangle()
	: Mesh( 4, 2 )
{
	Clear();
}

Rectangle::Rectangle( const Vec2<Mesh::coord_t> &top_left, const Vec2<Mesh::coord_t> &bottom_right, const coord_t z )
	: Mesh( 4, 2 )
{
	Clear();
	SetCoords( top_left, bottom_right, z );
}

void Rectangle::SetCoords( const Vec2<Mesh::coord_t> &top_left, const Vec2<Mesh::coord_t> &bottom_right, const coord_t z )
{
	SetVertex( 0, { top_left.x, top_left.y, z }, { 0, 0 } );
	SetVertex( 1, { bottom_right.x, top_left.y, z }, { 1, 0 } );
	SetVertex( 2, { bottom_right.x, bottom_right.y, z }, { 1, 1 } );
	SetVertex( 3, { top_left.x, bottom_right.y, z }, { 0, 1 } );
	SetSurface( 0, { 0, 3, 2 } );
	SetSurface( 1, { 0, 1, 2 } );
	Update();
}

}
}
