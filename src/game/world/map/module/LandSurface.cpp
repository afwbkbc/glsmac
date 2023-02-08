#include "LandSurface.h"

namespace game {
namespace world {
namespace map {

void LandSurface::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	switch ( tile->moisture ) {
		case Tile::M_NONE: {
			// invisible tile (for dev/test purposes)
			break;
		}
		case Tile::M_ARID: {
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.pcx_textures.arid[ 0 ],
				Texture::AM_DEFAULT,
				RandomRotate()
			);
			break;
		}
		case Tile::M_MOIST: {
			auto txinfo = m_map->GetTileTextureInfo( tile, Map::TG_MOISTURE );
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.pcx_textures.moist[ txinfo.texture_variant ],
				Texture::AM_DEFAULT,
				txinfo.rotate_direction
			);
			break;
		}
		case Tile::M_RAINY: {
			auto txinfo = m_map->GetTileTextureInfo( tile, Map::TG_MOISTURE );
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.pcx_textures.rainy[ txinfo.texture_variant ],
				Texture::AM_DEFAULT,
				txinfo.rotate_direction
			);
			break;
		}
		default:
			ASSERT( false, "invalid moisture value" );
	}
	
	switch ( tile->rockyness ) {
		case Tile::R_NONE:
		case Tile::R_FLAT: {
			// nothing
			break;
		}
		case Tile::R_ROLLING: {
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.pcx_textures.rocks[ m_map->GetRandom()->GetUInt( 0, 1 ) * 2 ],
				Texture::AM_MERGE,
				RandomRotate()
			);
			break;
		}
		case Tile::R_ROCKY: {
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.pcx_textures.rocks[ m_map->GetRandom()->GetUInt( 0, 1 ) * 2 + 1 ],
				Texture::AM_MERGE,
				RandomRotate()
			);
			break;
		}
		default:
			ASSERT( false, "invalid rockyness value" );
	}
	
	if ( tile->features	& Tile::F_JUNGLE ) {
		auto txinfo = m_map->GetTileTextureInfo( tile, Map::TG_FEATURE, Tile::F_JUNGLE );
		m_map->AddTexture(
			Map::LAYER_LAND,
			Map::s_consts.pcx_textures.jungle[ txinfo.texture_variant ],
			Texture::AM_MERGE,
			txinfo.rotate_direction
		);
	}
	
	if ( tile->features & Tile::F_XENOFUNGUS ) {
		auto txinfo = m_map->GetTileTextureInfo( tile, Map::TG_FEATURE, Tile::F_XENOFUNGUS );
		
		if ( tile->is_water_tile || ts->is_coastline_corner ) {
			m_map->AddTexture(
				Map::LAYER_WATER,
				Map::s_consts.pcx_textures.fungus_sea[ txinfo.texture_variant ],
				Texture::AM_MERGE,
				txinfo.rotate_direction
			);
		}
		if ( !tile->is_water_tile ) {
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.pcx_textures.fungus_land[ txinfo.texture_variant ],
				Texture::AM_MERGE,
				txinfo.rotate_direction
			);
		}
	}
}

}
}
}
