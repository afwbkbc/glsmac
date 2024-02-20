#pragma once

#include <vector>

#include "task/game/ui/bottom_bar/BBSection.h"
#include "ui/object/Section.h"

#include "task/game/Types.h"

#include "UnitsListItem.h"

namespace task {
namespace game {
namespace ui {

CLASS( UnitsList, BBSection )

	UnitsList( Game* game )
		: BBSection( game, "UnitsList" ) {}

	void Create() override;
	void Destroy() override;

	void ClearUnits();
	void ListUnits( const std::vector< unit_data_t >& units );

private:
	std::vector< UnitsListItem* > m_items = {};

};

}
}
}
