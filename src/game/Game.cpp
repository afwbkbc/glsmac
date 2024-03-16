#include "Game.h"

#include "engine/Engine.h"
#include "types/Exception.h"
#include "util/FS.h"
#include "types/Buffer.h"

#include "game/State.h"

using namespace game::connection;
using namespace game::bindings;

namespace game {

InvalidEvent::InvalidEvent( const std::string& reason, const event::Event* event )
	: Exception(
	"InvalidEvent", (std::string)
		"Event validation failed!\n" +
		TS_OFFSET + "reason: '" + reason + "'\n" +
		TS_OFFSET + "event: " + event->ToString( TS_OFFSET )
) {}

mt_id_t Game::MT_Ping() {
	MT_Request request = {};
	request.op = OP_PING;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_Init( State* state ) {
	MT_Request request = {};
	request.op = OP_INIT;
	request.data.init.state = state;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_Chat( const std::string& message ) {
	MT_Request request = {};
	request.op = OP_CHAT;
	NEW( request.data.save_map.path, std::string, message );
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_GetMapData() {
	MT_Request request = {};
	request.op = OP_GET_MAP_DATA;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_Reset() {
	m_init_cancel = true; // stop initialization in Iterate()
	MT_Request request = {};
	request.op = OP_RESET;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_SelectTile(
	const tile_query_purpose_t tile_query_purpose,
	const types::Vec2< size_t >& tile_coords,
	const map::Tile::direction_t tile_direction,
	const tile_query_metadata_t tile_query_metadata
) {
	MT_Request request = {};
	request.op = OP_SELECT_TILE;
	request.data.select_tile.purpose = tile_query_purpose;
	request.data.select_tile.tile_x = tile_coords.x;
	request.data.select_tile.tile_y = tile_coords.y;
	request.data.select_tile.tile_direction = tile_direction;
	request.data.select_tile.metadata = tile_query_metadata;
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

mt_id_t Game::MT_GetFrontendRequests() {
	MT_Request request = {};
	request.op = OP_GET_FRONTEND_REQUESTS;
	return MT_CreateRequest( request );
}

mt_id_t Game::MT_AddEvent( const event::Event* event ) {
	MT_Request request = {};
	request.op = OP_ADD_EVENT;
	NEW( request.data.add_event.serialized_event, std::string, event::Event::Serialize( event ).ToString() );
	return MT_CreateRequest( request );
}

#ifdef DEBUG
#define x( _method, _op ) \
    mt_id_t Game::_method( const std::string& path ) { \
        ASSERT( !path.empty(), "dump path is empty" ); \
        MT_Request request = {}; \
        request.op = _op; \
        NEW( request.data.dump.path, std::string ); \
        *request.data.dump.path = path; \
        return MT_CreateRequest( request ); \
    }

x( MT_SaveDump, OP_SAVE_DUMP )

x( MT_LoadDump, OP_LOAD_DUMP )

#undef x
#endif

void Game::Start() {
	MTModule::Start();

	Log( "Starting thread" );

	m_game_state = GS_NONE;
	m_init_cancel = false;

	ASSERT( !m_pending_frontend_requests, "frontend requests already set" );
	NEW( m_pending_frontend_requests, std::vector< FrontendRequest > );

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

	if ( m_state ) {
		DELETE( m_state );
		m_state = nullptr;
		m_connection = nullptr;
	}

	ResetGame();

	DELETE( m_map_editor );
	m_map_editor = nullptr;

	MTModule::Stop();
}

void Game::Iterate() {
	MTModule::Iterate();

	try {
		if ( m_state ) {
			m_state->Iterate();
		}

		if ( m_game_state == GS_INITIALIZING ) {

			auto* ui = g_engine->GetUI();

			bool ready = true;

			if ( m_state->IsMaster() ) {
#ifdef DEBUG
				static std::string waiting_for_players_old = "";
				std::string waiting_for_players = "";
#endif
				for ( const auto& slot : m_state->m_slots.GetSlots() ) {
					if ( slot.GetState() == Slot::SS_PLAYER && !slot.HasPlayerFlag( Slot::PF_MAP_DOWNLOADED ) ) {
						ready = false;
#ifdef DEBUG
						waiting_for_players += " " + slot.GetPlayer()->GetPlayerName();
#else
						break;
#endif
					}
				}

#ifdef DEBUG
				if ( waiting_for_players != waiting_for_players_old ) {
					waiting_for_players_old = waiting_for_players;
					if ( !ready ) {
						Log( "Waiting for players:" + waiting_for_players );
					}
				}
#endif
			}
			else {
				// notify server of successful download and continue initialization
				m_slot->SetPlayerFlag( Slot::PF_MAP_DOWNLOADED );
				m_connection->UpdateSlot( m_slot_num, m_slot, true );
			}

			if ( ready ) {

				auto ec = m_map->Initialize( m_init_cancel );
				if ( !ec && m_init_cancel ) {
					ec = map::Map::EC_ABORTED;
				}

				const auto& f_init_failed = [ this ]( const std::string& error_text ) {
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
					m_initialization_error = error_text;

					if ( m_old_map ) {
						Log( "Restoring old map state" );
						DELETE( m_map );
						m_map = m_old_map; // restore old state // TODO: test
					}

					if ( m_connection ) {
						m_connection->Disconnect( "Failed to initialize game" );
					}
				};

				if ( !ec ) {

#ifdef DEBUG
					const auto* config = g_engine->GetConfig();
					// also handy to have dump of generated map
					if (
						!ec &&
							config->HasDebugFlag( config::Config::DF_MAPDUMP ) &&
							!config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_DUMP ) // no point saving if we just loaded it
						) {
						Log( (std::string)"Saving map dump to " + config->GetDebugPath() + map::s_consts.debug.lastdump_filename );
						ui->SetLoaderText( "Saving dump", false );
						util::FS::WriteFile( config->GetDebugPath() + map::s_consts.debug.lastdump_filename, m_map->Serialize().ToString() );
					}
#endif

					ASSERT( m_map, "map not set" );

					NEW( m_response_map_data, response_map_data_t );

					m_response_map_data->map_width = m_map->GetWidth();
					m_response_map_data->map_height = m_map->GetHeight();

					ASSERT( m_map->m_textures.terrain, "map terrain texture not generated" );
					m_response_map_data->terrain_texture = m_map->m_textures.terrain;

					ASSERT( m_map->m_meshes.terrain, "map terrain mesh not generated" );
					m_response_map_data->terrain_mesh = m_map->m_meshes.terrain;

					ASSERT( m_map->m_meshes.terrain_data, "map terrain data mesh not generated" );
					m_response_map_data->terrain_data_mesh = m_map->m_meshes.terrain_data;

					m_response_map_data->sprites.actors = &m_map->m_sprite_actors;
					m_response_map_data->sprites.instances = &m_map->m_sprite_instances;

					if ( m_old_map ) {
						Log( "Destroying old map state" );
						DELETE( m_old_map );
						m_old_map = nullptr;
					}

					// notify server of successful initialization
					m_slot->SetPlayerFlag( Slot::PF_GAME_INITIALIZED );
					if ( m_connection ) {
						m_connection->UpdateSlot( m_slot_num, m_slot, true );
					}

					const auto& slots = m_state->m_slots.GetSlots();
					auto* slot_defines = new FrontendRequest::slot_defines_t();
					for ( const auto& slot : slots ) {
						if ( slot.GetState() == Slot::SS_OPEN || slot.GetState() == Slot::SS_CLOSED ) {
							continue;
						}
						ASSERT( slot.GetState() == Slot::SS_PLAYER, "unknown slot state: " + std::to_string( slot.GetState() ) );
						const auto* player = slot.GetPlayer();
						ASSERT( player, "slot player not set" );
						const auto& faction = player->GetFaction();
						const auto& c = faction.m_colors.border.value;
						slot_defines->push_back(
							FrontendRequest::slot_define_t{
								slot.GetIndex(),
								{
									c.red,
									c.green,
									c.blue,
									c.alpha
								},
								( faction.m_flags & rules::Faction::FF_PROGENITOR ) != 0
							}
						);
					}
					auto fr = FrontendRequest( FrontendRequest::FR_SLOT_DEFINE );
					fr.data.slot_define.slotdefs = slot_defines;
					AddFrontendRequest( fr );

					ASSERT( !m_current_turn, "turn is already initialized" );

					try {
						// start main loop
						m_game_state = GS_RUNNING;
					}
					catch ( gse::Exception& e ) {
						Log( (std::string)"Initialization failed: " + e.ToStringAndCleanup() );
						f_init_failed( e.what() );
					}

					if ( m_game_state == GS_RUNNING ) {

						for ( auto& it : m_unprocessed_units ) {
							SpawnUnit( it );
						}
						m_unprocessed_units.clear();
						for ( auto& it : m_unprocessed_events ) {
							ASSERT( m_connection, "connection not set" );
							try {
								ValidateEvent( it );
							}
							catch ( const InvalidEvent& e ) {
								for ( auto& it2 : m_unprocessed_events ) {
									delete it2;
								}
								m_unprocessed_events.clear();
								throw e;
							}
						}
						for ( auto& it : m_unprocessed_events ) {
							ProcessEvent( it );
						}
						m_unprocessed_events.clear();

						g_engine->GetUI()->SetLoaderText( "Starting game...", false );
						NEW( m_current_turn, Turn );
						if ( m_state->IsMaster() ) {
							m_state->m_bindings->Call( Bindings::CS_ON_START );
						}
						NextTurn();
						CheckTurnComplete();

					}
				}
				else {
					f_init_failed( map::Map::GetErrorString( ec ) );
				}
			}
		}
	}
	catch ( const InvalidEvent& e ) {
		Log( (std::string)e.what() );
		Quit( "Event validation error" ); // TODO: fix reason
	}
}

util::Random* Game::GetRandom() const {
	return m_random;
}

map::Map* Game::GetMap() const {
	return m_map;
}

State* Game::GetState() const {
	ASSERT( m_state, "state not set" );
	return m_state;
}

const Player* Game::GetPlayer() const {
	ASSERT( m_state, "state not set" );
	if ( m_connection ) {
		return m_connection->GetPlayer();
	}
	else {
		return m_state->m_slots.GetSlot( 0 ).GetPlayer();
	}
}

const size_t Game::GetSlotNum() const {
	return m_slot_num;
}

const MT_Response Game::ProcessRequest( const MT_Request& request, MT_CANCELABLE ) {
	MT_Response response = {};
	response.op = request.op;

	switch ( request.op ) {
		case OP_INIT: {
			//Log( "Got init request" );
			ASSERT( request.data.init.state, "state not set" );
			m_state = request.data.init.state;
			m_state->SetGame( this );
			InitGame( response, MT_C );
			response.data.init.slot_index = m_slot_num;
			break;
		}
		case OP_GET_MAP_DATA: {
			//Log( "Got get-map-data request" );
			if ( m_game_state != GS_RUNNING ) {
				if ( m_initialization_error.empty() ) {
					response.result = R_PENDING;
				}
				else {
					response.result = R_ERROR;
					NEW( response.data.error.error_text, std::string, m_initialization_error );
				}
			}
			else if ( m_response_map_data ) {
				response.result = R_SUCCESS;
				response.data.get_map_data = m_response_map_data;
				m_response_map_data = nullptr;
			}
			else if ( m_init_cancel ) {
				response.result = R_ABORTED;
			}
			else {
				response.result = R_ERROR;
				NEW( response.data.error.error_text, std::string, m_initialization_error );
			}
			break;
		}
		case OP_RESET: {
			//Log( "Got reset request" );
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

			auto* tile = m_map
				->GetTile( request.data.select_tile.tile_x, request.data.select_tile.tile_y )
				->GetNeighbour( request.data.select_tile.tile_direction );
			auto* ts = m_map
				->GetTileState( request.data.select_tile.tile_x, request.data.select_tile.tile_y )
				->GetNeighbour( request.data.select_tile.tile_direction );

			//Log( "Selecting tile at " + tile->coord.ToString() );

			response.result = R_SUCCESS;

			response.data.select_tile.purpose = request.data.select_tile.purpose;

			response.data.select_tile.tile_x = tile->coord.x;
			response.data.select_tile.tile_y = tile->coord.y;

			response.data.select_tile.coords.x = ts->coord.x;
			response.data.select_tile.coords.y = ts->coord.y;

			response.data.select_tile.elevation.center = ts->elevations.center;

			map::TileState::tile_layer_type_t lt = ( tile->is_water_tile
				? map::TileState::LAYER_WATER
				: map::TileState::LAYER_LAND
			);
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

			lt = ( ( tile->is_water_tile || ts->is_coastline_corner )
				? map::TileState::LAYER_WATER
				: map::TileState::LAYER_LAND
			);
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
			preview_coords.center = {
				0.0f,
				0.0f,
				0.0f
			};

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
				info_line_new = info_line + ( info_line.empty()
					? ""
					: ", "
				) + line;
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
			response.data.select_tile.scroll_adaptively = request.data.select_tile.tile_direction != map::Tile::D_NONE;

			NEW( response.data.select_tile.unit_ids, std::vector< size_t > );
			for ( const auto& it : tile->units ) {
				response.data.select_tile.unit_ids->push_back( it.first );
			}

			response.data.select_tile.metadata = request.data.select_tile.metadata;

			break;
		}
		case OP_SAVE_MAP: {
			//Log( "got save map request" );
			const auto ec = m_map->SaveToFile( *request.data.save_map.path );
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
		case OP_CHAT: {
			Log( "got chat message request: " + *request.data.chat.message );

			if ( m_connection ) {
				m_connection->SendMessage( *request.data.chat.message );
			}
			else {
				Message( "<" + m_player->GetPlayerName() + "> " + *request.data.chat.message );
			}

			response.result = R_SUCCESS;
			break;
		}
		case OP_GET_FRONTEND_REQUESTS: {
			//Log( "got events request" );
			if ( !m_pending_frontend_requests->empty() ) {
				Log( "sending " + std::to_string( m_pending_frontend_requests->size() ) + " events to frontend" );
				response.data.get_frontend_requests.requests = m_pending_frontend_requests; // will be destroyed in DestroyResponse
				NEW( m_pending_frontend_requests, std::vector< FrontendRequest > ); // reset
			}
			else {
				response.data.get_frontend_requests.requests = nullptr;
			}
			response.result = R_SUCCESS;
			break;
		}
		case OP_ADD_EVENT: {
			const std::string* errmsg = nullptr;
			event::Event* event = nullptr;
			ASSERT( m_current_turn, "turn not initialized" );
			auto buf = types::Buffer( *request.data.add_event.serialized_event );
			event = event::Event::Unserialize( buf );
			if ( !m_current_turn->IsActive() && event->m_type != event::Event::ET_UNCOMPLETE_TURN ) {
				errmsg = new std::string( "Turn not active" );
			}
			else {
				errmsg = event->Validate( this );
			}
			if ( errmsg ) {
				// log and do nothing
				Log( "Event declined: " + *errmsg );
				delete errmsg;
				delete event;
			}
			else {
				AddEvent( event );
			}
			break;
		}
		default: {
			THROW( "unknown request op " + std::to_string( request.op ) );
		}
	}

	return response;
}

void Game::DestroyRequest( const MT_Request& request ) {
	switch ( request.op ) {
		case OP_SAVE_MAP: {
			if ( request.data.save_map.path ) {
				DELETE( request.data.save_map.path );
			}
			break;
		}
		case OP_CHAT: {
			if ( request.data.chat.message ) {
				DELETE( request.data.chat.message );
			}
			break;
		}
		case OP_ADD_EVENT: {
			DELETE( request.data.add_event.serialized_event );
			break;
		}
		default: {
			// nothing to delete
		}
	}
}

void Game::DestroyResponse( const MT_Response& response ) {
	if ( response.result == R_SUCCESS ) {
		switch ( response.op ) {
			case OP_GET_MAP_DATA: {
				if ( response.data.get_map_data ) {
					DELETE( response.data.get_map_data );
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
				if ( response.data.select_tile.unit_ids ) {
					DELETE( response.data.select_tile.unit_ids );
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
			case OP_GET_FRONTEND_REQUESTS: {
				if ( response.data.get_frontend_requests.requests ) {
					DELETE( response.data.get_frontend_requests.requests );
				}
				break;
			}
			default: {
				// nothing to delete
			}
		}
	}
}

void Game::Message( const std::string& text ) {
	auto fr = FrontendRequest( FrontendRequest::FR_GLOBAL_MESSAGE );
	NEW( fr.data.global_message.message, std::string, text );
	AddFrontendRequest( fr );
}

void Game::Quit( const std::string& reason ) {
	auto fr = FrontendRequest( FrontendRequest::FR_QUIT );
	NEW( fr.data.quit.reason, std::string, "Lost connection to server" );
	AddFrontendRequest( fr );
}

void Game::OnGSEError( gse::Exception& err ) {
	auto fr = FrontendRequest( FrontendRequest::FR_ERROR );
	NEW( fr.data.error.what, std::string, (std::string)"Script error: " + err.what() );
	NEW( fr.data.error.stacktrace, std::string, err.ToStringAndCleanup() );
	AddFrontendRequest( fr );
}

unit::Unit* Game::GetUnit( const size_t id ) const {
	const auto& it = m_units.find( id );
	if ( it != m_units.end() ) {
		return it->second;
	}
	return nullptr;
}

const unit::Def* Game::GetUnitDef( const std::string& name ) const {
	const auto& it = m_unit_defs.find( name );
	if ( it != m_unit_defs.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

void Game::AddEvent( event::Event* event ) {
	ASSERT( event->m_initiator_slot == m_slot_num, "initiator slot mismatch" );
	if ( m_connection ) {
		m_connection->SendGameEvent( event );
	}
	if ( m_state->IsMaster() ) {
		ProcessEvent( event );
	}
}

void Game::DefineUnit( const unit::Def* def ) {
	Log( "Defining unit ('" + def->m_id + "')" );

	ASSERT( m_unit_defs.find( def->m_id ) == m_unit_defs.end(), "Unit definition '" + def->m_id + "' already exists" );

	m_unit_defs.insert_or_assign( def->m_id, def );

	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_DEFINE );
	NEW( fr.data.unit_define.serialized_unitdef, std::string, unit::Def::Serialize( def ).ToString() );
	AddFrontendRequest( fr );
}

void Game::SpawnUnit( unit::Unit* unit ) {
	if ( m_game_state != GS_RUNNING ) {
		m_unprocessed_units.push_back( unit );
		return;
	}

	Log( "Spawning unit #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + unit->m_tile->ToString() );

	ASSERT( m_units.find( unit->m_id ) == m_units.end(), "duplicate unit id" );

	auto* tile = unit->m_tile;

	ASSERT( m_units.find( unit->m_id ) == m_units.end(), "duplicate unit id" );
	m_units.insert_or_assign( unit->m_id, unit );
	unit->m_tile = tile;
	ASSERT( tile->units.find( unit->m_id ) == tile->units.end(), "duplicate unit id in tile" );
	tile->units.insert(
		{
			unit->m_id,
			unit
		}
	);

	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_SPAWN );
	fr.data.unit_spawn.unit_id = unit->m_id;
	NEW( fr.data.unit_spawn.unitdef_id, std::string, unit->m_def->m_id );
	fr.data.unit_spawn.slot_index = unit->m_owner->GetIndex();
	fr.data.unit_spawn.tile_coords = {
		tile->coord.x,
		tile->coord.y
	};
	const auto c = GetUnitRenderCoords( unit );
	fr.data.unit_spawn.render_coords = {
		c.x,
		c.y,
		c.z
	};

	fr.data.unit_spawn.movement = unit->m_movement;
	fr.data.unit_spawn.morale = unit->m_morale;
	fr.data.unit_spawn.health = unit->m_health;

	AddFrontendRequest( fr );

	CheckTurnComplete();

	if ( m_state->IsMaster() ) {
		m_state->m_bindings->Call( Bindings::CS_ON_SPAWN_UNIT, { unit->Wrap() } );
	}
}

void Game::SkipUnitTurn( const size_t unit_id ) {
	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );
	auto* unit = it->second;

	Log( "Skipping unit turn #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + unit->m_tile->ToString() );

	unit->m_movement = 0.0f;

	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_REFRESH );
	fr.data.unit_refresh.unit_id = unit->m_id;
	fr.data.unit_refresh.movement_left = unit->m_movement;
	AddFrontendRequest( fr );
	CheckTurnComplete();
}

void Game::DespawnUnit( const size_t unit_id ) {

	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );
	auto* unit = it->second;

	Log( "Despawning unit #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + unit->m_tile->ToString() );

	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_DESPAWN );
	fr.data.unit_despawn.unit_id = unit_id;
	AddFrontendRequest( fr );

	ASSERT( unit->m_tile, "unit tile not assigned" );
	auto* tile = unit->m_tile;
	const auto& tile_it = tile->units.find( unit->m_id );
	ASSERT( tile_it != tile->units.end(), "unit id not found in tile" );
	tile->units.erase( tile_it );

	m_units.erase( it );

	if ( m_state->IsMaster() ) {
		m_state->m_bindings->Call( Bindings::CS_ON_DESPAWN_UNIT, { unit->Wrap() } );
	}

	delete unit;
}

void Game::MoveUnit( unit::Unit* unit, map::Tile* dst_tile, const bool is_move_successful ) {
	ASSERT( dst_tile, "dst tile not set" );

	Log( "Moving unit #" + std::to_string( unit->m_id ) + " to " + dst_tile->coord.ToString() );

	auto* src_tile = unit->m_tile;
	ASSERT( src_tile, "src tile not set" );
	ASSERT( src_tile->units.find( unit->m_id ) != src_tile->units.end(), "src tile does not contain this unit" );
	ASSERT( dst_tile->units.find( unit->m_id ) == dst_tile->units.end(), "dst tile already contains this unit" );

	unit->UpdateMoves( this, dst_tile );

	if ( is_move_successful ) {

		src_tile->units.erase( unit->m_id );
		dst_tile->units.insert(
			{
				unit->m_id,
				unit
			}
		);
		unit->m_tile = dst_tile;

		auto fr = FrontendRequest( FrontendRequest::FR_UNIT_MOVE );
		fr.data.unit_move.unit_id = unit->m_id;
		fr.data.unit_move.tile_coords = {
			dst_tile->coord.x,
			dst_tile->coord.y
		};
		const auto c = GetUnitRenderCoords( unit );
		fr.data.unit_move.render_coords = {
			c.x,
			c.y,
			c.z
		};
		fr.data.unit_move.movement_left = unit->m_movement;
		AddFrontendRequest( fr );
		if ( !unit->HasMovesLeft() ) {
			CheckTurnComplete();
		}
	}
	else {
		// refresh unit badge on frontend
		auto fr = FrontendRequest( FrontendRequest::FR_UNIT_REFRESH );
		fr.data.unit_refresh.unit_id = unit->m_id;
		fr.data.unit_refresh.movement_left = unit->m_movement;
		AddFrontendRequest( fr );
		CheckTurnComplete();
	}

}

const bool Game::IsTurnActive() const {
	return m_current_turn && m_current_turn->IsActive();
}

const bool Game::IsTurnCompleted( const size_t slot_num ) const {
	const auto& slot = m_state->m_slots.GetSlot( slot_num );
	ASSERT( slot.GetState() == Slot::SS_PLAYER, "slot is not player" );
	const auto* player = slot.GetPlayer();
	ASSERT_NOLOG( player, "slot player not set" );
	return player->IsTurnCompleted();
}

void Game::CompleteTurn( const size_t slot_num ) {
	const auto& slot = m_state->m_slots.GetSlot( slot_num );
	ASSERT_NOLOG( slot.GetState() == Slot::SS_PLAYER, "slot is not player" );
	auto* player = slot.GetPlayer();
	ASSERT_NOLOG( player, "slot player not set" );
	player->CompleteTurn();
	if ( slot_num == m_slot_num ) {
		ASSERT( m_current_turn, "turn not set" );
		m_current_turn->Deactivate();
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_ACTIVE_STATUS );
		fr.data.turn_active_status.is_turn_active = false;
		AddFrontendRequest( fr );
	}
}

void Game::UncompleteTurn( const size_t slot_num ) {
	const auto& slot = m_state->m_slots.GetSlot( slot_num );
	ASSERT_NOLOG( slot.GetState() == Slot::SS_PLAYER, "slot is not player" );
	auto* player = slot.GetPlayer();
	ASSERT_NOLOG( player, "slot player not set" );
	player->UncompleteTurn();
	if ( slot_num == m_slot_num ) {
		ASSERT( m_current_turn, "turn not set" );
		m_current_turn->Activate();
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_ACTIVE_STATUS );
		fr.data.turn_active_status.is_turn_active = true;
		AddFrontendRequest( fr );
		m_is_turn_complete = false;
		CheckTurnComplete();
	}
}

void Game::ValidateEvent( event::Event* event ) const {
	if ( !event->m_is_validated ) {
		const auto* errmsg = event->Validate( this );
		if ( errmsg ) {
			InvalidEvent err( *errmsg, event );
			delete errmsg;
			delete event;
			throw err;
		}
		event->m_is_validated = true;
	}
}

const gse::Value Game::ProcessEvent( event::Event* event ) {
	ASSERT( m_current_turn, "turn not initialized" );

	if ( m_state->IsMaster() ) { // TODO: validate in either case?
		ValidateEvent( event );
	}

	ASSERT( event->m_is_validated, "event was not validated" );

	if ( m_state->IsMaster() ) {
		// things like random outcomes must be resolved on server only
		event->Resolve( this );
	}

	m_current_turn->AddEvent( event );
	return event->Apply( this );
}

const types::Vec3 Game::GetUnitRenderCoords( const unit::Unit* unit ) {
	ASSERT( unit, "unit not set" );
	const auto* tile = unit->m_tile;
	ASSERT( tile, "tile not set" );
	const auto* ts = m_map->GetTileState( tile );
	ASSERT( ts, "ts not set" );
	const auto l = tile->is_water_tile
		? map::TileState::LAYER_WATER
		: map::TileState::LAYER_LAND;
	const auto c = unit->m_def->GetSpawnCoords(
		ts->coord.x,
		ts->coord.y,
		ts->layers[ l ].coords
	);
	return {
		c.x,
		-c.y,
		c.z
	};
}

void Game::SerializeUnits( types::Buffer& buf ) const {
	Log( "Serializing " + std::to_string( m_unit_defs.size() ) + " unit defs" );
	buf.WriteInt( m_unit_defs.size() );
	for ( const auto& it : m_unit_defs ) {
		buf.WriteString( it.first );
		buf.WriteString( unit::Def::Serialize( it.second ).ToString() );
	}
	buf.WriteInt( m_units.size() );
	Log( "Serializing " + std::to_string( m_units.size() ) + " units" );
	for ( const auto& it : m_units ) {
		buf.WriteInt( it.first );
		buf.WriteString( unit::Unit::Serialize( it.second ).ToString() );
	}
	buf.WriteInt( unit::Unit::GetNextId() );
	Log( "Saved next unit id: " + std::to_string( unit::Unit::GetNextId() ) );
}

void Game::UnserializeUnits( types::Buffer& buf ) {
	ASSERT( m_unit_defs.empty(), "unit defs not empty" );
	ASSERT( m_units.empty(), "units not empty" );
	size_t sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " unit defs" );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = Buffer( buf.ReadString() );
		DefineUnit( unit::Def::Unserialize( b ) );
	}
	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " units" );
	ASSERT( m_unprocessed_units.empty(), "unprocessed units not empty" );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto unit_id = buf.ReadInt();
		auto b = Buffer( buf.ReadString() );
		SpawnUnit( unit::Unit::Unserialize( b, this ) );
	}
	unit::Unit::SetNextId( buf.ReadInt() );
	Log( "Restored next unit id: " + std::to_string( unit::Unit::GetNextId() ) );
}

void Game::AddFrontendRequest( const FrontendRequest& request ) {
	//Log( "Sending frontend request (type=" + std::to_string( request.type ) + ")" ); // spammy
	m_pending_frontend_requests->push_back( request );
}

void Game::InitGame( MT_Response& response, MT_CANCELABLE ) {

	ASSERT( !m_connection || m_game_state == GS_NONE, "multiplayer game already initializing or running" );
	ASSERT( m_game_state == GS_NONE || m_game_state == GS_RUNNING, "game still initializing" );

	Log( "Initializing game" );

	ASSERT( m_pending_frontend_requests, "pending events not set" );
	m_pending_frontend_requests->clear();

	m_game_state = GS_PREPARING_MAP;
	m_initialization_error = "";

	m_connection = m_state->GetConnection();

	if ( m_state->IsMaster() ) {

		// assign random factions to players
		const auto& factions = m_state->m_settings.global.game_rules.m_factions;
		std::vector< std::string > m_available_factions = {};
		const auto& slots = m_state->m_slots.GetSlots();
		for ( const auto& slot : slots ) {
			if ( slot.GetState() == ::game::Slot::SS_PLAYER ) {
				auto* player = slot.GetPlayer();
				ASSERT( player, "player not set" );
				if ( player->GetFaction().m_id == ::game::Player::RANDOM_FACTION.m_id ) {
					if ( m_available_factions.empty() ) {
						// (re)load factions list
						for ( const auto& it : factions ) {
							m_available_factions.push_back( it.first );
						}
						ASSERT( !m_available_factions.empty(), "no factions found" );
					}
					const std::vector< std::string >::iterator it = m_available_factions.begin() + m_random->GetUInt( 0, m_available_factions.size() - 1 );
					player->SetFaction( factions.at( *it ) );
					m_available_factions.erase( it );
				}
			}
		}
		if ( m_connection ) {
			m_connection->AsServer()->SendPlayersList();
		}

	}

	if ( m_connection ) {

		m_slot_num = m_connection->GetSlotNum();

		m_connection->ResetHandlers();
		m_connection->IfServer(
			[ this ]( Server* connection ) -> void {

				connection->m_on_player_join = [ this, connection ]( const size_t slot_num, Slot* slot, const Player* player ) -> void {
					Log( "Player " + player->GetFullName() + " is connecting..." );
					connection->GlobalMessage( "Connection from " + player->GetFullName() + "..." );
					// actual join will happen after he downloads and initializes map
				};

				connection->m_on_flags_update = [ this, connection ]( const size_t slot_num, Slot* slot, const Slot::player_flag_t old_flags, const Slot::player_flag_t new_flags ) -> void {
					if ( !( old_flags & Slot::PF_GAME_INITIALIZED ) && ( new_flags & Slot::PF_GAME_INITIALIZED ) ) {
						const auto* player = slot->GetPlayer();
						Log( player->GetFullName() + " joined the game." );
						connection->GlobalMessage( player->GetFullName() + " joined the game." );
					}
				};

				connection->m_on_player_leave = [ this, connection ]( const size_t slot_num, Slot* slot, const Player* player ) -> void {
					Log( "Player " + player->GetFullName() + " left the game." );
					connection->GlobalMessage( player->GetFullName() + " left the game." );
				};

				connection->m_on_download_request = [ this ]() -> const std::string {
					if ( !m_map ) {
						// map not generated yet
						return "";
					}
					Log( "Preparing snapshot for download" );
					types::Buffer buf;

					// map
					m_map->SaveToBuffer( buf );

					// units
					{
						types::Buffer b;
						SerializeUnits( b );
						buf.WriteString( b.ToString() );
					}

					return buf.ToString();
				};

				connection->SetGameState( Connection::GS_INITIALIZING );
			}
		);

		m_connection->IfClient(
			[ this ]( Client* connection ) -> void {
				connection->m_on_disconnect = [ this ]() -> bool {
					Log( "Connection lost" );
					m_state->DetachConnection();
					m_connection = nullptr;
					if ( m_game_state != GS_RUNNING ) {
						m_initialization_error = "Lost connection to server";
					}
					else {
						Quit( "Lost connection to server" );
					}
					return true;
				};
				connection->m_on_error = [ this ]( const std::string& reason ) -> bool {
					m_initialization_error = reason;
					return true;
				};
			}
		);

		m_connection->m_on_message = [ this ]( const std::string& message ) -> void {
			auto fr = FrontendRequest( FrontendRequest::FR_GLOBAL_MESSAGE );
			NEW( fr.data.global_message.message, std::string, message );
			AddFrontendRequest( fr );
		};

		m_connection->m_on_game_event = [ this ]( event::Event* event ) -> void {

			if ( m_state->IsMaster() ) {
				ASSERT( m_game_state == GS_RUNNING, "game is not running but received event" );
			}

			if ( m_game_state == GS_RUNNING ) {
				ValidateEvent( event );
				ProcessEvent( event );
			}
			else {
				m_unprocessed_events.push_back( event );
			}

		};

	}
	else {
		m_slot_num = 0;
	}
	m_player = m_state->m_slots.GetSlot( m_slot_num ).GetPlayer();
	m_slot = m_player->GetSlot();

	auto* ui = g_engine->GetUI();

	if ( m_state->IsMaster() ) {
		// generate map

		Log( "Game seed: " + m_random->GetStateString() );

		ASSERT( !m_old_map, "old map not null" );
		m_old_map = nullptr;
		if ( m_map ) {
			m_old_map = m_map;
		}
		NEW( m_map, map::Map, this );

#ifdef DEBUG
		const auto* config = g_engine->GetConfig();

		// if crash happens - it's handy to have a seed to reproduce it
		util::FS::WriteFile( config->GetDebugPath() + map::s_consts.debug.lastseed_filename, m_random->GetStateString() );
#endif

		map::Map::error_code_t ec = map::Map::EC_UNKNOWN;

#ifdef DEBUG
		if ( !m_connection && config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_DUMP ) ) {
			const std::string& filename = config->GetQuickstartMapDump();
			ASSERT( util::FS::FileExists( filename ), "map dump file \"" + filename + "\" not found" );
			Log( (std::string)"Loading map dump from " + filename );
			ui->SetLoaderText( "Loading dump", false );
			m_map->Unserialize( types::Buffer( util::FS::ReadFile( filename ) ) );
			ec = map::Map::EC_NONE;
		}
		else
#endif
		{
#ifdef DEBUG
			if ( !m_connection && config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_FILE ) ) {
				const std::string& filename = config->GetQuickstartMapFile();
				ec = m_map->LoadFromFile( filename );
			}
			else
#endif
			{
				const auto& map_settings = m_state->m_settings.global.map;
				if ( map_settings.type == MapSettings::MT_MAPFILE ) {
					ASSERT( !map_settings.filename.empty(), "loading map requested but map file not specified" );
					ec = m_map->LoadFromFile( map_settings.filename );
				}
				else {
					ec = m_map->Generate( map_settings, MT_C );
				}
			}

		}

		if ( !ec && canceled ) {
			ec = map::Map::EC_ABORTED;
		}

		if ( !ec ) {
			m_slot->SetPlayerFlag( Slot::PF_MAP_DOWNLOADED ); // map was generated locally
			if ( m_connection ) {
				m_connection->IfServer(
					[ this ]( Server* connection ) -> void {
						connection->SetGameState( Connection::GS_RUNNING ); // allow clients to download map
					}
				);
				m_connection->UpdateSlot( m_slot_num, m_slot, true );
			}

			ui->SetLoaderText( "Waiting for players" );
			m_game_state = GS_INITIALIZING;
			response.result = R_SUCCESS;
		}

	}
	else {
		m_connection->IfClient(
			[ this, &response, ui ]( Client* connection ) -> void {

				// wait for server to initialize
				ui->SetLoaderText( "Waiting for server" );

				connection->m_on_players_list_update = [ this ]() -> void {
					size_t slots_i = 0;
					for ( auto& slot : m_state->m_slots.GetSlots() ) {
						//m_players_section->UpdateSlot( slots_i++, &slot );
						int a = 5;
						a++;
					}

				};

				const auto f_download_map = [ this, ui, connection ] {

					ui->SetLoaderText( "Downloading world snapshot" );

					connection->m_on_download_progress = [ ui ]( const float progress ) -> void {
						ui->SetLoaderText( "Downloading world snapshot:  " + std::to_string( (size_t)std::round( progress * 100 ) ) + "%" );
					};
					connection->m_on_download_complete = [ this, connection ]( const std::string serialized_snapshot ) -> void {
						connection->m_on_download_complete = nullptr;
						connection->m_on_download_progress = nullptr;
						Log( "Unpacking world snapshot" );
						auto buf = types::Buffer( serialized_snapshot );

						// map
						auto b = types::Buffer( buf.ReadString() );
						NEW( m_map, map::Map, this );
						const auto ec = m_map->LoadFromBuffer( b );
						if ( ec == map::Map::EC_NONE ) {

							// units
							auto b = types::Buffer( buf.ReadString() );
							UnserializeUnits( b );

							m_game_state = GS_INITIALIZING;
						}
						else {
							Log( "WARNING: failed to unpack world snapshot (code=" + std::to_string( ec ) + ")" );
							connection->Disconnect( "Snapshot format mismatch" );
						}
					};
					connection->RequestDownload();
				};
				if ( connection->GetGameState() == Connection::GS_RUNNING ) {
					// server already initialized
					f_download_map();
				}
				else {
					// wait for server to initialize
					connection->m_on_game_state_change = [ this, f_download_map ]( const Connection::game_state_t state ) -> void {
						if ( state == Connection::GS_RUNNING ) {
							f_download_map();
						}
						else {
							THROW( "unexpected game state: " + std::to_string( state ) );
						}
					};
				}
				response.result = R_SUCCESS;
			}
		);
	}
}

void Game::ResetGame() {

	if ( m_game_state != GS_NONE ) {
		// TODO: do something?
		m_game_state = GS_NONE;
	}
	m_init_cancel = false;
	m_player = nullptr;
	m_slot_num = 0;
	m_slot = nullptr;

	for ( auto& it : m_unprocessed_units ) {
		delete it;
	}
	m_unprocessed_units.clear();

	for ( auto& it : m_unprocessed_events ) {
		delete it;
	}
	m_unprocessed_events.clear();

	for ( auto& it : m_units ) {
		delete it.second;
	}
	m_units.clear();
	for ( auto& it : m_unit_defs ) {
		delete it.second;
	}
	m_unit_defs.clear();

	if ( m_map ) {
		Log( "Resetting map" );
		DELETE( m_map );
		m_map = nullptr;
	}

	ASSERT( m_pending_frontend_requests, "pending events not set" );
	m_pending_frontend_requests->clear();

	if ( m_current_turn ) {
		DELETE( m_current_turn );
		m_current_turn = nullptr;
	}
	m_is_turn_complete = false;

	if ( m_state ) {
		// ui thread will reset state as needed
		m_state->UnsetGame();
		m_state = nullptr;
		if ( m_connection ) {
			m_connection->Disconnect();
			m_connection->ResetHandlers();
		}
		m_connection = nullptr;
	}
}

void Game::NextTurn() {
	m_current_turn->Activate();
	Log( "turn started" );

	m_state->m_bindings->Call( Bindings::CS_ON_TURN );

	auto fr = FrontendRequest( FrontendRequest::FR_TURN_ACTIVE_STATUS );
	fr.data.turn_active_status.is_turn_active = true;
	AddFrontendRequest( fr );
}

void Game::CheckTurnComplete() {
	bool is_turn_complete = true;

	for ( const auto& unit : m_units ) {
		if ( unit.second->m_owner == m_slot && unit.second->HasMovesLeft() ) {
			is_turn_complete = false;
			break;
		}
	}

	if ( m_is_turn_complete != is_turn_complete ) {
		m_is_turn_complete = is_turn_complete;
		Log( "Sending turn complete status: " + std::to_string( m_is_turn_complete ) );
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_COMPLETE_STATUS );
		fr.data.turn_complete_status.is_turn_complete = m_is_turn_complete;
		AddFrontendRequest( fr );
	}
}

}
