#pragma once

#include "Tool.h"

#include "game/map/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Rockiness, Tool )

	Rockiness( Game* game );

	const MapEditor::tiles_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) override;

};

}
}
}
