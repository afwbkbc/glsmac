#include "Dot.h"

namespace game {
namespace world {
namespace map_editor {
namespace brush {

Dot::Dot( World* world )
	: Brush( world, MapEditor::BT_DOT )
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
}
