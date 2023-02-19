#include <regex>

#include "TilePreview.h"

#include "engine/Engine.h"

// in future it may be needed to add additional modifications to preview image
// for now we can just set texture of terrain
#define COPY_TEXTURE 0

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

void TilePreview::PreviewTile( const Map* map, const Map::tile_info_t& tile_info ) {
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
	coords.center.z = 0.0f;
	
	std::vector< map::Map::tile_layer_type_t > layers = {};
	
	if ( tile->is_water_tile ) {
		layers.push_back( map::Map::LAYER_LAND );
		layers.push_back( map::Map::LAYER_WATER_SURFACE );
		layers.push_back( map::Map::LAYER_WATER_SURFACE_EXTRA ); // TODO: only near coastlines?
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
	
#if COPY_TEXTURE
	// texture size (px)
	const uint8_t tw = map::Map::s_consts.pcx_texture_block.dimensions.x;
	const uint8_t th = map::Map::s_consts.pcx_texture_block.dimensions.y;
	// scale map terrain tex_coords to local tex_coords
	const float sx = (float)map->GetTerrainTexture()->m_width / (float)tw;
	const float sy = (float)map->GetTerrainTexture()->m_height / (float)th;
#endif
	
	// looks a bit too bright without lighting otherwise
	const float tint_modifier = 0.7f;
	
	for ( auto &lt : layers ) {
		
		NEWV( mesh, mesh::Render, 5, 4 );
	
		auto& layer = ts->layers[ lt ];
		
		auto tint = layer.colors;
	
#if COPY_TEXTURE
		NEWV( texture, types::Texture, "TilePreviewImage", tw, th );
		map->GetTextureFromLayer(
			texture,
			lt,
			tile->coord.x * tw,
			tile->coord.y * th
		);
#endif
		
#if COPY_TEXTURE
		#define x( _k ) auto _k = mesh->AddVertex( coords._k, { \
			layer.tex_coords._k.x * sx, \
			layer.tex_coords._k.y * sy, \
		}, tint._k * tint_modifier )
#else
		#define x( _k ) auto _k = mesh->AddVertex( coords._k, layer.tex_coords._k, tint._k * tint_modifier )
#endif
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
#if COPY_TEXTURE
			preview->SetTexture( texture );
#else
			preview->SetTexture( map->GetTerrainTexture() );
#endif
		m_preview_layers.push_back({
			preview,
#if COPY_TEXTURE
			texture
#else
			nullptr
#endif
		});
		
		AddChild( preview );
		
	}
	
	size_t label_top = m_preview_layers.front().object->GetHeight() + 6;
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
	for ( auto& preview_layer : m_preview_layers ) {
		RemoveChild( preview_layer.object );
#if COPY_TEXTURE
		DELETE( preview_layer.texture );
#endif
	}
	m_preview_layers.clear();
}

}
}
}
