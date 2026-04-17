#pragma once

#include <vector>

#include "types/Vec3.h"

namespace game {
namespace frontend {

class Game;

namespace tile {
class Tile;
}

class TileObject {
public:
	enum tile_object_type_t {
		TOT_NONE,
		TOT_UNIT,
		TOT_BASE,
	};
	TileObject( const tile_object_type_t type, tile::Tile* tile );

	void UpdateFromTile();

	const tile_object_type_t GetType() const;

protected:

	const tile_object_type_t m_type;
	tile::Tile* m_tile = nullptr;

	virtual void SetRenderCoords( const types::Vec3& coords ) = 0;

private:

};

}
}