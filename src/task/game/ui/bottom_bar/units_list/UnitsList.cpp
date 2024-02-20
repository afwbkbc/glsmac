#include "UnitsList.h"

#include "engine/Engine.h"

namespace task {
namespace game {
namespace ui {

UnitsList::UnitsList( Game* game, UnitPreview* unit_preview )
	: BBSection( game, "UnitsList" )
	, m_unit_preview( unit_preview ) {}

void UnitsList::Create() {
	BBSection::Create();

}

void UnitsList::Destroy() {

	ClearUnits();

	BBSection::Destroy();
}

void UnitsList::ClearUnits() {
	if ( m_body ) {
		for ( const auto& item : m_items ) {
			m_body->RemoveChild( item );
		}
		m_items.clear();
		RemoveChild( m_body );
		m_body = nullptr;
	}
}

void UnitsList::ListUnits( const std::vector< unit_data_t >& units ) {
	ClearUnits();
	NEW( m_body, ::ui::object::ScrollView, ::ui::object::ScrollView::ST_HORIZONTAL_INLINE );
	m_body->SetSticky( false );
	m_body->SetScrollSpeed( 32 );
	AddChild( m_body );
	size_t left = 0;
	for ( const auto& unit : units ) {
		NEWV( item, UnitsListItem, m_game, m_unit_preview, unit );
		item->SetLeft( left );
		m_body->AddChild( item );
		m_items.push_back( item );
		left += item->GetWidth();
	}
}

}
}
}
