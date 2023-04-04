#pragma once

#include "Tool.h"

#include "task/game/map/Tile.h"

namespace task {
namespace game {
namespace map_editor {
namespace tool {

CLASS( Resource, Tool )
	
	Resource( Game* game );
	
	const MapEditor::tiles_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode );
	
};
	
}
}
}
}
