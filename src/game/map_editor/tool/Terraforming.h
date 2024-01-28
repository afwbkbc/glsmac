#pragma once

#include "Tool.h"

#include "game/world/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Terraforming, Tool )

	Terraforming( Game* game, const MapEditor::tool_type_t type, const world::Tile::terraforming_t terraforming );

	const MapEditor::tiles_t Draw( world::Tile* tile, const MapEditor::draw_mode_t mode ) override;

private:
	const world::Tile::terraforming_t m_terraforming = world::Tile::T_NONE;

};

}
}
}
