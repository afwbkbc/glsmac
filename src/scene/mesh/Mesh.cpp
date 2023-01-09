#include <cmath>

#include "Mesh.h"

namespace scene {
namespace mesh {

Mesh::Mesh( const uint8_t vertex_size ) : m_vertex_size ( vertex_size ), m_surface_size( 3 /* trimesh */ ) {

}

Mesh::~Mesh() {
}

Mesh::index_t Mesh::AddCoord( const coord_t coord ) {
	index_t idx = ceil( m_vertex_data.size() / m_vertex_size );
	m_vertex_data.push_back( coord );
	return idx;
}

void Mesh::AddIndex( const index_t index ) {
	if ( index >= m_vertex_data.size() )
		throw MeshError( "index out of bounds" );
	m_index_data.push_back( index );
}

void Mesh::AddSurface( const index_t v1, const index_t v2, const index_t v3 ) {
	AddIndex( v1 );
	AddIndex( v2 );
	AddIndex( v3 );
}

} /* namespace mesh */
} /* namespace scene */
