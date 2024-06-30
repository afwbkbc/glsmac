#include "Cross.h"

#include "game/map/tile/Tile.h"

namespace game {
namespace map_editor {
namespace brush {

Cross::Cross( Game* game )
	: Brush( game, BT_CROSS ) {
	//
}

const tiles_t Cross::Draw( map::tile::Tile* center_tile ) {
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
