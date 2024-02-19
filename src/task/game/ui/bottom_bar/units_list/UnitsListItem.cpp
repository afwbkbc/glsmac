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

#define X( _key, _class ) \
    NEW( m_sprites._key, object::Mesh, "BBUnitsListPreview" _class ); \
    m_sprites._key->SetMesh( m_unit_data->_key.mesh ); \
    m_sprites._key->SetTexture( m_unit_data->_key.texture ); \
    Section::AddChild( m_sprites._key );

	// order is important
	X( unit, "Unit" );
	X( healthbar, "Healthbar" );
	X( badge, "Badge" );

#undef X

	NEW( m_label, ::ui::object::Label, "BBUnitsListPreviewLabel" );
	m_label->SetTop( 0 );
	m_label->SetText( m_unit_data->short_power_label );

	AddChild( m_label );
}

void UnitsListItem::Destroy() {

	RemoveChild( m_sprites.unit );
	RemoveChild( m_sprites.healthbar );
	RemoveChild( m_sprites.badge );

	RemoveChild( m_label );

	Section::Destroy();
}

}
}
}
