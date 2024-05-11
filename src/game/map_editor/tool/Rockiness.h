#pragma once

#include "Tool.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Rockiness, Tool )

	Rockiness( Game* game );

	const tiles_t Draw( map::tile::Tile* tile, const draw_mode_t mode ) override;

};

}
}
}
