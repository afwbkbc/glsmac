#pragma once

#include "Coastlines.h"

namespace util {
class Perlin;
}

namespace game {
namespace map {
namespace module {

CLASS( Coastlines2, Coastlines )

	Coastlines2( Map* const map );
	~Coastlines2();

	void GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) override;

private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
