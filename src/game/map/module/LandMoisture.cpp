#include "LandMoisture.h"

#include "types/texture/Texture.h"
#include "game/map/Map.h"
#include "game/map/Consts.h"
#include "game/map/tile/Tile.h"
#include "game/map/tile/TileState.h"

namespace game {
namespace map {
namespace module {

void LandMoisture::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	const auto w = s_consts.tc.texture_pcx.dimensions.x;
	const auto h = s_consts.tc.texture_pcx.dimensions.y;

	pcx_texture_coordinates_t tc = {};
	uint8_t rotate = 0;

	if ( !ts->moisture_original ) {
		NEW( ts->moisture_original, types::texture::Texture, "MoistureOriginal", w, h );
	}

	auto add_flags = types::texture::AM_DEFAULT;

	switch ( tile->moisture ) {
		case tile::MOISTURE_NONE: {
			// invisible tile (for dev/test purposes)
			break;
		}
		case tile::MOISTURE_ARID: {
			tc = s_consts.tc.texture_pcx.arid[ 0 ];
			rotate = RandomRotate();
			add_flags = types::texture::AM_RANDOM_STRETCH_SHUFFLE;
			break;
		}
		case tile::MOISTURE_MOIST: {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_MOISTURE );
			tc = s_consts.tc.texture_pcx.moist[ txinfo.texture_variant ];
			rotate = txinfo.rotate_direction;
			add_flags = txinfo.texture_flags;
			break;
		}
		case tile::MOISTURE_RAINY: {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_MOISTURE );
			tc = s_consts.tc.texture_pcx.rainy[ txinfo.texture_variant ];
			rotate = txinfo.rotate_direction;
			add_flags = txinfo.texture_flags;
			break;
		}
		default:
			THROW( "invalid moisture value" );
	}

	m_map->GetTexture( ts->moisture_original, tc, add_flags, rotate );
}

}
}
}
