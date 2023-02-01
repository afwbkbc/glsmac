#include "MapGenerator.h"
#include "util/Clamper.h"

#define SEA_LEVELS_COAST 0

namespace game {
namespace world {
namespace map_generator {

void MapGenerator::SmoothTerrain( Tiles* tiles, const bool smooth_land, const bool smooth_sea ) {
	
	Tile::elevation_t c;
	Tile::elevation_t sharpest;
	int8_t idx;
	int8_t mod;
	Tile* tile;
	uint8_t i;
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			tile = tiles->At( x, y );
			
			tile->Update();
			
			if (
				( tile->is_water_tile && !smooth_sea ) ||
				( !tile->is_water_tile && !smooth_land )
			) {
				continue;
			}
				
			mod = tile->is_water_tile ? -1 : 1;
			
			// flatten every corner
			for( auto& c : tile->elevation.corners ) {
				*c = ( *c + *tile->elevation.center ) / 2;
			}
			
		}
	}
}

void MapGenerator::Finalize( Tiles* tiles ) {
	
	tiles->Finalize();
	
}

}
}
}
