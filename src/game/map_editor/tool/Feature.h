#pragma once

#include "Tool.h"

#include "game/world/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Feature, Tool )

	Feature( Game* game, const MapEditor::tool_type_t type, const world::Tile::feature_t feature );

	const MapEditor::tiles_t Draw( world::Tile* tile, const MapEditor::draw_mode_t mode ) override;

private:
	const world::Tile::feature_t m_feature = world::Tile::F_NONE;

};

}
}
}
