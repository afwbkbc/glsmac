#include "Minimap.h"

#include "ui/dom/Widget.h"
#include "types/mesh/Rectangle.h"
#include "ui/geometry/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "types/texture/Texture.h"
#include "ui/UI.h"

namespace game {
namespace frontend {
namespace widget {

Minimap::Minimap( ui::UI* const ui )
	: Widget( ui, ui::WT_MINIMAP, "minimap" ) {}

void Minimap::Register( ui::dom::Widget* const widget ) {
	NEWV( mesh, types::mesh::Rectangle );
	auto* g = widget->GetGeometry()->AsRectangle();
	g->SetMesh( mesh );
	NEWV( actor, scene::actor::Mesh, "UI::Widget::MiniMap", mesh );
	g->SetActor( actor );
	NEWV( texture, types::texture::Texture, g->m_area.width, g->m_area.height );
	actor->SetTexture( texture );
	widget->AddTexture( texture );
	widget->AddActor( actor );
}

const types::Vec2< ui::coord_t > Minimap::FindLargestArea() const {
	types::Vec2< ui::coord_t > result = { 0.0f, 0.0f };
	WITH_WIDGET( &result ) {
		const auto& area = widget->GetGeometry()->m_area;
		if ( area.width > result.x ) {
			result.x = area.width;
		}
		if ( area.height > result.y ) {
			result.y = area.height;
		}
	} );
	return result;
}

void Minimap::Update( const types::texture::Texture* minimap_texture ) {
	WITH_WIDGET( &minimap_texture ) {
		// TODO: downscale in case of multiple minimaps of different sizes
		auto* const texture = widget->GetTexture();
		texture->AddFrom( minimap_texture, types::texture::AM_MIRROR_Y, 0, 0, texture->GetWidth() - 1, texture->GetHeight() - 1 );
		texture->FullUpdate();
	} );
}

}
}
}
