#pragma once

#include "scene/mesh/Rectangle.h"
#include "scene/actor/Mesh.h"

#include "UIObject.h"

#include "types/Color.h"
#include "types/Texture.h"

namespace ui {
namespace object {

CLASS( Surface, UIObject )

	Surface( const string& class_name = "" );
	
	void SetTexture( const types::Texture* texture);
	void ClearTexture();
	void SetStretchTexture( const bool stretch_texture );
	
	virtual void Create();
	virtual void Destroy();
	virtual void Align();
	virtual void Draw();
	
	
protected:
	
	virtual void ApplyStyle();
	
	const types::Texture* m_texture = nullptr;
	bool m_stretch_texture = false;

	scene::actor::Mesh *m_background = nullptr;
	scene::mesh::Rectangle *m_background_mesh = nullptr;
};

} /* namespace object */
} /* namespace ui */
