#pragma once

#include "base/Base.h"

namespace game {
namespace map {

class Map;
class MapState;

namespace tile {
class Tile;
class TileState;
}

namespace module {

CLASS( Module, base::Base )

	Module( Map* const map );

	virtual void GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) = 0;

protected:
	Map* const m_map;

	const uint8_t RandomRotate() const;
};

}
}
}
