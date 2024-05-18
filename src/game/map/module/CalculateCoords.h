#pragma once

#include "Module.h"

namespace game {
namespace map {
namespace module {

CLASS( CalculateCoords, Module )

	CalculateCoords( Map* const map )
		: Module( map ) {}

	void GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) override;

};

}
}
}
