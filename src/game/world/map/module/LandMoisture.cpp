#include "LandMoisture.h"

namespace game {
namespace world {
namespace map {

void LandMoisture::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	const auto w = Map::s_consts.tile_texture.dimensions.x;
	const auto h = Map::s_consts.tile_texture.dimensions.y;
	
	Map::pcx_texture_coordinates_t tc = {};
	uint8_t rotate = 0;
	
	if ( !ts->moisture_original ) {
		NEW( ts->moisture_original, Texture, "MoistureOriginal", w, h );
	}

	auto add_flags = Texture::AM_DEFAULT;
	
	switch ( tile->moisture ) {
		case Tile::M_NONE: {
			// invisible tile (for dev/test purposes)
			break;
		}
		case Tile::M_ARID: {
			tc = Map::s_consts.tc.texture_pcx.arid[ 0 ];
			rotate = RandomRotate();
			add_flags = Texture::AM_RANDOM_STRETCH_SHUFFLE;
			break;
		}
		case Tile::M_MOIST: {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_MOISTURE );
			tc = Map::s_consts.tc.texture_pcx.moist[ txinfo.texture_variant ];
			rotate = txinfo.rotate_direction;
			add_flags = txinfo.texture_flags;
			break;
		}
		case Tile::M_RAINY: {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_MOISTURE );
			tc = Map::s_consts.tc.texture_pcx.rainy[ txinfo.texture_variant ];
			rotate = txinfo.rotate_direction;
			add_flags = txinfo.texture_flags;
			break;
		}
		default:
			ASSERT( false, "invalid moisture value" );
	}
	
	m_map->GetTexture( ts->moisture_original, tc, add_flags, rotate );
}

}
}
}
