#include "Tile.h"

namespace game {
namespace world {
namespace map {

void Tile::Update() {
	
	*elevation.center = ( *elevation.left + *elevation.top + *elevation.right + *elevation.bottom ) / 4;
	
	uint8_t corners_in_water = *elevation.center < ELEVATION_LEVEL_COAST ? 1 : 0;
	for ( auto& c : elevation.corners ) {
		if ( *c < ELEVATION_LEVEL_COAST ) {
			corners_in_water++;
		}
	}
	
	is_water_tile = corners_in_water > 2;
}

}
}
}
