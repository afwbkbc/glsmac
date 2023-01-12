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

	void SetTexture( const types::Texture* texture) {
		m_texture = texture;
	}
	
	void SetSizeFromTexture() {
		SetWidth(m_texture->m_width);
		SetHeight(m_texture->m_height);
	}

	virtual void Create() {
		UIObject::Create();

		m_background_mesh = new scene::mesh::Rectangle();
		m_background = new scene::actor::MeshActor( "UI::Surface", m_background_mesh );
		m_actors.push_back( m_background );

	}
	virtual void Destroy() {

		delete m_background;

		UIObject::Destroy();
	}
	virtual void Align() {
		UIObject::Align();

		m_background_mesh->SetCoords(
			{ ClampX( m_object_area.left ), ClampY( m_object_area.top ) },
			{ ClampX( m_object_area.right ), ClampY( m_object_area.bottom ) }
		);
	}
	virtual void Draw() {
		UIObject::Draw();

		if ( m_texture ) {
			m_background->SetTexture( m_texture );
		}
	}
protected:
	const types::Texture* m_texture = nullptr;

	scene::actor::MeshActor *m_background = nullptr;
	scene::mesh::Rectangle *m_background_mesh = nullptr;
};

} /* namespace object */
} /* namespace ui */
