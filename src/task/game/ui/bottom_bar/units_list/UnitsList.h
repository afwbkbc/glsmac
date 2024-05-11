#pragma once

#include <vector>

#include "task/game/ui/bottom_bar/BBSection.h"

namespace ui::object {
class ScrollView;
}

namespace task {
namespace game {

class Unit;

namespace ui {

class UnitPreview;
class UnitsListItem;

CLASS( UnitsList, BBSection )

	UnitsList( Game* game, UnitPreview* unit_preview );

	void Create() override;
	void Destroy() override;

	void ClearUnits();
	void ListUnits( const std::vector< Unit* >& units, const size_t selected_unit_id );

private:
	friend class UnitsListItem;
	void PreviewUnit( Unit* unit );
	void HideUnitPreview( Unit* unit );
	void SelectUnit( Unit* unit, const bool activate_unit );

private:
	::ui::object::ScrollView* m_body = nullptr;
	UnitPreview* m_unit_preview;
	std::unordered_map< size_t, UnitsListItem* > m_items = {};
	Unit* m_selected_unit = nullptr;
	Unit* m_previewing_unit = nullptr;
};

}
}
}
