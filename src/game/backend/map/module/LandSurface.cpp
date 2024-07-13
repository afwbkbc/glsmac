#include "LandSurface.h"

#include "types/texture/Texture.h"
#include "game/backend/map/Map.h"
#include "game/backend/map/Consts.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/map/tile/TileState.h"
#include "util/random/Random.h"

namespace game {
namespace backend {
namespace map {
namespace module {

void LandSurface::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	ASSERT( ts->moisture_original, "moisture original texture not set" );

	m_map->SetTexture( tile::LAYER_LAND, ts->moisture_original, types::texture::AM_DEFAULT );

	// blend a bit from rainy to non-rainy and vice versa
	for ( auto& t : tile->neighbours ) {
		if ( !t->is_water_tile && ( t->moisture == tile::MOISTURE_RAINY ) != ( tile->moisture == tile::MOISTURE_RAINY ) ) {

			// TODO: add pointer connection between tile and tile_t?
			auto src = m_map->GetTileState( t->coord.x, t->coord.y )->moisture_original;

			types::texture::add_flag_t add_flags = types::texture::AM_DEFAULT;

			if ( t == tile->NW ) {
				add_flags = types::texture::AM_GRADIENT_LEFT;
			}
			else if ( t == tile->N ) {
				add_flags = types::texture::AM_GRADIENT_LEFT | types::texture::AM_GRADIENT_TOP;
			}
			else if ( t == tile->NE ) {
				add_flags = types::texture::AM_GRADIENT_TOP;
			}
			else if ( t == tile->E ) {
				add_flags = types::texture::AM_GRADIENT_TOP | types::texture::AM_GRADIENT_RIGHT;
			}
			else if ( t == tile->SE ) {
				add_flags = types::texture::AM_GRADIENT_RIGHT;
			}
			else if ( t == tile->S ) {
				add_flags = types::texture::AM_GRADIENT_RIGHT | types::texture::AM_GRADIENT_BOTTOM;
			}
			else if ( t == tile->SW ) {
				add_flags = types::texture::AM_GRADIENT_BOTTOM;
			}
			else if ( t == tile->W ) {
				add_flags = types::texture::AM_GRADIENT_BOTTOM | types::texture::AM_GRADIENT_LEFT;
			}

			if ( add_flags & ( types::texture::AM_GRADIENT_LEFT | types::texture::AM_GRADIENT_RIGHT ) ) {
				add_flags |= types::texture::AM_MIRROR_X;
			}
			if ( add_flags & ( types::texture::AM_GRADIENT_TOP | types::texture::AM_GRADIENT_BOTTOM ) ) {
				add_flags |= types::texture::AM_MIRROR_Y;
			}

			ASSERT( add_flags != types::texture::AM_DEFAULT, "could not determine moisture add flags" );

			m_map->SetTexture(
				tile::LAYER_LAND, src,
				add_flags |
					types::texture::AM_MERGE |
					types::texture::AM_GRADIENT_TIGHTER |
					types::texture::AM_RANDOM_STRETCH, 0, 0.72f
			);
		}
	}

	// add underwater color
	if ( tile->is_water_tile ) {
#define x( _k ) ts->layers[ tile::LAYER_LAND ].colors._k = s_consts.underwater_tint;
		x( center );
		x( left );
		x( top );
		x( right );
		x( bottom );
#undef x
	}

	// add map details
	// order is important (textures are drawn on top of previous ones)

	if ( tile->features & tile::FEATURE_DUNES ) {
		m_map->AddTexture(
			tile::LAYER_LAND,
			s_consts.tc.texture_pcx.dunes[ 0 ],
			types::texture::AM_DEFAULT,
			0
		);
	}

	switch ( tile->rockiness ) {
		case tile::ROCKINESS_NONE:
		case tile::ROCKINESS_FLAT: {
			// nothing
			break;
		}
		case tile::ROCKINESS_ROLLING: {
			m_map->AddTexture(
				tile::LAYER_LAND,
				s_consts.tc.texture_pcx.rocks[ m_map->GetRandom()->GetUInt( 0, 1 ) * 2 ],
				types::texture::AM_MERGE | types::texture::AM_RANDOM_STRETCH | types::texture::AM_RANDOM_STRETCH_SHRINK | types::texture::AM_RANDOM_STRETCH_SHIFT,
				RandomRotate()
			);
			break;
		}
		case tile::ROCKINESS_ROCKY: {
			m_map->AddTexture(
				tile::LAYER_LAND,
				s_consts.tc.texture_pcx.rocks[ m_map->GetRandom()->GetUInt( 0, 1 ) * 2 + 1 ],
				types::texture::AM_MERGE | types::texture::AM_RANDOM_STRETCH | types::texture::AM_RANDOM_STRETCH_SHRINK,
				RandomRotate()
			);
			break;
		}
		default:
			THROW( "invalid rockiness value" );
	}

	if ( tile->features & tile::FEATURE_JUNGLE ) {
		auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_FEATURE, tile::FEATURE_JUNGLE );
		m_map->AddTexture(
			tile::LAYER_LAND,
			s_consts.tc.texture_pcx.jungle[ txinfo.texture_variant ],
			types::texture::AM_MERGE | txinfo.texture_flags,
			txinfo.rotate_direction
		);
	}

	if ( !tile->is_water_tile ) {

		if ( tile->terraforming & tile::TERRAFORMING_FARM || tile->terraforming & tile::TERRAFORMING_SOIL_ENRICHER ) {
			// TODO: select based on nutrients yields instead of moisture
			m_map->AddTexture(
				tile::LAYER_LAND,
				s_consts.tc.texture_pcx.farm[ m_map->GetRandom()->GetUInt( 0, 2 ) * 3 + ( tile->moisture - 1 ) ],
				types::texture::AM_MERGE,
				RandomRotate()
			);
		}

		if ( tile->terraforming & tile::TERRAFORMING_FOREST ) {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_RIVERS_FORESTS, tile, Map::TG_TERRAFORMING, tile::TERRAFORMING_FOREST );
			auto& tc = s_consts.tc.texture_pcx.forest[ txinfo.texture_variant ];
			auto add_flags = types::texture::AM_MERGE | txinfo.texture_flags;
			m_map->AddTexture(
				tile::LAYER_LAND,
				tc,
				add_flags,
				txinfo.rotate_direction
			);
		}

		if ( tile->features & tile::FEATURE_XENOFUNGUS ) {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_FEATURE, tile::FEATURE_XENOFUNGUS );
			m_map->AddTexture(
				tile::LAYER_LAND,
				s_consts.tc.texture_pcx.fungus_land[ txinfo.texture_variant ],
				types::texture::AM_MERGE | txinfo.texture_flags,
				txinfo.rotate_direction
			);
		}

		if ( tile->features & tile::FEATURE_RIVER ) {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_RIVERS_FORESTS, tile, Map::TG_FEATURE, tile::FEATURE_RIVER );
			auto& tc = s_consts.tc.texture_pcx.river[ txinfo.texture_variant ];
			auto add_flags = types::texture::AM_MERGE | txinfo.texture_flags;
			m_map->AddTexture(
				tile::LAYER_LAND,
				tc,
				add_flags,
				txinfo.rotate_direction
			);
			if ( ts->has_water ) {
				// need to save original river texture to draw it on top of coastline border later (to erase 'beach' on river exit)
				if ( !ts->river_original ) {
					NEW(
						ts->river_original,
						types::texture::Texture,
						"RiverOriginal",
						s_consts.tc.texture_pcx.dimensions.x,
						s_consts.tc.texture_pcx.dimensions.y
					);
				}
				m_map->GetTexture( ts->river_original, tc, add_flags, txinfo.rotate_direction );
			}
		}

		for ( const auto& t : {
			tile::TERRAFORMING_ROAD,
			tile::TERRAFORMING_MAG_TUBE
		} ) {
			if ( tile->terraforming & t ) {
				std::vector< uint8_t > road_variants = {};
				road_variants.reserve( 9 ); // to minimize reallocations

#define x( _side, _variant ) { \
                    if ( tile->_side->terraforming & t ) \
                        road_variants.push_back( _variant ); \
                    }
				x( NE, 1 );
				x( E, 2 );
				x( SE, 3 );
				x( S, 4 );
				x( SW, 5 );
				x( W, 6 );
				x( NW, 7 );
				x( N, 8 );
#undef x

				if ( road_variants.empty() ) {
					road_variants.push_back( 0 );
				}

				for ( auto& variant : road_variants ) {
					m_map->AddTexture(
						tile::LAYER_LAND,
						t == tile::TERRAFORMING_ROAD
							? s_consts.tc.texture_pcx.road[ variant ]
							: s_consts.tc.texture_pcx.mag_tube[ variant ],
						types::texture::AM_MERGE,
						0
					);
				}
			}
		}
	}
}

}
}
}
}
