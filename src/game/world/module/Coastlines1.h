#pragma once

#include "Coastlines.h"

namespace game {
namespace world {
namespace module {

CLASS( Coastlines1, Coastlines )

	Coastlines1( World* const map );
	~Coastlines1();

	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) override;

private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
