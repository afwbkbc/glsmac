#include "MeshActor.h"

namespace scene {
namespace actor {

MeshActor::MeshActor( const std::string &name, const mesh::Mesh *mesh ) : Actor( Actor::TYPE_MESH, name ), m_mesh( mesh ) {
	m_tint_color.alpha = 0; // not visible unless some color/texture is assigned
}

MeshActor::~MeshActor() {
	delete m_mesh;
}

} /* namespace actor */
} /* namespace scene */
