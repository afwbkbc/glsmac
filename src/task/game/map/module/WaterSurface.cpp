#include "WaterSurface.h"

namespace task {
namespace game {
namespace map {

void WaterSurface::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	if ( ts->has_water ) {
		m_map->AddTexture(
			Map::LAYER_WATER_SURFACE,
			Map::s_consts.tc.texture_pcx.water[ 1 ],
			Texture::AM_DEFAULT,
			RandomRotate()
		);
	
		#define x( _k ) ts->layers[ Map::LAYER_WATER_SURFACE ].colors._k = (Color){ \
			Map::s_consts.clampers.elevation_to_water_r.Clamp( ts->elevations._k ), \
			Map::s_consts.clampers.elevation_to_water_g.Clamp( ts->elevations._k ), \
			Map::s_consts.clampers.elevation_to_water_b.Clamp( ts->elevations._k ), \
			Map::s_consts.clampers.elevation_to_water_a.Clamp( ts->elevations._k ) \
		}
			x( center );
			x( left );
			x( top );
			x( right );
			x( bottom );
		#undef x
	}
}

}
}
}
