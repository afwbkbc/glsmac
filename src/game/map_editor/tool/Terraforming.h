#pragma once

#include "Tool.h"

#include "game/map/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Terraforming, Tool )

	Terraforming( Game* game, const MapEditor::tool_type_t type, const map::Tile::terraforming_t terraforming );

	const MapEditor::tiles_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) override;

private:
	const map::Tile::terraforming_t m_terraforming = map::Tile::T_NONE;

};

}
}
}
