#include <regex>

#include "TilePreview.h"

#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {

void TilePreview::Create() {
	Section::Create();
	
}

void TilePreview::Destroy() {
	HideTilePreview();
	
	Section::Destroy();
}

void TilePreview::PreviewTile( const Map::tile_info_t& tile_info ) {
	HideTilePreview();
	
	auto tile = tile_info.tile;
	auto ts = tile_info.ts;
	auto ms = tile_info.ms;
	
	// TODO: copy actual coords
	Map::tile_vertices_t coords;
	
	if ( tile->is_water_tile ) {
		coords = ts->layers[ Map::LAYER_WATER ].coords;
	}
	else {
		if ( ts->is_coastline_corner ) {
			coords = ts->layers[ Map::LAYER_WATER ].coords;
		}
		else {
			coords = ts->layers[ Map::LAYER_LAND ].coords;
		}
	}
	
	// absolute coords to relative
	#define x( _k ) coords._k -= coords.center
		x( left );
		x( top );
		x( right );
		x( bottom );
	#undef x
	coords.center.x = 0.0f;
	coords.center.y = 0.0f;
	coords.center.z = 0.0f; //( coords.left.z + coords.top.z + coords.right.z + coords.bottom.z ) / 4;
	
	std::vector< map::Map::tile_layer_type_t > layers = {};
	
	if ( tile->is_water_tile ) {
		layers.push_back( map::Map::LAYER_LAND );
		layers.push_back( map::Map::LAYER_WATER_SURFACE );
		layers.push_back( map::Map::LAYER_WATER );
	}
	else {
		if ( ts->is_coastline_corner ) {
			layers.push_back( map::Map::LAYER_WATER_SURFACE );
			layers.push_back( map::Map::LAYER_WATER_SURFACE_EXTRA );
			layers.push_back( map::Map::LAYER_WATER );
		}
		else {
			layers.push_back( map::Map::LAYER_LAND );
		}
	}
	
	for ( auto &lt : layers ) {
		
		NEWV( mesh, mesh::Render, 5, 4 );
	
		auto& layer = ts->layers[ lt ];
		
		auto tint = layer.colors;
		
		// copy texcoords from tile
		#define x( _k ) auto _k = mesh->AddVertex( coords._k, layer.tex_coords._k, tint._k )
			x( center );
			x( left );
			x( top );
			x( right );
			x( bottom );
		#undef x

		#define x( _a, _b, _c ) mesh->AddSurface( { _a, _b, _c } )
			x( center, left, top );
			x( center, top, right );
			x( center, right, bottom );
			x( center, bottom, left );
		#undef x

		mesh->Finalize();

		NEWV( preview, object::Mesh, "MapBottomBarTilePreviewImage" );
			preview->SetMesh( mesh );
			preview->SetTexture( tile_info.ms->terrain_texture );
		m_previews.push_back( preview );
		AddChild( preview );
		
	}
	
	size_t label_top = m_previews.front()->GetHeight() + 6;
	std::vector< std::string > info_lines;
	
	auto e = *tile->elevation.center;
	if ( tile->is_water_tile ) {
		if ( e < map::Tile::ELEVATION_LEVEL_TRENCH ) {
			info_lines.push_back( "Ocean Trench" );
		}
		else if ( e < map::Tile::ELEVATION_LEVEL_OCEAN ) {
			info_lines.push_back( "Ocean" );
		}
		else {
			info_lines.push_back( "Ocean Shelf" );
		}
		info_lines.push_back( "Depth: " + std::to_string( -e ) + "m" );
		if ( tile->features & map::Tile::F_XENOFUNGUS ) {
			info_lines.push_back( "Sea Fungus" );
		}
	}
	else {
		info_lines.push_back( "Elev: " + std::to_string( e ) + "m" );
		std::string tilestr = "";
		switch ( tile->rockyness ) {
			case map::Tile::R_FLAT: {
				tilestr += "Flat";
				break;
			}
			case map::Tile::R_ROLLING: {
				tilestr += "Rolling";
				break;
			}
			case map::Tile::R_ROCKY: {
				tilestr += "Rocky";
				break;
			}
		}
		tilestr += " & ";
		switch ( tile->moisture ) {
			case map::Tile::M_ARID: {
				tilestr += "Arid";
				break;
			}
			case map::Tile::M_MOIST: {
				tilestr += "Moist";
				break;
			}
			case map::Tile::M_RAINY: {
				tilestr += "Rainy";
				break;
			}
		}
		info_lines.push_back( tilestr );
		if ( tile->features & map::Tile::F_JUNGLE ) {
			info_lines.push_back( "Jungle" );
		}
		if ( tile->features & map::Tile::F_XENOFUNGUS ) {
			info_lines.push_back( "Xenofungus" );
		}
	}
	
	for ( auto& line : info_lines ) {
		NEWV( label, object::Label, "MapBottomBarTilePreviewTextLine" );
			label->SetText( line );
			label->SetTop( label_top );
		m_info_lines.push_back( label );
		AddChild( label );
		label_top += label->GetHeight();
	}
	
	NEWV( label, object::Label, "MapBottomBarTilePreviewTextFooter" );
		label->SetText( "(" + std::to_string( tile->coord.x ) + "," + std::to_string( tile->coord.y ) + ")" );
	m_info_lines.push_back( label );
	AddChild( label );
}

void TilePreview::HideTilePreview() {
	for ( auto& label : m_info_lines ) {
		RemoveChild( label );
	}
	m_info_lines.clear();
	for ( auto& preview : m_previews ) {
		RemoveChild( preview );
	}
	m_previews.clear();
}

}
}
}
