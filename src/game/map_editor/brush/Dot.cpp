#include "Dot.h"

namespace game {
namespace map_editor {
namespace brush {

Dot::Dot( Game* game )
	: Brush( game, BT_DOT ) {
	//
}

const tiles_t Dot::Draw( map::tile::Tile* center_tile ) {
	return {
		center_tile
	};
}

}
}
}
