#include "Finalize.h"

namespace game {
namespace map {
namespace module {

void Finalize::GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) {

	TileState::tile_vertices_t vertices;
	TileState::tile_tex_coords_t tex_coords;
	TileState::tile_colors_t tint;
	
	#define do_x() \
		x( center ); \
		x( left ); \
		x( top ); \
		x( right ); \
		x( bottom )

	#define do_xs() \
		x( center, left, top ); \
		x( center, top, right ); \
		x( center, right, bottom ); \
		x( center, bottom, left )


	// fix some rare elevation glitches (slightly positive elevations on water tiles, slightly negative on land tiles)
	// TODO: investigate why it happens
	const uint8_t em = 1; // for elevations
	const float emf = 0.00000;//1f; // for vertices z
	if ( tile->is_water_tile && *tile->elevation.center > -em ) {
		*tile->elevation.center = -em;
	}
	else if ( !tile->is_water_tile && *tile->elevation.center < em ) {
		*tile->elevation.center = em;
	}
	
	for ( auto lt = 0 ; lt < TileState::LAYER_MAX ; lt++ ) {

		// raise everything on z axis to prevent negative z values ( camera doesn't like it when zoomed in )
		#define x( _k ) ts->layers[ lt ].coords._k.z += s_consts.tile.scale.z;
			do_x();
		#undef x
		
		vertices = ts->layers[ lt ].coords;
		if ( lt == TileState::LAYER_LAND && !tile->is_water_tile && !ts->has_water ) {
			
			// smooth center vertices a bit and add some randomness
			
			auto& cw = ts->W->layers[ lt ].coords;
			auto& cn = ts->N->layers[ lt ].coords;
			auto& ce = ts->E->layers[ lt ].coords;
			auto& cs = ts->S->layers[ lt ].coords;
			vertices.center.z += (
				( cw.right.z - cw.left.z ) +
				( cn.bottom.z - cn.top.z ) +
				( ce.left.z - ce.right.z ) +
				( cs.top.z - cs.bottom.z )
			) / 12; // TODO: fix black lines when texture is perpendicular to camera
			
			if ( tile->is_water_tile && vertices.center.z > s_consts.tile.scale.z - emf ) {
				vertices.center.z = s_consts.tile.scale.z - emf;
			}
			if ( !tile->is_water_tile && vertices.center.z < s_consts.tile.scale.z + emf ) {
				vertices.center.z = s_consts.tile.scale.z + emf;
			}
			
			#define xx( _k ) vertices.center._k += m_map->GetRandom()->GetFloat( -s_consts.tile.random.center_coordinates_shift, s_consts.tile.random.center_coordinates_shift ) * 0.05f
				xx( x );
				xx( y );
			#undef xx
			
		}
		else if ( lt != TileState::LAYER_LAND ) {
			#define x( _k ) vertices._k.z = s_consts.tile.water_level_z + s_consts.tile.scale.z
				do_x();
			#undef x
		}
		
		#define x( _k ) tex_coords._k = ts->layers[ lt ].tex_coords._k = { \
				ts->layers[ lt ].tex_coords._k.x * ms->variables.texture_scaling.x, \
				( ts->layers[ lt ].tex_coords._k.y + lt * ms->dimensions.y * s_consts.tc.texture_pcx.dimensions.y ) * ms->variables.texture_scaling.y \
			}
			do_x();
		#undef x
		
		tint = ts->layers[ lt ].colors;
		
		// fix some rare elevation glitches (slightly positive elevations on water tiles, slightly negative on land tiles)
		// TODO: investigate why it happens
		{
			#define x( _k ) \
				if ( !tile->is_water_tile ) { \
					if ( lt == TileState::LAYER_LAND && vertices._k.z < s_consts.tile.scale.z + emf ) { \
						vertices._k.z = s_consts.tile.scale.z + emf; \
					} \
					else if ( lt == TileState::LAYER_WATER_SURFACE && vertices._k.z > s_consts.tile.scale.z - emf ) { \
						vertices._k.z = s_consts.tile.scale.z - emf; \
					} \
				}
				do_x();
			#undef x
		}
		
		if ( ms->first_run ) {
			#define x( _k ) ts->layers[ lt ].indices._k = m_map->m_meshes.terrain->AddEmptyVertex()
				do_x();
			#undef x
			#define x( _a, _b, _c ) ts->layers[ lt ].surfaces._b##_##_c = m_map->m_meshes.terrain->AddSurface( { ts->layers[ lt ].indices._a, ts->layers[ lt ].indices._b, ts->layers[ lt ].indices._c } )
				do_xs();
			#undef x
		}
		
		#define x( _k ) m_map->m_meshes.terrain->SetVertex( ts->layers[ lt ].indices._k, vertices._k, tex_coords._k, tint._k )
			do_x();
		#undef x
		
		if ( tile->coord.x == 0 && lt == TileState::LAYER_LAND ) {
			
			// also copy tile to overdraw column
			
			auto* layer = &ts->layers[ lt ];

			#define x( _k ) { \
				ts->overdraw_column.coords._k.x = vertices._k.x + ms->dimensions.x * 0.5f; /* TODO: why 0.5? */ \
				ts->overdraw_column.coords._k.y = vertices._k.y; \
				ts->overdraw_column.coords._k.z = vertices._k.z; \
			}
				do_x();
			#undef x

			/*Log( (string) "Copying land layer for overdraw from " +
				"[ ~" + to_string( vertices.center.x ) + ", ~" + to_string( vertices.center.y ) + " ]" +
				" to " +
				"[ ~" + to_string( ts->overdraw_column.coords.center.x ) + ", ~" + to_string( ts->overdraw_column.coords.center.y ) + " ]"
			);*/

			if ( ms->first_run ) {
				#define x( _k ) ts->overdraw_column.indices._k = m_map->m_meshes.terrain->AddEmptyVertex()
					do_x();
				#undef x
				#define x( _a, _b, _c ) ts->overdraw_column.surfaces._b##_##_c = m_map->m_meshes.terrain->AddSurface( { ts->overdraw_column.indices._a, ts->overdraw_column.indices._b, ts->overdraw_column.indices._c } )
					do_xs();
				#undef x
			}
			
			#define x( _k ) m_map->m_meshes.terrain->SetVertex( ts->overdraw_column.indices._k, ts->overdraw_column.coords._k, tex_coords._k, tint._k )
				do_x();
			#undef x

		}
	
	}
	
	// also add to data mesh for click lookups
	
	if ( tile->is_water_tile ) {
		vertices = ts->layers[ TileState::LAYER_WATER ].coords;
	}
	else {
		vertices = ts->layers[ TileState::LAYER_LAND ].coords;
		if ( ts->is_coastline_corner ) {
			if ( tile->W->is_water_tile ) {
				vertices.left = ts->layers[ TileState::LAYER_WATER ].coords.left;
			}
			if ( tile->N->is_water_tile ) {
				vertices.top = ts->layers[ TileState::LAYER_WATER ].coords.top;
			}
			if ( tile->E->is_water_tile ) {
				vertices.right = ts->layers[ TileState::LAYER_WATER ].coords.right;
			}
			if ( tile->S->is_water_tile ) {
				vertices.bottom = ts->layers[ TileState::LAYER_WATER ].coords.bottom;
			}
			vertices.center.z = ( vertices.left.z + vertices.top.z + vertices.right.z + vertices.bottom.z ) / 4;
		}
	}
		
	// store tile coordinates
	mesh::Data::data_t data = tile->coord.y * ms->dimensions.x + tile->coord.x + 1; // +1 because we need to differentiate 'tile at 0,0' from 'no tiles'
	
	if ( ms->first_run ) {
		#define x( _k ) ts->data_mesh.indices._k = m_map->m_meshes.terrain_data->AddEmptyVertex()
			do_x();
		#undef x
		#define x( _a, _b, _c ) m_map->m_meshes.terrain_data->AddSurface( { ts->data_mesh.indices._a, ts->data_mesh.indices._b, ts->data_mesh.indices._c } )
			do_xs();
		#undef x
	}
	
	#define x( _k ) m_map->m_meshes.terrain_data->SetVertex( ts->data_mesh.indices._k, vertices._k, data )
		do_x();
	#undef x
	
	#undef do_x
	#undef do_xs
	
}

}
}
}
