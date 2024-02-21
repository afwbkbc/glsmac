#include "UnitsListItem.h"

namespace task {
namespace game {
namespace ui {

UnitsListItem::UnitsListItem( Game* game, UnitPreview* unit_preview, const unit_data_t& unit_data )
	: Section( game, "BBUnitsListItem", "BB" )
	, m_unit_preview( unit_preview )
	, m_unit_data( unit_data ) {
	m_config.no_outer_border = true;
	m_config.no_inner_border = true;
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

void UnitsListItem::Create() {
	Section::Create();

	const types::mesh::Mesh* mesh;
#define X( _key, _class ) \
    NEW( mesh, types::mesh::Mesh, *m_unit_data._key.mesh ); /* make a copy */ \
    NEW( m_sprites._key.obj, object::Mesh, "BBUnitsListPreview" _class ); \
    m_sprites._key.obj->SetMesh( mesh ); \
    m_sprites._key.obj->SetTexture( m_unit_data._key.texture ); \
    AddChild( m_sprites._key.obj );

	// order is important
	X( unit, "Unit" );
	X( healthbar, "Healthbar" );
	X( badge, "Badge" );

#undef X

	NEW( m_label, ::ui::object::Label, "BBUnitsListPreviewLabel" );
	m_label->SetTop( 0 );
	m_label->SetText( m_unit_data.short_power_label );
	AddChild( m_label );

	On(
		UIEvent::EV_MOUSE_OVER, EH( this ) {
			m_unit_preview->PreviewUnit( &m_unit_data );
			return true;
		}
	);
	On(
		UIEvent::EV_MOUSE_OUT, EH( this ) {
			m_unit_preview->HideUnitPreview( &m_unit_data );
			return true;
		}
	);

}

void UnitsListItem::Destroy() {

	m_unit_preview->HideUnitPreview( &m_unit_data );

	RemoveChild( m_sprites.unit.obj );
	DELETE( m_unit_data.unit.mesh );
	RemoveChild( m_sprites.healthbar.obj );
	DELETE( m_unit_data.healthbar.mesh );
	RemoveChild( m_sprites.badge.obj );
	DELETE( m_unit_data.badge.mesh );

	RemoveChild( m_label );

	Section::Destroy();
}

}
}
}
