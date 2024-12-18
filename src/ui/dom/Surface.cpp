#include "Surface.h"

#include "types/mesh/Rectangle.h"
#include "scene/actor/Mesh.h"

#include "ui/UI.h"
#include "ui/Geometry.h"
#include "scene/Scene.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "graphics/Graphics.h"

namespace ui {
namespace dom {

Surface::Surface( GSE_CALLABLE, UI* const ui, const std::string& tag, ui::dom::Object* const parent, const ui::dom::Object::properties_t& properties )
	: Object( ctx, call_si, ui, tag, parent, properties ) {

	std::string cls = "UI::Mesh::Surface";
	NEW( m_mesh, types::mesh::Rectangle );
	NEW( m_actor, scene::actor::Mesh, cls, m_mesh );
	m_actor->SetTexture(
		g_engine->GetTextureLoader()->GetColorTexture(
			{
				1.0f,
				1.0f,
				1.0f,
				1.0f
			}
		)
	);

	m_ui->m_scene->AddActor( m_actor );
	NEW( m_geometry, Geometry, ui, m_parent
		? m_parent->GetGeometry()
		: nullptr, m_mesh );

#define GEOMPROP( _key, _method ) \
    Property( \
        ctx, call_si, _key, gse::type::Type::T_INT, VALUE( gse::type::Int, 0 ), PF_NONE, [ this ]( const gse::Value& v ) { \
            m_geometry->_method( ( (gse::type::Int*)v.Get() )->value ); \
        } \
    )
	GEOMPROP( "width", SetWidth );
	GEOMPROP( "height", SetHeight );
	GEOMPROP( "left", SetLeft );
	GEOMPROP( "top", SetTop );
	GEOMPROP( "right", SetRight );
	GEOMPROP( "bottom", SetBottom );
#undef GEOMPROP

}

Surface::~Surface() {
	m_ui->m_scene->RemoveActor( m_actor );
	delete m_actor;
	delete m_geometry;
}

}
}
