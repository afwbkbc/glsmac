#include "WaterSurface.h"

namespace game {
namespace world {
namespace map {

void WaterSurface::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	if ( ts->has_water ) {
		m_map->AddTexture(
			Map::LAYER_WATER_SURFACE,
			Map::s_consts.pcx_textures.water[ 1 ],
			Texture::AM_DEFAULT,
			RandomRotate()
		);
	
		if ( tile->features & Tile::F_XENOFUNGUS ) {
			auto txinfo = m_map->GetTileTextureInfo( tile, Map::TG_FEATURE, Tile::F_XENOFUNGUS );
			auto mode = Texture::AM_MERGE;
			if ( txinfo.texture_variant >= 14 ) {
				ts->layers[ Map::LAYER_WATER ].texture_stretch_at_edges = true;
				mode |= Texture::AM_RANDOM_SHIFT_X | Texture::AM_RANDOM_SHIFT_Y | Texture::AM_RANDOM_MIRROR_X | Texture::AM_RANDOM_MIRROR_Y;
			}
			m_map->AddTexture(
				Map::LAYER_WATER,
				Map::s_consts.pcx_textures.fungus_sea[ txinfo.texture_variant ],
				mode,
				txinfo.rotate_direction
			);
		}


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
