#pragma once

#include "Module.h"

namespace game {
namespace world {
namespace module {

CLASS( LandMoisture, Module )

	LandMoisture( World* const map )
		: Module( map ) {}

	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) override;

};

}
}
}
