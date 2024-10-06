#pragma once

#include "Tool.h"

namespace game {
namespace backend {
namespace map_editor {
namespace tool {

CLASS( Moisture, Tool )

	Moisture( Game* game );

	const tiles_t Draw( map::tile::Tile* tile, const draw_mode_t mode ) override;

};

}
}
}
}
