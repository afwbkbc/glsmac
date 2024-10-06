#pragma once

#include "Coastlines.h"

namespace util {
class Perlin;
}

namespace game {
namespace backend {
namespace map {

namespace module {

CLASS( Coastlines1, Coastlines )

	Coastlines1( Map* const map );
	~Coastlines1();

	void GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) override;

private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
}
