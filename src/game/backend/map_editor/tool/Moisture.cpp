#include "Moisture.h"

#include "game/backend/map/tile/Tile.h"
#include "game/backend/map_editor/Types.h"

namespace game {
namespace backend {
namespace map_editor {
namespace tool {

Moisture::Moisture( Game* game )
	: Tool( game, TT_MOISTURE ) {
	//
}

const tiles_t Moisture::Draw( map::tile::Tile* tile, const draw_mode_t mode ) {
	if ( mode == DM_DEC ) {
		if ( tile->moisture <= map::tile::MOISTURE_ARID ) {
			return {}; // can't decrease further
		}
		tile->moisture--;
	}
	else if ( mode == DM_INC ) {
		if ( tile->moisture >= map::tile::MOISTURE_RAINY ) {
			return {}; // can't increase further
		}
		tile->moisture++;
	}

	// we need to reload surrounding tiles too because they need to blend correctly
	return {
		tile,
		tile->W,
		tile->NW,
		tile->N,
		tile->NE,
		tile->E,
		tile->SE,
		tile->S,
		tile->SW,
	};
}

}
}
}
}
