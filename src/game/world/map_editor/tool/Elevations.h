#pragma once

#include "Tool.h"

#include "game/world/map/Tile.h"

namespace game {
namespace world {
namespace map_editor {
namespace tool {

CLASS( Elevations, Tool )
	
	static constexpr map::Tile::elevation_t elevation_change_min = 200;
	static constexpr map::Tile::elevation_t elevation_change_max = 400;
	
	Elevations( World* world );
	
	const MapEditor::tiles_to_reload_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode );
	
};
	
}
}
}
}
