#pragma once

/* simple rectangle surface */

#include "scene/mesh/Rectangle.h"
#include "scene/actor/MeshActor.h"

#include "UIObject.h"

#include "types/Color.h"
#include "types/Texture.h"

namespace ui {
namespace object {

CHILD_CLASS( Surface, UIObject )

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

	scene::actor::MeshActor *m_background = nullptr;
	scene::mesh::Rectangle *m_background_mesh = nullptr;
};

} /* namespace object */
} /* namespace ui */
