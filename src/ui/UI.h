#pragma once

#include "base/Module.h"

#include "object/Root.h"
#include "scene/Scene.h"
#include "util/Clamper.h"

namespace ui {

MAJOR_CLASS( UI, base::Module )

	typedef float coord_t;
	typedef types::Vec2<coord_t> vertex_t;

	void Start();
	void Stop();
	void Iterate();
	void AddObject( object::UIObject *object ) {
		m_root_object.AddChild( object );
	}
	void RemoveObject( object::UIObject *object ) {
		m_root_object.RemoveChild( object );
	}
	scene::Scene *GetShapeScene() { return m_shape_scene; }
	scene::Scene *GetTextScene() { return m_text_scene; }
	const coord_t ClampX( const coord_t value ) const { return m_clamp.x.Clamp( value ); }
	const coord_t ClampY( const coord_t value ) const { return m_clamp.y.Clamp( value ); }
	void Resize();
protected:
	object::Root m_root_object;

	scene::Scene *m_shape_scene;
	scene::Scene *m_text_scene;

	struct {
		util::Clamper<coord_t> x;
		util::Clamper<coord_t> y;
	} m_clamp;
};

} /* namespace ui */
