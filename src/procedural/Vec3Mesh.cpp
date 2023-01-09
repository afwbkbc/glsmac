#include "Vec3Mesh.h"

namespace procedural {

scene::mesh::Vec3Mesh *Vec3Mesh::Generate() {

	Log( "Generating mesh" );

	auto *mesh = new scene::mesh::Vec3Mesh();

	for ( auto vertex = m_vertices.begin() ; vertex < m_vertices.end() ; ++vertex ) {
		(*vertex)->m_index = mesh->AddVertex( (*vertex)->m_position );
		//printf("VERTEX %u %f %f %f\n", (*vertex)->m_index, (*vertex)->m_position.x, (*vertex)->m_position.y, (*vertex)->m_position.z );
	}

	for ( auto vertex = m_vertices.begin() ; vertex < m_vertices.end() ; ++vertex ) {
		//printf("VERTEX %u %u\n", (*vertex)->m_index, (*vertex)->m_out.size() );
		if ( !(*vertex)->m_out.empty() ) {
			for ( auto out = (*vertex)->m_out.begin() ; out < (*vertex)->m_out.end() - 1 ; out+=2 ) {
				//printf("ADD %u %u %u\n", (*vertex)->m_index, (*out)->m_index, (*(out+1))->m_index );
				mesh->AddIndex( (*vertex)->m_index );
				mesh->AddIndex( (*out)->m_index );
				mesh->AddIndex( (*(out+1))->m_index );
			}
		}
	}

	return mesh;
}

Vec3Mesh::~Vec3Mesh() {
	Clear();
}

void Vec3Mesh::Clear() {
	for ( auto it = m_vertices.begin() ; it < m_vertices.end() ; ++it )
		delete (*it);
	m_vertices.clear();
}

Vec3MeshVertex *Vec3Mesh::AddVertex( Vec3MeshVertex *vertex ) {
	m_vertices.push_back( vertex );
	return vertex;
}

} /* namespace procedural */
