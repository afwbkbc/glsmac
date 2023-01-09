#include "MeshActor.h"

namespace scene {
namespace actor {

MeshActor::MeshActor( const std::string &name, mesh::Mesh *mesh ) : Actor( Actor::TYPE_MESH, name ), m_mesh( mesh ) {

}

MeshActor::~MeshActor() {
	delete m_mesh;
}

} /* namespace actor */
} /* namespace scene */
