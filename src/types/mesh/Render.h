#pragma once

#include <vector>

#include "Mesh.h"

namespace types {
namespace mesh {

CLASS( Render, Mesh )
	
	static const uint8_t VERTEX_TEXCOORD_SIZE = 2; // tx, ty
	static const uint8_t VERTEX_TINT_SIZE = 4; // rgba
	static const uint8_t VERTEX_NORMAL_SIZE = 3; // x, y, z
	static const uint8_t VERTEX_SIZE = VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE + VERTEX_NORMAL_SIZE;

	Render( const size_t vertex_count, const size_t surface_count );
	
	index_t AddVertex( const Vec3 &coord, const Vec2<coord_t> &tex_coord = { 0.0f, 0.0f }, const Color tint = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vec3 &normal = { 0.0f, 0.0f, 0.0f } );
	index_t AddVertex( const Vec2<coord_t> &coord, const Vec2<coord_t> &tex_coord = { 0.0f, 0.0f }, const Color tint = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vec3 &normal = { 0.0f, 0.0f, 0.0f } );
	
	void SetVertex( const index_t index, const Vec3 &coord, const Vec2<coord_t> &tex_coord, const Color tint = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vec3 &normal = { 0.0f, 0.0f, 0.0f } );
	void SetVertex( const index_t index, const Vec2<coord_t> &coord, const Vec2<coord_t> &tex_coord, const Color tint = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vec3 &normal = { 0.0f, 0.0f, 0.0f } );

	void SetVertexTexCoord( const index_t index, const Vec2<coord_t> &tex_coord );
	void SetVertexTint( const index_t index, const Color tint );
	void SetVertexNormal( const index_t index, const Vec3& normal );
	
	const Vec3 GetVertexNormal( const index_t index ) const;
	
	void Finalize();
	
private:
	void UpdateNormals();
	
};

}
}
