#include "WaterSurface.h"

namespace game {
namespace world {
namespace module {

void WaterSurface::GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) {

	if ( ts->has_water ) {
		m_map->AddTexture(
			TileState::LAYER_WATER_SURFACE,
			s_consts.tc.texture_pcx.water[ 1 ],
			Texture::AM_DEFAULT,
			RandomRotate()
		);

#define x( _k ) ts->layers[ TileState::LAYER_WATER_SURFACE ].colors._k = (Color){ \
            s_consts.tile.elevation_to_water_r.Clamp( ts->elevations._k ), \
            s_consts.tile.elevation_to_water_g.Clamp( ts->elevations._k ), \
            s_consts.tile.elevation_to_water_b.Clamp( ts->elevations._k ), \
            s_consts.tile.elevation_to_water_a.Clamp( ts->elevations._k ) \
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
