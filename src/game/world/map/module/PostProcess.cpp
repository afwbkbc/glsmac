#include "PostProcess.h"

namespace game {
namespace world {
namespace map {

void PostProcess::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	if ( !tile->is_water_tile ) {
		if ( tile->features & Tile::F_RIVER ) {
			if ( ts->is_coastline ) {
				
				// apply river texture again on top of coastline border to erase beach

				ASSERT( ts->river_original, "river original texture not set" );

				const auto lt = Map::LAYER_WATER;
				const auto mode = Texture::AM_MERGE | Texture::AM_KEEP_TRANSPARENCY;

				if ( tile->NW->is_water_tile ) {
					m_map->SetTexture( lt, ts->NW, ts->river_original, mode | Texture::AM_MIRROR_X );
				}
				if ( tile->NE->is_water_tile ) {
					m_map->SetTexture( lt, ts->NE, ts->river_original, mode | Texture::AM_MIRROR_Y );
				}
				if ( tile->SE->is_water_tile ) {
					m_map->SetTexture( lt, ts->SE, ts->river_original, mode | Texture::AM_MIRROR_X );
				}
				if ( tile->SW->is_water_tile ) {
					m_map->SetTexture( lt, ts->SW, ts->river_original, mode | Texture::AM_MIRROR_Y );
				}
				
			}
			
			if ( ts->is_coastline_corner ) {
				// apply on top of current tile because on corner tiles perlin edge is drawn on same tile
				m_map->SetTexture( Map::LAYER_LAND, ts, ts->river_original, Texture::AM_MERGE );
				m_map->SetTexture( Map::LAYER_WATER, ts, ts->river_original, Texture::AM_MERGE );
			}
		}
	}
}

}
}
}
