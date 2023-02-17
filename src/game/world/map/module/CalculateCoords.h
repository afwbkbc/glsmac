#pragma once

#include "Module.h"

namespace game {
namespace world {
namespace map {

CLASS( CalculateCoords, Module )
	
	CalculateCoords( Map* const map ) : Module( map ) {}
	
	void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms );
	
};

}
}
}
