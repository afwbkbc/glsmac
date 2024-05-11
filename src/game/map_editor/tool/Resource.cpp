#include "Resource.h"

#include "../../Game.h"

namespace game {
namespace map_editor {
namespace tool {

Resource::Resource( Game* game )
	: Tool( game, TT_RESOURCES ) {
	//
}

const tiles_t Resource::Draw( map::tile::Tile* tile, const draw_mode_t mode ) {
	if ( mode == DM_DEC ) {
		if ( tile->bonus == map::tile::BONUS_NONE ) {
			return {}; // nothing to remove
		}
		tile->bonus = map::tile::BONUS_NONE;
	}
	else if ( mode == DM_INC ) {
		// rotate
		if ( tile->bonus == map::tile::BONUS_MINERALS ) {
			tile->bonus = map::tile::BONUS_NUTRIENT;
		}
		else {
			tile->bonus++;
		}
	}

	return { tile };
}

}
}
}
