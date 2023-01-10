#pragma once

#include "Actor.h"

#include "../mesh/Mesh.h"
#include "types/Color.h"
#include "types/Texture.h"

namespace scene {
namespace actor {

class MeshActor : public Actor {
public:
	MeshActor( const std::string &name, const mesh::Mesh *mesh );
	~MeshActor();
	const mesh::Mesh *GetMesh() const {
		return m_mesh;
	}
	void SetTintColor( const types::Color &tint_color ) {
		m_tint_color = tint_color;
	}
	void SetTexture( const types::Texture* texture ) {
		m_texture = texture;
	}
	const types::Color & GetTintColor() {
		return m_tint_color;
	}
	const types::Texture* GetTexture() {
		return m_texture;
	}
protected:
	const mesh::Mesh *m_mesh;

	types::Color m_tint_color;
	const types::Texture* m_texture = nullptr;
};

} /* namespace scene */
} /* namespace actor */
