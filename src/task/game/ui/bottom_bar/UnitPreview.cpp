#include "UnitPreview.h"

#include "engine/Engine.h"

namespace task {
namespace game {
namespace ui {

UnitPreview::UnitPreview( Game* game )
	: BBSection( game, "UnitPreview" ) {}

void UnitPreview::Create() {
	BBSection::Create();

}

void UnitPreview::Destroy() {
	HideUnitPreview();

	BBSection::Destroy();
}

void UnitPreview::PreviewUnit( const unit_data_t* unit_data ) {
	HideUnitPreview();

	m_unit_data = unit_data;

	const types::mesh::Mesh* mesh;
#define X( _key, _class ) \
    NEW( mesh, types::mesh::Mesh, *m_unit_data->_key.mesh ); /* make a copy */ \
    NEW( m_sprites._key, object::Mesh, "BBUnitPreview" _class ); \
    m_sprites._key->SetMesh( mesh ); \
    m_sprites._key->SetTexture( m_unit_data->_key.texture ); \
    AddChild( m_sprites._key );

	// order is important
	X( unit, "Unit" );
	X( healthbar, "Healthbar" );
	X( badge, "Badge" );

#undef X

	size_t top = 86;
	::ui::object::Label* label;
#define X( _text, _align ) \
    NEW( label, ::ui::object::Label, "BBUnitPreviewLabel" #_align ); \
    label->SetText( _text ); \
    label->SetTop( top ); \
    AddChild( label ); \
    m_labels.push_back( label ); \
    top += label->GetHeight();

	X( m_unit_data->unit_name, Header );
	X( "( " + m_unit_data->short_power_label + " )", Center );
	X( m_unit_data->morale_label, Left );

	// HACK: fix ( and ) vertical misalignment
	auto& bugged_label = m_labels.at( 1 );
	bugged_label->SetTop( bugged_label->GetTop() - 4 );

#undef X

}

void UnitPreview::HideUnitPreview() {
	if ( m_unit_data ) {
		RemoveChild( m_sprites.unit );
		RemoveChild( m_sprites.healthbar );
		RemoveChild( m_sprites.badge );

		for ( const auto& label : m_labels ) {
			RemoveChild( label );
		}
		m_labels.clear();

		m_unit_data = nullptr;
	}
}

void UnitPreview::HideUnitPreview( const unit_data_t* unit_data ) {
	if ( m_unit_data == unit_data ) {
		HideUnitPreview();
	}
}

}
}
}
