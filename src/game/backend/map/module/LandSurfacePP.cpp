#include "LandSurfacePP.h"

#include "types/texture/Texture.h"
#include "game/backend/map/Map.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/map/tile/TileState.h"

namespace game {
namespace backend {
namespace map {
namespace module {

void LandSurfacePP::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	if ( !tile->is_water_tile ) {

		if ( tile->features & tile::FEATURE_RIVER ) {
			if ( ts->has_water ) {

				// apply river texture again on top of coastline border to erase beach

				ASSERT( ts->river_original, "river original texture not set" );

				const auto lt = tile::LAYER_WATER;
				const auto mode = types::texture::AM_MERGE | types::texture::AM_KEEP_TRANSPARENCY;

				if ( tile->NW->is_water_tile ) {
					m_map->SetTexture( lt, ts->NW, ts->river_original, mode | types::texture::AM_MIRROR_X );
				}
				if ( tile->NE->is_water_tile ) {
					m_map->SetTexture( lt, ts->NE, ts->river_original, mode | types::texture::AM_MIRROR_Y );
				}
				if ( tile->SE->is_water_tile ) {
					m_map->SetTexture( lt, ts->SE, ts->river_original, mode | types::texture::AM_MIRROR_X );
				}
				if ( tile->SW->is_water_tile ) {
					m_map->SetTexture( lt, ts->SW, ts->river_original, mode | types::texture::AM_MIRROR_Y );
				}

			}

			if ( ts->is_coastline_corner ) {
				// apply on top of current tile because on corner tiles perlin edge is drawn on same tile
				m_map->SetTexture( tile::LAYER_LAND, ts, ts->river_original, types::texture::AM_MERGE );
				m_map->SetTexture( tile::LAYER_WATER, ts, ts->river_original, types::texture::AM_MERGE );
			}
		}
	}
}

}
}
}
}
