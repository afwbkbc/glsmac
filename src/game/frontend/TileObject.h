#pragma once

#include "types/Vec3.h"

namespace game {
namespace frontend {

namespace tile {
class Tile;
}

class TileObject {
public:
	TileObject( tile::Tile* tile );

	void UpdateFromTile();

protected:

	tile::Tile* m_tile = nullptr;

	virtual void SetRenderCoords( const types::Vec3& coords ) = 0;

private:

};

}
}