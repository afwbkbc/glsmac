#include <cstring>

#include "Data.h"

#include "util/Math.h"

namespace types {
namespace mesh {

Data::Data( const size_t vertex_count, const size_t surface_count )
	: Mesh( MT_DATA, VERTEX_SIZE, vertex_count, surface_count ) {
	ASSERT( sizeof( Mesh::coord_t ) == sizeof( data_t ), "coord_t is different size from data_t, this is not supported with current Data logic" );
}

Mesh::index_t Data::AddVertex( const Vec3& coord, const data_t data ) {
	ASSERT( !m_is_final, "addvertex on already finalized mesh" );
	ASSERT( m_vertex_i < m_vertex_count, "vertex out of bounds (" + std::to_string( m_vertex_i ) + " >= " + std::to_string( m_vertex_count ) + ")" );
	size_t offset = m_vertex_i * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof( coord ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( data ) ), &data, sizeof( data ) );
	index_t ret = m_vertex_i;
	m_vertex_i++;
	return ret;
}

void Data::SetVertex( const index_t index, const Vec3& coord, const data_t data ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	size_t offset = index * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof( coord ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( data ) ), &data, sizeof( data ) );
	Update();
}

void Data::SetVertexData( const index_t index, const data_t data ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( data ) ), &data, sizeof( data ) );
	Update();
}

}
}
