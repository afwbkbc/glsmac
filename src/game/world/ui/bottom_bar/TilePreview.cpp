#include "TilePreview.h"

#include "engine/Engine.h"

// in future it may be needed to add additional modifications to preview image
// for now we can just set texture of terrain
#define COPY_TEXTURE 0

namespace game {
namespace world {
namespace ui {

void TilePreview::Create() {
	BBSection::Create();
	
}

void TilePreview::Destroy() {
	HideTilePreview();
	
	BBSection::Destroy();
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
	coords.center = { 0.0f, 0.0f, 0.0f };
	
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
	const uint8_t tw = map::Map::s_consts.tile_texture.dimensions.x;
	const uint8_t th = map::Map::s_consts.tile_texture.dimensions.y;
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
		
		//Log( "Coords = " + coords.center.ToString() + " " + coords.left.ToString() + " " + coords.top.ToString() + " " + coords.right.ToString() + " " + coords.bottom.ToString() );
		
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
		
		NEWV( preview, object::Mesh, "BBTilePreviewImage" );
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
	}
	else {
		info_lines.push_back( "Elev: " + std::to_string( e ) + "m" );
		std::string tilestr = "";
		switch ( tile->rockiness ) {
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
		
	}

#define FEATURE( _feature, _line ) \
	if ( tile->features & map::Tile::_feature ) { \
		info_lines.push_back( _line ); \
	}
	
	if ( tile->is_water_tile ) {
		FEATURE( F_XENOFUNGUS, "Sea Fungus" )
	}
	else {
		FEATURE( F_XENOFUNGUS, "Xenofungus" )
	}
	
	switch ( tile->bonus ) {
		case Tile::B_NUTRIENT: {
			info_lines.push_back( "Nutrient bonus" );
			break;
		}
		case Tile::B_ENERGY: {
			info_lines.push_back( "Energy bonus" );
			break;
		}
		case Tile::B_MINERALS: {
			info_lines.push_back( "Minerals bonus" );
			break;
		}
		default: {
			// nothing
		}
	}
	
	if ( tile->is_water_tile ) {
		FEATURE( F_GEOTHERMAL, "Geothermal" )
	}
	else {
		FEATURE( F_RIVER, "River" )
		FEATURE( F_JUNGLE, "Jungle" )
		FEATURE( F_DUNES, "Dunes" )
		FEATURE( F_URANIUM, "Uranium" )
	}
	FEATURE( F_MONOLITH, "Monolith" )
			
#undef FEATURE
		
#define TERRAFORMING( _terraforming, _line ) \
	if ( tile->terraforming & map::Tile::_terraforming ) { \
		info_lines.push_back( _line ); \
	}
	
	if ( tile->is_water_tile ) {
		TERRAFORMING( T_FARM, "Kelp Farm" );
		TERRAFORMING( T_SOLAR, "Tidal Harness" );
		TERRAFORMING( T_MINE, "Mining Platform" );
		
		TERRAFORMING( T_SENSOR, "Sensor Buoy" );
	}
	else {
		TERRAFORMING( T_FOREST, "Forest" );
		TERRAFORMING( T_FARM, "Farm" );
		TERRAFORMING( T_SOIL_ENRICHER, "Soil Enricher" );
		TERRAFORMING( T_MINE, "Mine" );
		TERRAFORMING( T_SOLAR, "Solar Collector" );
		
		TERRAFORMING( T_CONDENSER, "Condenser" );
		TERRAFORMING( T_MIRROR, "Echelon Mirror" );
		TERRAFORMING( T_BOREHOLE, "Thermal Borehole" );
		
		TERRAFORMING( T_ROAD, "Road" );
		TERRAFORMING( T_MAG_TUBE, "Mag Tube" );
		
		TERRAFORMING( T_SENSOR, "Sensor Array" );
		TERRAFORMING( T_BUNKER, "Bunker" );
		TERRAFORMING( T_AIRBASE, "Airbase" );
	}
	
#undef TERRAFORMING
	
	// combine into printable lines
	std::string info_line = "";
	std::string info_line_new = "";
	constexpr size_t max_length = 16; // TODO: determine width from actual text because different symbols are different
	std::vector< std::string > print_lines = {};
	for ( auto& line : info_lines ) {
		info_line_new = info_line + ( info_line.empty() ? "" : ", " ) + line;
		if ( info_line_new.size() > max_length ) {
			print_lines.push_back( info_line );
			info_line = line;
		}
		else {
			info_line = info_line_new;
		}
	}
	if ( !info_line.empty() ) {
		print_lines.push_back( info_line );
	}
	
	// print lines
	for ( auto& line : print_lines ) {
		NEWV( label, object::Label, "BBTilePreviewTextLine" );
			label->SetText( line );
			label->SetTop( label_top );
		m_info_lines.push_back( label );
		AddChild( label );
		label_top += label->GetHeight();
	}
	
	// print tile coordinates
	NEWV( label, object::Label, "BBTilePreviewTextFooter" );
		label->SetText( "(" + std::to_string( tile->coord.x ) + "," + std::to_string( tile->coord.y ) + ")" );
	m_info_lines.push_back( label );
	AddChild( label );
	
	// copy sprites from tile
	uint8_t sprite_top = 0;
	for ( auto& s : ts->sprites ) {
		auto sprite = s.actor->GetSpriteActor();
		auto mesh = sprite->GenerateMesh();
		NEWV( sprite_preview, object::Mesh, "BBTilePreviewSprite" );
		sprite_preview->SetMesh( mesh );
		sprite_preview->SetTintColor({ 0.7f, 0.7f, 0.7f, 1.0f }); // lower brightness a bit because it's too high otherwise for some reason
		sprite_preview->SetTexture( sprite->GetTexture() );
		m_preview_layers.push_back({
			sprite_preview,
			nullptr
		});
		AddChild( sprite_preview );
	}
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
