#pragma once

#include <vector>

#include "Mesh.h"

namespace types {
namespace mesh {

CLASS( Render, Mesh )

	static constexpr uint8_t VERTEX_TEXCOORD_SIZE = 2; // tx, ty
	static constexpr uint8_t VERTEX_TINT_SIZE = 4; // rgba
	static constexpr uint8_t VERTEX_NORMAL_SIZE = 3; // x, y, z
	static constexpr uint8_t VERTEX_SIZE = VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE + VERTEX_NORMAL_SIZE;

	Render( const size_t vertex_count, const size_t surface_count );

	index_t AddVertex(
		const types::Vec3& coord, const Vec2< coord_t >& tex_coord = {
		0.0f,
		0.0f
	}, const Color tint = {
		1.0f,
		1.0f,
		1.0f,
		1.0f
	}, const types::Vec3& normal = {
		0.0f,
		0.0f,
		0.0f
	}
	);
	index_t AddVertex(
		const Vec2< coord_t >& coord, const Vec2< coord_t >& tex_coord = {
		0.0f,
		0.0f
	}, const Color tint = {
		1.0f,
		1.0f,
		1.0f,
		1.0f
	}, const types::Vec3& normal = {
		0.0f,
		0.0f,
		0.0f
	}
	);

	void SetVertex(
		const index_t index, const types::Vec3& coord, const Vec2< coord_t >& tex_coord, const Color tint = {
		1.0f,
		1.0f,
		1.0f,
		1.0f
	}, const types::Vec3& normal = {
		0.0f,
		0.0f,
		0.0f
	}
	);
	void SetVertex(
		const index_t index, const Vec2< coord_t >& coord, const Vec2< coord_t >& tex_coord, const Color tint = {
		1.0f,
		1.0f,
		1.0f,
		1.0f
	}, const types::Vec3& normal = {
		0.0f,
		0.0f,
		0.0f
	}
	);

	void SetVertexTexCoord( const index_t index, const Vec2< coord_t >& tex_coord );
	void SetVertexTint( const index_t index, const Color tint );
	void SetVertexNormal( const index_t index, const types::Vec3& normal );

	void GetVertexTexCoord( const index_t index, Vec2< coord_t >* tex_coord ) const;
	const types::Vec3 GetVertexNormal( const index_t index ) const;

	void CombineNormals( const std::vector< index_t >& indices );

	void Finalize() override;
	void UpdateNormals( const std::vector< surface_id_t >& surfaces );
	void UpdateAllNormals();

	static Render* Rectangle(
		float w = 0.5f, float h = 0.5f, const tex_coords_t tex_coords = {
		{ 0.0f, 1.0f },
		{ 1.0f, 0.0f }
	}
	); // TODO: fix inverse y

};

}
}
