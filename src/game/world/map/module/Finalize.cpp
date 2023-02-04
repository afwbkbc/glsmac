#include "Finalize.h"

namespace game {
namespace world {
namespace map {

void Finalize::GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) {

	Map::tile_vertices_t vertices;
	Map::tile_tex_coords_t tex_coords;
	Map::tile_colors_t tint;
	
	#define do_x() \
		x( center ); \
		x( left ); \
		x( top ); \
		x( right ); \
		x( bottom )

	for ( auto lt = 0 ; lt < Map::LAYER_MAX ; lt++ ) {
		
		vertices = ts->layers[ lt ].coords;
		if ( lt != Map::LAYER_LAND ) {
			vertices.center.z = vertices.left.z = vertices.top.z = vertices.right.z = vertices.bottom.z = Map::s_consts.levels.water;
		}
		
		#define x( _k ) tex_coords._k = { \
				ts->layers[ lt ].tex_coords._k.x * ms->variables.texture_scaling.x, \
				( ts->layers[ lt ].tex_coords._k.y + lt * ms->dimensions.y * Map::s_consts.pcx_texture_block.dimensions.y ) * ms->variables.texture_scaling.y \
			}
				do_x();
			#undef x
		
		if ( lt == Map::LAYER_LAND && tile->is_water_tile ) {
			tint.center = tint.left = tint.top = tint.right = tint.bottom = Map::s_consts.underwater_tint;
		}
		else if ( lt == Map::LAYER_WATER_SURFACE ) {
			#define x( _k ) tint._k = (Color){ \
				Map::s_consts.clampers.elevation_to_water_r.Clamp( ts->elevations._k ), \
				Map::s_consts.clampers.elevation_to_water_g.Clamp( ts->elevations._k ), \
				Map::s_consts.clampers.elevation_to_water_b.Clamp( ts->elevations._k ), \
				Map::s_consts.clampers.elevation_to_water_a.Clamp( ts->elevations._k ) \
			}
				do_x();
			#undef x
		}
		else {
			#define x( _k ) tint._k = ts->layers[ lt ].colors._k;
				do_x();
			#undef x
		}
		
		#define x( _k ) ts->layers[ lt ].indices._k = m_map->m_mesh_terrain->AddVertex( vertices._k, tex_coords._k, tint._k )
			do_x();
		#undef x

		#define x( _a, _b, _c ) m_map->m_mesh_terrain->AddSurface( { ts->layers[ lt ].indices._a, ts->layers[ lt ].indices._b, ts->layers[ lt ].indices._c } )
			x( center, left, top );
			x( center, top, right );
			x( center, right, bottom );
			x( center, bottom, left );
		#undef x

	}
	
	#undef do_x
	
}

}
}
}
