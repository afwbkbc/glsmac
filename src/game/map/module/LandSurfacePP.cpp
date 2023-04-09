#include "LandSurfacePP.h"

namespace game {
namespace map {
namespace module {

void LandSurfacePP::GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) {
	
	if ( !tile->is_water_tile ) {
		
		if ( tile->features & Tile::F_RIVER ) {
			if ( ts->has_water ) {
				
				// apply river texture again on top of coastline border to erase beach

				ASSERT( ts->river_original, "river original texture not set" );

				const auto lt = TileState::LAYER_WATER;
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
				m_map->SetTexture( TileState::LAYER_LAND, ts, ts->river_original, Texture::AM_MERGE );
				m_map->SetTexture( TileState::LAYER_WATER, ts, ts->river_original, Texture::AM_MERGE );
			}
		}
	}
}

}
}
}
