#include "LandSurface.h"

namespace game {
namespace world {
namespace map {

void LandSurface::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	ASSERT( ts->moisture_original, "moisture original texture not set" );
	
	m_map->SetTexture( Map::LAYER_LAND, ts->moisture_original, Texture::AM_DEFAULT );
	
	// blend a bit from rainy to non-rainy and vice versa
	for ( auto& t : tile->neighbours ) {
		if ( !t->is_water_tile && ( t->moisture == Tile::M_RAINY ) != ( tile->moisture == Tile::M_RAINY ) ) {
			
			// TODO: add pointer connection between tile and tile_state_t?
			auto src = m_map->GetTileState( t->coord.x, t->coord.y )->moisture_original;
			
			Texture::add_flag_t add_flags = Texture::AM_DEFAULT;

			if ( t == tile->NW ) {
				add_flags = Texture::AM_GRADIENT_LEFT;
			}
			else if ( t == tile->N ) {
				add_flags = Texture::AM_GRADIENT_LEFT | Texture::AM_GRADIENT_TOP;
			}
			else if ( t == tile->NE ) {
				add_flags = Texture::AM_GRADIENT_TOP;
			}
			else if ( t == tile->E ) {
				add_flags = Texture::AM_GRADIENT_TOP | Texture::AM_GRADIENT_RIGHT;
			}
			else if ( t == tile->SE ) {
				add_flags = Texture::AM_GRADIENT_RIGHT;
			}
			else if ( t == tile->S ) {
				add_flags = Texture::AM_GRADIENT_RIGHT | Texture::AM_GRADIENT_BOTTOM;
			}
			else if ( t == tile->SW ) {
				add_flags = Texture::AM_GRADIENT_BOTTOM;
			}
			else if ( t == tile->W ) {
				add_flags = Texture::AM_GRADIENT_BOTTOM | Texture::AM_GRADIENT_LEFT;
			}
			
			if ( add_flags & ( Texture::AM_GRADIENT_LEFT | Texture::AM_GRADIENT_RIGHT ) ) {
				add_flags |= Texture::AM_MIRROR_X;
			}
			if ( add_flags & ( Texture::AM_GRADIENT_TOP | Texture::AM_GRADIENT_BOTTOM ) ) {
				add_flags |= Texture::AM_MIRROR_Y;
			}

			ASSERT( add_flags != Texture::AM_DEFAULT, "could not determine moisture add flags" );
			
			m_map->SetTexture( Map::LAYER_LAND, src,
				add_flags |
				Texture::AM_MERGE |
				Texture::AM_GRADIENT_TIGHTER |
				Texture::AM_RANDOM_STRETCH
			, 0, 0.72f );
		}
	}
	
	// add underwater color
	if ( tile->is_water_tile ) {
		#define x( _k ) ts->layers[ Map::LAYER_LAND ].colors._k = Map::s_consts.underwater_tint;
			x( center );
			x( left );
			x( top );
			x( right );
			x( bottom );
		#undef x
	}
	
	// add map details
	// order is important (textures are drawn on top of previous ones)
	
	switch ( tile->rockyness ) {
		case Tile::R_NONE:
		case Tile::R_FLAT: {
			// nothing
			break;
		}
		case Tile::R_ROLLING: {
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.tc.texture_pcx.rocks[ m_map->GetRandom()->GetUInt( 0, 1 ) * 2 ],
				Texture::AM_MERGE | Texture::AM_RANDOM_STRETCH | Texture::AM_RANDOM_STRETCH_SHRINK | Texture::AM_RANDOM_STRETCH_SHIFT,
				RandomRotate()
			);
			break;
		}
		case Tile::R_ROCKY: {
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.tc.texture_pcx.rocks[ m_map->GetRandom()->GetUInt( 0, 1 ) * 2 + 1 ],
				Texture::AM_MERGE | Texture::AM_RANDOM_STRETCH | Texture::AM_RANDOM_STRETCH_SHRINK,
				RandomRotate()
			);
			break;
		}
		default:
			ASSERT( false, "invalid rockyness value" );
	}
	
	if ( tile->features	& Tile::F_JUNGLE ) {
		auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_FEATURE, Tile::F_JUNGLE );
		m_map->AddTexture(
			Map::LAYER_LAND,
			Map::s_consts.tc.texture_pcx.jungle[ txinfo.texture_variant ],
			Texture::AM_MERGE | txinfo.texture_flags,
			txinfo.rotate_direction
		);
	}
	
	if ( !tile->is_water_tile ) {
		
		if ( tile->features & Tile::F_XENOFUNGUS ) {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_TILES, tile, Map::TG_FEATURE, Tile::F_XENOFUNGUS );
			m_map->AddTexture(
				Map::LAYER_LAND,
				Map::s_consts.tc.texture_pcx.fungus_land[ txinfo.texture_variant ],
				Texture::AM_MERGE | txinfo.texture_flags,
				txinfo.rotate_direction
			);
		}

		if ( tile->features & Tile::F_RIVER ) {
			auto txinfo = m_map->GetTileTextureInfo( Map::TVT_RIVERS, tile, Map::TG_FEATURE, Tile::F_RIVER );
			auto& tc = Map::s_consts.tc.texture_pcx.river[ txinfo.texture_variant ];
			auto add_flags = Texture::AM_MERGE | txinfo.texture_flags;
			m_map->AddTexture(
				Map::LAYER_LAND,
				tc,
				add_flags,
				txinfo.rotate_direction
			);
			if ( ts->has_water ) {
				// need to save original river texture to draw it on top of coastline border later (to erase 'beach' on river exit)
				if ( !ts->river_original ) {
					NEW(
						ts->river_original,
						Texture,
						"RiverOriginal",
						Map::s_consts.tile_texture.dimensions.x,
						Map::s_consts.tile_texture.dimensions.y
					);
				}
				m_map->GetTexture( ts->river_original, tc, add_flags, txinfo.rotate_direction );
			}
		}
		
	}
}

}
}
}
