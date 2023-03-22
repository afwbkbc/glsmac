#include "Resource.h"

#include "../../World.h"

namespace game {
namespace world {
namespace map_editor {
namespace tool {

Resource::Resource( World* world )
	: Tool( world, MapEditor::TT_RESOURCES )
{
	//
}

const MapEditor::tiles_t Resource::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	if ( mode == MapEditor::DM_DEC ) {
		if ( tile->bonus == Tile::B_NONE ) {
			return {}; // nothing to remove
		}
		tile->bonus = Tile::B_NONE;
	}
	else if ( mode == MapEditor::DM_INC ) {
		// rotate
		if ( tile->bonus == Tile::B_MINERALS ) {
			tile->bonus = Tile::B_NUTRIENT;
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
}
