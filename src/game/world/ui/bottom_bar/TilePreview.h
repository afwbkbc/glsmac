#pragma once

#include "../UI.h"

#include "ui/object/Section.h"
#include "ui/object/Mesh.h"

#include "../../map/Map.h"

namespace game {
namespace world {

using namespace map;

namespace ui {

CLASS( TilePreview, UI )

	TilePreview( const World* world ) : UI( world, "MapBottomBarTilePreview" ) {}
	
	void Create();
	void Destroy();

	void PreviewTile( const Map::tile_info_t& tile_info );
	void HideTilePreview();
	
private:
	Section* m_outer = nullptr;
	Section* m_inner = nullptr;
	
	mesh::Simple* m_mesh = nullptr;
	object::Mesh* m_preview = nullptr;
	
};
	
}
}
}
