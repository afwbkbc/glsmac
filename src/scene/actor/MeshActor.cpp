#include "MeshActor.h"

namespace scene {
namespace actor {

MeshActor::MeshActor( const std::string &name, const mesh::Mesh *mesh ) : Actor( Actor::TYPE_MESH, name ), m_mesh( mesh ) {
	
}

MeshActor::~MeshActor() {
	delete m_mesh;
}

const mesh::Mesh *MeshActor::GetMesh() const {
	return m_mesh;
}

void MeshActor::SetTexture( const types::Texture* texture ) {
	m_texture = texture;
}

const types::Texture* MeshActor::GetTexture() {
	return m_texture;
}


} /* namespace actor */
} /* namespace scene */
