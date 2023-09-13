#pragma once

#include "Tool.h"

#include "game/map/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Feature, Tool )
	
	Feature( Game* game, const MapEditor::tool_type_t type, const map::Tile::feature_t feature );
	
	const MapEditor::tiles_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) override;

private:
	const map::Tile::feature_t m_feature = map::Tile::F_NONE;
	
};
	
}
}
}
