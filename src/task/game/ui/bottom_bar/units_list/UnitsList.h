#pragma once

#include <vector>

#include "task/game/ui/bottom_bar/BBSection.h"
#include "ui/object/Section.h"
#include "ui/object/ScrollView.h"
#include "task/game/Types.h"
#include "../UnitPreview.h"
#include "UnitsListItem.h"

namespace task {
namespace game {
namespace ui {

CLASS( UnitsList, BBSection )

	UnitsList( Game* game, UnitPreview* unit_preview );

	void Create() override;
	void Destroy() override;

	void ClearUnits();
	void ListUnits( const std::vector< unit_data_t >& units );

private:
	friend class UnitsListItem;
	void PreviewUnit( const unit_data_t& unit ) const;
	void HideUnitPreview( const unit_data_t& unit ) const;
	void SelectUnit( const unit_data_t* unit );

private:
	::ui::object::ScrollView* m_body = nullptr;
	UnitPreview* m_unit_preview;
	std::unordered_map< size_t, UnitsListItem* > m_items = {};
	const unit_data_t* m_selected_unit = nullptr;
};

}
}
}
