#include "UnitsList.h"

#include "engine/Engine.h"
#include "task/game/Game.h"

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
		m_unit_preview->HideUnitPreview();
		m_selected_unit = nullptr;
		for ( const auto& it : m_items ) {
			m_body->RemoveChild( it.second );
		}
		m_items.clear();
		RemoveChild( m_body );
		m_body = nullptr;
	}
}

void UnitsList::ListUnits( const std::vector< unit_data_t >& units, const size_t selected_unit_id ) {
	ClearUnits();
	NEW( m_body, ::ui::object::ScrollView, ::ui::object::ScrollView::ST_HORIZONTAL_INLINE );
	m_body->SetSticky( false );
	m_body->SetScrollSpeed( 70 );
	AddChild( m_body );
	float left = 0;
	const unit_data_t* selected_unit = nullptr;
	for ( const auto& unit : units ) {
		NEWV( item, UnitsListItem, m_game, this, unit );
		item->SetLeft( left );
		m_body->AddChild( item );
		if (
			(
				selected_unit_id && item->GetUnit().id == selected_unit_id // select unit as requested
			) ||
				(
					!selected_unit_id && m_items.empty() // auto-select first unit by default
				)
			) {
			selected_unit = &item->GetUnit();
		}
		m_items.insert(
			{
				unit.id,
				item
			}
		);
		left += item->GetWidth();
	}
	if ( selected_unit ) {
		SelectUnit( selected_unit, false );
		m_unit_preview->PreviewUnit( selected_unit );
	}
}

void UnitsList::PreviewUnit( const unit_data_t& unit ) {
	if ( &unit != m_previewing_unit ) {
		m_previewing_unit = &unit;
		m_unit_preview->PreviewUnit( &unit );
	}
}

void UnitsList::HideUnitPreview( const unit_data_t& unit ) {
	if ( &unit == m_previewing_unit ) {
		m_previewing_unit = nullptr;
	}
	if ( &unit != m_selected_unit ) {
		if ( m_previewing_unit ) {
			m_unit_preview->PreviewUnit( m_previewing_unit );
		}
		else if ( m_selected_unit ) {
			m_unit_preview->PreviewUnit( m_selected_unit );
		}
		else {
			m_unit_preview->HideUnitPreview( &unit );
		}
	}
}

void UnitsList::SelectUnit( const unit_data_t* unit, const bool actually_select_unit ) {
	if ( unit != m_selected_unit ) {
		m_selected_unit = unit;
		for ( auto& it : m_items ) {
			if ( it.first == unit->id ) {
				it.second->SelectUnit();
			}
			else {
				it.second->DeselectUnit();
			}
		}
		m_game->SelectUnit( *m_selected_unit, actually_select_unit );
	}
}

}
}
}
