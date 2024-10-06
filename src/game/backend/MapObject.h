#pragma once

#include "types/Vec3.h"

namespace game {
namespace backend {

namespace map {
class Map;
namespace tile {
class Tile;
class TileState;
}
}

class MapObject {
public:

	MapObject( map::Map* map, map::tile::Tile* tile );

	map::tile::Tile* GetTile() const;

	const types::Vec3 GetRenderCoords() const;

protected:
	map::tile::Tile* m_tile = nullptr;

private:
	map::Map* m_map = nullptr;
};

}
}
