#include "Cross.h"

#include "game/world/map/Tile.h"

namespace game {
namespace world {
namespace map_editor {
namespace brush {

Cross::Cross( World* world )
	: Brush( world, MapEditor::BT_CROSS )
{
	//
}
	
const MapEditor::tiles_t Cross::Draw( map::Tile* center_tile ) {
	// order is important, center tile must be last for greatest effect
	// TODO: shuffle
	return {
		center_tile->NW,
		center_tile->SE,
		center_tile->NE,
		center_tile->SW,
		center_tile
	};
}

}
}
}
}
