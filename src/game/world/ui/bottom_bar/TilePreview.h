#pragma once

#include "Section.h"

#include "ui/object/Mesh.h"
#include "ui/object/Label.h"

#include "../../map/Map.h"

namespace game {
namespace world {

using namespace map;

namespace ui {

CLASS( TilePreview, Section )

	TilePreview( const World* world ) : Section( world, "TilePreview" ) {}
	
	void Create();
	void Destroy();

	void PreviewTile( const Map::tile_info_t& tile_info );
	void HideTilePreview();
	
private:
	
	std::vector< object::Mesh* > m_previews = {}; // multiple layers of textures
	
	// TODO: multiline labels?
	std::vector< Label* > m_info_lines = {};
	
};
	
}
}
}
