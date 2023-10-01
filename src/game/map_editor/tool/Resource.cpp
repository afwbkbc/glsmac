#include "Resource.h"

#include "../../Game.h"

namespace game {
namespace map_editor {
namespace tool {

Resource::Resource( Game* game )
	: Tool( game, MapEditor::TT_RESOURCES ) {
	//
}

const MapEditor::tiles_t Resource::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	if ( mode == MapEditor::DM_DEC ) {
		if ( tile->bonus == map::Tile::B_NONE ) {
			return {}; // nothing to remove
		}
		tile->bonus = map::Tile::B_NONE;
	}
	else if ( mode == MapEditor::DM_INC ) {
		// rotate
		if ( tile->bonus == map::Tile::B_MINERALS ) {
			tile->bonus = map::Tile::B_NUTRIENT;
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
