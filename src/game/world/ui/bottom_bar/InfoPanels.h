#pragma once

#include "../UI.h"

#include "ui/object/Surface.h"
#include "ui/object/Section.h"
#include "ui/object/Button.h"

// sections
#include "UnitPreview.h"
#include "TilePreview.h"
#include "InfoPanel.h"
#include "UnitsList.h"
#include "MiniMap.h"

namespace game {
namespace world {
	
using namespace map;

namespace ui {
	
CLASS( InfoPanels, UI )
	
	InfoPanels( const World* world ) : UI( world, "MapBottomBarInfoPanels" ) {}

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
