#include "UnitsListItem.h"

namespace task {
namespace game {
namespace ui {

UnitsListItem::UnitsListItem( Game* game, const unit_data_t* unit_data )
	: Section( game, "BBUnitsListItem", "BB" )
	, m_unit_data( unit_data ) {
	m_config.no_outer_border = true;
	m_config.no_inner_border = true;
}

void UnitsListItem::Create() {
	Section::Create();

	NEW( m_preview, object::Mesh, "BBUnitsListPreviewImage" );
	m_preview->SetMesh( m_unit_data->preview_mesh );
	m_preview->SetTexture( m_unit_data->preview_texture );
	Section::AddChild( m_preview );
}

void UnitsListItem::Destroy() {

	RemoveChild( m_preview );

	Section::Destroy();
}

}
}
}
