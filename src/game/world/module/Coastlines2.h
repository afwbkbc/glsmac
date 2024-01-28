#pragma once

#include "Coastlines.h"

namespace game {
namespace world {
namespace module {

CLASS( Coastlines2, Coastlines )

	Coastlines2( World* const map );
	~Coastlines2();

	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) override;

private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
