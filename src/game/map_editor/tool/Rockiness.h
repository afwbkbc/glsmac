#pragma once

#include "Tool.h"

#include "game/world/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Rockiness, Tool )

	Rockiness( Game* game );

	const MapEditor::tiles_t Draw( world::Tile* tile, const MapEditor::draw_mode_t mode ) override;

};

}
}
}
