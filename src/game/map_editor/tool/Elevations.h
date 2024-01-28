#pragma once

#include "Tool.h"

#include "game/world/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Elevations, Tool )

	static constexpr world::Tile::elevation_t elevation_change_min = 250;
	static constexpr world::Tile::elevation_t elevation_change_max = 500;

	Elevations( Game* game );

	const MapEditor::tiles_t Draw( world::Tile* tile, const MapEditor::draw_mode_t mode ) override;

};

}
}
}
