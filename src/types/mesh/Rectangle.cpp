#include "Rectangle.h"

#include "engine/Engine.h"
#include "graphics/Graphics.h"

namespace types {
namespace mesh {

Rectangle::Rectangle()
	: Render( 4, 2, MT_RECTANGLE ) {
	Clear();
}

Rectangle::Rectangle( const Vec2< coord_t >& top_left, const Vec2< coord_t >& bottom_right, const coord_t z )
	: Render( 4, 2, MT_RECTANGLE ) {
	Clear();
	SetCoords( top_left, bottom_right, z );
}

// TODO: refactor these

void Rectangle::SetCoordsTiled( const Vec2< coord_t >& top_left, const Vec2< coord_t >& bottom_right, const Vec2< size_t >& tx, const coord_t z ) {
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
		}, m_full_tint
	);
	SetVertex(
		1, {
			bottom_right.x,
			top_left.y,
			z
		}, {
			sx,
			0
		}, m_full_tint
	);
	SetVertex(
		2, {
			bottom_right.x,
			bottom_right.y,
			z
		}, {
			sx,
			sy
		}, m_full_tint
	);
	SetVertex(
		3, {
			top_left.x,
			bottom_right.y,
			z
		}, {
			0,
			sy
		}, m_full_tint
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
	const Vec2< coord_t >& top_left,
	const Vec2< coord_t >& bottom_right,
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
		}, m_full_tint
	);
	SetVertex(
		1, {
			bottom_right.x,
			top_left.y,
			z
		}, {
			x2,
			y1
		}, m_full_tint
	);
	SetVertex(
		2, {
			bottom_right.x,
			bottom_right.y,
			z
		}, {
			x2,
			y2
		}, m_full_tint
	);
	SetVertex(
		3, {
			top_left.x,
			bottom_right.y,
			z
		}, {
			x1,
			y2
		}, m_full_tint
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

void Rectangle::SetCoords( const Vec2< coord_t >& top_left, const Vec2< coord_t >& bottom_right, const coord_t z, bool keep_tex ) {
	SetVertex(
		0, {
			top_left.x,
			bottom_right.y,
			z
		}
	);
	SetVertex(
		1, {
			bottom_right.x,
			bottom_right.y,
			z
		}
	);
	SetVertex(
		2, {
			bottom_right.x,
			top_left.y,
			z
		}
	);
	SetVertex(
		3, {
			top_left.x,
			top_left.y,
			z
		}
	);
	SetVertexTint( 0, m_full_tint );
	SetVertexTint( 1, m_full_tint );
	SetVertexTint( 2, m_full_tint );
	SetVertexTint( 3, m_full_tint );
	if ( !keep_tex ) {
		SetVertexTexCoord( 0, { 0.0f, 1.0f } );
		SetVertexTexCoord( 1, { 1.0f, 1.0f } );
		SetVertexTexCoord( 2, { 1.0f, 0.0f } );
		SetVertexTexCoord( 3, { 0.0f, 0.0f } );
	}
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

void Rectangle::GetVertex( const index_t index, Vec3* coord, Vec2< coord_t >* tex_coord ) const {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	size_t offset = index * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( coord, 0, sizeof( Vec3 ) ), ptr( m_vertex_data, offset, sizeof( Vec3 ) ), sizeof( Vec3 ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( tex_coord, 0, sizeof( Vec2< coord_t > ) ), ptr( m_vertex_data, offset, sizeof( Vec2< coord_t > ) ), sizeof( Vec2< coord_t > ) );
}

Render* Rectangle::CopyAsRenderMesh() const {
	Vec3 coord;
	Vec2< float > tex_coord;
	NEWV( render_mesh, Render, 4, 2 );
	for ( uint8_t i = 0 ; i < 4 ; i++ ) {
		GetVertex( i, &coord, &tex_coord );
		coord.y *= -1; // TODO: why?
		render_mesh->SetVertex( i, coord, tex_coord );
	}
	render_mesh->SetSurface(
		0, {
			0,
			3,
			2
		}
	);
	render_mesh->SetSurface(
		1, {
			2,
			1,
			0
		}
	);
	return render_mesh;
}

Render* Rectangle::ToRenderMesh() {
	auto* render_mesh = CopyAsRenderMesh();
	delete this;
	return render_mesh;
}

}
}
