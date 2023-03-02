#pragma once

#include "../UI.h"

#include "ui/object/Surface.h"
#include "ui/object/Section.h"
#include "ui/object/Button.h"

#include "types/Texture.h"

// sections
#include "UnitPreview.h"
#include "TilePreview.h"
#include "InfoPanels.h"
#include "UnitsList.h"
#include "MiniMap.h"

namespace game {
namespace world {
	
using namespace map;

namespace ui {
	
CLASS( BottomBar, UI )
	
	BottomBar( const World* world ) : UI( world, "MapBottomBar" ) {}

	void Create();
	void Destroy();
	void Align();
	
	void PreviewTile( const Map* map, const Map::tile_info_t& tile_info );
	void HideTilePreview();
	
	void SetMinimapTexture( types::Texture* texture );
	
protected:
	struct {
		Surface* left = nullptr;
		Surface* right = nullptr;
		Surface* middle = nullptr;
	} m_frames = {};
	
	std::vector< Surface* > m_backgrounds = {};
	
	struct {
		Button* menu = nullptr;
		Button* commlink = nullptr;
	} m_buttons = {};
	
	struct {
		UnitPreview* unit_preview = nullptr;
		TilePreview* tile_preview = nullptr;
		InfoPanels* info_panels = nullptr;
		UnitsList* units_list = nullptr;
		MiniMap* mini_map = nullptr;
	} m_sections = {};
	
	struct {
		types::Texture* minimap = nullptr;
	} m_textures = {};
	
private:
	const UIEventHandler* m_mouse_blocker = nullptr;
	
};

}
}
}
