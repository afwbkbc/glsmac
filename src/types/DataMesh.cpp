#include "DataMesh.h"

#include <cmath>
#include <cstring>

#include "util/Math.h"

using namespace std;
using namespace util;

namespace types {

DataMesh::DataMesh( const size_t vertex_count, const size_t surface_count )
	: Mesh( VERTEX_SIZE, vertex_count, surface_count )
{
	ASSERT( sizeof( Mesh::coord_t ) == sizeof( data_t), "coord_t is different size from data_t, this is not supported with current DataMesh logic" );
}

Mesh::index_t DataMesh::AddVertex( const Vec3 &coord, const data_t data ) {
	ASSERT( !m_is_final, "addvertex on already finalized mesh" );
	ASSERT( m_vertex_i < m_vertex_count, "vertex out of bounds (" + to_string( m_vertex_i ) + " >= " + to_string( m_vertex_count ) + ")" );
	size_t offset = m_vertex_i * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof(coord) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( data ) ), &data, sizeof( data ) );
	index_t ret = m_vertex_i;
	m_vertex_i++;
	return ret;
}

void DataMesh::SetVertex( const index_t index, const Vec3 &coord, const data_t data ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	size_t offset = index * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof( coord ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( data ) ), &data, sizeof( data ) );
	Update();
}

void DataMesh::SetVertexData( const index_t index, const data_t data ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( data ) ), &data, sizeof( data ) );
	Update();
}

}
