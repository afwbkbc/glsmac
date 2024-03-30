#include "UnitsListItem.h"

#include "UnitsList.h"

namespace task {
namespace game {
namespace ui {

UnitsListItem::UnitsListItem( Game* game, UnitsList* units_list, const unit_data_t& unit_data )
	: Section( game, "BBUnitsListItem", "BB" )
	, m_units_list( units_list )
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
	m_label->SetText( m_unit_data.short_power_string );
	AddChild( m_label );

	On(
		UIEvent::EV_MOUSE_OVER, EH( this ) {
			m_units_list->PreviewUnit( m_unit_data );
			return true;
		}
	);
	On(
		UIEvent::EV_MOUSE_OUT, EH( this ) {
			m_units_list->HideUnitPreview( m_unit_data );
			return true;
		}
	);
	On(
		UIEvent::EV_MOUSE_DOWN, EH( this ) {
			if ( data->mouse.button == UIEvent::M_LEFT ) {
				m_units_list->SelectUnit( &m_unit_data, true );
				return true;
			}
			return false;
		}
	);

	NEW( m_selection_frame, ::ui::object::Panel, "BBUnitsListSelection" );
	AddChild( m_selection_frame );
}

void UnitsListItem::Destroy() {

	RemoveChild( m_selection_frame );

	m_units_list->HideUnitPreview( m_unit_data );

	RemoveChild( m_sprites.unit.obj );
	DELETE( m_unit_data.unit.mesh );
	RemoveChild( m_sprites.healthbar.obj );
	DELETE( m_unit_data.healthbar.mesh );
	RemoveChild( m_sprites.badge.obj );
	DELETE( m_unit_data.badge.mesh );

	RemoveChild( m_label );

	Section::Destroy();
}

void UnitsListItem::SelectUnit() {
	m_selection_frame->AddStyleModifier( Style::M_ACTIVE );
}

void UnitsListItem::DeselectUnit() {
	m_selection_frame->RemoveStyleModifier( Style::M_ACTIVE );
}

const unit_data_t& UnitsListItem::GetUnit() const {
	return m_unit_data;
}

}
}
}
