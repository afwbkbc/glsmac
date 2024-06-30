#include "WaterSurface.h"

#include "game/map/Map.h"
#include "game/map/Consts.h"
#include "game/map/tile/Tile.h"
#include "game/map/tile/TileState.h"

namespace game {
namespace map {
namespace module {

void WaterSurface::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	if ( ts->has_water ) {
		m_map->AddTexture(
			tile::LAYER_WATER_SURFACE,
			s_consts.tc.texture_pcx.water[ 1 ],
			types::texture::AM_DEFAULT,
			RandomRotate()
		);

#define x( _k ) ts->layers[ tile::LAYER_WATER_SURFACE ].colors._k = types::Color{ \
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
