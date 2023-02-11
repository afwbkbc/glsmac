#include <cmath>
#include <cstring>

#include "Simple.h"

#include "util/Math.h"

namespace types {
namespace mesh {

Simple::Simple( const size_t vertex_count, const size_t surface_count )
	: Mesh( MT_SIMPLE, VERTEX_SIZE, vertex_count, surface_count )
{
	
}

Simple::index_t Simple::AddVertex( const Vec3 &coord, const Vec2<coord_t> &tex_coord ) {
	ASSERT( !m_is_final, "addvertex on already finalized mesh" );
	ASSERT( m_vertex_i < m_vertex_count, "vertex out of bounds (" + std::to_string( m_vertex_i ) + " >= " + std::to_string( m_vertex_count ) + ")" );
	size_t offset = m_vertex_i * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof(coord) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	index_t ret = m_vertex_i;
	m_vertex_i++;
	return ret;
}
Simple::index_t Simple::AddVertex( const Vec2<coord_t> &coord, const Vec2<coord_t> &tex_coord ) {
	return AddVertex( Vec3( coord.x, coord.y, 0.0f ), tex_coord );
}

void Simple::SetVertex( const index_t index, const Vec3 &coord, const Vec2<coord_t> &tex_coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	size_t offset = index * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof( coord ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	Update();
}
void Simple::SetVertex( const index_t index, const Vec2<coord_t> &coord, const Vec2<coord_t> &tex_coord ) {
	SetVertex( index, { coord.x, coord.y, 0.0f }, tex_coord );
}

void Simple::SetVertexTexCoord( const index_t index, const Vec2<coord_t> &tex_coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	Update();
}

void Simple::GetVertexTexCoord( const index_t index, Vec2<coord_t>* coord ) const {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( coord, ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( coord ) ), sizeof( coord ) );
}

}
}
