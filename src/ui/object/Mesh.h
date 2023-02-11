#pragma once

#include "types/mesh/Rectangle.h"

#include "scene/actor/Mesh.h"

#include "UIObject.h"

#include "types/Color.h"
#include "types/Texture.h"

namespace ui {
namespace object {

CLASS( Mesh, UIObject )

	Mesh( const std::string& class_name = "" );
	
	void SetMesh( const types::mesh::Mesh* mesh );
	void SetTexture( const types::Texture* texture);
	void ClearTexture();
	
	virtual void Destroy();
	virtual void Draw();
	
	
protected:
	
	const types::Texture* m_texture = nullptr;
	bool m_stretch_texture = false;

	scene::actor::Mesh *m_actor = nullptr;
};

} /* namespace object */
} /* namespace ui */
