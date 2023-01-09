#pragma once

#include "Actor.h"

#include "../mesh/Mesh.h"
#include "types/Color.h"

namespace scene {
namespace actor {

class MeshActor : public Actor {
public:
	MeshActor( const std::string &name, mesh::Mesh *mesh );
	~MeshActor();
	mesh::Mesh *GetMesh() const {
		return m_mesh;
	}
	void SetTintColor( const types::Color &tint_color ) { m_tint_color = tint_color; }
	const types::Color & GetTintColor() { return m_tint_color; }
protected:
	mesh::Mesh *m_mesh;

	types::Color m_tint_color;
};

} /* namespace scene */
} /* namespace actor */
