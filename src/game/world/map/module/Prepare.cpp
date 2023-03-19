#include "Prepare.h"

namespace game {
namespace world {
namespace map {

void Prepare::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	if ( ms->first_run ) {
		// set some defaults
		ts->coord.x = ms->coord.x + tile->coord.x * Map::s_consts.tile.radius.x;
		ts->coord.y = ms->coord.y + tile->coord.y * Map::s_consts.tile.radius.y;
		ts->tex_coord.x1 = tile->coord.x * Map::s_consts.tile_texture.dimensions.x;
		ts->tex_coord.y1 = tile->coord.y * Map::s_consts.tile_texture.dimensions.y;
		ts->tex_coord.x2 = ts->tex_coord.x1 + Map::s_consts.tile_texture.dimensions.x;
		ts->tex_coord.y2 = ts->tex_coord.y1 + Map::s_consts.tile_texture.dimensions.y;
		ts->tex_coord.x = ts->tex_coord.x1 + Map::s_consts.tile_texture.radius.x;
		ts->tex_coord.y = ts->tex_coord.y1 + Map::s_consts.tile_texture.radius.y;
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
	int8_t em = tile->is_water_tile ? -1 : 3; // setting -1 : 100 gives interesting shadow effect, but it's not very realistic
	if (
		( tile->is_water_tile != tile->W->is_water_tile ) ||
		( tile->is_water_tile != tile->NW->is_water_tile ) ||
		( tile->is_water_tile != tile->SW->is_water_tile )
	) {
		ts->elevations.left = Tile::ELEVATION_LEVEL_COAST + em;
	}
	if (
		( tile->is_water_tile != tile->E->is_water_tile ) ||
		( tile->is_water_tile != tile->NE->is_water_tile ) ||
		( tile->is_water_tile != tile->SE->is_water_tile )
	) {
		ts->elevations.right = Tile::ELEVATION_LEVEL_COAST + em;
	}
	if (
		( tile->is_water_tile != tile->N->is_water_tile ) ||
		( tile->is_water_tile != tile->NE->is_water_tile ) ||
		( tile->is_water_tile != tile->NW->is_water_tile )
	) {
		ts->elevations.top = Tile::ELEVATION_LEVEL_COAST + em;
	}
	if (
		( tile->is_water_tile != tile->S->is_water_tile ) ||
		( tile->is_water_tile != tile->SE->is_water_tile ) ||
		( tile->is_water_tile != tile->SW->is_water_tile )
	) {
		ts->elevations.bottom = Tile::ELEVATION_LEVEL_COAST + em;
	}
	
	if ( tile->is_water_tile ) {
		// do not allow anything above water on water tiles
		if ( ts->elevations.left >= Tile::ELEVATION_LEVEL_COAST + em )
			ts->elevations.left = Tile::ELEVATION_LEVEL_COAST + em;
		if ( ts->elevations.right >= Tile::ELEVATION_LEVEL_COAST + em )
			ts->elevations.right = Tile::ELEVATION_LEVEL_COAST + em;
		if ( ts->elevations.top >= Tile::ELEVATION_LEVEL_COAST + em )
			ts->elevations.top = Tile::ELEVATION_LEVEL_COAST + em;
		if ( ts->elevations.bottom >= Tile::ELEVATION_LEVEL_COAST + em )
			ts->elevations.bottom = Tile::ELEVATION_LEVEL_COAST + em;
	}
	else {
		// do not allow anything below water on land tiles
		if ( ts->elevations.left <= Tile::ELEVATION_LEVEL_COAST - em )
			ts->elevations.left = Tile::ELEVATION_LEVEL_COAST - em;
		if ( ts->elevations.right <= Tile::ELEVATION_LEVEL_COAST - em )
			ts->elevations.right = Tile::ELEVATION_LEVEL_COAST - em;
		if ( ts->elevations.top <= Tile::ELEVATION_LEVEL_COAST - em )
			ts->elevations.top = Tile::ELEVATION_LEVEL_COAST - em;
		if ( ts->elevations.bottom <= Tile::ELEVATION_LEVEL_COAST - em )
			ts->elevations.bottom = Tile::ELEVATION_LEVEL_COAST - em;
	}
	
	// average new center from computed corners
	ts->elevations.center = ( ts->elevations.left + ts->elevations.top + ts->elevations.right + ts->elevations.bottom ) / 4;
	
	for ( auto lt = 0 ; lt < Map::LAYER_MAX ; lt++ ) {
	
		// full color by default
		ts->layers[ lt ].colors = {
			(Color){ 1.0, 1.0, 1.0, 1.0 },
			(Color){ 1.0, 1.0, 1.0, 1.0 },
			(Color){ 1.0, 1.0, 1.0, 1.0 },
			(Color){ 1.0, 1.0, 1.0, 1.0 },
			(Color){ 1.0, 1.0, 1.0, 1.0 },
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
		ts->elevations.center <= Tile::ELEVATION_LEVEL_COAST ||
		ts->elevations.left <= Tile::ELEVATION_LEVEL_COAST ||
		ts->elevations.top <= Tile::ELEVATION_LEVEL_COAST ||
		ts->elevations.right <= Tile::ELEVATION_LEVEL_COAST ||
		ts->elevations.bottom <= Tile::ELEVATION_LEVEL_COAST
	);
	
}

}
}
}
