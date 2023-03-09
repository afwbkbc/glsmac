#pragma once

#include "MiddleAreaPage.h"

#include "InfoPanel.h"

namespace game {
namespace world {
	
namespace ui {
	
CLASS( InfoPanels, MiddleAreaPage )
	
	InfoPanels( World* world ) : MiddleAreaPage( world, "InfoPanels" ) {}

	void Create();
	void Destroy();
	void Align();
	
protected:
	
	std::vector<InfoPanel*> m_panels = {};
	
private:
	
};

}
}
}
