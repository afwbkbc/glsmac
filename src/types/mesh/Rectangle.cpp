#include "Rectangle.h"
#include "engine/Engine.h"

namespace types {
namespace mesh {

Rectangle::Rectangle()
	: Simple( 4, 2 ) {
	Clear();
}

Rectangle::Rectangle( const Vec2< Mesh::coord_t >& top_left, const Vec2< Mesh::coord_t >& bottom_right, const coord_t z )
	: Simple( 4, 2 ) {
	Clear();
	SetCoords( top_left, bottom_right, z );
}

// TODO: refactor these

void Rectangle::SetCoordsTiled( const Vec2< Mesh::coord_t >& top_left, const Vec2< Mesh::coord_t >& bottom_right, const Vec2< size_t >& tx, const coord_t z ) {
	const float rectwidth = ( bottom_right.x - top_left.x ) / 2;
	const float rectheight = ( top_left.y - bottom_right.y ) / 2;
	const float texwidth = (float)tx.x / g_engine->GetGraphics()->GetViewportWidth();
	const float texheight = (float)tx.y / g_engine->GetGraphics()->GetViewportHeight();
	const float sx = rectwidth / texwidth;
	const float sy = rectheight / texheight;

	SetVertex(
		0, {
			top_left.x,
			top_left.y,
			z
		}, {
			0,
			0
		}
	);
	SetVertex(
		1, {
			bottom_right.x,
			top_left.y,
			z
		}, {
			sx,
			0
		}
	);
	SetVertex(
		2, {
			bottom_right.x,
			bottom_right.y,
			z
		}, {
			sx,
			sy
		}
	);
	SetVertex(
		3, {
			top_left.x,
			bottom_right.y,
			z
		}, {
			0,
			sy
		}
	);
	SetSurface(
		0, {
			0,
			3,
			2
		}
	);
	SetSurface(
		1, {
			0,
			1,
			2
		}
	);
	Update();

}

void Rectangle::SetCoords(
	const Vec2< Mesh::coord_t >& top_left,
	const Vec2< Mesh::coord_t >& bottom_right,
	const Vec2< size_t >& tx_top_left,
	const Vec2< size_t >& tx_bottom_right,
	const Vec2< size_t >& tx_wh,
	const coord_t z
) {
	const float tx = tx_wh.x + 1;
	const float ty = tx_wh.y + 1;
	const float x1 = (float)tx_top_left.x / tx;
	const float y1 = (float)tx_bottom_right.y / ty;
	const float x2 = (float)tx_bottom_right.x / tx;
	const float y2 = (float)tx_top_left.y / ty;

	SetVertex(
		0, {
			top_left.x,
			top_left.y,
			z
		}, {
			x1,
			y1
		}
	);
	SetVertex(
		1, {
			bottom_right.x,
			top_left.y,
			z
		}, {
			x2,
			y1
		}
	);
	SetVertex(
		2, {
			bottom_right.x,
			bottom_right.y,
			z
		}, {
			x2,
			y2
		}
	);
	SetVertex(
		3, {
			top_left.x,
			bottom_right.y,
			z
		}, {
			x1,
			y2
		}
	);
	SetSurface(
		0, {
			0,
			3,
			2
		}
	);
	SetSurface(
		1, {
			0,
			1,
			2
		}
	);
	Update();
}

void Rectangle::SetCoords( const Vec2< Mesh::coord_t >& top_left, const Vec2< Mesh::coord_t >& bottom_right, const coord_t z ) {
	SetVertex(
		0, {
			top_left.x,
			top_left.y,
			z
		}, {
			0.0f,
			0.0f
		}
	);
	SetVertex(
		1, {
			bottom_right.x,
			top_left.y,
			z
		}, {
			1.0f,
			0.0f
		}
	);
	SetVertex(
		2, {
			bottom_right.x,
			bottom_right.y,
			z
		}, {
			1.0f,
			1.0f
		}
	);
	SetVertex(
		3, {
			top_left.x,
			bottom_right.y,
			z
		}, {
			0.0f,
			1.0f
		}
	);
	SetSurface(
		0, {
			0,
			3,
			2
		}
	);
	SetSurface(
		1, {
			0,
			1,
			2
		}
	);
	Update();
}

}
}
