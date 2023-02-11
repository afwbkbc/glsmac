#pragma once

#include "../UI.h"

#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( TilePreview, UI )

	TilePreview( const World* world ) : UI( world, "MapBottomBarTilePreview" ) {}
	
	void Create();
	void Destroy();

private:
	Section* m_outer = nullptr;
	Section* m_inner = nullptr;
	
};
	
}
}
}
