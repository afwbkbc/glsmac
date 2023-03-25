#pragma once

#include <vector>

#include "MiddleAreaPage.h"

#include "InfoPanel.h"
#include "types/Vec2.h"

namespace game {
namespace world {
namespace ui {
	
CLASS( InfoPanels, MiddleAreaPage )
	
	static constexpr size_t s_min_info_panel_width = 200;
	const types::Vec2< ssize_t > m_info_panel_margin = { -4, -4 };

	InfoPanels( World* world ) : MiddleAreaPage( world, "InfoPanels" ) {}

	void Create();
	void Destroy();
	void Align();
	
protected:	
	std::vector<InfoPanel*> m_panels = {};
	
};

}
}
}
