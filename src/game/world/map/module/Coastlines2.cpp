#include "Coastlines2.h"

namespace game {
namespace world {
namespace map {

Coastlines2::Coastlines2( Map* const map )
	: Coastlines( map )
{
	NEW( m_perlin, util::Perlin, map->GetRandom()->GetUInt( 0, UINT32_MAX - 1 ) );
}

Coastlines2::~Coastlines2() {
	DELETE( m_perlin );
}

void Coastlines2::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	// fix coastline texture center alpha for corners
	
	if ( ts->is_coastline ) {
		if ( ts->is_coastline_corner ) {
			if (
				tile->W->is_water_tile &&
				( tile->SW->is_water_tile || tile->coord.y == ms->dimensions.y - 1 ) &&
				( tile->NW->is_water_tile || tile->coord.y == 0 )
			) {
				ts->W->layers[ Map::LAYER_WATER_SURFACE_EXTRA ].colors.center.value.alpha *= Map::s_consts.coastlines.coast_water_center_alpha_corner_mod;
			}
			if (
				( tile->N->is_water_tile || tile->coord.y <= 1 ) &&
				( tile->NW->is_water_tile || tile->coord.y == 0 ) &&
				( tile->NE->is_water_tile || tile->coord.y == 0 )
			) {
				ts->N->layers[ Map::LAYER_WATER_SURFACE_EXTRA ].colors.center.value.alpha *= Map::s_consts.coastlines.coast_water_center_alpha_corner_mod;
			}
			if (
				tile->E->is_water_tile &&
				( tile->SE->is_water_tile || tile->coord.y == ms->dimensions.y - 1 ) &&
				( tile->NE->is_water_tile || tile->coord.y == 0 )
			) {
				ts->E->layers[ Map::LAYER_WATER_SURFACE_EXTRA ].colors.center.value.alpha *= Map::s_consts.coastlines.coast_water_center_alpha_corner_mod;
			}
			if (
				( tile->S->is_water_tile || tile->coord.y <= ms->dimensions.y - 2 ) &&
				( tile->SE->is_water_tile || tile->coord.y == ms->dimensions.y - 1 ) &&
				( tile->SW->is_water_tile || tile->coord.y == ms->dimensions.y - 1 )
			) {
				ts->S->layers[ Map::LAYER_WATER_SURFACE_EXTRA ].colors.center.value.alpha *= Map::s_consts.coastlines.coast_water_center_alpha_corner_mod;
			}
		}
	}
	if ( tile->is_water_tile ) {
		if (
			( !tile->SW->is_water_tile && !tile->NW->is_water_tile ) ||
			( !tile->NW->is_water_tile && !tile->NE->is_water_tile ) ||
			( !tile->SE->is_water_tile && !tile->NE->is_water_tile ) ||
			( !tile->SE->is_water_tile && !tile->SW->is_water_tile )
		) {
			ts->layers[ Map::LAYER_WATER_SURFACE_EXTRA ].colors.center.value.alpha /= Map::s_consts.coastlines.coast_water_center_alpha_corner_mod / 2;
		}

		// add perlin borders where needed
		
		std::vector< coastline_corner_t > coastline_corners = {};
		coastline_corner_t coastline_corner_tmp = {};
		
		if ( !tile->NW->is_water_tile && tile->coord.y > 0 ) {
			coastline_corner_tmp = {};
			coastline_corner_tmp.flags = Texture::AM_MIRROR_X | Texture::AM_PERLIN_LEFT;
			if ( tile->N->is_water_tile ) {
				coastline_corner_tmp.flags |= Texture::AM_PERLIN_CUT_TOP;
			}
			if ( tile->W->is_water_tile ) {
				coastline_corner_tmp.flags |= Texture::AM_PERLIN_CUT_BOTTOM;
			}
			if ( tile->coord.x > 0 ) {
				coastline_corner_tmp.msx = tile->coord.x - 1;
			}
			else {
				coastline_corner_tmp.msx = ms->dimensions.x - 1;
			}
			coastline_corner_tmp.msy = tile->coord.y - 1;
			coastline_corners.push_back( coastline_corner_tmp );
		}
		if ( !tile->NE->is_water_tile && tile->coord.y > 0 ) {
			coastline_corner_tmp = {};
			coastline_corner_tmp.flags |= Texture::AM_MIRROR_Y | Texture::AM_PERLIN_TOP;
			if ( tile->N->is_water_tile ) {
				coastline_corner_tmp.flags |= Texture::AM_PERLIN_CUT_LEFT;
			}
			if ( tile->E->is_water_tile ) {
				coastline_corner_tmp.flags |= Texture::AM_PERLIN_CUT_RIGHT;
			}
			if ( tile->coord.x < ms->dimensions.x - 1 ) {
				coastline_corner_tmp.msx = tile->coord.x + 1;
			}
			else {
				coastline_corner_tmp.msx = 0;
			}
			coastline_corner_tmp.msy = tile->coord.y - 1;
			coastline_corners.push_back( coastline_corner_tmp );
		}
		if ( !tile->SE->is_water_tile && tile->coord.y < ms->dimensions.y - 1 ) {
			coastline_corner_tmp = {};
			coastline_corner_tmp.flags |= Texture::AM_MIRROR_X | Texture::AM_PERLIN_RIGHT;
			if ( tile->E->is_water_tile ) {
				coastline_corner_tmp.flags |= Texture::AM_PERLIN_CUT_TOP;
			}
			if ( tile->S->is_water_tile ) {
				coastline_corner_tmp.flags |= Texture::AM_PERLIN_CUT_BOTTOM;
			}
			if ( tile->coord.x < ms->dimensions.x - 1 ) {
				coastline_corner_tmp.msx = tile->coord.x + 1;
			}
			else {
				coastline_corner_tmp.msx = 0;
			}
			coastline_corner_tmp.msy = tile->coord.y + 1;
			coastline_corners.push_back( coastline_corner_tmp );
		}
		if ( !tile->SW->is_water_tile && tile->coord.y < ms->dimensions.y - 1 ) {
			coastline_corner_tmp = {};
			coastline_corner_tmp.flags |= Texture::AM_MIRROR_Y | Texture::AM_PERLIN_BOTTOM;
			if ( tile->W->is_water_tile ) {
				coastline_corner_tmp.flags |= Texture::AM_PERLIN_CUT_LEFT;
			}
			if ( tile->S->is_water_tile ) {
				coastline_corner_tmp.flags |= Texture::AM_PERLIN_CUT_RIGHT;
			}
			if ( tile->coord.x > 0 ) {
				coastline_corner_tmp.msx = tile->coord.x - 1;
			}
			else {
				coastline_corner_tmp.msx = ms->dimensions.x - 1;
			}
			coastline_corner_tmp.msy = tile->coord.y + 1;
			coastline_corners.push_back( coastline_corner_tmp );
		}

		for ( auto& c : coastline_corners ) {
			ASSERT( ( c.msx % 2 ) == ( c.msy % 2 ), "msx and msy oddity differs" );
			
			/*const float pb = m_perlin->Noise(
				( ( c.msx + tile->coord.x ) * ms->dimensions.x + c.flags ) * 10000,
				( ( c.msy + tile->coord.y ) * ms->dimensions.y + c.flags ) * 10000,
				0.5f
			) / 2 + 0.5f;*/
			const float pb = (float)m_map->GetRandom()->GetUInt( 1, 1000000 ) / 1000000;
			
			m_map->CopyTextureDeferred(
				Map::LAYER_LAND,
				c.msx * Map::s_consts.pcx_texture_block.dimensions.x,
				c.msy * Map::s_consts.pcx_texture_block.dimensions.y,
				Map::LAYER_WATER,
				Texture::AM_MERGE | c.flags | Texture::AM_COASTLINE_BORDER,
				0,
				pb,
				m_perlin
			);
		}

	}
}

}
}
}
