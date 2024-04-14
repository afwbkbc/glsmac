#include "UnitsListItem.h"

#include "UnitsList.h"

namespace task {
namespace game {
namespace ui {

UnitsListItem::UnitsListItem( Game* game, UnitsList* units_list, Unit* unit )
	: Section( game, "BBUnitsListItem", "BB" )
	, m_units_list( units_list )
	, m_unit( unit ) {
	m_config.no_outer_border = true;
	m_config.no_inner_border = true;
	SetEventContexts( EC_MOUSE | EC_MOUSEMOVE );
}

void UnitsListItem::Create() {
	Section::Create();

	const auto& render = m_unit->GetRenderData();

	const types::mesh::Mesh* mesh;
#define X( _key, _class ) \
    ASSERT( render._key.mesh, #_key " mesh not defined" ); \
    NEW( mesh, types::mesh::Mesh, *render._key.mesh ); /* make a copy */ \
    NEW( m_sprites._key.obj, object::Mesh, "BBUnitsListPreview" _class ); \
    m_sprites._key.obj->SetMesh( mesh ); \
    m_sprites._key.obj->SetTexture( render._key.texture ); \
    AddChild( m_sprites._key.obj );

	// order is important
	X( unit, "Unit" );
	X( healthbar, "Healthbar" );
	X( badge, "Badge" );

#undef X

	NEW( m_label, ::ui::object::Label, "BBUnitsListPreviewLabel" );
	m_label->SetTop( 0 );
	m_label->SetText( render.labels.power );
	AddChild( m_label );

	On(
		UIEvent::EV_MOUSE_OVER, EH( this ) {
			m_units_list->PreviewUnit( m_unit );
			return true;
		}
	);
	On(
		UIEvent::EV_MOUSE_OUT, EH( this ) {
			m_units_list->HideUnitPreview( m_unit );
			return true;
		}
	);
	On(
		UIEvent::EV_MOUSE_DOWN, EH( this ) {
			if ( data->mouse.button == UIEvent::M_LEFT ) {
				m_units_list->SelectUnit( m_unit, true );
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

	m_units_list->HideUnitPreview( m_unit );

	RemoveChild( m_sprites.unit.obj );
	RemoveChild( m_sprites.healthbar.obj );
	RemoveChild( m_sprites.badge.obj );

	RemoveChild( m_label );

	Section::Destroy();
}

void UnitsListItem::SelectUnit() {
	m_selection_frame->AddStyleModifier( Style::M_ACTIVE );
}

void UnitsListItem::DeselectUnit() {
	m_selection_frame->RemoveStyleModifier( Style::M_ACTIVE );
}

const Unit* UnitsListItem::GetUnit() const {
	return m_unit;
}

}
}
}
