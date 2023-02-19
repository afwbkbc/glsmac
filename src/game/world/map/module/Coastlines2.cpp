#include "Coastlines2.h"

namespace game {
namespace world {
namespace map {

Coastlines2::Coastlines2( Map* const map )
	: Module( map )
{
	NEW( m_perlin, util::Perlin, map->GetRandom()->GetUInt( 0, UINT32_MAX - 1 ) );
}

Coastlines2::~Coastlines2() {
	DELETE( m_perlin );
}

void Coastlines2::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	// fix coastline texture center alpha for corners
	
	if ( ts->has_water ) {
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

	}
}

}
}
}
