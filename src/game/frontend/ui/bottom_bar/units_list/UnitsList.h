#pragma once

#include <vector>

#include "game/frontend/ui/bottom_bar/BBSection.h"

namespace ui::object {
class ScrollView;
}

namespace game {
namespace frontend {

namespace unit {
class Unit;
}

namespace ui {

class UnitPreview;
class UnitsListItem;

CLASS( UnitsList, BBSection )

	UnitsList( Game* game, UnitPreview* unit_preview );

	void Create() override;
	void Destroy() override;

	void ClearUnits();
	void ListUnits( const std::vector< unit::Unit* >& units, const size_t selected_unit_id );

private:
	friend class UnitsListItem;
	void PreviewUnit( unit::Unit* unit );
	void HideUnitPreview( unit::Unit* unit );
	void SelectUnit( unit::Unit* unit, const bool activate_unit );

private:
	::ui::object::ScrollView* m_body = nullptr;
	UnitPreview* m_unit_preview;
	std::unordered_map< size_t, UnitsListItem* > m_items = {};
	unit::Unit* m_selected_unit = nullptr;
	unit::Unit* m_previewing_unit = nullptr;
};

}
}
}
