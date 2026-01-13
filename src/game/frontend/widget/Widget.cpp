#include "Widget.h"

#include "ui/UI.h"
#include "ui/dom/Widget.h"
#include "ui/geometry/Rectangle.h"
#include "game/frontend/Game.h"
#include "types/mesh/Mesh.h"
#include "scene/actor/Mesh.h"
#include "types/mesh/Rectangle.h"
#include "types/texture/Texture.h"

namespace game {
namespace frontend {
namespace widget {

Widget::Widget( const Game* const game, ui::UI* const ui, const ui::widget_type_t type, const std::string& str, const ui::widget_data_config_t& data_config )
	: m_game( game )
	, m_ui( ui )
	, m_type( type ) {
	m_ui->RegisterWidget(
		m_type, {
			str,
			std::bind( &Widget::Register, this, std::placeholders::_1 ),
			data_config
		}
	);
}

Widget::~Widget() {
	m_ui->UnregisterWidget( m_type );
}

void Widget::AddMeshAndTexture( ui::dom::Widget* const widget, size_t index, const types::mesh::Mesh* const mesh, types::texture::Texture* const texture, const bool keep_tex, const types::Vec2< float >& scale, const types::Vec2< float >& offset ) {
	auto* g = widget->GetGeometry()->AsRectangle();
	ASSERT( g, "geometry is null or not rectangle" );
	types::texture::Texture* t;
	types::mesh::Mesh* m;
	if ( texture ) {
		t = texture;
	}
	else {
		NEW( t, types::texture::Texture, g->m_area.width, g->m_area.height );
	}
	if ( mesh ) {
		NEW( m, types::mesh::Mesh, *mesh );
	}
	else {
		NEW( m, types::mesh::Rectangle );
	}
	g->AddMesh( m, keep_tex, scale, offset );
	NEWV( actor, scene::actor::Mesh, "UI::Widget::Actor", m );
	g->AddActor( actor );
	actor->SetTexture( t );
	widget->SetActor( actor, index );
	if ( !texture ) {
		widget->SetTexture( t, 0 );
	}
}

void Widget::WithWidget( const ui::f_with_widget_t& f ) const {
	m_ui->WithWidget( m_type, f );
}

}
}
}
