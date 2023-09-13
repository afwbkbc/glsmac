#pragma once

#include <vector>

#include "MiddleAreaPage.h"

#include "InfoPanel.h"
#include "types/Vec2.h"

namespace task {
namespace game {
namespace ui {
	
CLASS( InfoPanels, MiddleAreaPage )
	
	static constexpr size_t s_min_info_panel_width = 200;
	const types::Vec2< ssize_t > m_info_panel_margin = { -4, -4 };

	InfoPanels( Game* game ) : MiddleAreaPage( game, "InfoPanels" ) {}

	void Create() override;
	void Destroy() override;
	void Align() override;
	
protected:	
	std::vector<InfoPanel*> m_panels = {};
	
};

}
}
}
