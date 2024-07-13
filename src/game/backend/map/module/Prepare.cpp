#include "Prepare.h"

#include "game/backend/map/Map.h"
#include "game/backend/map/MapState.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/map/Consts.h"

namespace game {
namespace backend {
namespace map {
namespace module {

void Prepare::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	if ( ms->first_run ) {
		// set some defaults
		ts->coord.x = ms->coord.x + tile->coord.x * s_consts.tile.radius.x;
		ts->coord.y = ms->coord.y + tile->coord.y * s_consts.tile.radius.y;
		ts->tex_coord.x1 = tile->coord.x * s_consts.tc.texture_pcx.dimensions.x;
		ts->tex_coord.y1 = tile->coord.y * s_consts.tc.texture_pcx.dimensions.y;
		ts->tex_coord.x2 = ts->tex_coord.x1 + s_consts.tc.texture_pcx.dimensions.x;
		ts->tex_coord.y2 = ts->tex_coord.y1 + s_consts.tc.texture_pcx.dimensions.y;
		ts->tex_coord.x = ts->tex_coord.x1 + s_consts.tc.texture_pcx.radius.x;
		ts->tex_coord.y = ts->tex_coord.y1 + s_consts.tc.texture_pcx.radius.y;
	}
	else {
		// start drawing with empty state
		m_map->ClearTexture();
	}

	ts->elevations.left = *tile->elevation.left;
	ts->elevations.top = *tile->elevation.top;
	ts->elevations.right = *tile->elevation.right;
	ts->elevations.bottom = *tile->elevation.bottom;
	ts->elevations.center = *tile->elevation.center;

	// modify elevations based on water / not water, to avoid displaying half-submerged tiles
	// original tile isn't modified, this is just for rendering
	int8_t em = tile->is_water_tile
		? -1
		: 3; // setting -1 : 100 gives interesting shadow effect, but it's not very realistic
	if (
		( tile->is_water_tile != tile->W->is_water_tile ) ||
			( tile->is_water_tile != tile->NW->is_water_tile ) ||
			( tile->is_water_tile != tile->SW->is_water_tile )
		) {
		ts->elevations.left = tile::ELEVATION_LEVEL_COAST + em;
	}
	if (
		( tile->is_water_tile != tile->E->is_water_tile ) ||
			( tile->is_water_tile != tile->NE->is_water_tile ) ||
			( tile->is_water_tile != tile->SE->is_water_tile )
		) {
		ts->elevations.right = tile::ELEVATION_LEVEL_COAST + em;
	}
	if (
		( tile->is_water_tile != tile->N->is_water_tile ) ||
			( tile->is_water_tile != tile->NE->is_water_tile ) ||
			( tile->is_water_tile != tile->NW->is_water_tile )
		) {
		ts->elevations.top = tile::ELEVATION_LEVEL_COAST + em;
	}
	if (
		( tile->is_water_tile != tile->S->is_water_tile ) ||
			( tile->is_water_tile != tile->SE->is_water_tile ) ||
			( tile->is_water_tile != tile->SW->is_water_tile )
		) {
		ts->elevations.bottom = tile::ELEVATION_LEVEL_COAST + em;
	}

	if ( tile->is_water_tile ) {
		// do not allow anything above water on water tiles
		if ( ts->elevations.left >= tile::ELEVATION_LEVEL_COAST + em ) {
			ts->elevations.left = tile::ELEVATION_LEVEL_COAST + em;
		}
		if ( ts->elevations.right >= tile::ELEVATION_LEVEL_COAST + em ) {
			ts->elevations.right = tile::ELEVATION_LEVEL_COAST + em;
		}
		if ( ts->elevations.top >= tile::ELEVATION_LEVEL_COAST + em ) {
			ts->elevations.top = tile::ELEVATION_LEVEL_COAST + em;
		}
		if ( ts->elevations.bottom >= tile::ELEVATION_LEVEL_COAST + em ) {
			ts->elevations.bottom = tile::ELEVATION_LEVEL_COAST + em;
		}
	}
	else {
		// do not allow anything below water on land tiles
		if ( ts->elevations.left <= tile::ELEVATION_LEVEL_COAST - em ) {
			ts->elevations.left = tile::ELEVATION_LEVEL_COAST - em;
		}
		if ( ts->elevations.right <= tile::ELEVATION_LEVEL_COAST - em ) {
			ts->elevations.right = tile::ELEVATION_LEVEL_COAST - em;
		}
		if ( ts->elevations.top <= tile::ELEVATION_LEVEL_COAST - em ) {
			ts->elevations.top = tile::ELEVATION_LEVEL_COAST - em;
		}
		if ( ts->elevations.bottom <= tile::ELEVATION_LEVEL_COAST - em ) {
			ts->elevations.bottom = tile::ELEVATION_LEVEL_COAST - em;
		}
	}

	// average new center from computed corners
	ts->elevations.center = ( ts->elevations.left + ts->elevations.top + ts->elevations.right + ts->elevations.bottom ) / 4;

	for ( auto lt = 0 ; lt < tile::LAYER_MAX ; lt++ ) {

		// full color by default
		ts->layers[ lt ].colors = {
			types::Color{
				1.0,
				1.0,
				1.0,
				1.0
			},
			types::Color{
				1.0,
				1.0,
				1.0,
				1.0
			},
			types::Color{
				1.0,
				1.0,
				1.0,
				1.0
			},
			types::Color{
				1.0,
				1.0,
				1.0,
				1.0
			},
			types::Color{
				1.0,
				1.0,
				1.0,
				1.0
			},
		};

		ts->layers[ lt ].texture_stretch_at_edges = false;

	}

	ts->is_coastline_corner = !tile->is_water_tile && (
		( tile->W->is_water_tile && ( tile->NW->is_water_tile || tile->SW->is_water_tile ) ) ||
			( tile->N->is_water_tile && ( tile->NW->is_water_tile || tile->NE->is_water_tile ) ) ||
			( tile->E->is_water_tile && ( tile->NE->is_water_tile || tile->SE->is_water_tile ) ) ||
			( tile->S->is_water_tile && ( tile->SW->is_water_tile || tile->SE->is_water_tile ) )
	);

	ts->has_water = (
		ts->is_coastline_corner ||
			ts->elevations.center <= tile::ELEVATION_LEVEL_COAST ||
			ts->elevations.left <= tile::ELEVATION_LEVEL_COAST ||
			ts->elevations.top <= tile::ELEVATION_LEVEL_COAST ||
			ts->elevations.right <= tile::ELEVATION_LEVEL_COAST ||
			ts->elevations.bottom <= tile::ELEVATION_LEVEL_COAST
	);

}

}
}
}
}
