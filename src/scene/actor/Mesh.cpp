#include "Mesh.h"

namespace scene {
namespace actor {

Mesh::Mesh( const std::string &name, const types::Mesh *mesh ) : Actor( Actor::TYPE_MESH, name ), m_mesh( mesh ) {
	
}

Mesh::~Mesh() {
	DELETE( m_mesh );
}

const types::Mesh *Mesh::GetMesh() const {
	return m_mesh;
}

void Mesh::SetTexture( const types::Texture* texture ) {
	m_texture = texture;
}

const types::Texture* Mesh::GetTexture() {
	return m_texture;
}


} /* namespace actor */
} /* namespace scene */
