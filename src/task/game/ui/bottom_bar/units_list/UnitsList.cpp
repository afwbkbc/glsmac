#include "UnitsList.h"

#include "engine/Engine.h"

namespace task {
namespace game {
namespace ui {

void UnitsList::Create() {
	BBSection::Create();

}

void UnitsList::Destroy() {

	ClearUnits();

	BBSection::Destroy();
}

void UnitsList::ClearUnits() {
	for ( const auto& item : m_items ) {
		RemoveChild( item );
	}
	m_items.clear();
}

void UnitsList::ListUnits( const std::vector< unit_data_t >& units ) {
	ClearUnits();
	size_t left = 0;
	for ( const auto& unit : units ) {
		NEWV( item, UnitsListItem, m_game, &unit );
		item->SetLeft( left );
		AddChild( item );
		m_items.push_back( item );
		left += item->GetWidth();
	}
}

}
}
}
