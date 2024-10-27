#include "Game.h"

#include "engine/Engine.h"
#include "types/Exception.h"
#include "types/texture/Texture.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "util/FS.h"
#include "types/Buffer.h"
#include "State.h"
#include "map_editor/MapEditor.h"
#include "event/FinalizeTurn.h"
#include "event/TurnFinalized.h"
#include "event/AdvanceTurn.h"
#include "event/DespawnUnit.h"
#include "event/RequestTileLocks.h"
#include "event/LockTiles.h"
#include "event/RequestTileUnlocks.h"
#include "event/UnlockTiles.h"
#include "util/random/Random.h"
#include "config/Config.h"
#include "slot/Slots.h"
#include "Player.h"
#include "connection/Connection.h"
#include "connection/Server.h"
#include "connection/Client.h"
#include "map/Map.h"
#include "map/Consts.h"
#include "gse/type/String.h"
#include "gse/type/Int.h"
#include "gse/type/Undefined.h"
#include "gse/type/Array.h"
#include "ui/UI.h"
#include "map/tile/Tiles.h"
#include "map/MapState.h"
#include "bindings/Bindings.h"
#include "graphics/Graphics.h"
#include "Resource.h"
#include "animation/Def.h"
#include "unit/Def.h"
#include "unit/Unit.h"
#include "unit/MoraleSet.h"
#include "base/PopDef.h"
#include "base/Base.h"

namespace game {
namespace backend {

response_map_data_t::~response_map_data_t() {
	if ( terrain_texture ) {
		DELETE( terrain_texture );
	}
	if ( terrain_mesh ) {
		DELETE( terrain_mesh );
	}
	if ( terrain_data_mesh ) {
		DELETE( terrain_data_mesh );
	}
};

InvalidEvent::InvalidEvent( const std::string& reason, const event::Event* event )
	: Exception(
	"InvalidEvent", (std::string)
		"Event validation failed!\n" +
		TS_OFFSET + "reason: '" + reason + "'\n" +
		TS_OFFSET + "event: " + event->ToString( TS_OFFSET )
) {}

common::mt_id_t Game::MT_Ping() {
	MT_Request request = {};
	request.op = OP_PING;
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_Init( State* state ) {
	MT_Request request = {};
	request.op = OP_INIT;
	request.data.init.state = state;
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_Chat( const std::string& message ) {
	MT_Request request = {};
	request.op = OP_CHAT;
	NEW( request.data.save_map.path, std::string, message );
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_GetMapData() {
	MT_Request request = {};
	request.op = OP_GET_MAP_DATA;
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_Reset() {
	m_init_cancel = true; // stop initialization in Iterate()
	MT_Request request = {};
	request.op = OP_RESET;
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_SaveMap( const std::string& path ) {
	ASSERT( !path.empty(), "savemap path is empty" );
	MT_Request request = {};
	request.op = OP_SAVE_MAP;
	NEW( request.data.save_map.path, std::string );
	*request.data.save_map.path = path;
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_EditMap( const types::Vec2< size_t >& tile_coords, map_editor::tool_type_t tool, map_editor::brush_type_t brush, map_editor::draw_mode_t draw_mode ) {
	MT_Request request = {};
	request.op = OP_EDIT_MAP;
	request.data.edit_map.tile_x = tile_coords.x;
	request.data.edit_map.tile_y = tile_coords.y;
	request.data.edit_map.tool = tool;
	request.data.edit_map.brush = brush;
	request.data.edit_map.draw_mode = draw_mode;
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_GetFrontendRequests() {
	MT_Request request = {};
	request.op = OP_GET_FRONTEND_REQUESTS;
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_SendBackendRequests( const std::vector< BackendRequest >& requests ) {
	MT_Request request = {};
	request.op = OP_SEND_BACKEND_REQUESTS;
	NEW( request.data.send_backend_requests.requests, std::vector< BackendRequest >, requests );
	return MT_CreateRequest( request );
}

common::mt_id_t Game::MT_AddEvent( const event::Event* event ) {
	MT_Request request = {};
	request.op = OP_ADD_EVENT;
	NEW( request.data.add_event.serialized_event, std::string, event::Event::Serialize( event ).ToString() );
	return MT_CreateRequest( request );
}

#ifdef DEBUG
#define x( _method, _op ) \
    common::mt_id_t Game::_method( const std::string& path ) { \
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

	NEW( m_random, util::random::Random );

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
				for ( const auto& slot : m_state->m_slots->GetSlots() ) {
					if ( slot.GetState() == slot::Slot::SS_PLAYER && !slot.HasPlayerFlag( slot::PF_MAP_DOWNLOADED ) ) {
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
				m_slot->SetPlayerFlag( slot::PF_MAP_DOWNLOADED );
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

					m_response_map_data->tiles = m_map->GetTilesPtr()->GetTilesPtr();
					m_response_map_data->tile_states = m_map->GetMapState()->GetTileStatesPtr();

					if ( m_old_map ) {
						Log( "Destroying old map state" );
						DELETE( m_old_map );
						m_old_map = nullptr;
					}

					// notify server of successful initialization
					m_slot->SetPlayerFlag( slot::PF_GAME_INITIALIZED );
					if ( m_connection ) {
						m_connection->UpdateSlot( m_slot_num, m_slot, true );
					}

					{
						const auto& factions = m_state->m_settings.global.game_rules.m_factions;
						auto* faction_defines = new FrontendRequest::faction_defines_t();
						for ( const auto& it : factions ) {
							faction_defines->push_back( &it.second );
						}
						auto fr = FrontendRequest( FrontendRequest::FR_FACTION_DEFINE );
						fr.data.faction_define.factiondefs = faction_defines;
						AddFrontendRequest( fr );
					}

					{
						const auto& slots = m_state->m_slots->GetSlots();
						auto* slot_defines = new FrontendRequest::slot_defines_t();
						for ( const auto& slot : slots ) {
							if ( slot.GetState() == slot::Slot::SS_OPEN || slot.GetState() == slot::Slot::SS_CLOSED ) {
								continue;
							}
							ASSERT( slot.GetState() == slot::Slot::SS_PLAYER, "unknown slot state: " + std::to_string( slot.GetState() ) );
							auto* player = slot.GetPlayer();
							ASSERT( player, "slot player not set" );
							slot_defines->push_back(
								FrontendRequest::slot_define_t{
									slot.GetIndex(),
									player->GetFaction()->m_id
								}
							);
						}
						auto fr = FrontendRequest( FrontendRequest::FR_SLOT_DEFINE );
						fr.data.slot_define.slotdefs = slot_defines;
						AddFrontendRequest( fr );
					}

					// start main loop
					m_game_state = GS_RUNNING;

					if ( m_game_state == GS_RUNNING ) {

						for ( auto& it : m_unprocessed_units ) {
							SpawnUnit( it );
						}
						m_unprocessed_units.clear();
						for ( auto& it : m_unprocessed_bases ) {
							SpawnBase( it );
						}
						m_unprocessed_bases.clear();
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
						if ( m_state->IsMaster() ) {
							try {
								m_state->m_bindings->Call( bindings::Bindings::CS_ON_START );
							}
							catch ( gse::Exception& e ) {
								Log( (std::string)"Initialization failed: " + e.ToStringAndCleanup() );
								f_init_failed( e.what() );
							}
							GlobalAdvanceTurn();
						}
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
	PushUnitUpdates();
	PushBaseUpdates();
	ProcessTileLockRequests();
}

util::random::Random* Game::GetRandom() const {
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
		return m_state->m_slots->GetSlot( 0 ).GetPlayer();
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

				typedef std::unordered_map< std::string, map::sprite_actor_t > t1; // can't use comma in macro below
				NEW( response.data.edit_map.sprites.actors_to_add, t1 );
				*response.data.edit_map.sprites.actors_to_add = m_map->m_sprite_actors_to_add;

				typedef std::unordered_map< size_t, std::string > t2; // can't use comma in macro below
				NEW( response.data.edit_map.sprites.instances_to_remove, t2 );
				*response.data.edit_map.sprites.instances_to_remove = m_map->m_sprite_instances_to_remove;

				typedef std::unordered_map< size_t, std::pair< std::string, types::Vec3 > > t3; // can't use comma in macro below
				NEW( response.data.edit_map.sprites.instances_to_add, t3 );
				*response.data.edit_map.sprites.instances_to_add = m_map->m_sprite_instances_to_add;

				// TODO: remove invalid units and terraforming

				auto fr = FrontendRequest( FrontendRequest::FR_UPDATE_TILES );
				NEWV( tile_updates, FrontendRequest::tile_updates_t );
				tile_updates->reserve( tiles_to_reload.size() );
				for ( const auto& tile : tiles_to_reload ) {
					tile_updates->push_back(
						{
							tile,
							m_map->GetTileState( tile )
						}
					);
				}
				fr.data.update_tiles.tile_updates = tile_updates;
				AddFrontendRequest( fr );
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
				Log( "Sending " + std::to_string( m_pending_frontend_requests->size() ) + " events to frontend" );
				response.data.get_frontend_requests.requests = m_pending_frontend_requests; // will be destroyed in DestroyResponse
				NEW( m_pending_frontend_requests, std::vector< FrontendRequest > ); // reset
			}
			else {
				response.data.get_frontend_requests.requests = nullptr;
			}
			response.result = R_SUCCESS;
			break;
		}
		case OP_SEND_BACKEND_REQUESTS: {
			try {
				for ( const auto& r : *request.data.send_backend_requests.requests ) {
					switch ( r.type ) {
						case BackendRequest::BR_GET_TILE_DATA: {
							const auto& tile = m_map->GetTile( r.data.get_tile_data.tile_x, r.data.get_tile_data.tile_y );
							const auto result = m_state->m_bindings->Call(
								bindings::Bindings::CS_ON_GET_TILE_YIELDS, {
									{
										"tile",
										tile->Wrap()
									},
									{
										"player",
										m_slot->Wrap()
									},
								}
							);
							if ( result.Get()->type != gse::type::Type::T_OBJECT ) {
								THROW( "unexpected return type: expected Object, got " + result.GetTypeString() );
							}
							const auto& values = ( (gse::type::Object*)result.Get() )->value;
							for ( const auto& v : values ) {
								if ( m_resources.find( v.first ) == m_resources.end() ) {
									THROW( "unknown resource type: " + v.first );
								}
							}
							NEWV( yields, FrontendRequest::tile_yields_t, {} );
							yields->reserve( m_resource_idx.size() );
							for ( const auto& idx : m_resource_idx ) {
								const auto& v = values.find( idx );
								if ( v == values.end() ) {
									DELETE( yields );
									THROW( "missing yields for resource: " + idx );
								}
								if ( v->second.Get()->type != gse::type::Type::T_INT ) {
									DELETE( yields );
									THROW( "invalid resource value, expected Int, got " + v->second.GetTypeString() + ": " + v->second.ToString() );
								}
								yields->push_back(
									{
										idx,
										( (gse::type::Int*)v->second.Get() )->value
									}
								);
							}
							auto fr = FrontendRequest( FrontendRequest::FR_TILE_DATA );
							fr.data.tile_data.tile_x = tile->coord.x;
							fr.data.tile_data.tile_y = tile->coord.y;
							fr.data.tile_data.tile_yields = yields;
							AddFrontendRequest( fr );
							break;
						}
						case BackendRequest::BR_ANIMATION_FINISHED: {
							const auto& it = m_running_animations_callbacks.find( r.data.animation_finished.animation_id );
							ASSERT( it != m_running_animations_callbacks.end(), "animation " + std::to_string( r.data.animation_finished.animation_id ) + " is not running" );
							const auto on_complete = it->second;
							m_running_animations_callbacks.erase( it );
							on_complete();
							break;
						}
						default:
							THROW( "unknown backend request type: " + std::to_string( r.type ) );
					}
				}
				response.result = R_SUCCESS;
			}
			catch ( gse::Exception& e ) {
				OnGSEError( e );
				response.result = R_ERROR;
			}
			catch ( std::runtime_error& e ) {
				OnError( e );
				response.result = R_ERROR;
			}
			break;
		}
		case OP_ADD_EVENT: {
			const std::string* errmsg = nullptr;
			event::Event* event = nullptr;
			auto buf = types::Buffer( *request.data.add_event.serialized_event );
			event = event::Event::Unserialize( buf );
			if ( !m_current_turn.IsActive() && event->m_type != event::Event::ET_UNCOMPLETE_TURN ) {
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
		case OP_SEND_BACKEND_REQUESTS: {
			if ( request.data.send_backend_requests.requests ) {
				DELETE( request.data.send_backend_requests.requests );
			}
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

void Game::OnError( std::runtime_error& err ) {
	auto fr = FrontendRequest( FrontendRequest::FR_ERROR );
	NEW( fr.data.error.what, std::string, (std::string)"Script error: " + err.what() );
	fr.data.error.stacktrace = nullptr;
	AddFrontendRequest( fr );
}

void Game::OnGSEError( gse::Exception& err ) {
	auto fr = FrontendRequest( FrontendRequest::FR_ERROR );
	NEW( fr.data.error.what, std::string, (std::string)"Script error: " + err.what() );
	NEW( fr.data.error.stacktrace, std::string, err.ToStringAndCleanup() );
	AddFrontendRequest( fr );
}

unit::MoraleSet* Game::GetMoraleSet( const std::string& name ) const {
	const auto& it = m_unit_moralesets.find( name );
	if ( it != m_unit_moralesets.end() ) {
		return it->second;
	}
	return nullptr;
}

unit::Unit* Game::GetUnit( const size_t id ) const {
	const auto& it = m_units.find( id );
	if ( it != m_units.end() ) {
		return it->second;
	}
	return nullptr;
}

unit::Def* Game::GetUnitDef( const std::string& name ) const {
	const auto& it = m_unit_defs.find( name );
	if ( it != m_unit_defs.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

base::PopDef* Game::GetPopDef( const std::string& id ) const {
	const auto& it = m_base_popdefs.find( id );
	if ( it != m_base_popdefs.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

base::Base* Game::GetBase( const size_t id ) const {
	const auto& it = m_bases.find( id );
	if ( it != m_bases.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

const gse::Value Game::AddEvent( event::Event* event ) {
	ASSERT( event->m_initiator_slot == m_slot_num, "initiator slot mismatch" );
	if ( m_connection ) {
		m_connection->SendGameEvent( event );
	}
	if ( m_state->IsMaster() ) {
		// note that this will work only on master, do slaves need return values too? i.e. for callbacks
		return ProcessEvent( event );
	}
	return VALUE( gse::type::Undefined );
}

void Game::RefreshUnit( const unit::Unit* unit ) {
	if ( unit->m_health <= 0.0f ) {
		if ( GetState()->IsMaster() ) {
			AddEvent( new event::DespawnUnit( GetSlotNum(), unit->m_id ) );
		}
	}
	else {
		QueueUnitUpdate( unit, UUO_REFRESH );
	}
}

void Game::RefreshBase( const base::Base* base ) {
	QueueBaseUpdate( base, BUO_REFRESH );
}

void Game::DefineAnimation( animation::Def* def ) {
	Log( "Defining animation ('" + def->m_id + "')" );

	ASSERT( m_animation_defs.find( def->m_id ) == m_animation_defs.end(), "Animation definition '" + def->m_id + "' already exists" );

	// backend doesn't need any animation details, just keep track of it's existence for validations
	m_animation_defs.insert(
		{
			def->m_id,
			def
		}
	);

	auto fr = FrontendRequest( FrontendRequest::FR_ANIMATION_DEFINE );
	NEW( fr.data.animation_define.serialized_animation, std::string, animation::Def::Serialize( def ).ToString() );
	AddFrontendRequest( fr );
}

const std::string* Game::ShowAnimationOnTile( const std::string& animation_id, map::tile::Tile* tile, const cb_oncomplete& on_complete ) {
	if ( m_animation_defs.find( animation_id ) == m_animation_defs.end() ) {
		return new std::string( "Animation '" + animation_id + "' is not defined" );
	}
	if ( !tile->IsLocked() ) {
		return new std::string( "Tile must be locked before showing animation" );
	}
	const auto running_animation_id = m_next_running_animation_id++;
	m_running_animations_callbacks.insert(
		{
			running_animation_id,
			on_complete
		}
	);
	auto fr = FrontendRequest( FrontendRequest::FR_ANIMATION_SHOW );
	NEW( fr.data.animation_show.animation_id, std::string, animation_id );
	fr.data.animation_show.running_animation_id = running_animation_id;
	const auto c = GetTileRenderCoords( tile );
	fr.data.animation_show.render_coords = {
		c.x,
		c.y,
		c.z,
	};
	AddFrontendRequest( fr );
	return nullptr; // no error
}

void Game::DefineResource( Resource* resource ) {
	Log( "Defining resource ('" + resource->m_id + "')" );

	ASSERT( m_resources.find( resource->m_id ) == m_resources.end(), "Resource '" + resource->m_id + "' already exists" );

	m_resources.insert(
		{
			resource->m_id,
			resource
		}
	);
	m_resource_idx_map.insert(
		{
			resource->m_id,
			m_resource_idx.size()
		}
	);
	m_resource_idx.push_back( resource->m_id );
}

void Game::DefineMoraleSet( unit::MoraleSet* moraleset ) {
	Log( "Defining unit moraleset ('" + moraleset->m_id + "')" );

	ASSERT( m_unit_moralesets.find( moraleset->m_id ) == m_unit_moralesets.end(), "Unit moraleset '" + moraleset->m_id + "' already exists" );

	m_unit_moralesets.insert(
		{
			moraleset->m_id,
			moraleset
		}
	);
}

void Game::DefineUnit( unit::Def* def ) {
	Log( "Defining unit ('" + def->m_id + "')" );

	ASSERT( m_unit_defs.find( def->m_id ) == m_unit_defs.end(), "Unit definition '" + def->m_id + "' already exists" );

	m_unit_defs.insert(
		{
			def->m_id,
			def
		}
	);

	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_DEFINE );
	NEW( fr.data.unit_define.serialized_unitdef, std::string, unit::Def::Serialize( def ).ToString() );
	AddFrontendRequest( fr );
}

void Game::SpawnUnit( unit::Unit* unit ) {
	if ( m_game_state != GS_RUNNING ) {
		m_unprocessed_units.push_back( unit );
		return;
	}

	auto* tile = unit->GetTile();

	Log( "Spawning unit #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + tile->ToString() );

	ASSERT( m_units.find( unit->m_id ) == m_units.end(), "duplicate unit id" );
	m_units.insert_or_assign( unit->m_id, unit );

	QueueUnitUpdate( unit, UUO_SPAWN );

	if ( m_state->IsMaster() ) {
		m_state->m_bindings->Call(
			bindings::Bindings::CS_ON_UNIT_SPAWN, {
				{
					"unit",
					unit->Wrap()
				},
			}
		);
	}
}

void Game::SkipUnitTurn( const size_t unit_id ) {
	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );
	auto* unit = it->second;

	Log( "Skipping unit turn #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + unit->GetTile()->ToString() );

	unit->m_movement = 0.0f;

	RefreshUnit( unit );
}

void Game::DespawnUnit( const size_t unit_id ) {

	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );
	auto* unit = it->second;

	Log( "Despawning unit #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + unit->GetTile()->ToString() );

	QueueUnitUpdate( unit, UUO_DESPAWN );

	auto* tile = unit->GetTile();
	ASSERT( tile, "unit tile not set" );
	const auto& tile_it = tile->units.find( unit->m_id );
	ASSERT( tile_it != tile->units.end(), "unit id not found in tile" );
	tile->units.erase( tile_it );

	m_units.erase( it );

	if ( m_state->IsMaster() ) {
		m_state->m_bindings->Call(
			bindings::Bindings::CS_ON_UNIT_DESPAWN, {
				{
					"unit",
					unit->Wrap()
				}
			}
		);
	}

	delete unit;
}

void Game::DefinePop( base::PopDef* pop_def ) {
	Log( "Defining base pop ('" + pop_def->m_id + "')" );

	ASSERT( m_base_popdefs.find( pop_def->m_id ) == m_base_popdefs.end(), "Base pop def '" + pop_def->m_id + "' already exists" );

	m_base_popdefs.insert(
		{
			pop_def->m_id,
			pop_def
		}
	);

	auto fr = FrontendRequest( FrontendRequest::FR_BASE_POP_DEFINE );
	NEW( fr.data.base_pop_define.serialized_popdef, std::string, base::PopDef::Serialize( pop_def ).ToString() );
	AddFrontendRequest( fr );

}

void Game::SpawnBase( base::Base* base ) {
	if ( m_game_state != GS_RUNNING ) {
		m_unprocessed_bases.push_back( base );
		return;
	}

	auto* tile = base->GetTile();

	// validate and fix name if needed (or assign if empty)
	std::vector< std::string > names_to_try = {};
	if ( base->m_name.empty() ) {
		const auto& names = base->m_owner->GetPlayer()->GetFaction()->m_base_names;
		names_to_try = tile->is_water_tile
			? names.water
			: names.land;
	}
	else if ( m_registered_base_names.find( base->m_name ) != m_registered_base_names.end() ) {
		names_to_try = { base->m_name };
	}
	if ( !names_to_try.empty() ) {
		size_t cycle = 0;
		bool found = false;
		while ( !found ) {
			cycle++;
			for ( const auto& name_to_try : names_to_try ) {
				base->m_name = cycle == 1
					? name_to_try
					: name_to_try + " " + std::to_string( cycle );
				if ( m_registered_base_names.find( base->m_name ) == m_registered_base_names.end() ) {
					found = true;
					break;
				}
			}
		}
	}
	m_registered_base_names.insert( base->m_name );

	Log( "Spawning base #" + std::to_string( base->m_id ) + " ( " + base->m_name + " ) at " + base->GetTile()->ToString() );

	ASSERT( m_bases.find( base->m_id ) == m_bases.end(), "duplicate base id" );
	m_bases.insert_or_assign( base->m_id, base );

	QueueBaseUpdate( base, BUO_SPAWN );

	if ( m_state->IsMaster() ) {
		m_state->m_bindings->Call(
			bindings::Bindings::CS_ON_BASE_SPAWN, {
				{
					"base",
					base->Wrap()
				},
			}
		);
	}

	RefreshBase( base );
}

const std::string* Game::MoveUnitValidate( unit::Unit* unit, map::tile::Tile* dst_tile ) {
	const auto result = m_state->m_bindings->Call(
		bindings::Bindings::CS_ON_UNIT_MOVE_VALIDATE, {
			{
				"unit",
				unit->Wrap()
			},
			{
				"src_tile",
				unit->GetTile()->Wrap()
			},
			{
				"dst_tile",
				dst_tile->Wrap()
			},
		}
	);
	switch ( result.Get()->type ) {
		case gse::type::Type::T_NULL:
		case gse::type::Type::T_UNDEFINED:
			return nullptr; // no errors
		case gse::type::Type::T_STRING:
			return new std::string( ( (gse::type::String*)result.Get() )->value ); // error
		default:
			THROW( "unexpected validation result type: " + gse::type::Type::GetTypeString( result.Get()->type ) );
	}
}

const gse::Value Game::MoveUnitResolve( unit::Unit* unit, map::tile::Tile* dst_tile ) {
	return m_state->m_bindings->Call(
		bindings::Bindings::CS_ON_UNIT_MOVE_RESOLVE, {
			{
				"unit",
				unit->Wrap()
			},
			{
				"src_tile",
				unit->GetTile()->Wrap()
			},
			{
				"dst_tile",
				dst_tile->Wrap()
			},
		}
	);
}

void Game::MoveUnitApply( unit::Unit* unit, map::tile::Tile* dst_tile, const gse::Value resolutions ) {
	ASSERT( dst_tile, "dst tile not set" );

	Log( "Moving unit #" + std::to_string( unit->m_id ) + " to " + dst_tile->coord.ToString() );

	auto* src_tile = unit->GetTile();
	ASSERT( src_tile, "src tile not set" );
	ASSERT( src_tile->units.find( unit->m_id ) != src_tile->units.end(), "src tile does not contain this unit" );
	ASSERT( dst_tile->units.find( unit->m_id ) == dst_tile->units.end(), "dst tile already contains this unit" );

	const auto result = m_state->m_bindings->Call(
		bindings::Bindings::CS_ON_UNIT_MOVE_APPLY, {
			{
				"unit",
				unit->Wrap( true )
			},
			{
				"src_tile",
				src_tile->Wrap()
			},
			{
				"dst_tile",
				dst_tile->Wrap()
			},
			{
				"resolutions",
				resolutions
			}
		}
	);
}

const std::string* Game::MoveUnitToTile( unit::Unit* unit, map::tile::Tile* dst_tile, const cb_oncomplete& on_complete ) {
	const auto* src_tile = unit->GetTile();
	if ( src_tile == dst_tile ) {
		return new std::string( "Unit can't move because it's already on target tile" );
	}
	if ( !src_tile->IsLocked() ) {
		return new std::string( "Source tile must be locked before moving unit" );
	}
	if ( !dst_tile->IsLocked() ) {
		return new std::string( "Destination tile must be locked before moving unit" );
	}
	const auto running_animation_id = m_next_running_animation_id++;
	m_running_animations_callbacks.insert(
		{
			running_animation_id,
			[ this, on_complete, unit, dst_tile ]() {
				unit->SetTile( dst_tile );
				on_complete();
			}
		}
	);
	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_MOVE );
	fr.data.unit_move.unit_id = unit->m_id;
	fr.data.unit_move.dst_tile_coords = {
		dst_tile->coord.x,
		dst_tile->coord.y
	};
	fr.data.unit_move.running_animation_id = running_animation_id;
	AddFrontendRequest( fr );
	return nullptr; // no error
}

const std::string* Game::AttackUnitValidate( unit::Unit* attacker, unit::Unit* defender ) {
	const auto result = m_state->m_bindings->Call(
		bindings::Bindings::CS_ON_UNIT_ATTACK_VALIDATE, {
			{
				"attacker",
				attacker->Wrap()
			},
			{
				"defender",
				defender->Wrap()
			},
		}
	);
	switch ( result.Get()->type ) {
		case gse::type::Type::T_NULL:
		case gse::type::Type::T_UNDEFINED:
			return nullptr; // no errors
		case gse::type::Type::T_STRING:
			return new std::string( ( (gse::type::String*)result.Get() )->value ); // error
		default:
			THROW( "unexpected validation result type: " + gse::type::Type::GetTypeString( result.Get()->type ) );
	}
	return nullptr;
}

const gse::Value Game::AttackUnitResolve( unit::Unit* attacker, unit::Unit* defender ) {
	return m_state->m_bindings->Call(
		bindings::Bindings::CS_ON_UNIT_ATTACK_RESOLVE, {
			{
				"attacker",
				attacker->Wrap()
			},
			{
				"defender",
				defender->Wrap()
			},
		}
	);
}

void Game::AttackUnitApply( unit::Unit* attacker, unit::Unit* defender, const gse::Value resolutions ) {
	m_state->m_bindings->Call(
		bindings::Bindings::CS_ON_UNIT_ATTACK_APPLY, {
			{
				"attacker",
				attacker->Wrap( true )
			},
			{
				"defender",
				defender->Wrap( true )
			},
			{
				"resolutions",
				resolutions
			}
		}
	);
	if ( attacker->m_health <= 0.0f ) {
		if ( GetState()->IsMaster() ) {
			AddEvent( new event::DespawnUnit( GetSlotNum(), attacker->m_id ) );
		}
	}
	else {
		RefreshUnit( attacker );
	}
	if ( defender->m_health <= 0.0f ) {
		if ( GetState()->IsMaster() ) {
			AddEvent( new event::DespawnUnit( GetSlotNum(), defender->m_id ) );
		}
	}
	else {
		RefreshUnit( defender );
	}
}

const size_t Game::GetTurnId() const {
	return m_current_turn.GetId();
}

const bool Game::IsTurnActive() const {
	return m_current_turn.IsActive();
}

const bool Game::IsTurnCompleted( const size_t slot_num ) const {
	const auto& slot = m_state->m_slots->GetSlot( slot_num );
	ASSERT( slot.GetState() == slot::Slot::SS_PLAYER, "slot is not player" );
	const auto* player = slot.GetPlayer();
	ASSERT( player, "slot player not set" );
	return player->IsTurnCompleted();
}

const bool Game::IsTurnChecksumValid( const util::crc32::crc_t checksum ) const {
	return m_turn_checksum == checksum;
}

void Game::CompleteTurn( const size_t slot_num ) {
	const auto& slot = m_state->m_slots->GetSlot( slot_num );
	ASSERT( slot.GetState() == slot::Slot::SS_PLAYER, "slot is not player" );
	auto* player = slot.GetPlayer();
	ASSERT( player, "slot player not set" );
	player->CompleteTurn();

	if ( slot_num == m_slot_num ) {
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_STATUS );
		fr.data.turn_status.status = turn::TS_WAITING_FOR_PLAYERS;
		AddFrontendRequest( fr );
	}

	if ( m_state->IsMaster() ) {
		// check if all players completed their turns
		bool is_turn_complete = true;
		for ( const auto& slot : m_state->m_slots->GetSlots() ) {
			if ( slot.GetState() == slot::Slot::SS_PLAYER && !slot.GetPlayer()->IsTurnCompleted() ) {
				is_turn_complete = false;
				break;
			}
		}
		if ( is_turn_complete ) {
			GlobalFinalizeTurn();
		}
	}
}

void Game::UncompleteTurn( const size_t slot_num ) {
	const auto& slot = m_state->m_slots->GetSlot( slot_num );
	ASSERT( slot.GetState() == slot::Slot::SS_PLAYER, "slot is not player" );
	auto* player = slot.GetPlayer();
	ASSERT( player, "slot player not set" );
	player->UncompleteTurn();
	if ( slot_num == m_slot_num ) {
		m_is_turn_complete = false;
		CheckTurnComplete();
		if ( !m_is_turn_complete ) {
			auto fr = FrontendRequest( FrontendRequest::FR_TURN_STATUS );
			fr.data.turn_status.status = turn::TS_TURN_ACTIVE;
			AddFrontendRequest( fr );
		}
	}
}

void Game::FinalizeTurn() {
	m_turn_checksum = m_current_turn.FinalizeAndChecksum();
	AddEvent( new event::TurnFinalized( m_slot_num, m_turn_checksum ) );
}

void Game::AdvanceTurn( const size_t turn_id ) {
	m_current_turn.AdvanceTurn( turn_id );
	m_is_turn_complete = false;
	Log( "Turn started: " + std::to_string( turn_id ) );

	{
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_ADVANCE );
		fr.data.turn_advance.turn_id = turn_id;
		AddFrontendRequest( fr );
	}

	for ( auto& it : m_units ) {
		auto* unit = it.second;
		m_state->m_bindings->Call(
			bindings::Bindings::CS_ON_UNIT_TURN, {
				{
					"unit",
					unit->Wrap( true )
				},
			}
		);
		unit->m_moved_this_turn = false;
		RefreshUnit( unit );
	}

	for ( auto& it : m_bases ) {
		auto* base = it.second;
		m_state->m_bindings->Call(
			bindings::Bindings::CS_ON_BASE_TURN, {
				{
					"base",
					base->Wrap( true )
				},
			}
		);
		RefreshBase( base );
	}

	m_state->m_bindings->Call( bindings::Bindings::CS_ON_TURN );

	for ( const auto& slot : m_state->m_slots->GetSlots() ) {
		if ( slot.GetState() == slot::Slot::SS_PLAYER ) {
			slot.GetPlayer()->UncompleteTurn();
		}
	}

	m_is_turn_complete = false;
	CheckTurnComplete();
	if ( !m_is_turn_complete ) {
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_STATUS );
		fr.data.turn_status.status = turn::TS_TURN_ACTIVE;
		AddFrontendRequest( fr );
	}

}

void Game::GlobalFinalizeTurn() {
	ASSERT( m_state->IsMaster(), "not master" );
	ASSERT( m_current_turn.IsActive(), "current turn not active" );
	ASSERT( m_verified_turn_checksum_slots.empty(), "turn finalization slots not empty" );
	Log( "Finalizing turn ( checksum = " + std::to_string( m_turn_checksum ) + " )" );
	AddEvent( new event::FinalizeTurn( m_slot_num ) );
}

void Game::GlobalProcessTurnFinalized( const size_t slot_num, const util::crc32::crc_t checksum ) {
	ASSERT( m_state->IsMaster(), "not master" );
	ASSERT( m_turn_checksum == checksum, "turn checksum mismatch" );
	ASSERT( m_verified_turn_checksum_slots.find( slot_num ) == m_verified_turn_checksum_slots.end(), "duplicate turn finalization from " + std::to_string( slot_num ) );
	m_verified_turn_checksum_slots.insert( slot_num );

	bool is_turn_finalized = true;
	for ( const auto& slot : m_state->m_slots->GetSlots() ) {
		if (
			slot.GetState() == slot::Slot::SS_PLAYER &&
				m_verified_turn_checksum_slots.find( slot.GetIndex() ) == m_verified_turn_checksum_slots.end()
			) {
			is_turn_finalized = false;
			break;
		}
	}

	if ( is_turn_finalized ) {
		GlobalAdvanceTurn();
	}
}

static size_t s_turn_id = 0;
void Game::GlobalAdvanceTurn() {
	ASSERT( m_state->IsMaster(), "not master" );

	// reset some states
	s_turn_id++;
	m_verified_turn_checksum_slots.clear();
	m_turn_checksum = 0;

	Log( "Advancing turn ( id = " + std::to_string( s_turn_id ) + " )" );
	AddEvent( new event::AdvanceTurn( m_slot_num, s_turn_id ) );
}

void Game::SendTileLockRequest( const map::tile::positions_t& tile_positions, const cb_oncomplete& on_complete ) {
	// testing
	/*m_tile_lock_callbacks.push_back(
		{
			tile_positions,
			on_complete
		}
	);*/
	LockTiles( m_slot_num, tile_positions );
	on_complete();
	//AddEvent( new event::RequestTileLocks( m_slot_num, tile_positions ) );
}

void Game::RequestTileLocks( const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	if ( m_state->IsMaster() ) {
		Log( "Tile locks request from " + std::to_string( initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( tile_positions, "" ) );
		AddTileLockRequest( true, initiator_slot, tile_positions );
	}
}

void Game::LockTiles( const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	for ( const auto& pos : tile_positions ) {
		auto* tile = m_map->GetTile( pos.x, pos.y );
		ASSERT_NOLOG( !tile->IsLocked(), "tile " + pos.ToString() + " is already locked" );
		tile->Lock( initiator_slot );
	}
	for ( auto it = m_tile_lock_callbacks.begin() ; it != m_tile_lock_callbacks.end() ; it++ ) {
		const auto& it_positions = it->first;
		const auto sz = it_positions.size();
		if ( sz == tile_positions.size() ) {
			bool match = true;
			for ( size_t i = 0 ; i < sz ; i++ ) {
				if ( tile_positions.at( i ) != it_positions.at( i ) ) {
					match = false;
					break;
				}
			}
			if ( match ) {
				const auto cb = it->second;
				m_tile_lock_callbacks.erase( it );
				cb();
				break;
			}
		}
	}
}

void Game::SendTileUnlockRequest( const map::tile::positions_t& tile_positions ) {
	// testing
	UnlockTiles( m_slot_num, tile_positions );
	//AddEvent( new event::RequestTileUnlocks( m_slot_num, tile_positions ) );
}

void Game::RequestTileUnlocks( const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	if ( m_state->IsMaster() ) {
		Log( "Tile unlocks request from " + std::to_string( initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( tile_positions, "" ) );
		AddTileLockRequest( false, initiator_slot, tile_positions );
	}
}

void Game::UnlockTiles( const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	for ( const auto& pos : tile_positions ) {
		auto* tile = m_map->GetTile( pos.x, pos.y );
		ASSERT_NOLOG( tile->IsLockedBy( initiator_slot ), "tile " + pos.ToString() + " is not locked by " + std::to_string( initiator_slot ) );
		tile->Unlock();
	}
}

rules::Faction* Game::GetFaction( const std::string& id ) const {
	const auto& it = m_state->m_settings.global.game_rules.m_factions.find( id ); // TODO: store factions in Game itself?
	ASSERT( it != m_state->m_settings.global.game_rules.m_factions.end(), "faction not found: " + id );
	return &it->second;
}

void Game::ValidateEvent( event::Event* event ) {
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
	if ( m_state->IsMaster() ) { // TODO: validate in either case?
		ValidateEvent( event );
	}

	ASSERT( event->m_is_validated, "event was not validated" );

	if ( m_state->IsMaster() ) {
		// things like random outcomes must be resolved on server only
		event->Resolve( this );
	}

	m_current_turn.AddEvent( event );
	return event->Apply( this );
}

const types::Vec3 Game::GetTileRenderCoords( const map::tile::Tile* tile ) {
	const auto* ts = m_map->GetTileState( tile );
	ASSERT( ts, "ts not set" );
	const auto l = tile->is_water_tile
		? map::tile::LAYER_WATER
		: map::tile::LAYER_LAND;
	const auto& layer = ts->layers[ l ];
	const auto& c = layer.coords.center;
	return {
		c.x,
		-c.y,
		c.z
	};
}

void Game::SerializeResources( types::Buffer& buf ) const {
	Log( "Serializing " + std::to_string( m_resources.size() ) + " resources" );
	buf.WriteInt( m_resources.size() );
	for ( const auto& it : m_resource_idx ) {
		ASSERT( m_resources.find( it ) != m_resources.end(), "invalid resource idx" );
		const auto& res = m_resources.at( it );
		buf.WriteString( res->m_id );
		buf.WriteString( Resource::Serialize( res ).ToString() );
	}
}

void Game::UnserializeResources( types::Buffer& buf ) {
	ASSERT( m_resources.empty(), "resources not empty" );
	size_t sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " resources" );
	m_resources.reserve( sz );
	m_resource_idx.reserve( sz );
	m_resource_idx_map.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineResource( Resource::Unserialize( b ) );
	}
}

void Game::SerializeUnits( types::Buffer& buf ) const {

	Log( "Serializing " + std::to_string( m_unit_moralesets.size() ) + " unit moralesets" );
	buf.WriteInt( m_unit_moralesets.size() );
	for ( const auto& it : m_unit_moralesets ) {
		buf.WriteString( it.first );
		buf.WriteString( unit::MoraleSet::Serialize( it.second ).ToString() );
	}

	Log( "Serializing " + std::to_string( m_unit_defs.size() ) + " unit defs" );
	buf.WriteInt( m_unit_defs.size() );
	for ( const auto& it : m_unit_defs ) {
		buf.WriteString( it.first );
		buf.WriteString( unit::Def::Serialize( it.second ).ToString() );
	}

	Log( "Serializing " + std::to_string( m_units.size() ) + " units" );
	buf.WriteInt( m_units.size() );
	for ( const auto& it : m_units ) {
		buf.WriteInt( it.first );
		buf.WriteString( unit::Unit::Serialize( it.second ).ToString() );
	}
	buf.WriteInt( unit::Unit::GetNextId() );

	Log( "Saved next unit id: " + std::to_string( unit::Unit::GetNextId() ) );
}

void Game::UnserializeUnits( types::Buffer& buf ) {
	ASSERT( m_unit_moralesets.empty(), "unit moralesets not empty" );
	ASSERT( m_unit_defs.empty(), "unit defs not empty" );
	ASSERT( m_units.empty(), "units not empty" );
	ASSERT( m_unprocessed_units.empty(), "unprocessed units not empty" );

	size_t sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " unit moralesets" );
	m_unit_moralesets.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineMoraleSet( unit::MoraleSet::Unserialize( b ) );
	}

	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " unit defs" );
	m_unit_defs.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineUnit( unit::Def::Unserialize( b ) );
	}

	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " units" );
	if ( m_game_state != GS_RUNNING ) {
		m_unprocessed_units.reserve( sz );
	}
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto unit_id = buf.ReadInt();
		auto b = types::Buffer( buf.ReadString() );
		SpawnUnit( unit::Unit::Unserialize( b, this ) );
	}

	unit::Unit::SetNextId( buf.ReadInt() );
	Log( "Restored next unit id: " + std::to_string( unit::Unit::GetNextId() ) );
}

void Game::SerializeBases( types::Buffer& buf ) const {

	Log( "Serializing " + std::to_string( m_base_popdefs.size() ) + " base pop defs" );
	buf.WriteInt( m_base_popdefs.size() );
	for ( const auto& it : m_base_popdefs ) {
		buf.WriteString( it.first );
		buf.WriteString( base::PopDef::Serialize( it.second ).ToString() );
	}

	Log( "Serializing " + std::to_string( m_bases.size() ) + " bases" );
	buf.WriteInt( m_bases.size() );
	for ( const auto& it : m_bases ) {
		buf.WriteInt( it.first );
		buf.WriteString( base::Base::Serialize( it.second ).ToString() );
	}
	buf.WriteInt( base::Base::GetNextId() );

	Log( "Saved next base id: " + std::to_string( base::Base::GetNextId() ) );
}

void Game::UnserializeBases( types::Buffer& buf ) {
	ASSERT( m_base_popdefs.empty(), "base pop defs not empty" );
	ASSERT( m_bases.empty(), "bases not empty" );
	ASSERT( m_unprocessed_bases.empty(), "unprocessed bases not empty" );

	size_t sz = buf.ReadInt();
	m_base_popdefs.reserve( sz );
	Log( "Unserializing " + std::to_string( sz ) + " base pop defs" );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefinePop( base::PopDef::Unserialize( b ) );
	}

	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " bases" );
	if ( m_game_state != GS_RUNNING ) {
		m_unprocessed_bases.reserve( sz );
	}
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto base_id = buf.ReadInt();
		auto b = types::Buffer( buf.ReadString() );
		SpawnBase( base::Base::Unserialize( b, this ) );
	}

	base::Base::SetNextId( buf.ReadInt() );
	Log( "Restored next base id: " + std::to_string( base::Base::GetNextId() ) );
}

void Game::SerializeAnimations( types::Buffer& buf ) const {
	Log( "Serializing " + std::to_string( m_animation_defs.size() ) + " animation defs" );
	buf.WriteInt( m_animation_defs.size() );
	for ( const auto& it : m_animation_defs ) {
		buf.WriteString( it.first );
		buf.WriteString( animation::Def::Serialize( it.second ).ToString() );
	}
}

void Game::UnserializeAnimations( types::Buffer& buf ) {
	ASSERT( m_animation_defs.empty(), "animation defs not empty" );
	size_t sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " animation defs" );
	m_animation_defs.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineAnimation( animation::Def::Unserialize( b ) );
	}
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
		const auto& slots = m_state->m_slots->GetSlots();
		for ( const auto& slot : slots ) {
			if ( slot.GetState() == slot::Slot::SS_PLAYER ) {
				auto* player = slot.GetPlayer();
				ASSERT( player, "player not set" );
				if ( !player->GetFaction().has_value() ) {
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
			[ this ]( connection::Server* connection ) -> void {

				connection->m_on_player_join = [ this, connection ]( const size_t slot_num, slot::Slot* slot, const Player* player ) -> void {
					Log( "Player " + player->GetFullName() + " is connecting..." );
					connection->GlobalMessage( "Connection from " + player->GetFullName() + "..." );
					// actual join will happen after he downloads and initializes map
				};

				connection->m_on_flags_update = [ this, connection ]( const size_t slot_num, slot::Slot* slot, const slot::player_flag_t old_flags, const slot::player_flag_t new_flags ) -> void {
					if ( !( old_flags & slot::PF_GAME_INITIALIZED ) && ( new_flags & slot::PF_GAME_INITIALIZED ) ) {
						const auto* player = slot->GetPlayer();
						Log( player->GetFullName() + " joined the game." );
						connection->GlobalMessage( player->GetFullName() + " joined the game." );
					}
				};

				connection->m_on_player_leave = [ this, connection ]( const size_t slot_num, slot::Slot* slot, const Player* player ) -> void {
					Log( "Player " + player->GetFullName() + " left the game." );
					connection->GlobalMessage( player->GetFullName() + " left the game." );
					const auto& it = m_tile_locks.find( slot_num );
					if ( it != m_tile_locks.end() ) {
						Log( "Releasing " + std::to_string( it->second.size() ) + " tile locks" );
						m_tile_locks.erase( it );
					}
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

					// resources
					{
						types::Buffer b;
						SerializeResources( b );
						buf.WriteString( b.ToString() );
					}

					// units
					{
						types::Buffer b;
						SerializeUnits( b );
						buf.WriteString( b.ToString() );
					}

					// bases
					{
						types::Buffer b;
						SerializeBases( b );
						buf.WriteString( b.ToString() );
					}

					// animations
					{
						types::Buffer b;
						SerializeAnimations( b );
						buf.WriteString( b.ToString() );
					}

					// send turn info
					Log( "Sending turn ID: " + std::to_string( s_turn_id ) );
					buf.WriteInt( s_turn_id );

					return buf.ToString();
				};

				connection->SetGameState( connection::Connection::GS_INITIALIZING );
			}
		);

		m_connection->IfClient(
			[ this ]( connection::Client* connection ) -> void {
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

		m_connection->m_on_game_event_validate = [ this ]( event::Event* event ) -> void {
			if ( m_state->IsMaster() ) {
				ASSERT( m_game_state == GS_RUNNING, "game is not running but received event" );
			}
			if ( m_game_state == GS_RUNNING ) {
				ValidateEvent( event );
			}
			else {
				m_unprocessed_events.push_back( event );
			}

		};

		m_connection->m_on_game_event_apply = [ this ]( event::Event* event ) -> void {
			if ( m_state->IsMaster() ) {
				ASSERT( m_game_state == GS_RUNNING, "game is not running but received event" );
			}
			if ( m_game_state == GS_RUNNING ) {
				ProcessEvent( event );
			}
		};

	}
	else {
		m_slot_num = 0;
	}
	m_player = m_state->m_slots->GetSlot( m_slot_num ).GetPlayer();
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
				auto& map_settings = m_state->m_settings.global.map;
				if ( map_settings.type == settings::MapSettings::MT_MAPFILE ) {
					ASSERT( !map_settings.filename.empty(), "loading map requested but map file not specified" );
					ec = m_map->LoadFromFile( map_settings.filename );
				}
				else {
					ec = m_map->Generate( &map_settings, MT_C );
				}
			}

		}

		if ( !ec && canceled ) {
			ec = map::Map::EC_ABORTED;
		}

		if ( !ec ) {
			m_slot->SetPlayerFlag( slot::PF_MAP_DOWNLOADED ); // map was generated locally
			if ( m_connection ) {
				m_connection->IfServer(
					[ this ]( connection::Server* connection ) -> void {
						connection->SetGameState( connection::Connection::GS_RUNNING ); // allow clients to download map
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
			[ this, &response, ui ]( connection::Client* connection ) -> void {

				// wait for server to initialize
				ui->SetLoaderText( "Waiting for server" );

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

							// resources
							{
								auto ub = types::Buffer( buf.ReadString() );
								UnserializeResources( ub );
							}

							// units
							{
								auto ub = types::Buffer( buf.ReadString() );
								UnserializeUnits( ub );
							}

							// bases
							{
								auto bb = types::Buffer( buf.ReadString() );
								UnserializeBases( bb );
							}

							// animations
							{
								auto ab = types::Buffer( buf.ReadString() );
								UnserializeAnimations( ab );
							}

							// get turn info
							const auto turn_id = buf.ReadInt();
							if ( turn_id > 0 ) {
								Log( "Received turn ID: " + std::to_string( turn_id ) );
								AdvanceTurn( turn_id );
							}

							m_game_state = GS_INITIALIZING;
						}
						else {
							Log( "WARNING: failed to unpack world snapshot (code=" + std::to_string( ec ) + ")" );
							connection->Disconnect( "Snapshot format mismatch" );
						}
					};
					connection->RequestDownload();
				};
				if ( connection->GetGameState() == connection::Connection::GS_RUNNING ) {
					// server already initialized
					f_download_map();
				}
				else {
					// wait for server to initialize
					connection->m_on_game_state_change = [ this, f_download_map ]( const connection::Connection::game_state_t state ) -> void {
						if ( state == connection::Connection::GS_RUNNING ) {
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

	m_running_animations_callbacks.clear();
	m_next_running_animation_id = 1;

	for ( auto& it : m_resources ) {
		delete it.second;
	}
	m_resources.clear();
	m_resource_idx.clear();
	m_resource_idx_map.clear();

	for ( auto& it : m_unit_moralesets ) {
		delete it.second;
	}
	m_unit_moralesets.clear();

	for ( auto& it : m_units ) {
		delete it.second;
	}
	m_units.clear();

	for ( auto& it : m_unit_defs ) {
		delete it.second;
	}
	m_unit_defs.clear();

	for ( auto& it : m_base_popdefs ) {
		delete it.second;
	}
	m_base_popdefs.clear();
	for ( auto& it : m_bases ) {
		delete it.second;
	}
	m_bases.clear();

	for ( auto& it : m_animation_defs ) {
		delete it.second;
	}
	m_animation_defs.clear();

	if ( m_map ) {
		Log( "Resetting map" );
		DELETE( m_map );
		m_map = nullptr;
	}

	ASSERT( m_pending_frontend_requests, "pending events not set" );
	m_pending_frontend_requests->clear();

	m_unit_updates.clear();
	m_base_updates.clear();

	m_tile_lock_requests.clear();
	m_tile_locks.clear();

	m_tile_lock_callbacks.clear();

	m_current_turn.Reset();
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

void Game::CheckTurnComplete() {
	if ( !m_current_turn.IsActive() ) {
		// turn not active
		return;
	}

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
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_STATUS );
		fr.data.turn_status.status = m_is_turn_complete
			? turn::TS_TURN_COMPLETE
			: turn::TS_TURN_ACTIVE;
		AddFrontendRequest( fr );
	}
}

void Game::QueueUnitUpdate( const unit::Unit* unit, const unit_update_op_t op ) {
	auto it = m_unit_updates.find( unit->m_id );
	if ( it == m_unit_updates.end() ) {
		it = m_unit_updates.insert(
			{
				unit->m_id,
				{
					{},
					unit,
				}
			}
		).first;
	}
	auto& update = it->second;
	if ( op == UUO_DESPAWN ) {
		if ( op & UUO_SPAWN ) {
			// if unit is despawned immediately after spawning - frontend doesn't need to know
			m_unit_updates.erase( it );
			return;
		}
		update.ops = UUO_NONE; // clear other actions if unit was despawned
	}
	// add to operations list
	update.ops = (unit_update_op_t)( (uint8_t)update.ops | (uint8_t)op );
}

void Game::QueueBaseUpdate( const base::Base* base, const base_update_op_t op ) {
	auto it = m_base_updates.find( base->m_id );
	if ( it == m_base_updates.end() ) {
		it = m_base_updates.insert(
			{
				base->m_id,
				{
					{},
					base,
				}
			}
		).first;
	}
	auto& update = it->second;
	if ( op == BUO_DESPAWN ) {
		if ( op & BUO_SPAWN ) {
			// if base is despawned immediately after spawning - frontend doesn't need to know
			m_base_updates.erase( it );
			return;
		}
		update.ops = BUO_NONE; // clear other actions if base was despawned
	}
	// add to operations list
	update.ops = (base_update_op_t)( (uint8_t)update.ops | (uint8_t)op );
}

void Game::PushUnitUpdates() {
	if ( m_game_state == GS_RUNNING && !m_unit_updates.empty() ) {
		for ( const auto& it : m_unit_updates ) {
			const auto unit_id = it.first;
			const auto& uu = it.second;
			const auto& unit = uu.unit;
			if ( uu.ops & UUO_SPAWN ) {
				auto fr = FrontendRequest( FrontendRequest::FR_UNIT_SPAWN );
				fr.data.unit_spawn.unit_id = unit->m_id;
				NEW( fr.data.unit_spawn.unitdef_id, std::string, unit->m_def->m_id );
				fr.data.unit_spawn.slot_index = unit->m_owner->GetIndex();
				const auto* tile = unit->GetTile();
				fr.data.unit_spawn.tile_coords = {
					tile->coord.x,
					tile->coord.y
				};
				const auto c = unit->GetRenderCoords();
				fr.data.unit_spawn.render_coords = {
					c.x,
					c.y,
					c.z
				};

				fr.data.unit_spawn.movement = unit->m_movement;
				fr.data.unit_spawn.morale = unit->m_morale;
				NEW( fr.data.unit_spawn.morale_string, std::string, unit->GetMoraleString() );
				fr.data.unit_spawn.health = unit->m_health;
				AddFrontendRequest( fr );
			}
			if ( uu.ops & UUO_REFRESH ) {
				auto fr = FrontendRequest( FrontendRequest::FR_UNIT_UPDATE );
				fr.data.unit_update.unit_id = unit->m_id;
				fr.data.unit_update.movement = unit->m_movement;
				fr.data.unit_update.health = unit->m_health;
				const auto* tile = unit->GetTile();
				fr.data.unit_update.tile_coords = {
					tile->coord.x,
					tile->coord.y
				};
				const auto c = unit->GetRenderCoords();
				fr.data.unit_update.render_coords = {
					c.x,
					c.y,
					c.z
				};
				AddFrontendRequest( fr );
			}
			if ( uu.ops & UUO_DESPAWN ) {
				auto fr = FrontendRequest( FrontendRequest::FR_UNIT_DESPAWN );
				fr.data.unit_despawn.unit_id = unit_id;
				AddFrontendRequest( fr );
			}
		}
		m_unit_updates.clear();
		CheckTurnComplete();
	}
}

void Game::PushBaseUpdates() {
	if ( m_game_state == GS_RUNNING && !m_base_updates.empty() ) {
		for ( const auto& it : m_base_updates ) {
			const auto base_id = it.first;
			const auto& bu = it.second;
			const auto& base = bu.base;
			if ( bu.ops & BUO_SPAWN ) {
				auto fr = FrontendRequest( FrontendRequest::FR_BASE_SPAWN );
				fr.data.base_spawn.base_id = base->m_id;
				fr.data.base_spawn.slot_index = base->m_owner->GetIndex();
				const auto* tile = base->GetTile();
				fr.data.base_spawn.tile_coords = {
					tile->coord.x,
					tile->coord.y
				};
				const auto c = base->GetRenderCoords();
				fr.data.base_spawn.render_coords = {
					c.x,
					c.y,
					c.z
				};
				NEW( fr.data.base_spawn.name, std::string, base->m_name );
				AddFrontendRequest( fr );
			}
			if ( bu.ops & BUO_REFRESH ) {
				auto fr = FrontendRequest( FrontendRequest::FR_BASE_UPDATE );
				fr.data.base_update.base_id = base->m_id;
				fr.data.base_update.slot_index = base->m_owner->GetIndex();
				NEW( fr.data.base_update.name, std::string, base->m_name );
				NEW( fr.data.base_update.faction_id, std::string, base->m_faction->m_id );
				NEW( fr.data.base_update.pops, FrontendRequest::base_pops_t, {} );
				for ( const auto& pop : base->m_pops ) {
					fr.data.base_update.pops->push_back(
						{
							pop.m_def->m_id,
							pop.m_variant
						}
					);
				}
				AddFrontendRequest( fr );
			}
			if ( bu.ops & UUO_DESPAWN ) {
				THROW( "TODO: BASE DESPAWN" );
			}
		}
		m_base_updates.clear();
	}
}

void Game::AddTileLockRequest( const bool is_lock, const size_t initiator_slot, const map::tile::positions_t& tile_positions ) {
	ASSERT_NOLOG( m_state && m_state->IsMaster(), "only master can manage tile locks" );
	m_tile_lock_requests.push_back(
		{
			is_lock,
			initiator_slot,
			tile_positions
		}
	);
}

void Game::ProcessTileLockRequests() {
	if ( m_state && m_game_state == GS_RUNNING && m_state->IsMaster() && !m_tile_lock_requests.empty() ) {
		const auto tile_lock_requests = m_tile_lock_requests;
		m_tile_lock_requests.clear();
		for ( const auto& req : tile_lock_requests ) {
			const auto state = m_state->m_slots->GetSlot( req.initiator_slot ).GetState();
			if ( state != slot::Slot::SS_PLAYER ) {
				// player disconnected?
				Log( "Skipping tile locks/unlocks for slot " + std::to_string( req.initiator_slot ) + " (invalid slot state: " + std::to_string( state ) + ")" );
				continue;
			}
			auto it = m_tile_locks.find( req.initiator_slot );
			if ( it == m_tile_locks.end() ) {
				it = m_tile_locks.insert(
					{
						req.initiator_slot,
						{}
					}
				).first;
			}
			auto& locks = it->second;
			if ( req.is_lock ) {
				// lock
				Log( "Locking tiles for " + std::to_string( req.initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( req.tile_positions ) );
				locks.push_back( TileLock{ req.tile_positions } );
				auto e = new event::LockTiles( m_slot_num, req.tile_positions, req.initiator_slot );
				e->SetDestinationSlot( req.initiator_slot );
				AddEvent( e );
			}
			else {
				// unlock
				bool found = false;
				for ( auto locks_it = locks.begin() ; locks_it != locks.end() ; locks_it++ ) {
					if ( locks_it->Matches( req.tile_positions ) ) {
						found = true;
						Log( "Unlocking tiles for " + std::to_string( req.initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( req.tile_positions ) );
						locks.erase( locks_it );
						if ( locks.empty() ) {
							m_tile_locks.erase( it );
						}
						auto e = new event::UnlockTiles( m_slot_num, req.tile_positions, req.initiator_slot );
						e->SetDestinationSlot( req.initiator_slot );
						AddEvent( e );
						break;
					}
				}
				if ( !found ) {
					Log( "Could not find matching tile locks for " + std::to_string( req.initiator_slot ) + ": " + map::tile::Tile::TilePositionsToString( req.tile_positions ) );
				}
			}
		}
	}
}

}
}
