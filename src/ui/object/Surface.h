#pragma once

/* simple rectangle surface */

#include "scene/mesh/vec2/Rectangle.h"
#include "scene/actor/MeshActor.h"

#include "types/Color.h"

namespace ui {
namespace object {

CHILD_CLASS( Surface, UIObject )

	void SetColor( const types::Color &color) {
		m_colors.background = color;
		Redraw();
	}

	virtual void Create() {
		UIObject::Create();

		m_background_mesh = new scene::mesh::vec2::Rectangle;
		m_background = new scene::actor::MeshActor( "Panel::Background", m_background_mesh );
		m_actors.push_back( m_background );

	}
	virtual void Destroy() {

		delete m_background;

		UIObject::Destroy();
	}
	virtual void Align() {
		UIObject::Align();

		m_background_mesh->SetCoords(
			ClampX( m_object_area.left ),
			ClampY( m_object_area.top ),
			ClampX( m_object_area.right ),
			ClampY( m_object_area.bottom )
		);
	}
	virtual void Draw() {
		UIObject::Draw();

		m_background->SetTintColor( m_colors.background );
	}
protected:
	struct {
		types::Color background;
	} m_colors;

	scene::actor::MeshActor *m_background;
	scene::mesh::vec2::Rectangle *m_background_mesh;
};

} /* namespace object */
} /* namespace ui */
