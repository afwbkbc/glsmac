#pragma once

#include "Tool.h"

#include "game/world/map/Tile.h"

namespace game {
namespace world {
namespace map_editor {
namespace tool {

CLASS( Resource, Tool )
	
	Resource( World* world );
	
	const MapEditor::tiles_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode );
	
};
	
}
}
}
}
