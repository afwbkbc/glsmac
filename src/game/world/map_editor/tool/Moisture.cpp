#include "Moisture.h"

#include "../../World.h"

namespace game {
namespace world {
namespace map_editor {
namespace tool {

Moisture::Moisture( World* world )
	: Tool( world, MapEditor::TT_MOISTURE )
{
	//
}

const MapEditor::tiles_t Moisture::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	if ( mode == MapEditor::DM_DEC ) {
		if ( tile->moisture <= Tile::M_ARID ) {
			return {}; // can't decrease further
		}
		tile->moisture--;
	}
	else if ( mode == MapEditor::DM_INC ) {
		if ( tile->moisture >= Tile::M_RAINY ) {
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
