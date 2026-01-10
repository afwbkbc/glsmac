#pragma once

#include "Mesh.h"

namespace types {
namespace mesh {

class Render;

CLASS( Simple, Mesh )

	static constexpr uint8_t VERTEX_TEXCOORD_SIZE = 2; // tx, ty
	static constexpr uint8_t VERTEX_SIZE = VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE;

	Simple( const size_t vertex_count, const size_t surface_count, const type_t type = MT_SIMPLE );

	index_t AddVertex(
		const types::Vec3& coord, const Vec2< coord_t >& tex_coord = {
		0.0f,
		0.0f
	}
	);
	index_t AddVertex(
		const Vec2< coord_t >& coord, const Vec2< coord_t >& tex_coord = {
		0.0f,
		0.0f
	}
	);

	void SetVertex( const index_t index, const types::Vec3& coord, const Vec2< coord_t >& tex_coord );
	void GetVertex( const index_t index, Vec3* coord, Vec2< coord_t >* tex_coord ) const;
	void SetVertex( const index_t index, const Vec2< coord_t >& coord, const Vec2< coord_t >& tex_coord );

	void SetVertexTexCoord( const index_t index, const Vec2< coord_t >& tex_coord );
	void GetVertexTexCoord( const index_t index, Vec2< coord_t >* tex_coord ) const;

	Render* CopyAsRenderMesh() const;
	Render* ToRenderMesh();

};

}
}
