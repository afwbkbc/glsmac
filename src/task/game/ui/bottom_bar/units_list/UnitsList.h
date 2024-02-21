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

	::ui::object::ScrollView* m_body = nullptr;

	UnitPreview* m_unit_preview;
	std::vector< UnitsListItem* > m_items = {};

};

}
}
}
