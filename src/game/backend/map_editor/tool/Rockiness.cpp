#include "Rockiness.h"

#include "game/backend/Game.h"

namespace game {
namespace backend {
namespace map_editor {
namespace tool {

Rockiness::Rockiness( Game* game )
	: Tool( game, TT_ROCKINESS ) {
	//
}

const tiles_t Rockiness::Draw( map::tile::Tile* tile, const draw_mode_t mode ) {
	if ( mode == DM_DEC ) {
		if ( tile->rockiness <= map::tile::ROCKINESS_FLAT ) {
			return {}; // can't decrease further
		}
		tile->rockiness--;
	}
	else if ( mode == DM_INC ) {
		if ( tile->rockiness >= map::tile::ROCKINESS_ROCKY ) {
			return {}; // can't increase further
		}
		tile->rockiness++;
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
