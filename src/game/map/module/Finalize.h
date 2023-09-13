#pragma once

#include "Module.h"

namespace game {
namespace map {
namespace module {

CLASS( Finalize, Module )
	
	Finalize( Map* const map ) : Module( map ) {}
	
	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) override;
	
};

}
}
}
