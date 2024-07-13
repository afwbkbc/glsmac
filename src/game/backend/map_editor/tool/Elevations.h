#pragma once

#include "Tool.h"

#include "game/backend/map/tile/Types.h"

namespace game {
namespace backend {
namespace map_editor {
namespace tool {

CLASS( Elevations, Tool )

	static constexpr map::tile::elevation_t elevation_change_min = 250;
	static constexpr map::tile::elevation_t elevation_change_max = 500;

	Elevations( Game* game );

	const tiles_t Draw( map::tile::Tile* tile, const draw_mode_t mode ) override;

};

}
}
}
}
