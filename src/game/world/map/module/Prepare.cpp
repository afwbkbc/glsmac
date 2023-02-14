#include "Prepare.h"

namespace game {
namespace world {
namespace map {

void Prepare::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	// set some defaults
	ts->coord.x = ms->coord.x + tile->coord.x * Map::s_consts.tile.radius.x;
	ts->coord.y = ms->coord.y + tile->coord.y * Map::s_consts.tile.radius.y;
	ts->tex_coord.x1 = tile->coord.x * Map::s_consts.pcx_texture_block.dimensions.x;
	ts->tex_coord.y1 = tile->coord.y * Map::s_consts.pcx_texture_block.dimensions.y;
	ts->tex_coord.x2 = ts->tex_coord.x1 + Map::s_consts.pcx_texture_block.dimensions.x;
	ts->tex_coord.y2 = ts->tex_coord.y1 + Map::s_consts.pcx_texture_block.dimensions.y;
	ts->tex_coord.x = ts->tex_coord.x1 + Map::s_consts.pcx_texture_block.radius.x;
	ts->tex_coord.y = ts->tex_coord.y1 + Map::s_consts.pcx_texture_block.radius.y;
	
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
	
	// each tile has one 'public' stretch value (for bottom vertex), and one 'private' for center
	ts->texture_stretch = {
		m_map->GetRandom()->GetFloat( 0, Map::s_consts.tile.texture_stretch_randomness ), // x
		m_map->GetRandom()->GetFloat( 0, Map::s_consts.tile.texture_stretch_randomness ), // y
	};
		
	for ( auto lt = 0 ; lt < Map::LAYER_MAX ; lt++ ) {
	
		// vertex coodinates for tile corners ( everything but land is flat at sea level )
		#define vcz( _lt, _val ) ( ( _lt == Map::LAYER_LAND ) ? Map::s_consts.clampers.elevation_to_vertex_z.Clamp( _val ) : Map::s_consts.levels.water )

		ts->layers[ lt ].coords.left = { ts->coord.x - Map::s_consts.tile.radius.x, ts->coord.y, vcz( lt, ts->elevations.left ) };
		ts->layers[ lt ].coords.top = { ts->coord.x, ts->coord.y - Map::s_consts.tile.radius.y, vcz( lt, ts->elevations.top ) };
		ts->layers[ lt ].coords.right = { ts->coord.x + Map::s_consts.tile.radius.x, ts->coord.y, vcz( lt, ts->elevations.right ) };
		ts->layers[ lt ].coords.bottom = { ts->coord.x, ts->coord.y + Map::s_consts.tile.radius.y, vcz( lt, ts->elevations.bottom ) };
		ts->layers[ lt ].coords.center = { ( ts->layers[ lt ].coords.left.x + ts->layers[ Map::LAYER_LAND ].coords.right.x ) / 2, ( ts->layers[ Map::LAYER_LAND ].coords.top.y + ts->layers[ Map::LAYER_LAND ].coords.bottom.y ) / 2, vcz( lt, ts->elevations.center ) };

		float ofs = 1.0f; // remove lines between tiles
		
		// initialize tex_coords with some stretching
		ts->layers[ lt ].tex_coords = {
			{ ts->tex_coord.x + m_map->GetRandom()->GetFloat( -Map::s_consts.tile.texture_stretch_randomness, Map::s_consts.tile.texture_stretch_randomness ), ts->tex_coord.y + m_map->GetRandom()->GetFloat( -Map::s_consts.tile.texture_stretch_randomness, Map::s_consts.tile.texture_stretch_randomness ) }, // center
			{ ts->tex_coord.x1 + ofs + ts->NW->texture_stretch.x, ts->tex_coord.y2 - ofs - ts->NW->texture_stretch.y }, // left
			{ ts->tex_coord.x1 + ofs + ts->N->texture_stretch.x, ts->tex_coord.y1 + ofs + ts->N->texture_stretch.y }, // top
			{ ts->tex_coord.x2 - ofs - ts->NE->texture_stretch.x, ts->tex_coord.y1 + ofs + ts->NE->texture_stretch.x }, // right
			{ ts->tex_coord.x2 - ofs - ts->texture_stretch.x, ts->tex_coord.y2 - ofs - ts->texture_stretch.y } // bottom
		};
		
		ts->layers[ lt ].colors = {
			(Color){ 1.0, 1.0, 1.0, 1.0 },
			(Color){ 1.0, 1.0, 1.0, 1.0 },
			(Color){ 1.0, 1.0, 1.0, 1.0 },
			(Color){ 1.0, 1.0, 1.0, 1.0 },
			(Color){ 1.0, 1.0, 1.0, 1.0 },
		};
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
