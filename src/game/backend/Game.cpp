#include "Game.h"

#include "engine/Engine.h"
#include "types/Exception.h"
#include "types/texture/Texture.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "util/FS.h"
#include "types/Buffer.h"
#include "State.h"
#include "game/backend/faction/Faction.h"
#include "game/backend/faction/FactionManager.h"
#include "map_editor/MapEditor.h"
#include "event/FinalizeTurn.h"
#include "event/TurnFinalized.h"
#include "event/AdvanceTurn.h"
#include "Random.h"
#include "config/Config.h"
#include "slot/Slots.h"
#include "Player.h"
#include "connection/Connection.h"
#include "connection/Server.h"
#include "connection/Client.h"
#include "map/Map.h"
#include "map/Consts.h"
#include "gse/value/String.h"
#include "gse/value/Int.h"
#include "gse/value/Undefined.h"
#include "gse/value/Array.h"
#include "ui_legacy/UI.h"
#include "map/tile/TileManager.h"
#include "map/tile/Tiles.h"
#include "map/MapState.h"
#include "resource/ResourceManager.h"
#include "Bindings.h"
#include "graphics/Graphics.h"
#include "animation/Def.h"
#include "unit/Def.h"
#include "unit/UnitManager.h"
#include "unit/Unit.h"
#include "unit/MoraleSet.h"
#include "base/BaseManager.h"
#include "base/PopDef.h"
#include "base/Base.h"
#include "animation/AnimationManager.h"

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

	NEW( m_random, Random, this );

	const auto* config = g_engine->GetConfig();
	if ( config->HasLaunchFlag( config::Config::LF_QUICKSTART_SEED ) ) {
		m_random->SetState( config->GetQuickstartSeed() );
	}

	// init map editor
	NEW( m_map_editor, map_editor::MapEditor, this );

	NEW( m_tm, map::tile::TileManager, this );
	NEW( m_rm, resource::ResourceManager, this );
	NEW( m_um, unit::UnitManager, this );
	NEW( m_bm, base::BaseManager, this );
	NEW( m_am, animation::AnimationManager, this );

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

	DELETE( m_tm );
	m_tm = nullptr;

	DELETE( m_rm );
	m_rm = nullptr;

	DELETE( m_um );
	m_um = nullptr;

	DELETE( m_bm );
	m_bm = nullptr;

	DELETE( m_am );
	m_am = nullptr;

	DELETE( m_pending_frontend_requests );
	m_pending_frontend_requests = nullptr;

	DELETE( m_random );
	m_random = nullptr;

	MTModule::Stop();
}

void Game::Iterate() {
	MTModule::Iterate();

	try {
		if ( m_state ) {
			m_state->Iterate();
		}

		if ( m_game_state == GS_INITIALIZING ) {

			bool ready = true;

			ASSERT_NOLOG( m_state, "state is null" );

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
						SetLoaderText( "Saving dump" );
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

					for ( auto& tile : m_map->m_tiles->GetVector( m_init_cancel ) ) {
						UpdateYields( tile );
					}
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
						const auto factions = m_state->GetFM()->GetAll();
						auto* faction_defines = new FrontendRequest::faction_defines_t();
						for ( const auto& faction : factions ) {
							faction_defines->push_back( faction );
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

						HideLoader();

						m_um->ProcessUnprocessed();
						m_bm->ProcessUnprocessed();

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

						if ( m_state->IsMaster() ) {
							try {
								m_state->TriggerObject( this, "start", {
									{
										"game",
										Wrap( GetGCSpace() )
									},
								});
							}
							catch ( const gse::Exception& e ) {
								Log( (std::string)"Initialization failed: " + e.ToString() );
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
	m_um->PushUpdates();
	m_bm->PushUpdates();
	m_tm->ProcessTileLockRequests();
}

Random* Game::GetRandom() const {
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

void Game::ShowLoader( const std::string& text ) {
	auto fr = FrontendRequest( FrontendRequest::FR_LOADER_SHOW );
	NEW( fr.data.loader.text, std::string, text );
	AddFrontendRequest( fr );
}

void Game::SetLoaderText( const std::string& text ) {
	auto fr = FrontendRequest( FrontendRequest::FR_LOADER_TEXT );
	NEW( fr.data.loader.text, std::string, text );
	AddFrontendRequest( fr );
	ProcessRequests(); // allow frontend to update % while backend is busy
}

void Game::HideLoader() {
	auto fr = FrontendRequest( FrontendRequest::FR_LOADER_HIDE );
	AddFrontendRequest( fr );
}

WRAPIMPL_BEGIN( Game )
	WRAPIMPL_PROPS
		{
			"year",
			VALUE( gse::value::Int,, 2100/*tmp*/ + m_current_turn.GetId() )
		},
		{
			"random",
			m_random->Wrap( gc_space )
		},
		{
			"tm",
			m_tm->Wrap( gc_space )
		},
		{
			"rm",
			m_rm->Wrap( gc_space )
		},
		{
			"um",
			m_um->Wrap( gc_space )
		},
		{
			"bm",
			m_bm->Wrap( gc_space ),
		},
		{
			"am",
			m_am->Wrap( gc_space )
		},
		{
		"message",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( text, 0, String );
				Message( text );
				return VALUE( gse::value::Undefined );
			})
		},
		{
			"get_players",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				auto& slots = m_state->m_slots->GetSlots();
				gse::value::array_elements_t elements = {};
				for ( auto& slot : slots ) {
					const auto state = slot.GetState();
					if ( state == slot::Slot::SS_OPEN || state == slot::Slot::SS_CLOSED ) {
						continue; // skip
					}
					elements.push_back( slot.Wrap( gc_space ) );
				}
				return VALUE( gse::value::Array,, elements );
			} )
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Game )

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
				//Log( "Sending " + std::to_string( m_pending_frontend_requests->size() ) + " events to frontend" );
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
						case BackendRequest::BR_ANIMATION_FINISHED: {
							m_am->FinishAnimation( r.data.animation_finished.animation_id );
							break;
						}
						default:
							THROW( "unknown backend request type: " + std::to_string( r.type ) );
					}
				}
				response.result = R_SUCCESS;
			}
			catch ( const gse::Exception& e ) {
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
			event = event::Event::Unserialize( GetGCSpace(), buf );
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

void Game::OnGSEError( const gse::Exception& err ) {
	auto fr = FrontendRequest( FrontendRequest::FR_ERROR );
	NEW( fr.data.error.what, std::string, (std::string)"Script error: " + err.what() );
	NEW( fr.data.error.stacktrace, std::string, err.ToString() );
	AddFrontendRequest( fr );
}

gse::Value* const Game::AddEvent( event::Event* event ) {
	ASSERT( event->m_initiator_slot == m_slot_num, "initiator slot mismatch" );
	if ( m_connection ) {
		m_connection->SendGameEvent( event );
	}
	if ( m_state->IsMaster() ) {
		// note that this will work only on master, do slaves need return values too? i.e. for callbacks
		return ProcessEvent( event );
	}
	return VALUEEXT( gse::value::Undefined, GetGCSpace() );
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
	auto* gc_space = GetGCSpace();
	m_current_turn.AdvanceTurn( turn_id );
	m_is_turn_complete = false;
	Log( "Turn started: " + std::to_string( turn_id ) );

	{
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_ADVANCE );
		fr.data.turn_advance.turn_id = turn_id;
		AddFrontendRequest( fr );
	}

	for ( auto& it : m_um->GetUnits() ) {
		auto* unit = it.second;
		m_state->TriggerObject( m_um, "unit_turn", {
			{
				"unit",
				unit->Wrap( gc_space, true )
			},
		});
		unit->m_moved_this_turn = false;
		m_um->RefreshUnit( unit );
	}

	for ( auto& it : m_bm->GetBases() ) {
		auto* base = it.second;
		m_state->TriggerObject( m_bm, "base_turn", {
			{
				"base",
				base->Wrap( gc_space, true )
			},
		});
		m_bm->RefreshBase( base );
	}

	m_state->TriggerObject( this, "turn", {
		{
			"game",
			Wrap( gc_space )
		}
	});

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

faction::Faction* Game::GetFaction( const std::string& id ) const {
	auto* faction = m_state->GetFM()->Get( id ); // TODO: store factions in Game itself?
	ASSERT( faction, "faction not found: " + id );
	return faction;
}

map::tile::TileManager* Game::GetTM() const {
	return m_tm;
}

resource::ResourceManager* Game::GetRM() const {
	return m_rm;
}

unit::UnitManager* Game::GetUM() const {
	return m_um;
}

base::BaseManager* Game::GetBM() const {
	return m_bm;
}

animation::AnimationManager* Game::GetAM() const {
	return m_am;
}

gc::Space* const Game::GetGCSpace() const {
	ASSERT( m_state, "state not set" );
	ASSERT( m_state->m_gc_space, "state gc space not set" );
	return m_state->m_gc_space;
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

gse::Value* const Game::ProcessEvent( event::Event* event ) {
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

void Game::UpdateYields( map::tile::Tile* tile ) const {
	tile->yields = m_rm->GetYields(
		GetGCSpace(),
		tile,
		m_slot
	);
}

void Game::AddFrontendRequest( const FrontendRequest& request ) {
	//Log( "Sending frontend request (type=" + std::to_string( request.type ) + ")" ); // spammy
	m_pending_frontend_requests->push_back( request );
}

void Game::InitGame( MT_Response& response, MT_CANCELABLE ) {

	ASSERT( !m_connection || m_game_state == GS_NONE, "multiplayer game already initializing or running" );
	ASSERT( m_game_state == GS_NONE || m_game_state == GS_RUNNING, "game still initializing" );

	Log( "Initializing game" );

	auto* gc_space = GetGCSpace();

	m_state->TriggerObject( this, "configure", {
		{
			"game",
			Wrap( gc_space )
		},
	});

	ASSERT( m_pending_frontend_requests, "pending events not set" );
	m_pending_frontend_requests->clear();

	m_game_state = GS_PREPARING_MAP;
	m_initialization_error = "";

	m_connection = m_state->GetConnection();

	if ( m_state->IsMaster() ) {

		// assign random factions to players
		auto factions = m_state->GetFM()->GetAll();
		std::vector< size_t > m_available_factions = {};
		const auto& slots = m_state->m_slots->GetSlots();
		for ( const auto& slot : slots ) {
			if ( slot.GetState() == slot::Slot::SS_PLAYER ) {
				auto* player = slot.GetPlayer();
				ASSERT( player, "player not set" );
				if ( !player->GetFaction() ) {
					if ( m_available_factions.empty() ) {
						// (re)load factions list
						for ( size_t i = 0 ; i < factions.size() ; i++ ) {
						//for ( const auto& faction : factions ) {
							m_available_factions.push_back( i );
						}
						ASSERT( !m_available_factions.empty(), "no factions found" );
					}
					const auto it = m_available_factions.begin() + m_random->GetUInt( 0, m_available_factions.size() - 1 );
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
					m_tm->ReleaseTileLocks( slot_num );
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
						m_rm->Serialize( b );
						buf.WriteString( b.ToString() );
					}

					// units
					{
						types::Buffer b;
						m_um->Serialize( b );
						buf.WriteString( b.ToString() );
					}

					// bases
					{
						types::Buffer b;
						m_bm->Serialize( b );
						buf.WriteString( b.ToString() );
					}

					// animations
					{
						types::Buffer b;
						m_am->Serialize( b );
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

		const auto* config = g_engine->GetConfig();

#ifdef DEBUG
		// if crash happens - it's handy to have a seed to reproduce it
		util::FS::WriteFile( config->GetDebugPath() + map::s_consts.debug.lastseed_filename, m_random->GetStateString() );
#endif

		map::Map::error_code_t ec = map::Map::EC_UNKNOWN;

#ifdef DEBUG
		if ( !m_connection && config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_DUMP ) ) {
			const std::string& filename = config->GetQuickstartMapDump();
			ASSERT( util::FS::FileExists( filename ), "map dump file \"" + filename + "\" not found" );
			Log( (std::string)"Loading map dump from " + filename );
			SetLoaderText( "Loading dump" );
			m_map->Unserialize( types::Buffer( util::FS::ReadTextFile( filename ) ) );
			ec = map::Map::EC_NONE;
		}
		else
#endif
		{
			if ( !m_connection && config->HasLaunchFlag( config::Config::LF_QUICKSTART_MAP_FILE ) ) {
				const std::string& filename = config->GetQuickstartMapFile();
				ec = m_map->LoadFromFile( filename );
			}
			else {
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
				SetLoaderText( "Waiting for players" );
			}
			else {
				SetLoaderText( "Starting game" );
			}

			m_game_state = GS_INITIALIZING;
			response.result = R_SUCCESS;
		}
	}
	else {
		m_connection->IfClient(
			[ this, &response, ui ]( connection::Client* connection ) -> void {

				// wait for server to initialize
				SetLoaderText( "Waiting for server" );

				const auto f_download_map = [ this, ui, connection ] {

					SetLoaderText( "Downloading world snapshot" );

					connection->m_on_download_progress = [ this ]( const float progress ) -> void {
						SetLoaderText( "Downloading world snapshot:  " + std::to_string( (size_t)std::round( progress * 100 ) ) + "%" );
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
								m_rm->Unserialize( ub );
							}

							// units
							{
								auto ub = types::Buffer( buf.ReadString() );
								m_um->Unserialize( ub );
							}

							// bases
							{
								auto bb = types::Buffer( buf.ReadString() );
								m_bm->Unserialize( bb );
							}

							// animations
							{
								auto ab = types::Buffer( buf.ReadString() );
								m_am->Unserialize( ab );
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

	for ( auto& it : m_unprocessed_events ) {
		delete it;
	}
	m_unprocessed_events.clear();

	m_tm->Clear();
	m_rm->Clear();
	m_um->Clear();
	m_bm->Clear();
	m_am->Clear();

	if ( m_map ) {
		Log( "Resetting map" );
		DELETE( m_map );
		m_map = nullptr;
	}

	ASSERT( m_pending_frontend_requests, "pending events not set" );
	m_pending_frontend_requests->clear();

	m_current_turn.Reset();
	m_is_turn_complete = false;

	if ( m_state ) {
		// ui thread will reset state as needed
		m_state->UnsetGame();
		if ( m_connection ) {
			m_connection->Disconnect();
			m_connection->ResetHandlers();
			m_connection = nullptr;
		}
		delete m_state;
		m_state = nullptr;
	}
}

void Game::CheckTurnComplete() {
	if ( !m_current_turn.IsActive() ) {
		// turn not active
		return;
	}

	bool is_turn_complete = true;

	for ( const auto& unit : m_um->GetUnits() ) {
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

const bool Game::IsRunning() const {
	return m_game_state == GS_RUNNING;
}

}
}
