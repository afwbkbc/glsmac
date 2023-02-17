#include "WaterSurface.h"

namespace game {
namespace world {
namespace map {

void WaterSurface::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	if ( ts->has_water && !ts->is_coastline_corner ) {
		m_map->AddTexture(
			Map::LAYER_WATER_SURFACE,
			Map::s_consts.pcx_textures.water[ 1 ],
			Texture::AM_DEFAULT,
			RandomRotate()
		);
	}
	
	// TODO: fix to match corner color and alpha to neighbour water tiles
	if ( ( tile->features & Tile::F_XENOFUNGUS ) && ( tile->is_water_tile || ts->is_coastline_corner ) ) {
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

	
}

}
}
}
