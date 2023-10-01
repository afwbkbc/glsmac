#pragma once

#include <vector>

#include "base/Base.h"

#include "../MapEditor.h"

namespace game {
class Game;
namespace map_editor {
namespace tool {

CLASS( Tool, base::Base )

	Tool( Game* game, const MapEditor::tool_type_t type );

	const MapEditor::tool_type_t GetType() const;

	// return tiles that were drawn on and need reload
	virtual const MapEditor::tiles_t Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) = 0;

protected:
	const Game* m_game = nullptr;
	const MapEditor::tool_type_t m_type = MapEditor::TT_NONE;

};

}
}
}
