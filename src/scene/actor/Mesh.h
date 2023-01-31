#pragma once

#include "Actor.h"

#include "types/Mesh.h"
#include "types/Color.h"
#include "types/Texture.h"

namespace scene {
namespace actor {

CLASS( Mesh, Actor )
	Mesh( const std::string &name, const types::Mesh *mesh );
	~Mesh();
	const types::Mesh *GetMesh() const;
	void SetTexture( const types::Texture* texture );
	const types::Texture* GetTexture();
protected:
	const types::Mesh *m_mesh;

	const types::Texture* m_texture = nullptr;
	const bool m_is_texture_tiled = false;
};

} /* namespace scene */
} /* namespace actor */
