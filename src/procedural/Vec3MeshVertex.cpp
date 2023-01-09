#include "Vec3MeshVertex.h"

namespace procedural {

Vec3MeshVertex::Vec3MeshVertex( types::Vec3 position ) {
	m_position = position;
	m_index = 0;
}

void Vec3MeshVertex::ConnectToVertex( Vec3MeshVertex *vertex ) {
	m_out.push_back( vertex );
}

} /* namespace procedural */
