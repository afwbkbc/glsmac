#include "WaterSurfacePP.h"

namespace game {
namespace world {
namespace module {

void WaterSurfacePP::GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) {

	if ( ts->has_water ) {

		// it's here instead of WaterSurface because it needs to be drawn on top of coastline river fix redraw
		if ( tile->features & Tile::F_XENOFUNGUS ) {
			auto txinfo = m_map->GetTileTextureInfo( World::TVT_TILES, tile, World::TG_FEATURE, Tile::F_XENOFUNGUS );
			m_map->AddTexture(
				TileState::LAYER_WATER,
				s_consts.tc.texture_pcx.fungus_sea[ txinfo.texture_variant ],
				Texture::AM_MERGE | txinfo.texture_flags | Texture::AM_KEEP_TRANSPARENCY | Texture::AM_INVERT,
				txinfo.rotate_direction
			);
		}
	}
}

}
}
}
