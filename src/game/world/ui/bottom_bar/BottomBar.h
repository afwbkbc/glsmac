#pragma once

#include "../UI.h"

#include "ui/object/Surface.h"
#include "ui/object/Section.h"
#include "ui/object/Button.h"

#include "TilePreview.h"

namespace game {
namespace world {
	
using namespace map;

namespace ui {
	
CLASS( BottomBar, UI )
	
	BottomBar( const World* world ) : UI( world, "MapBottomBar" ) {}

	void Create();
	void Destroy();
	
	void PreviewTile( const Map::tile_info_t& tile_info );
	void HideTilePreview();
	
protected:
	struct {
		Surface* left = nullptr;
		Surface* right = nullptr;
		Surface* middle = nullptr;
	} m_frames;
	
	std::vector< Surface* > m_backgrounds;
	
	struct {
		Button* menu = nullptr;
		Button* commlink = nullptr;
	} m_buttons;
	
	TilePreview* m_tile_preview = nullptr;
	
	Section* m_unit_preview = nullptr;
};
	
}
}
}
