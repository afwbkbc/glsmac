#pragma once

#include "Render.h"

namespace types {
namespace mesh {

CLASS( Rectangle, Render )
	Rectangle();
	Rectangle( const Vec2< coord_t >& top_left, const Vec2< coord_t >& bottom_right, const coord_t z );

	void SetCoords(
		const Vec2< coord_t >& top_left,
		const Vec2< coord_t >& bottom_right,
		const Vec2< size_t >& tx_top_left,
		const Vec2< size_t >& tx_bottom_right,
		const Vec2< size_t >& tx_wh,
		const coord_t z
	);
	void SetCoordsTiled(
		const Vec2< coord_t >& top_left,
		const Vec2< coord_t >& bottom_right,
		const Vec2< size_t >& tx,
		const coord_t z
	);
	void SetCoords( const Vec2< coord_t >& top_left, const Vec2< coord_t >& bottom_right, const coord_t z, bool keep_tex = false );

	void GetVertex( const index_t index, Vec3* coord, Vec2< coord_t >* tex_coord ) const;

	Render* CopyAsRenderMesh() const;
	Render* ToRenderMesh();

private:
	const types::Color m_full_tint = { 1.0f, 1.0f, 1.0f, 1.0f };
};

}
}
