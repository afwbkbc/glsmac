#include <cstring>

#include "Simple.h"

#include "util/Math.h"
#include "Render.h"

namespace types {
namespace mesh {

Simple::Simple( const size_t vertex_count, const size_t surface_count )
	: Mesh( MT_SIMPLE, VERTEX_SIZE, vertex_count, surface_count ) {

}

index_t Simple::AddVertex( const types::Vec3& coord, const Vec2< coord_t >& tex_coord ) {
	ASSERT( !m_is_final, "addvertex on already finalized mesh" );
	ASSERT( m_vertex_i < m_vertex_count, "vertex out of bounds (" + std::to_string( m_vertex_i ) + " >= " + std::to_string( m_vertex_count ) + ")" );
	size_t offset = m_vertex_i * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof( coord ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	index_t ret = m_vertex_i;
	m_vertex_i++;
	return ret;
}

index_t Simple::AddVertex( const Vec2< coord_t >& coord, const Vec2< coord_t >& tex_coord ) {
	return AddVertex( types::Vec3( coord.x, coord.y, 0.0f ), tex_coord );
}

void Simple::SetVertex( const index_t index, const types::Vec3& coord, const Vec2< coord_t >& tex_coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	size_t offset = index * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof( coord ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	Update();
}

void Simple::GetVertex( const index_t index, Vec3* coord, Vec2< coord_t >* tex_coord ) const {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	size_t offset = index * VERTEX_SIZE * sizeof( coord_t );
	memcpy( coord, ptr( m_vertex_data, offset, sizeof( coord ) ), sizeof( *coord ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( tex_coord, ptr( m_vertex_data, offset, sizeof( tex_coord ) ), sizeof( *tex_coord ) );
}

void Simple::SetVertex( const index_t index, const Vec2< coord_t >& coord, const Vec2< coord_t >& tex_coord ) {
	SetVertex(
		index, {
			coord.x,
			coord.y,
			0.0f
		}, tex_coord
	);
}

void Simple::SetVertexTexCoord( const index_t index, const Vec2< coord_t >& tex_coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	Update();
}

void Simple::GetVertexTexCoord( const index_t index, Vec2< coord_t >* tex_coord ) const {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( tex_coord, ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( Vec2< coord_t > ) ), sizeof( Vec2< coord_t > ) );
}

Render* Simple::CopyAsRenderMesh() const {
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

Render* Simple::ToRenderMesh() {
	auto* render_mesh = CopyAsRenderMesh();
	delete this;
	return render_mesh;

}

}
}
