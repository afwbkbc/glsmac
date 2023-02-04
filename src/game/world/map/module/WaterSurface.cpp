#include "WaterSurface.h"

namespace game {
namespace world {
namespace map {

void WaterSurface::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	if ( ts->has_water ) {
		m_map->AddTexture( Map::LAYER_WATER_SURFACE, Map::s_consts.pcx_textures.water[ 1 ], Texture::AM_DEFAULT, rand() % 4 );
	}
	
}

}
}
}
