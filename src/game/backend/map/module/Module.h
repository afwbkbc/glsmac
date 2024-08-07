#pragma once

#include <cstdint>

#include "common/Common.h"

namespace game {
namespace backend {
namespace map {

class Map;
class MapState;

namespace tile {
class Tile;
class TileState;
}

namespace module {

CLASS( Module, common::Class )

	Module( Map* const map );

	virtual void GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) = 0;

protected:
	Map* const m_map;

	const uint8_t RandomRotate() const;
};

}
}
}
}
