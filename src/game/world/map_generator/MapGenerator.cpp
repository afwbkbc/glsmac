#include "MapGenerator.h"

#define SEA_LEVELS_COAST 0

namespace game {
namespace world {
namespace map_generator {

void MapGenerator::Finalize( Tiles* tiles ) {
	
	Tile* tile;
	
	// TODO
	
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = tiles->At( x, y );
			
			tile->Update();
		}
	}		

}

}
}
}
