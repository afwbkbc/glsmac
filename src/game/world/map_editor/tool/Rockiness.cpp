#include "Rockiness.h"

#include "../../World.h"

namespace game {
namespace world {
namespace map_editor {
namespace tool {

Rockiness::Rockiness( World* world )
	: Tool( world, MapEditor::TT_ROCKINESS )
{
	//
}

const MapEditor::tiles_t Rockiness::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	if ( mode == MapEditor::DM_DEC ) {
		if ( tile->rockiness <= Tile::R_FLAT ) {
			return {}; // can't decrease further
		}
		tile->rockiness--;
	}
	else if ( mode == MapEditor::DM_INC ) {
		if ( tile->rockiness >= Tile::R_ROCKY ) {
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
