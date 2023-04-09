#include "Game.h"

#include "engine/Engine.h"

#include "util/FS.h"
#include "types/Buffer.h"

namespace game {

mt_id_t Game::MT_Ping() {
	MT_Request request = {};
	request.op = OP_PING;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_Init( const MapSettings& settings, const std::string& map_filename ) {
	MT_Request request = {};
	request.op = OP_INIT;
	NEW( request.data.init.settings, MapSettings );
	*request.data.init.settings = settings;
	NEW( request.data.init.path, std::string );
	*request.data.init.path = map_filename;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_Reset() {
	MT_Request request = {};
	request.op = OP_RESET;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_SelectTile( const types::Vec2< size_t >& tile_coords, const tile_direction_t tile_direction ) {
	MT_Request request = {};
	request.op = OP_SELECT_TILE;
	request.data.select_tile.tile_x = tile_coords.x;
	request.data.select_tile.tile_y = tile_coords.y;
	request.data.select_tile.tile_direction = tile_direction;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_SaveMap( const std::string& path ) {
	ASSERT( !path.empty(), "savemap path is empty" );
	MT_Request request = {};
	request.op = OP_SAVE_MAP;
	NEW( request.data.save_map.path, std::string );
	*request.data.save_map.path = path;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_EditMap( const types::Vec2< size_t >& tile_coords, map_editor::MapEditor::tool_type_t tool, map_editor::MapEditor::brush_type_t brush, map_editor::MapEditor::draw_mode_t draw_mode ) {
	MT_Request request = {};
	request.op = OP_EDIT_MAP;
	request.data.edit_map.tile_x = tile_coords.x;
	request.data.edit_map.tile_y = tile_coords.y;
	request.data.edit_map.tool = tool;
	request.data.edit_map.brush = brush;
	request.data.edit_map.draw_mode = draw_mode;
	return MT_CreateRequest( request );
}

void Game::Start() {
	MTModule::Start();
	
	Log( "Starting thread" );
	
	NEW( m_random, util::Random );

#ifdef DEBUG
	const auto* config = g_engine->GetConfig();
	if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_SEED ) ) {
		m_random->SetState( config->GetQuickstartSeed() );
	}
#endif
	
	// init map editor
	NEW( m_map_editor, map_editor::MapEditor, this );

}

void Game::Stop() {
	Log( "Stopping thread" );
	
	DELETE( m_map_editor );
	m_map_editor = nullptr;
	
	MTModule::Stop();
}

void Game::Iterate() {
	MTModule::Iterate();
	
}

util::Random* Game::GetRandom() const {
	return m_random;
}

map::Map* Game::GetMap() const {
	return m_map;
}

const MT_Response Game::ProcessRequest( const MT_Request& request, MT_CANCELABLE ) {
	MT_Response response = {};
	response.op = request.op;
	
	switch ( request.op ) {
		case OP_INIT: {
			//Log( "Got init request" );
			ASSERT( request.data.init.settings, "settings not set" );
			m_map_settings = *(request.data.init.settings);
			InitGame( response, *request.data.init.path, MT_C );
			break;
		}
		case OP_RESET: {
			//Log( "Got reset request" );
			ASSERT( m_map, "map not created" );
			ResetGame();
			response.result = R_SUCCESS;
			break;
		}
		case OP_PING: {
			//Log( "Got ping request" );
			response.result = R_SUCCESS;
			break;
		}
		case OP_SELECT_TILE: {
			ASSERT( m_map, "map not set" );
			//Log( "Got tile select request [ " + std::to_string( request.data.select_tile.tile_x ) + " " + std::to_string( request.data.select_tile.tile_y ) + " ]" );
			
			auto* tile = m_map->GetTile( request.data.select_tile.tile_x, request.data.select_tile.tile_y );
			auto* ts = m_map->GetTileState( request.data.select_tile.tile_x, request.data.select_tile.tile_y );
			
			switch ( request.data.select_tile.tile_direction ) {
				#define x( _case, _ptr ) \
					case _case: { \
						tile = tile->_ptr; \
						ts = ts->_ptr; \
						break; \
					}
					x( TD_W, W )
					x( TD_NW, NW )
					x( TD_N, N )
					x( TD_NE, NE )
					x( TD_E, E )
					x( TD_SE, SE )
					x( TD_S, S )
					x( TD_SW, SW )
				#undef x
			};
			
			//Log( "Selecting tile at " + tile->coord.ToString() );
			
			response.result = R_SUCCESS;
			
			response.data.select_tile.tile_x = tile->coord.x;
			response.data.select_tile.tile_y = tile->coord.y;
			
			response.data.select_tile.coords.x = ts->coord.x;
			response.data.select_tile.coords.y = ts->coord.y;
			
			response.data.select_tile.elevation.center = ts->elevations.center;
			
			map::TileState::tile_layer_type_t lt = ( tile->is_water_tile ? map::TileState::LAYER_WATER : map::TileState::LAYER_LAND );
			const auto& layer = ts->layers[ lt ];
			#define x( _k ) response.data.select_tile.selection_coords._k = layer.coords._k
				x( center );
				x( left );
				x( top );
				x( right );
				x( bottom );
			#undef x

			if ( !tile->is_water_tile && ts->is_coastline_corner ) {
				if ( tile->W->is_water_tile ) {
					response.data.select_tile.selection_coords.left = ts->layers[ map::TileState::LAYER_WATER ].coords.left;
				}
				if ( tile->N->is_water_tile ) {
					response.data.select_tile.selection_coords.top = ts->layers[ map::TileState::LAYER_WATER ].coords.top;
				}
				if ( tile->E->is_water_tile ) {
					response.data.select_tile.selection_coords.right = ts->layers[ map::TileState::LAYER_WATER ].coords.right;
				}
				if ( tile->S->is_water_tile ) {
					response.data.select_tile.selection_coords.bottom = ts->layers[ map::TileState::LAYER_WATER ].coords.bottom;
				}
			}
			
			map::TileState::tile_vertices_t preview_coords = {};
			
			lt = ( ( tile->is_water_tile || ts->is_coastline_corner ) ? map::TileState::LAYER_WATER : map::TileState::LAYER_LAND );
			#define x( _k ) preview_coords._k = layer.coords._k
				x( center );
				x( left );
				x( top );
				x( right );
				x( bottom );
			#undef x
			// absolute coords to relative
			#define x( _k ) preview_coords._k -= preview_coords.center
				x( left );
				x( top );
				x( right );
				x( bottom );
			#undef x
			preview_coords.center = { 0.0f, 0.0f, 0.0f };
			
			std::vector< map::TileState::tile_layer_type_t > layers = {};
			if ( tile->is_water_tile ) {
				layers.push_back( map::TileState::LAYER_LAND );
				layers.push_back( map::TileState::LAYER_WATER_SURFACE );
				layers.push_back( map::TileState::LAYER_WATER_SURFACE_EXTRA ); // TODO: only near coastlines?
				layers.push_back( map::TileState::LAYER_WATER );
			}
			else {
				if ( ts->is_coastline_corner ) {
					layers.push_back( map::TileState::LAYER_WATER_SURFACE );
					layers.push_back( map::TileState::LAYER_WATER_SURFACE_EXTRA );
					layers.push_back( map::TileState::LAYER_WATER );
				}
				else {
					layers.push_back( map::TileState::LAYER_LAND );
				}
			}
			
			// looks a bit too bright without lighting otherwise
			const float tint_modifier = 0.7f;
			
			NEW( response.data.select_tile.preview_meshes, data_tile_meshes_t );
			response.data.select_tile.preview_meshes->reserve( layers.size() );
			for ( auto i = 0 ; i < layers.size() ; i++ ) {
				const auto& lt = layers[ i ];
				
				NEWV( mesh, mesh::Render, 5, 4 );

				auto& layer = ts->layers[ lt ];

				auto tint = layer.colors;

				//Log( "Coords = " + preview_coords.center.ToString() + " " + preview_coords.left.ToString() + " " + preview_coords.top.ToString() + " " + preview_coords.right.ToString() + " " + preview_coords.bottom.ToString() );

				#define x( _k ) auto _k = mesh->AddVertex( preview_coords._k, layer.tex_coords._k, tint._k * tint_modifier )
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

				response.data.select_tile.preview_meshes->push_back( mesh );
			}
			
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
				case map::Tile::B_NUTRIENT: {
					info_lines.push_back( "Nutrient bonus" );
					break;
				}
				case map::Tile::B_ENERGY: {
					info_lines.push_back( "Energy bonus" );
					break;
				}
				case map::Tile::B_MINERALS: {
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
			
			NEW( response.data.select_tile.preview_lines, std::vector< std::string > );
			
			for ( auto& line : info_lines ) {
				info_line_new = info_line + ( info_line.empty() ? "" : ", " ) + line;
				if ( info_line_new.size() > max_length ) {
					response.data.select_tile.preview_lines->push_back( info_line );
					info_line = line;
				}
				else {
					info_line = info_line_new;
				}
			}
			if ( !info_line.empty() ) {
				response.data.select_tile.preview_lines->push_back( info_line );
			}

			// copy sprites from tile
			NEW( response.data.select_tile.sprites, std::vector< std::string > );
			for ( auto& s : ts->sprites ) {
				response.data.select_tile.sprites->push_back( s.actor );
			}
			
			// adaptive scroll if tile was selected with arrows
			response.data.select_tile.scroll_adaptively = request.data.select_tile.tile_direction != TD_NONE;

			break;
		}
		case OP_SAVE_MAP: {
			//Log( "got save map request" );
			const auto ec = m_map->Save( *request.data.save_map.path );
			if ( ec ) {
				response.result = R_ERROR;
				response.data.error.error_text = &( map::Map::GetErrorString( ec ) );
			}
			else {
				response.result = R_SUCCESS;
				NEW( response.data.save_map.path, std::string );
				*response.data.save_map.path = *request.data.save_map.path;
			}
			break;
		}
		case OP_EDIT_MAP: {
			//Log( "got edit map request" );
			
			m_map_editor->SelectTool( request.data.edit_map.tool );
			m_map_editor->SelectBrush( request.data.edit_map.brush );
			const auto tiles_to_reload = m_map_editor->Draw( m_map->GetTile( request.data.edit_map.tile_x, request.data.edit_map.tile_y ), request.data.edit_map.draw_mode );
			
			if ( !tiles_to_reload.empty() ) {
				auto* graphics = g_engine->GetGraphics();
				
				m_map->m_sprite_actors_to_add.clear();
				m_map->m_sprite_instances_to_remove.clear();
				m_map->m_sprite_instances_to_add.clear();
				
				graphics->Lock(); // needed to avoid tearing artifacts
				m_map->LoadTiles( tiles_to_reload, MT_C );
				m_map->FixNormals( tiles_to_reload, MT_C );
				graphics->Unlock();
				
				typedef std::unordered_map< std::string, map::Map::sprite_actor_t > t1; // can't use comma in macro below
				NEW( response.data.edit_map.sprites.actors_to_add, t1 );
				*response.data.edit_map.sprites.actors_to_add = m_map->m_sprite_actors_to_add;
				
				typedef std::unordered_map< size_t, std::string > t2; // can't use comma in macro below
				NEW( response.data.edit_map.sprites.instances_to_remove, t2 );
				*response.data.edit_map.sprites.instances_to_remove = m_map->m_sprite_instances_to_remove;

				typedef std::unordered_map< size_t, std::pair< std::string, Vec3 > > t3; // can't use comma in macro below
				NEW( response.data.edit_map.sprites.instances_to_add, t3 );
				*response.data.edit_map.sprites.instances_to_add = m_map->m_sprite_instances_to_add;
			}
			
			response.result = R_SUCCESS;
			break;
		}
		default: {
			ASSERT( false, "unknown request op " + std::to_string( request.op ) );
		}
	}
	
	return response;
}

void Game::DestroyRequest( const MT_Request& request ) {
	switch ( request.op ) {
		case OP_INIT: {
			if ( request.data.init.settings ) {
				DELETE( request.data.init.settings );
			}
			if ( request.data.init.path ) {
				DELETE( request.data.init.path );
			}
			break;
		}
		case OP_SAVE_MAP: {
			if ( request.data.save_map.path ) {
				DELETE( request.data.save_map.path );
			}
			break;
		}
	}
}

void Game::DestroyResponse( const MT_Response& response ) {
	if ( response.result == R_ERROR ) {
		if ( response.data.error.error_text ) {
			DELETE( response.data.error.error_text );
		}
	}
	else {
		switch ( response.op ) {
			case OP_INIT: {
				if ( response.data.init.terrain_texture ) {
					DELETE( response.data.init.terrain_texture );
				}
				if ( response.data.init.terrain_mesh ) {
					DELETE( response.data.init.terrain_mesh );
				}
				if ( response.data.init.terrain_data_mesh ) {
					DELETE( response.data.init.terrain_data_mesh );
				}
				break;
			}
			case OP_SELECT_TILE: {
				if ( response.data.select_tile.preview_meshes ) {
					for ( auto& mesh : *response.data.select_tile.preview_meshes ) {
						DELETE( mesh );
					}
					DELETE( response.data.select_tile.preview_meshes );
				}
				if ( response.data.select_tile.preview_lines ) {
					DELETE( response.data.select_tile.preview_lines );
				}
				if ( response.data.select_tile.sprites ) {
					DELETE( response.data.select_tile.sprites );
				}
				break;
			}
			case OP_SAVE_MAP: {
				if ( response.data.save_map.path ) {
					DELETE( response.data.save_map.path );
				}
				break;
			}
			case OP_EDIT_MAP: {
				if ( response.data.edit_map.sprites.actors_to_add ) {
					DELETE( response.data.edit_map.sprites.actors_to_add );
				}
				if ( response.data.edit_map.sprites.instances_to_remove ) {
					DELETE( response.data.edit_map.sprites.instances_to_remove );
				}
				if ( response.data.edit_map.sprites.instances_to_add ) {
					DELETE( response.data.edit_map.sprites.instances_to_add );
				}
				break;
			}
		}
	}
}

void Game::InitGame( MT_Response& response, const std::string& map_filename, MT_CANCELABLE ) {
	
	Log( "Initializing game" );
	
	Log( "Game seed: " + m_random->GetStateString() );

	map::Map* old_map = nullptr;
	if ( m_map ) {
		old_map = m_map;
	}
	NEW( m_map, map::Map, this );
	
#ifdef DEBUG
	// if crash happens - it's handy to have a seed to reproduce it
	util::FS::WriteFile( map::s_consts.debug.lastseed_filename, m_random->GetStateString() );
#endif

	map::Map::error_code_t ec = map::Map::EC_UNKNOWN;
#ifdef DEBUG
	const auto* config = g_engine->GetConfig();
	if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_DUMP ) ) {
		const std::string& filename = config->GetQuickstartMapDump();
		ASSERT( util::FS::FileExists( filename ), "map dump file \"" + filename + "\" not found" );
		Log( (std::string) "Loading map dump from " + filename );
		m_map->Unserialize( types::Buffer( util::FS::ReadFile( filename ) ) );
		ec = map::Map::EC_NONE;
	}
	else
#endif
	{
#ifdef DEBUG
		if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_FILE ) ) {
			const std::string& filename = config->GetQuickstartMapFile();
			ec = m_map->Load( filename );
		}
		else
#endif
		{
			if ( m_map_settings.type == MapSettings::MT_MAPFILE ) {
				ASSERT( !map_filename.empty(), "loading map requested but map file not specified" );
				ec = m_map->Load( map_filename );
			}
			else {
				ec = m_map->Generate( m_map_settings, MT_C );
			}
		}
#ifdef DEBUG
		// also handy to have dump of generated map
		if ( !ec && !config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_DUMP ) ) { // no point saving if we just loaded it
			Log( (std::string) "Saving map dump to " + map::s_consts.debug.lastdump_filename );
			util::FS::WriteFile( map::s_consts.debug.lastdump_filename, m_map->Serialize().ToString() );
		}
#endif
	}
	
	if ( !ec ) {
		ec = m_map->Initialize( MT_C );
	}


	if ( !ec && canceled ) {
		ec = map::Map::EC_ABORTED;
	}
	

	if ( !ec ) {
		
		response.result = R_SUCCESS;
		
		ASSERT( m_map, "map not set" );
		
		response.data.init.map_width = m_map->GetWidth();
		response.data.init.map_height = m_map->GetHeight();
		
		ASSERT( m_map->m_textures.terrain, "map terrain texture not generated" );
		response.data.init.terrain_texture = m_map->m_textures.terrain;
		
		ASSERT( m_map->m_meshes.terrain, "map terrain mesh not generated" );
		response.data.init.terrain_mesh = m_map->m_meshes.terrain;
		
		ASSERT( m_map->m_meshes.terrain_data, "map terrain data mesh not generated" );
		response.data.init.terrain_data_mesh = m_map->m_meshes.terrain_data;
		
		response.data.init.sprites.actors = &m_map->m_sprite_actors;
		response.data.init.sprites.instances = &m_map->m_sprite_instances;
		
		if ( old_map ) {
			Log( "Destroying old map state" );
			DELETE( old_map );
		}
	}
	
	else {
		
		// need to delete these here because they weren't passed to main thread
		if ( m_map->m_textures.terrain ) {
			DELETE( m_map->m_textures.terrain );
			m_map->m_textures.terrain = nullptr;
		}
		if ( m_map->m_meshes.terrain ) {
			DELETE( m_map->m_meshes.terrain );
			m_map->m_meshes.terrain = nullptr;
		}
		if ( m_map->m_meshes.terrain_data ) {
			DELETE( m_map->m_meshes.terrain_data );
			m_map->m_meshes.terrain_data = nullptr;
		}
		
		ResetGame();
		if ( ec == map::Map::EC_ABORTED ) {
			response.result = R_ABORTED;
		}
		else {
			response.result = R_ERROR;
			response.data.error.error_text = &( map::Map::GetErrorString( ec ) );
		}
		if ( old_map ) {
			Log( "Restoring old map state" );
			m_map = old_map; // restore old state // TODO: test
		}
	}
}

void Game::ResetGame() {
	Log( "Reseting map" );
	DELETE( m_map );
	m_map = nullptr;
}

}
