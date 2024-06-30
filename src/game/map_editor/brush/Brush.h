#pragma once

#include <vector>

#include "common/Common.h"

#include "game/map_editor/Types.h"

namespace game {

class Game;

namespace map_editor {
namespace brush {

CLASS( Brush, common::Class )

	Brush( Game* game, const brush_type_t type );

	const brush_type_t GetType() const;

	// return tiles that need to be drawn
	virtual const tiles_t Draw( map::tile::Tile* center_tile ) = 0;

protected:
	const Game* m_game = nullptr;
	const brush_type_t m_type = BT_NONE;

};

}
}
}
