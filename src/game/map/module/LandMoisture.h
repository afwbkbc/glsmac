#pragma once

#include "Module.h"

namespace game {
namespace map {
namespace module {

CLASS( LandMoisture, Module )

	LandMoisture( Map* const map )
		: Module( map ) {}

	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) override;

};

}
}
}
