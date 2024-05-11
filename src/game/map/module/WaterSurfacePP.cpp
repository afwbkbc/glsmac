#include "WaterSurfacePP.h"

#include "game/map/Map.h"
#include "game/map/Consts.h"
#include "game/map/tile/Tile.h"
#include "game/map/tile/TileState.h"

namespace game {
namespace map {
namespace module {

void WaterSurfacePP::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	if ( ts->has_water ) {

		// it's here instead of WaterSurface because it needs to be drawn on top of coastline river fix redraw
		if ( tile->features & tile::FEATURE_XENOFUNGUS ) {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_FEATURE, tile::FEATURE_XENOFUNGUS );
			m_map->AddTexture(
				tile::LAYER_WATER,
				s_consts.tc.texture_pcx.fungus_water[ txinfo.texture_variant ],
				types::texture::AM_MERGE | txinfo.texture_flags | types::texture::AM_KEEP_TRANSPARENCY | types::texture::AM_INVERT,
				txinfo.rotate_direction
			);
		}
	}
}

}
}
}
