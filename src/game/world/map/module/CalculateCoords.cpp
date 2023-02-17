#include "CalculateCoords.h"

namespace game {
namespace world {
namespace map {

void CalculateCoords::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {
	
	for ( auto lt = 0 ; lt < Map::LAYER_MAX ; lt++ ) {
	
		// each tile has one 'public' stretch value (for bottom vertex), and one 'private' for center
		ts->layers[ lt ].texture_stretch = {
			m_map->GetRandom()->GetFloat( 0, Map::s_consts.tile.texture_stretch_randomness ), // x
			m_map->GetRandom()->GetFloat( 0, Map::s_consts.tile.texture_stretch_randomness ), // y
		};

		// vertex coodinates for tile corners ( everything but land is flat at sea level )
		#define vcz( _lt, _val ) ( ( _lt == Map::LAYER_LAND ) ? Map::s_consts.clampers.elevation_to_vertex_z.Clamp( _val ) : Map::s_consts.levels.water )

		ts->layers[ lt ].coords.left = { ts->coord.x - Map::s_consts.tile.radius.x, ts->coord.y, vcz( lt, ts->elevations.left ) };
		ts->layers[ lt ].coords.top = { ts->coord.x, ts->coord.y - Map::s_consts.tile.radius.y, vcz( lt, ts->elevations.top ) };
		ts->layers[ lt ].coords.right = { ts->coord.x + Map::s_consts.tile.radius.x, ts->coord.y, vcz( lt, ts->elevations.right ) };
		ts->layers[ lt ].coords.bottom = { ts->coord.x, ts->coord.y + Map::s_consts.tile.radius.y, vcz( lt, ts->elevations.bottom ) };
		ts->layers[ lt ].coords.center = { ( ts->layers[ lt ].coords.left.x + ts->layers[ Map::LAYER_LAND ].coords.right.x ) / 2, ( ts->layers[ Map::LAYER_LAND ].coords.top.y + ts->layers[ Map::LAYER_LAND ].coords.bottom.y ) / 2, vcz( lt, ts->elevations.center ) };

		float ofs = 1.0f; // remove lines between tiles
		
		// TODO: implement texture tiling and shifting
		if ( ts->layers[ lt ].texture_stretch_at_edges ) {
			#define RND m_map->GetRandom()->GetFloat( 0, Map::s_consts.tile.texture_stretch_randomness )
			// initialize tex_coords with some stretching
			ts->layers[ lt ].tex_coords = {
				{ ts->tex_coord.x + m_map->GetRandom()->GetFloat(
					-Map::s_consts.tile.texture_stretch_randomness,
					Map::s_consts.tile.texture_stretch_randomness
				), ts->tex_coord.y + m_map->GetRandom()->GetFloat(
					-Map::s_consts.tile.texture_stretch_randomness,
					Map::s_consts.tile.texture_stretch_randomness
				) }, // center
				{
					ts->tex_coord.x1 + ofs + ts->NW->layers[ lt ].texture_stretch.x + RND,
					ts->tex_coord.y2 - ofs - ts->NW->layers[ lt ].texture_stretch.y - RND
				}, // left
				{
					ts->tex_coord.x1 + ofs + ts->N->layers[ lt ].texture_stretch.x + RND,
					ts->tex_coord.y1 + ofs + ts->N->layers[ lt ].texture_stretch.y + RND
				}, // top
				{
					ts->tex_coord.x2 - ofs - ts->NE->layers[ lt ].texture_stretch.x - RND,
					ts->tex_coord.y1 + ofs + ts->NE->layers[ lt ].texture_stretch.x + RND
				}, // right
				{
					ts->tex_coord.x2 - ofs - ts->layers[ lt ].texture_stretch.x - RND,
					ts->tex_coord.y2 - ofs - ts->layers[ lt ].texture_stretch.y - RND
				} // bottom
			};
			#undef RND
		}
		else {
			// initialize tex_coords with some stretching
			ts->layers[ lt ].tex_coords = {
				{ ts->tex_coord.x + m_map->GetRandom()->GetFloat(
					-Map::s_consts.tile.texture_stretch_randomness,
					Map::s_consts.tile.texture_stretch_randomness
				), ts->tex_coord.y + m_map->GetRandom()->GetFloat(
					-Map::s_consts.tile.texture_stretch_randomness,
					Map::s_consts.tile.texture_stretch_randomness
				) }, // center
				{ ts->tex_coord.x1 + ofs + ts->NW->layers[ lt ].texture_stretch.x, ts->tex_coord.y2 - ofs - ts->NW->layers[ lt ].texture_stretch.y }, // left
				{ ts->tex_coord.x1 + ofs + ts->N->layers[ lt ].texture_stretch.x, ts->tex_coord.y1 + ofs + ts->N->layers[ lt ].texture_stretch.y }, // top
				{ ts->tex_coord.x2 - ofs - ts->NE->layers[ lt ].texture_stretch.x, ts->tex_coord.y1 + ofs + ts->NE->layers[ lt ].texture_stretch.x }, // right
				{ ts->tex_coord.x2 - ofs - ts->layers[ lt ].texture_stretch.x, ts->tex_coord.y2 - ofs - ts->layers[ lt ].texture_stretch.y } // bottom
			};
		}
		
	}
}

}
}
}
