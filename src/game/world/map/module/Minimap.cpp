#include "Minimap.h"

#include "scene/actor/Sprite.h"

namespace game {
namespace world {
namespace map {

void Minimap::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	// TODO: refactor Color to avoid excessive convertions
	
	const Color color_land = Color::FromRGB( 93, 76, 59 ) / 2;
	const Color color_water = Color::FromRGB( 14, 37, 75 ) / 2;
	
	if ( tile->is_water_tile ) {
		m_map->SetMinimapColor( color_water );
	}
	else {
		m_map->SetMinimapColor( color_land );
	}
	
}


}
}
}
