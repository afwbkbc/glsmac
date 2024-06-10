#pragma once

#include <vector>

#include "base/Base.h"

#include "game/map_editor/Types.h"

namespace game {
class Game;
namespace map_editor {
namespace tool {

CLASS( Tool, ::base::Base )

	Tool( Game* game, const tool_type_t type );

	const tool_type_t GetType() const;

	// return tiles that were drawn on and need reload
	virtual const tiles_t Draw( map::tile::Tile* tile, const draw_mode_t mode ) = 0;

protected:
	const Game* m_game = nullptr;
	const tool_type_t m_type = TT_NONE;

};

}
}
}
