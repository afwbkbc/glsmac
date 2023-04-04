#pragma once

#include "Module.h"

namespace task {
namespace game {
namespace map {

CLASS( Prepare, Module )
	
	Prepare( Map* const map ) : Module( map ) {}
	
	void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms );
	
};

}
}
}
