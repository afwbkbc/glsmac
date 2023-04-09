#pragma once

#include "Tool.h"

#include "game/map/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Elevations, Tool )
	
	static constexpr map::Tile::elevation_t elevation_change_min = 250;
	static constexpr map::Tile::elevation_t elevation_change_max = 500;
	
	Elevations( Game* game );
	
	const MapEditor::tiles_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode );
	
};
	
}
}
}
