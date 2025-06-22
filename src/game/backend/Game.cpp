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
#include "gc/Space.h"
#include "game/backend/event/Event.h"
#include "game/backend/event/EventHandler.h"
#include "gse/value/Bool.h"

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
		TS_OFFSET + "event: " + event->ToString( /*TS_OFFSET*/ )
) {}

Game::Game()
	: gse::GCWrappable( nullptr ) {}

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

	MTModule::Log( "Starting thread" );

	m_game_state = GS_NONE;
	m_init_cancel = false;

	ASSERT( !m_pending_frontend_requests, "frontend requests already set" );
	NEW( m_pending_frontend_requests, std::vector< FrontendRequest > );

	NEW( m_random, Random, this );

	const auto* config = g_engine->GetConfig();
	if ( config->HasLaunchFlag( config::Config::LF_QUICKSTART_SEED ) ) {
		m_random->SetState( config->GetQuickstartSeed() );
	}

}

void Game::Stop() {
	MTModule::Log( "Stopping thread" );

	if ( m_state ) {
	   m_state = nullptr;
	   m_connection = nullptr;
	}

	ResetGame();

	if ( m_map_editor ) {
		DELETE( m_map_editor );
		m_map_editor = nullptr;
	}

	m_tm = nullptr;
	m_rm = nullptr;
	m_um = nullptr;
	m_bm = nullptr;
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

			ASSERT( m_state, "state is null" );

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
						MTModule::Log( "Waiting for players:" + waiting_for_players );
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
						MTModule::Log( "Restoring old map state" );
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
						MTModule::Log( (std::string)"Saving map dump to " + config->GetDebugPath() + map::s_consts.debug.lastdump_filename );
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

					m_state->WithGSE( this, [ this ]( GSE_CALLABLE ) {
						for ( auto& tile : m_map->m_tiles->GetVector( m_init_cancel ) ) {
							UpdateYields( GSE_CALL, tile );
						}
					});
					m_response_map_data->tiles = m_map->GetTilesPtr()->GetTilesPtr();

					m_response_map_data->tile_states = m_map->GetMapState()->GetTileStatesPtr();

					if ( m_old_map ) {
						MTModule::Log( "Destroying old map state" );
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
						NEWV( faction_defines, FrontendRequest::faction_defines_t );
						for ( const auto& faction : factions ) {
							faction_defines->push_back( faction );
						}
						auto fr = FrontendRequest( FrontendRequest::FR_FACTION_DEFINE );
						fr.data.faction_define.factiondefs = faction_defines;
						AddFrontendRequest( fr );
					}

					{
						const auto& slots = m_state->m_slots->GetSlots();
						NEWV( slot_defines, FrontendRequest::slot_defines_t );
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

						auto* gc_space = m_state->m_gc_space;

						m_state->WithGSE( this, [ this, f_init_failed ]( GSE_CALLABLE ) {
							m_um->ProcessUnprocessed( GSE_CALL );
							m_bm->ProcessUnprocessed( GSE_CALL );

							if ( m_state->IsMaster() ) {
								try {
									m_state->TriggerObject(
										this, "start", ARGS_F( this ) {
											{
												"game",
												Wrap( GSE_CALL )
											},
										}; }
									);
								}
								catch ( const gse::Exception& e ) {
									MTModule::Log( (std::string)"Initialization failed: " + e.ToString() );
									f_init_failed( e.what() );
									return;
								}
								GlobalAdvanceTurn( GSE_CALL );
							}
							CheckTurnComplete();

						});

					}
				}
				else {
					f_init_failed( map::Map::GetErrorString( ec ) );
				}
			}
		}
		if ( m_state ) {

			m_state->m_gc_space->Accumulate( this, [ this ]() {
				std::vector< event::Event* > events;
				{
					std::lock_guard guard( m_pending_events_mutex );
					events = m_pending_events;
					m_pending_events.clear();
				}
				if ( !events.empty() ) {
					m_state->WithGSE( this, [ this, events ]( GSE_CALLABLE ) {
						const std::string* errptr = nullptr;
						for ( const auto& event : events ) {
#if defined(DEBUG) || defined(FASTDEBUG)
							MTModule::Log( "Processing event: " + event->ToString() );
#endif
							auto* obj = VALUE( gse::value::Object, , GSE_CALL_NOGC, event->GetData() );
							const auto fargs = gse::value::function_arguments_t{ obj };
							const auto& it = m_event_handlers.find( event->GetName() );
							const auto* handler = it != m_event_handlers.end() ? it->second : nullptr;
							if ( handler ) {
								errptr = handler->Validate( GSE_CALL, fargs );
							}
							else {
								errptr = new std::string( "Unknown event: " + event->GetName() );
							}
							if ( !errptr ) {
								auto* resolved = handler->Resolve( GSE_CALL, fargs );
								if ( resolved ) {
									obj->Set( "resolved", resolved, GSE_CALL );
								}
								WithRW( [ &handler, &fargs, &gc_space, &ctx, &si, &ep, &obj, &event ] () {
									auto* applied = handler->Apply( GSE_CALL, fargs );
									if ( applied ) {
										obj->Set( "applied", applied, GSE_CALL );
									}
									if ( event->GetName() == "complete_turn" ) {
										//handler->Rollback( GSE_CALL, fargs );
									}
								});
							}
							else {
								MTModule::Log( "Event rejected: " + *errptr );
								delete ( errptr );
							}
						}
					});
				}
			});
		}
	}
	catch ( const InvalidEvent& e ) {
		MTModule::Log( (std::string)e.what() );
		Quit( "Event validation error" ); // TODO: fix reason
	}
	if ( m_um ) {
		m_um->PushUpdates();
	}
	if ( m_bm ) {
		m_bm->PushUpdates();
	}
	if ( m_tm ) {
		m_tm->ProcessTileLockRequests();
	}
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

void Game::Event( GSE_CALLABLE, const std::string& name, const gse::value::object_properties_t& args ) {
	const auto& it = m_event_handlers.find( name );
	if ( it == m_event_handlers.end() ) {
		GSE_ERROR( gse::EC.INVALID_HANDLER, "Unknown event: " + name );
	}
	const auto& handler = it->second;
	auto* event = new event::Event( this, m_slot_num, GSE_CALL, name, args );
	{
		std::lock_guard guard( m_pending_events_mutex );
		m_pending_events.push_back( event );
	}
}

WRAPIMPL_BEGIN( Game )
	WRAPIMPL_PROPS
	WRAPIMPL_TRIGGERS
		{
			"is_master",
			VALUE( gse::value::Bool,, m_slot_num == 0 )
		},
		{
			"random",
			m_random->Wrap( GSE_CALL, true )
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
			"get_player",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( id, 0, Int );
				auto& slots = m_state->m_slots->GetSlots();
				if ( id < slots.size() ) {
					auto& slot = slots.at( id );
					return slot.Wrap( GSE_CALL );
				}
				GSE_ERROR( gse::EC.GAME_ERROR, "Player id " + std::to_string( id ) + " not found" );
			}),
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
					elements.push_back( slot.Wrap( GSE_CALL ) );
				}
				return VALUE( gse::value::Array,, elements );
			} )
		},
		{
			"get_turn_id",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );

				return VALUE( gse::value::Int,, m_current_turn.GetId() );
			} )
		},
		{
			"is_turn_complete",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( slot_id, 0, Int );

				const auto& slots = m_state->m_slots->GetSlots();
				if ( slot_id >= slots.size() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Player id " + std::to_string( slot_id ) + " not found" );
				}

				const auto& slot = slots.at( slot_id );
				ASSERT( slot.GetState() != slot::Slot::SS_PLAYER || slot.GetPlayer(), "player is null" );
				return VALUE( gse::value::Bool,,
					slot.GetState() == slot::Slot::SS_PLAYER
						? slot.GetPlayer()->IsTurnCompleted()
						: true // ai has always turn completed during turns of players
				);
			} )
		},
		{
			"complete_turn",
			NATIVE_CALL( this ) {

				CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( slot_id, 0, Int );

				const auto& slots = m_state->m_slots->GetSlots();
				if ( slot_id >= slots.size() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Player id " + std::to_string( slot_id ) + " not found" );
				}

				const auto& slot = slots.at( slot_id );
				if ( slot.GetState() != slot::Slot::SS_PLAYER ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Player id " + std::to_string( slot_id ) + " is not player" );
				}
				ASSERT( slot.GetPlayer(), "player is null" );
				ASSERT( !slot.GetPlayer()->IsTurnCompleted(), "player turn already completed" );
				CompleteTurn( GSE_CALL, slot_id );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"uncomplete_turn",
			NATIVE_CALL( this ) {

				CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( slot_id, 0, Int );

				const auto& slots = m_state->m_slots->GetSlots();
				if ( slot_id >= slots.size() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Player id " + std::to_string( slot_id ) + " not found" );
				}

				const auto& slot = slots.at( slot_id );
				if ( slot.GetState() != slot::Slot::SS_PLAYER ) {
					GSE_ERROR( gse:: EC.GAME_ERROR, "Player id " + std::to_string( slot_id ) + " is not player" );
				}
				ASSERT( slot.GetPlayer(), "player is null" );
				ASSERT( slot.GetPlayer()->IsTurnCompleted(), "player turn not completed" );
				UncompleteTurn( slot_id );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"advance_turn",
			NATIVE_CALL( this ) {

				CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( turn_id, 0, Int );

				AdvanceTurn( turn_id );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"get_settings", // deprecated
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				if ( !m_state ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Game not initialized" );
				}
				return m_state->m_settings.Wrap( GSE_CALL, true );
			} )
		},
		{
			"get_setting",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( key, 0, String );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"set_setting",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( key, 0, String );
				N_GETVALUE( value, 1, String );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"register_event",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( name, 0, String );
				if ( m_event_handlers.find( name ) != m_event_handlers.end() ) {
					GSE_ERROR( gse::EC.INVALID_HANDLER, "Event already registered: " + name );
				}
				N_GETVALUE( def, 1, Object );
				auto validate_it = def.find( "validate" );
				if ( validate_it == def.end() || validate_it->second->type != gse::Value::T_CALLABLE ) {
					GSE_ERROR( gse::EC.INVALID_HANDLER, "Event handler does not provide validate method" );
				}
				gse::value::Callable* resolve = nullptr;
				auto resolve_it = def.find( "resolve" );
				if ( resolve_it != def.end() ) {
					if ( resolve_it->second->type != gse::Value::T_CALLABLE ) {
						GSE_ERROR( gse::EC.INVALID_HANDLER, "Event handler does not provide resolve method" );
					}
					resolve = (gse::value::Callable*)resolve_it->second;
				}
				auto apply_it = def.find( "apply" );
				if ( apply_it == def.end() || apply_it->second->type != gse::Value::T_CALLABLE ) {
					GSE_ERROR( gse::EC.INVALID_HANDLER, "Event handler does not provide apply method" );
				}
				auto rollback_it = def.find( "rollback" );
				if ( rollback_it == def.end() || rollback_it->second->type != gse::Value::T_CALLABLE ) {
					GSE_ERROR( gse::EC.INVALID_HANDLER, "Event handler does not provide rollback method" );
				}
				m_event_handlers.insert( { name, new event::EventHandler( gc_space, name,
					(gse::value::Callable*)validate_it->second,
					resolve,
					(gse::value::Callable*)apply_it->second,
					(gse::value::Callable*)rollback_it->second
				) } );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"event",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( name, 0, String );
				const auto& it = m_event_handlers.find( name );
				if ( it == m_event_handlers.end() ) {
					GSE_ERROR( gse::EC.INVALID_HANDLER, "Unknown event: " + name );
				}
				const auto& handler = it->second;
				N_GETVALUE( args, 1, Object );
				auto* event = new event::Event( this, m_slot_num, GSE_CALL, name, args );
				{
					std::lock_guard guard( m_pending_events_mutex );
					m_pending_events.push_back( event );
				}
				return VALUE( gse::value::Undefined );
			} )
		},
	};
	if ( m_tm ) {
		properties.insert(
			{
				"tm",
				m_tm->Wrap( GSE_CALL, true )
			}
		);
	}
	if ( m_rm ) {
		properties.insert(
			{
				"rm",
				m_rm->Wrap( GSE_CALL, true )
			}
		);
	}
	if ( m_um ) {
		properties.insert(
			{
				"um",
				m_um->Wrap( GSE_CALL, true )
			}
		);
	}
	if ( m_bm ) {
		properties.insert(

			{
				"bm",
				m_bm->Wrap( GSE_CALL, true ),
			}
		);
	}
	if ( m_am ) {
		properties.insert(
			{
				"am",
				m_am->Wrap( GSE_CALL, true )
			}
		);
	}
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Game )

void Game::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	gse::GCWrappable::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Game" );

	if ( m_tm ) {
		GC_REACHABLE( m_tm );
	}
	if ( m_rm ) {
		GC_REACHABLE( m_rm );
	}
	if ( m_um ) {
		GC_REACHABLE( m_um );
	}
	if ( m_bm ) {
		GC_REACHABLE( m_bm );
	}
	if ( m_am ) {
		GC_REACHABLE( m_am );
	}

	GC_DEBUG_BEGIN( "events" );
	{
		std::lock_guard guard( m_pending_events_mutex );
		for ( const auto& event : m_pending_events ) {
			GC_REACHABLE( event );
		}
	}
	GC_DEBUG_END();

	GC_DEBUG_BEGIN( "event_handlers" );
	for ( const auto& it : m_event_handlers ) {
		GC_REACHABLE( it.second );
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

const MT_Response Game::ProcessRequest( const MT_Request& request, MT_CANCELABLE ) {
	MT_Response response = {};
	response.op = request.op;

	switch ( request.op ) {
		case OP_INIT: {
			//MTModule::Log( "Got init request" );
			ASSERT( request.data.init.state, "state not received" );
			ASSERT( !m_state, "state already set" );
			m_state = request.data.init.state;

			m_state->WithGSE(
				this,
				[ this ]( GSE_CALLABLE ) {
					m_state->TriggerObject(
						this, "initialize", ARGS_F( this ) {
							{
								"game",
								Wrap( GSE_CALL )
							}
						}; }
					);
				}
			);
			//m_state->SetGame( this );

			InitGame( response, MT_C );
			response.data.init.slot_index = m_slot_num;
			break;
		}
		case OP_GET_MAP_DATA: {
			//MTModule::Log( "Got get-map-data request" );
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
			//MTModule::Log( "Got reset request" );
			ResetGame();
			response.result = R_SUCCESS;
			break;
		}
		case OP_PING: {
			//MTModule::Log( "Got ping request" );
			response.result = R_SUCCESS;
			break;
		}
		case OP_SAVE_MAP: {
			//MTModule::Log( "got save map request" );
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
			//MTModule::Log( "got edit map request" );

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
			MTModule::Log( "got chat message request: " + *request.data.chat.message );

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
			//MTModule::Log( "got events request" );
			if ( !m_pending_frontend_requests->empty() ) {
				//MTModule::Log( "Sending " + std::to_string( m_pending_frontend_requests->size() ) + " events to frontend" );
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
				auto* gc_space = GetGCSpace();
				for ( const auto& r : *request.data.send_backend_requests.requests ) {
					switch ( r.type ) {
						case BackendRequest::BR_ANIMATION_FINISHED: {
							gc_space->Accumulate( this, [ this, &r ] () {
								m_am->FinishAnimation( r.data.animation_finished.animation_id );
							});
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
			THROW( "TODO: OP_ADD_EVENT" );
			/*m_state->WithGSE( [ this, &event, &errmsg, &buf ]( GSE_CALLABLE ) {
				event = event::LegacyEvent::Unserialize( GSE_CALL, buf );
				errmsg = event->Validate( GSE_CALL, this );
				if ( errmsg ) {
					// log and do nothing
					MTModule::Log( "Event declined: " + *errmsg );
					delete errmsg;
					delete event;
				}
				else {
					AddEvent( GSE_CALL, event );
				}
			});*/
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

const size_t Game::GetTurnId() const {
	return m_current_turn.GetId();
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

void Game::CompleteTurn( GSE_CALLABLE, const size_t slot_num ) {
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

void Game::AdvanceTurn( const size_t turn_id ) {
	auto* gc_space = GetGCSpace();
	m_current_turn.AdvanceTurn( turn_id );
	m_is_turn_complete = false;
	MTModule::Log( "Turn started: " + std::to_string( turn_id ) );

	{
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_ADVANCE );
		fr.data.turn_advance.turn_id = turn_id;
		AddFrontendRequest( fr );
	}

	m_state->WithGSE( this, [ this ]( GSE_CALLABLE ) {
		for ( auto& it : m_um->GetUnits() ) {
			auto* unit = it.second;
			m_state->TriggerObject(
				m_um, "unit_turn", ARGS_F( &unit ) {
					{
						"unit",
						unit->Wrap( GSE_CALL, true )
					},
				}; }
			);
			unit->m_moved_this_turn = false;
			m_um->RefreshUnit( GSE_CALL, unit );
		}

		for ( auto& it : m_bm->GetBases() ) {
			auto* base = it.second;
			m_state->TriggerObject(
				m_bm, "base_turn", ARGS_F( &base ) {
					{
						"base",
						base->Wrap( GSE_CALL, true )
					},
				}; }
			);
			m_bm->RefreshBase( base );
		}

		m_state->TriggerObject( this, "turn", ARGS_F( this ) {
			{
				"game",
				Wrap( GSE_CALL )
			}
		}; } );
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

void Game::GlobalFinalizeTurn( GSE_CALLABLE ) {
	ASSERT( m_state->IsMaster(), "not master" );
	ASSERT( m_verified_turn_checksum_slots.empty(), "turn finalization slots not empty" );
	MTModule::Log( "Finalizing turn ( checksum = " + std::to_string( m_turn_checksum ) + " )" );
	THROW( "TODO: GLOBAL FINALIZE TURN");
	//AddEvent( GSE_CALL, new event::FinalizeTurn( m_slot_num ) );
}

static size_t s_turn_id = 0;
void Game::GlobalAdvanceTurn( GSE_CALLABLE ) {
	ASSERT( m_state->IsMaster(), "not master" );

	// reset some states
	s_turn_id++;
	m_verified_turn_checksum_slots.clear();
	m_turn_checksum = 0;

	MTModule::Log( "Advancing turn ( id = " + std::to_string( s_turn_id ) + " )" );
	m_state->WithGSE( this, [ this ]( GSE_CALLABLE ){
		Event( GSE_CALL, "advance_turn", {
			{ "turn_id", VALUE( gse::value::Int,, s_turn_id ) }
		} );
	});
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

void Game::UpdateYields( GSE_CALLABLE, map::tile::Tile* tile ) const {
	tile->yields = m_rm->GetYields(
		GSE_CALL,
		tile,
		m_slot
	);
}

void Game::WithRW( const std::function< void() >& f ) {
	m_rw_counter++;
	f();
	m_rw_counter--;
}

void Game::CheckRW( GSE_CALLABLE ) {
	if ( !m_rw_counter ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Game state is read-only. Try using events?");
	}
}

void Game::AddFrontendRequest( const FrontendRequest& request ) {
	//MTModule::Log( "Sending frontend request (type=" + std::to_string( request.type ) + ")" ); // spammy
	m_pending_frontend_requests->push_back( request );
}

void Game::InitGame( MT_Response& response, MT_CANCELABLE ) {

	ASSERT( !m_connection || m_game_state == GS_NONE, "multiplayer game already initializing or running" );
	ASSERT( m_game_state == GS_NONE || m_game_state == GS_RUNNING, "game still initializing" );

	MTModule::Log( "Initializing game" );

	auto* gc_space = GetGCSpace();

	// init map editor
	NEW( m_map_editor, map_editor::MapEditor, this );

	m_state->WithGSE( this, [ this ]( GSE_CALLABLE ) {
		ASSERT( !m_tm, "tm not null" );
		m_tm = new map::tile::TileManager( this );
		ASSERT( !m_rm, "rm not null" );
		m_rm = new resource::ResourceManager( this );
		ASSERT( !m_um, "um not null" );
		m_um = new unit::UnitManager( this );
		ASSERT( !m_bm, "bm not null" );
		m_bm = new base::BaseManager( this );
		ASSERT( !m_am, "am not null" );
		m_am = new animation::AnimationManager( this );
		m_state->TriggerObject( this, "configure", ARGS_F( this ) {
			{
				"game",
				Wrap( GSE_CALL )
			},
		}; } );
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

				/*connection->m_on_player_join = [ this, connection ]( const size_t slot_num, slot::Slot* slot, const Player* player ) -> void {
					MTModule::Log( "Player " + player->GetFullName() + " is connecting..." );
					connection->GlobalMessage( "Connection from " + player->GetFullName() + "..." );
					// actual join will happen after he downloads and initializes map
				};*/

				connection->m_on_flags_update = [ this, connection ]( const size_t slot_num, slot::Slot* slot, const slot::player_flag_t old_flags, const slot::player_flag_t new_flags ) -> void {
					if ( !( old_flags & slot::PF_GAME_INITIALIZED ) && ( new_flags & slot::PF_GAME_INITIALIZED ) ) {
						const auto* player = slot->GetPlayer();
						MTModule::Log( player->GetFullName() + " joined the game." );
						connection->GlobalMessage( player->GetFullName() + " joined the game." );
					}
				};

				/*connection->m_on_player_leave = [ this, connection ]( const size_t slot_num, slot::Slot* slot, const Player* player ) -> void {
					MTModule::Log( "Player " + player->GetFullName() + " left the game." );
					connection->GlobalMessage( player->GetFullName() + " left the game." );
					m_tm->ReleaseTileLocks( slot_num );
				};*/

				connection->m_on_download_request = [ this ]() -> const std::string {
					if ( !m_map ) {
						// map not generated yet
						return "";
					}
					MTModule::Log( "Preparing snapshot for download" );
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
					MTModule::Log( "Sending turn ID: " + std::to_string( s_turn_id ) );
					buf.WriteInt( s_turn_id );

					return buf.ToString();
				};

				connection->SetGameState( connection::Connection::GS_INITIALIZING );
			}
		);

		m_connection->IfClient(
			[ this ]( connection::Client* connection ) -> void {
				connection->m_on_disconnect = [ this ]() -> bool {
					MTModule::Log( "Connection lost" );
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
			THROW( "TODO: m_on_game_event_validate");
			if ( m_game_state == GS_RUNNING ) {
/*				m_state->WithGSE( this, [ this, event ]( GSE_CALLABLE ) {
					ValidateEvent( GSE_CALL, event );
				} );*/
			}
			else {
				//m_unprocessed_events.push_back( event );
			}

		};

		m_connection->m_on_game_event_apply = [ this ]( event::Event* event ) -> void {
			if ( m_state->IsMaster() ) {
				ASSERT( m_game_state == GS_RUNNING, "game is not running but received event" );
			}
			THROW( "TODO: m_on_game_event_apply");
			if ( m_game_state == GS_RUNNING ) {
				/*m_state->WithGSE( this, [ this, event ]( GSE_CALLABLE ) {
					ProcessEvent( GSE_CALL, event );
				});*/
			}
		};

		m_connection->m_on_game_event_rollback = [ this ]( event::Event* event ) -> void {
			if ( m_state->IsMaster() ) {
				ASSERT( m_game_state == GS_RUNNING, "game is not running but received event" );
			}
			THROW( "TODO: m_on_game_event_rollback");
			if ( m_game_state == GS_RUNNING ) {
				/*m_state->WithGSE( this, [ this, event ]( GSE_CALLABLE ) {
					ProcessEvent( GSE_CALL, event );
				});*/
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

		MTModule::Log( "Game seed: " + m_random->GetStateString() );

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
			MTModule::Log( (std::string)"Loading map dump from " + filename );
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
						MTModule::Log( "Unpacking world snapshot" );
						auto buf = types::Buffer( serialized_snapshot );

						// map
						auto b = types::Buffer( buf.ReadString() );
						NEW( m_map, map::Map, this );
						const auto ec = m_map->LoadFromBuffer( b );
						if ( ec == map::Map::EC_NONE ) {

							m_state->WithGSE( this, [ this, &buf ]( GSE_CALLABLE ) {

								// resources
								{
									auto ub = types::Buffer( buf.ReadString() );
									m_rm->Unserialize( ub );
								}

								// units
								{
									auto ub = types::Buffer( buf.ReadString() );
									m_um->Unserialize( GSE_CALL, ub );
								}

								// bases
								{
									auto bb = types::Buffer( buf.ReadString() );
									m_bm->Unserialize( GSE_CALL, bb );
								}

								// animations
								{
									auto ab = types::Buffer( buf.ReadString() );
									m_am->Unserialize( ab );
								}

								// get turn info
								const auto turn_id = buf.ReadInt();
								if ( turn_id > 0 ) {
									MTModule::Log( "Received turn ID: " + std::to_string( turn_id ) );
									AdvanceTurn( turn_id );
								}
								
								m_game_state = GS_INITIALIZING;
								
							});

						}
						else {
							MTModule::Log( "WARNING: failed to unpack world snapshot (code=" + std::to_string( ec ) + ")" );
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

	{
		std::lock_guard guard( m_pending_events_mutex );
		m_pending_events.clear();
	}
	m_event_handlers.clear();
	m_next_event_id = 0;

	m_tm = nullptr;
	m_rm = nullptr;
	m_um = nullptr;
	m_bm = nullptr;
	m_am = nullptr;

	if ( m_map ) {
		MTModule::Log( "Resetting map" );
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
		m_state = nullptr;
	}
}

void Game::CheckTurnComplete() {

	bool is_turn_complete = true;

	for ( const auto& unit : m_um->GetUnits() ) {
		if ( unit.second->m_owner == m_slot && unit.second->HasMovesLeft() ) {
			is_turn_complete = false;
			break;
		}
	}

	if ( m_is_turn_complete != is_turn_complete ) {
		m_is_turn_complete = is_turn_complete;
		MTModule::Log( "Sending turn complete status: " + std::to_string( m_is_turn_complete ) );
		auto fr = FrontendRequest( FrontendRequest::FR_TURN_STATUS );
		fr.data.turn_status.status = m_is_turn_complete
			? turn::TS_TURN_COMPLETE
			: turn::TS_TURN_ACTIVE;
		AddFrontendRequest( fr );
	}

}

Game::Interface::Interface( gc::Space* const gc_space )
	: gse::GCWrappable( gc_space ) {}

const bool Game::IsRunning() const {
	return m_game_state == GS_RUNNING;
}

const std::string Game::GenerateEventId() {
	return std::to_string( m_slot_num ) + "_" + std::to_string( ++m_next_event_id );
}

void Game::SetState( State* const state ) {
	ASSERT( state, "state is null" );
	if ( state != m_state ) {
		ASSERT( !m_state, "game state already set" );
		m_state = state;
	}
}

}
}
