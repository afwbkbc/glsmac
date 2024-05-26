#include "UnitsList.h"

#include "engine/Engine.h"
#include "task/game/Game.h"
#include "ui/object/ScrollView.h"
#include "task/game/ui/bottom_bar/UnitPreview.h"
#include "UnitsListItem.h"
#include "task/game/unit/Unit.h"
#include "task/game/unit/UnitManager.h"

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

void UnitsList::ListUnits( const std::vector< unit::Unit* >& units, const size_t selected_unit_id ) {
	const auto last_scroll_x = m_body
		? m_body->GetScrollX()
		: 0;
	ClearUnits(); // TODO: avoid redraws?
	NEW( m_body, ::ui::object::ScrollView, ::ui::object::ScrollView::ST_HORIZONTAL_INLINE );
	m_body->SetSticky( false );
	m_body->SetScrollSpeed( 70 );
	AddChild( m_body );
	float left = 0;
	unit::Unit* selected_unit = nullptr;
	for ( const auto& unit : units ) {
		NEWV( item, UnitsListItem, m_game, this, unit );
		item->SetLeft( left );
		m_body->AddChild( item );
		if (
			(
				selected_unit_id && unit->GetId() == selected_unit_id // select unit as requested
			) ||
				(
					!selected_unit_id && m_items.empty() // auto-select first unit by default
				)
			) {
			selected_unit = unit;
		}
		m_items.insert(
			{
				unit->GetId(),
				item
			}
		);
		left += item->GetWidth();
	}
	m_body->SetScrollX( last_scroll_x );
	if ( selected_unit ) {
		SelectUnit( selected_unit, false );
		m_unit_preview->PreviewUnit( selected_unit );
	}
}

void UnitsList::PreviewUnit( unit::Unit* unit ) {
	if ( unit != m_previewing_unit ) {
		m_previewing_unit = unit;
		m_unit_preview->PreviewUnit( unit );
	}
}

void UnitsList::HideUnitPreview( unit::Unit* unit ) {
	if ( unit != m_selected_unit ) {
		if ( m_previewing_unit ) {
			m_unit_preview->PreviewUnit( m_previewing_unit );
		}
		else if ( m_selected_unit ) {
			m_unit_preview->PreviewUnit( m_selected_unit );
		}
		else if ( unit == m_previewing_unit ) { // TODO: test
			m_unit_preview->HideUnitPreview();
		}
	}
	if ( unit == m_previewing_unit ) {
		m_previewing_unit = nullptr;
	}
}

void UnitsList::SelectUnit( unit::Unit* unit, const bool activate_unit ) {
	if ( unit != m_selected_unit ) {
		m_selected_unit = unit;
		for ( auto& it : m_items ) {
			const auto& item = it.second;
			if ( it.first == unit->GetId() ) {
				item->SelectUnit();
				m_body->ScrollToItem( item );
			}
			else {
				item->DeselectUnit();
			}
		}
		if ( activate_unit ) {
			m_game->GetUM()->SelectUnit( m_selected_unit, true );
		}
	}
}

}
}
}
