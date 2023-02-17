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
			
			Texture::add_mode_t add_mode = Texture::AM_DEFAULT;

			if ( t == tile->NW ) {
				add_mode = Texture::AM_GRADIENT_LEFT;
			}
			else if ( t == tile->N ) {
				add_mode = Texture::AM_GRADIENT_LEFT | Texture::AM_GRADIENT_TOP;
			}
			else if ( t == tile->NE ) {
				add_mode = Texture::AM_GRADIENT_TOP;
			}
			else if ( t == tile->E ) {
				add_mode = Texture::AM_GRADIENT_TOP | Texture::AM_GRADIENT_RIGHT;
			}
			else if ( t == tile->SE ) {
				add_mode = Texture::AM_GRADIENT_RIGHT;
			}
			else if ( t == tile->S ) {
				add_mode = Texture::AM_GRADIENT_RIGHT | Texture::AM_GRADIENT_BOTTOM;
			}
			else if ( t == tile->SW ) {
				add_mode = Texture::AM_GRADIENT_BOTTOM;
			}
			else if ( t == tile->W ) {
				add_mode = Texture::AM_GRADIENT_BOTTOM | Texture::AM_GRADIENT_LEFT;
			}
			
			if ( add_mode & ( Texture::AM_GRADIENT_LEFT | Texture::AM_GRADIENT_RIGHT ) ) {
				add_mode |= Texture::AM_MIRROR_X;
			}
			if ( add_mode & ( Texture::AM_GRADIENT_TOP | Texture::AM_GRADIENT_BOTTOM ) ) {
				add_mode |= Texture::AM_MIRROR_Y;
			}

			ASSERT( add_mode != Texture::AM_DEFAULT, "could not determine moisture add mode" );
			
			float ratio = t->moisture == Tile::M_RAINY ? 0.7f : 0.5f;
			
			ratio = 1.0f;
			
			m_map->SetTexture( Map::LAYER_LAND, src, Texture::AM_MERGE | Texture::AM_GRADIENT_TIGHTER | add_mode, 0, ratio );
		}
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
	
	if ( ( tile->features & Tile::F_XENOFUNGUS ) && !tile->is_water_tile ) {
		auto txinfo = m_map->GetTileTextureInfo( tile, Map::TG_FEATURE, Tile::F_XENOFUNGUS );
		if ( !ts->has_water && txinfo.texture_variant >= 14 ) {
			ts->layers[ Map::LAYER_LAND ].stronger_texture_stretch = true;
		}
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
