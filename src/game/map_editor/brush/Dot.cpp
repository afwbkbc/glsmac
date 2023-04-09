#include "Dot.h"

namespace game {
namespace map_editor {
namespace brush {

Dot::Dot( Game* game )
	: Brush( game, MapEditor::BT_DOT )
{
	//
}
	
const MapEditor::tiles_t Dot::Draw( map::Tile* center_tile ) {
	return {
		center_tile
	};
}

}
}
}
