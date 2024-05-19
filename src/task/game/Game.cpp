#include "Game.h"

#include <algorithm>
#include <map>

#include "engine/Engine.h"
#include "util/random/Random.h"
#include "config/Config.h"
#include "scheduler/Scheduler.h"
#include "../../ui/UI.h" // TODO: fix path
#include "game/Game.h"
#include "game/unit/Def.h"
#include "game/animation/Def.h"
#include "game/connection/Connection.h"
#include "task/mainmenu/MainMenu.h"
#include "graphics/Graphics.h"
#include "util/FS.h"
#include "ui/popup/PleaseDontGo.h"
#include "game/State.h"
#include "Types.h"
#include "game/event/MoveUnit.h"
#include "game/event/AttackUnit.h"
#include "game/event/SkipUnitTurn.h"
#include "game/event/CompleteTurn.h"
#include "game/event/UncompleteTurn.h"
#include "ui/bottom_bar/BottomBar.h"
#include "InstancedSpriteManager.h"
#include "Animation.h"
#include "AnimationDef.h"
#include "Unit.h"
#include "UnitDef.h"
#include "Slot.h"
#include "BadgeDefs.h"
#include "scene/actor/Actor.h"
#include "scene/actor/Instanced.h"
#include "InstancedSprite.h"
#include "loader/texture/TextureLoader.h"
#include "actor/Actor.h"
#include "actor/TileSelection.h"
#include "scene/Camera.h"
#include "scene/Light.h"
#include "scene/Scene.h"
#include "types/texture/Texture.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "ui/style/Theme.h"

#define INITIAL_CAMERA_ANGLE { -M_PI * 0.5, M_PI * 0.75, 0 }

namespace task {
namespace game {

const Game::consts_t Game::s_consts = {};

Game::Game( ::game::State* state, ::ui::ui_handler_t on_start, ::ui::ui_handler_t on_cancel )
	: m_state( state )
	, m_on_start( on_start )
	, m_on_cancel( on_cancel )
	, m_is_map_editing_allowed( state->GetConnection() == nullptr ) { // singleplayer only

}

Game::~Game() {
	if ( m_is_initialized ) {
		Log( "WARNING: game task destroyed while still running" );
	}
}

void Game::Start() {

	// note: game thread has it's own random, this one is mostly for UI and small things
	NEW( m_random, util::random::Random );

	auto* ui = g_engine->GetUI();
	auto* game = g_engine->GetGame();
	auto* config = g_engine->GetConfig();

	if ( m_state->IsMaster() ) {
#ifdef DEBUG
		if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_FILE ) ) {
			m_state->m_settings.global.map.type = ::game::settings::MapSettings::MT_MAPFILE;
			m_state->m_settings.global.map.filename = config->GetQuickstartMapFile();
		}
#endif

		if ( m_state->m_settings.global.map.type == ::game::settings::MapSettings::MT_MAPFILE ) {
			m_map_data.filename = util::FS::GetBaseName( m_state->m_settings.global.map.filename );
			m_map_data.last_directory = util::FS::GetDirName( m_state->m_settings.global.map.filename );
		}

	}

	ui->ShowLoader(
		"Starting game", LCH( this ) {
			CancelGame();
			return false;
		}
	);
	m_mt_ids.init = game->MT_Init( m_state );
}

void Game::Stop() {

	if ( m_is_initialized ) {
		Deinitialize();
	}

	if ( m_random ) {
		DELETE( m_random );
		m_random = nullptr;
	}

	if ( m_state ) {
		// main menu has state copied
		DELETE( m_state );
		m_state = nullptr;
	}
}

void Game::Iterate() {

	auto* game = g_engine->GetGame();
	auto* ui = g_engine->GetUI();
	auto* config = g_engine->GetConfig();

	const auto f_handle_nonsuccess_init = [ this, ui ]( const ::game::MT_Response& response ) -> void {
		switch ( response.result ) {
			case ::game::R_ABORTED: {
				CancelGame();
				break;
			}
			case ::game::R_ERROR: {
				const std::string error_text = *response.data.error.error_text;
				ui->ShowError(
					error_text, UH( this ) {
						CancelGame();
					}
				);
				break;
			}
			default: {
				THROW( "unknown response result " + std::to_string( response.result ) );
			}
		}
	};

	if ( m_mt_ids.init ) {
		auto response = game->MT_GetResponse( m_mt_ids.init );
		if ( response.result != ::game::R_NONE ) {
			m_mt_ids.init = 0;
			if ( response.result == ::game::R_SUCCESS ) {
				// game thread will manage state from now on
				m_state = nullptr;
				// set own slot
				m_slot_index = response.data.init.slot_index;
				// get map data to display it
				m_mt_ids.get_map_data = game->MT_GetMapData();
			}
			else {
				f_handle_nonsuccess_init( response );
			}
		}
		game->MT_DestroyResponse( response );
	}
	if ( m_mt_ids.get_map_data ) {
		auto response = game->MT_GetResponse( m_mt_ids.get_map_data );
		if ( response.result != ::game::R_NONE ) {
			if ( response.result == ::game::R_PENDING ) {
				// still initializing, try later
				m_mt_ids.get_map_data = game->MT_GetMapData();
			}
			else {
				ui->HideLoader();
				m_mt_ids.get_map_data = 0;

				if ( response.result == ::game::R_SUCCESS ) {
					// at this point starting continues in this thread so is not cancelable anymore
					if ( m_on_start ) {
						m_on_start();
						m_on_start = nullptr;
					}

					UpdateMapData(
						{
							response.data.get_map_data->map_width,
							response.data.get_map_data->map_height
						}
					);

					if ( m_is_initialized ) {
						Deinitialize();
					}

					Initialize(
						response.data.get_map_data->terrain_texture,
						response.data.get_map_data->terrain_mesh,
						response.data.get_map_data->terrain_data_mesh,
						*response.data.get_map_data->sprites.actors,
						*response.data.get_map_data->sprites.instances,
						response.data.get_map_data->tiles,
						response.data.get_map_data->tile_states
					);
					response.data.get_map_data->terrain_texture = nullptr;
					response.data.get_map_data->terrain_mesh = nullptr;
					response.data.get_map_data->terrain_data_mesh = nullptr;

					UpdateCameraRange();
					UpdateMapInstances();
					UpdateMinimap();
				}
				else {
					f_handle_nonsuccess_init( response );
				}
			}

			game->MT_DestroyResponse( response );
		}
	}
	if ( m_mt_ids.reset ) {
		auto response = game->MT_GetResponse( m_mt_ids.reset );
		if ( response.result != ::game::R_NONE ) {
			ui->HideLoader();
			m_mt_ids.reset = 0;

			switch ( response.result ) {
				case ::game::R_SUCCESS: {
					ASSERT( m_on_game_exit, "game exit handler not set" );
					const auto on_game_exit = m_on_game_exit;
					m_on_game_exit = nullptr;
					on_game_exit();
					break;
				}
				default: {
					THROW( "unknown response result " + std::to_string( response.result ) );
				}
			}
		}
	}
	if ( m_mt_ids.ping ) {
		auto response = game->MT_GetResponse( m_mt_ids.ping );
		if ( response.result != ::game::R_NONE ) {
			ui->HideLoader();
			m_mt_ids.ping = 0;
			ASSERT( response.result == ::game::R_SUCCESS, "ping not successful" );
			CancelGame();
		}
	}
	if ( m_is_map_editing_allowed ) {
		if ( m_mt_ids.save_map ) {
			auto response = game->MT_GetResponse( m_mt_ids.save_map );
			if ( response.result != ::game::R_NONE ) {
				ui->HideLoader();
				m_mt_ids.save_map = 0;
				if ( ui->HasPopup() ) {
					ui->CloseLastPopup();
				}
				if ( response.result == ::game::R_SUCCESS ) {
					m_map_data.last_directory = util::FS::GetDirName( *response.data.save_map.path );
					m_map_data.filename = util::FS::GetBaseName( *response.data.save_map.path );
					m_ui.bottom_bar->UpdateMapFileName();
				}
				else {
					ui->ShowError(
						"Map saving failed.", UH( this ) {

						}
					);
				}
				game->MT_DestroyResponse( response );
			}
		}
		if ( m_mt_ids.edit_map ) {
			auto response = game->MT_GetResponse( m_mt_ids.edit_map );
			if ( response.result != ::game::R_NONE ) {
				m_mt_ids.edit_map = 0;

				ASSERT( response.result == ::game::R_SUCCESS, "edit map unsuccessful" );

				// add missing things, remove unneeded things
				if ( response.data.edit_map.sprites.actors_to_add ) {
					Log( "Need to add " + std::to_string( response.data.edit_map.sprites.actors_to_add->size() ) + " actors" );
					for ( auto& a : *response.data.edit_map.sprites.actors_to_add ) {
						GetTerrainInstancedSprite( a.second );
					}
				}

				if ( response.data.edit_map.sprites.instances_to_remove ) {
					Log( "Need to remove " + std::to_string( response.data.edit_map.sprites.instances_to_remove->size() ) + " instances" );
					for ( auto& i : *response.data.edit_map.sprites.instances_to_remove ) {
						auto* actor = m_ism->GetInstancedSpriteByKey( i.second )->actor;
						ASSERT( actor, "sprite actor not found" );
						ASSERT( actor->HasInstance( i.first ), "actor instance not found" );
						actor->RemoveInstance( i.first );
					}
				}

				if ( response.data.edit_map.sprites.instances_to_add ) {
					Log( "Need to add " + std::to_string( response.data.edit_map.sprites.instances_to_add->size() ) + " instances" );
					for ( auto& i : *response.data.edit_map.sprites.instances_to_add ) {
						const auto& instance = i.second;
						auto* actor = m_ism->GetInstancedSpriteByKey( instance.first )->actor;
						ASSERT( actor, "sprite actor not found" );
						ASSERT( !actor->HasInstance( i.first ), "actor instance already exists" );
						actor->SetInstance( i.first, instance.second );
					}
				}
			}
		}
	}
	if ( m_mt_ids.chat ) {
		auto response = game->MT_GetResponse( m_mt_ids.chat );
		if ( response.result != ::game::R_NONE ) {
			ASSERT( response.result == ::game::R_SUCCESS, "failed to send chat message to game thread" );
			m_mt_ids.chat = 0;
		}
	}

	auto it = m_animations.begin();
	while ( it != m_animations.end() ) {
		it->second->Iterate();
		if ( it->second->IsFinished() ) {
			auto br = ::game::BackendRequest( ::game::BackendRequest::BR_ANIMATION_FINISHED );
			br.data.animation_finished.animation_id = it->first;
			SendBackendRequest( &br );
			delete it->second;
			it = m_animations.erase( it );
		}
		else {
			it++;
		}
	}

	// check if previous backend requests were sent successfully
	if ( m_mt_ids.send_backend_requests ) {
		auto response = game->MT_GetResponse( m_mt_ids.send_backend_requests );
		if ( response.result != ::game::R_NONE ) {
			ASSERT( response.result == ::game::R_SUCCESS, "backend requests result not successful" );
			m_mt_ids.send_backend_requests = 0;
		}
	}

	// send pending backend requests if present and not sending already
	if ( !m_mt_ids.send_backend_requests && !m_pending_backend_requests.empty() ) {
		m_mt_ids.send_backend_requests = game->MT_SendBackendRequests( m_pending_backend_requests );
		m_pending_backend_requests.clear();
	}

	// poll backend for frontend requests
	if ( m_mt_ids.get_frontend_requests ) {
		auto response = game->MT_GetResponse( m_mt_ids.get_frontend_requests );
		if ( response.result != ::game::R_NONE ) {
			ASSERT( response.result == ::game::R_SUCCESS, "unexpected frontend requests response" );
			m_mt_ids.get_frontend_requests = 0;
			const auto* requests = response.data.get_frontend_requests.requests;
			if ( requests ) {
				Log( "got " + std::to_string( requests->size() ) + " frontend requests" );

				for ( const auto& request : *requests ) {
					ProcessRequest( &request );
					if ( m_on_game_exit ) {
						break; // exiting game
					}
				}
			}
			game->MT_DestroyResponse( response );
			if ( !m_on_game_exit ) {
				m_mt_ids.get_frontend_requests = game->MT_GetFrontendRequests();
			}
		}
	}
	else {
		m_mt_ids.get_frontend_requests = game->MT_GetFrontendRequests();
	}

	if ( m_is_initialized ) {

		if ( m_is_map_editing_allowed && m_editing_draw_timer.HasTicked() ) {
			if ( m_is_editing_mode && !IsTileAtRequestPending() ) {
				SelectTileAtPoint( ::game::TQP_TILE_SELECT, m_map_control.last_mouse_position.x, m_map_control.last_mouse_position.y ); // async
			}
		}

		// response for clicked tile (if click happened)
		const auto tile_at = GetTileAtScreenCoordsResult();
		if ( tile_at.is_set ) {
			ASSERT( m_tile_at_query_purpose != ::game::TQP_NONE, "tile preferred mode not set" );
			auto* tile = GetTile( tile_at.tile_pos );
			if ( m_is_map_editing_allowed && m_is_editing_mode ) {
				if ( !m_mt_ids.edit_map ) { // TODO: need to queue?
					m_mt_ids.edit_map = game->MT_EditMap( tile->GetCoords(), m_editor_tool, m_editor_brush, m_editor_draw_mode );
				}
				ASSERT( m_tile_at_query_purpose == ::game::TQP_TILE_SELECT, "only tile selections allowed in map editor" );
				SelectTileOrUnit( tile );
			}
			else {
				size_t selected_unit_id = 0;
				bool need_scroll = true;
				SelectTileOrUnit( tile, selected_unit_id );
				if ( need_scroll ) {
					ScrollToTile( tile, true );
				}
			}
		}

		auto minimap_texture = GetMinimapTextureResult();
		if ( minimap_texture ) {
			m_ui.bottom_bar->SetMinimapTexture( minimap_texture );
		}

		bool is_camera_position_updated = false;
		bool is_camera_scale_updated = false;
		while ( m_map_control.edge_scrolling.timer.HasTicked() ) {
			m_camera_position.x += m_map_control.edge_scrolling.speed.x;
			m_camera_position.y += m_map_control.edge_scrolling.speed.y;
			is_camera_position_updated = true;
		}

		while ( m_scroller.HasTicked() ) {
			const auto& new_position = m_scroller.GetPosition();
			is_camera_position_updated = true;
			if ( new_position.z != m_camera_position.z ) {
				is_camera_scale_updated = true;
			}

			m_camera_position = new_position;
		}
		if ( !m_scroller.IsRunning() ) {
			if ( m_map_control.key_zooming != 0 ) {
				SmoothScroll( m_map_control.key_zooming );
			}
		}

		if ( is_camera_scale_updated ) {
			UpdateCameraScale();
			UpdateCameraRange();
		}
		if ( is_camera_position_updated ) {
			UpdateCameraPosition();
		}

		for ( auto& actor : m_actors_map ) {
			actor.first->Iterate();
		}

		if ( m_selected_unit ) {
			m_selected_unit->Iterate();
		}
	}
}

const bool Game::IsMapEditingAllowed() const {
	return m_is_map_editing_allowed;
}

const size_t Game::GetMapWidth() const {
	return m_map_data.width;
}

const size_t Game::GetMapHeight() const {
	return m_map_data.height;
}

const std::string& Game::GetMapFilename() const {
	return m_map_data.filename;
}

const std::string& Game::GetMapLastDirectory() const {
	return m_map_data.last_directory;
}

InstancedSpriteManager* Game::GetISM() const {
	ASSERT( m_ism, "ism not set" );
	return m_ism;
}

types::texture::Texture* Game::GetSourceTexture( const std::string& name ) {
	const auto it = m_textures.source.find( name );
	if ( it != m_textures.source.end() ) {
		return it->second;
	}
	auto* texture = g_engine->GetTextureLoader()->LoadTexture( name );
	ASSERT( texture, "texture not loaded" );
	m_textures.source.insert(
		{
			name,
			texture
		}
	);
	return texture;
}

InstancedSprite* Game::GetTerrainInstancedSprite( const ::game::map::sprite_actor_t& actor ) {
	return m_ism->GetInstancedSprite(
		"Terrain_" + actor.name,
		GetSourceTexture( "ter1.pcx" ),
		actor.tex_coords,
		::game::map::s_consts.tc.ter1_pcx.dimensions,
		{
			actor.tex_coords.x + ::game::map::s_consts.tc.ter1_pcx.center.x,
			actor.tex_coords.y + ::game::map::s_consts.tc.ter1_pcx.center.y
		},
		{
			::game::map::s_consts.tile.scale.x,
			::game::map::s_consts.tile.scale.y * ::game::map::s_consts.sprite.y_scale
		},
		actor.z_index
	);
}

void Game::CenterAtCoordinatePercents( const types::Vec2< float > position_percents ) {
	const types::Vec2< float > position = {
		m_map_data.range.percent_to_absolute.x.Clamp( position_percents.x ),
		m_map_data.range.percent_to_absolute.y.Clamp( position_percents.y )
	};
	//Log( "Scrolling to percents " + position_percents.ToString() );
	m_camera_position = {
		GetFixedX( -position.x * m_camera_position.z * m_viewport.window_aspect_ratio ),
		-position.y * m_camera_position.z * m_viewport.ratio.y * 0.707f,
		m_camera_position.z
	};
	UpdateCameraPosition();
}

void Game::SetCameraPosition( const types::Vec3 camera_position ) {
	if ( camera_position != m_camera_position ) {
		bool position_updated =
			( m_camera_position.x != camera_position.x ) ||
				( m_camera_position.y != camera_position.y );
		bool scale_updated = m_camera_position.z != camera_position.z;
		m_camera_position = camera_position;
		if ( position_updated ) {
			UpdateCameraPosition();
		}
		if ( scale_updated ) {
			UpdateCameraScale();
		}
	}
}

const float Game::GetFixedX( float x ) const {
	if ( x < m_camera_range.min.x ) {
		x += m_camera_range.max.x - m_camera_range.min.x;
	}
	else if ( x > m_camera_range.max.x ) {
		x -= m_camera_range.max.x - m_camera_range.min.x;
	}
	return x;
}

void Game::FixCameraX() {
	m_camera_position.x = GetFixedX( m_camera_position.x );
}

void Game::UpdateViewport() {
	auto* graphics = g_engine->GetGraphics();
	m_viewport.window_width = graphics->GetViewportWidth();
	m_viewport.window_height = graphics->GetViewportHeight();
	m_viewport.window_aspect_ratio = graphics->GetAspectRatio();
	m_viewport.max.x = std::max< ssize_t >( m_viewport.min.x, m_viewport.window_width );
	m_viewport.max.y = std::max< ssize_t >( m_viewport.min.y, m_viewport.window_height - m_ui.bottom_bar->GetHeight() + m_viewport.bottom_bar_overlap );
	m_viewport.ratio.x = (float)m_viewport.window_width / m_viewport.max.x;
	m_viewport.ratio.y = (float)m_viewport.window_height / m_viewport.max.y;
	m_viewport.width = m_viewport.max.x - m_viewport.min.x;
	m_viewport.height = m_viewport.max.y - m_viewport.min.y;
	m_viewport.aspect_ratio = (float)m_viewport.width / m_viewport.height;
	m_viewport.is_fullscreen = graphics->IsFullscreen();
	m_clamp.x.SetSrcRange(
		{
			(float)m_viewport.min.x,
			(float)m_viewport.max.x
		}
	);
	m_clamp.y.SetSrcRange(
		{
			(float)m_viewport.min.y,
			(float)m_viewport.max.y
		}
	);
}

void Game::UpdateCameraPosition() {

	// prevent vertical scrolling outside viewport
	if ( m_camera_position.y < m_camera_range.min.y ) {
		m_camera_position.y = m_camera_range.min.y;
	}
	if ( m_camera_position.y > m_camera_range.max.y ) {
		m_camera_position.y = m_camera_range.max.y;
		if ( m_camera_position.y < m_camera_range.min.y ) {
			m_camera_position.y = ( m_camera_range.min.y + m_camera_range.max.y ) / 2;
		}
	}

	if ( !m_scroller.IsRunning() ) {
		FixCameraX();
	}

	m_camera->SetPosition(
		{
			( 0.5f + m_camera_position.x ) / m_viewport.window_aspect_ratio,
			( 0.5f + m_camera_position.y ) / m_viewport.ratio.y + ::game::map::s_consts.tile.scale.z * m_camera_position.z / 1.414f, // TODO: why 1.414?
			( 0.5f + m_camera_position.y ) / m_viewport.ratio.y + m_camera_position.z
		}
	);

	if ( m_ui.bottom_bar ) {

		m_ui.bottom_bar->SetMinimapSelection(
			{
				1.0f - m_map_data.range.percent_to_absolute.x.Unclamp( m_camera_position.x / m_camera_position.z / m_viewport.window_aspect_ratio ),
				1.0f - m_map_data.range.percent_to_absolute.y.Unclamp( m_camera_position.y / m_camera_position.z / m_viewport.ratio.y / 0.707f )
			}, {
				2.0f / ( (float)( m_map_data.width ) * m_camera_position.z * m_viewport.window_aspect_ratio ),
				2.0f / ( (float)( m_map_data.height ) * m_camera_position.z * m_viewport.ratio.y * 0.707f ),
			}
		);
	}
}

void Game::UpdateCameraScale() {
	m_camera->SetScale(
		{
			m_camera_position.z,
			m_camera_position.z,
			m_camera_position.z
		}
	);
	//UpdateUICamera();
}

void Game::UpdateCameraAngle() {
	m_camera->SetAngle( m_camera_angle );
	//UpdateUICamera();
}

void Game::UpdateCameraRange() {
	m_camera_range.min.z = 2.82f / ( m_map_data.height + 1 ) / m_viewport.ratio.y; // TODO: why 2.82?
	m_camera_range.max.z = 0.22f; // TODO: fix camera z and allow to zoom in more
	if ( m_camera_position.z < m_camera_range.min.z ) {
		m_camera_position.z = m_camera_range.min.z;
	}
	if ( m_camera_position.z > m_camera_range.max.z ) {
		m_camera_position.z = m_camera_range.max.z;
	}
	m_camera_range.max.y = ( m_camera_position.z - m_camera_range.min.z ) * ( m_map_data.height + 1 ) * m_viewport.ratio.y * 0.1768f; // TODO: why 0.1768?
	m_camera_range.min.y = -m_camera_range.max.y;

	//Log( "Camera range change: Z=[" + to_string( m_camera_range.min.z ) + "," + to_string( m_camera_range.max.z ) + "] Y=[" + to_string( m_camera_range.min.y ) + "," + to_string( m_camera_range.max.y ) + "], z=" + to_string( m_camera_position.z ) );

	m_camera_range.max.x = ( m_map_data.width ) * m_camera_position.z * m_viewport.window_aspect_ratio * 0.25f;
	m_camera_range.min.x = -m_camera_range.max.x;

	UpdateCameraPosition();
	UpdateCameraScale();
}

void Game::UpdateMapInstances() {
	// needed for horizontal scrolling
	std::vector< types::Vec3 > instances;

	const float mhw = ::game::map::s_consts.tile.scale.x * m_map_data.width / 2;

	uint8_t instances_before_after = floor(
		m_viewport.aspect_ratio
			/
				(
					(float)m_map_data.width
						/
							m_map_data.height
				)
			/
				2
	) + 1;

	for ( uint8_t i = instances_before_after ; i > 0 ; i-- ) {
		instances.push_back(
			{
				-mhw * i,
				0.0f,
				0.0f
			}
		);
	}
	instances.push_back(
		{
			0.0f,
			0.0f,
			0.0f
		}
	);
	for ( uint8_t i = 1 ; i <= instances_before_after ; i++ ) {
		instances.push_back(
			{
				+mhw * i,
				0.0f,
				0.0f
			}
		);
	}

	m_world_scene->SetWorldInstancePositions( instances );
}

void Game::UpdateUICamera() {
	// TODO: finish it
	// snapshot camera matrix for world ui
	/*m_camera->GetMatrix()*/
	// tmp/hack
	/*for ( auto& a : m_map->GetActors() ) {
		for ( auto& m : ((scene::actor::Instanced*)a)->GetGameMatrices() ) {
			g_engine->GetUI()->SetGameUIMatrix( m );
			break;
		}
		break;
	}*/
}

const size_t Game::GetBottomBarMiddleHeight() const {
	ASSERT( m_ui.bottom_bar, "bottom bar not initialized" );
	return m_ui.bottom_bar->GetMiddleHeight();
}

const size_t Game::GetViewportHeight() const {
	return m_viewport.height;
}

void Game::LoadMap( const std::string& path ) {
	ASSERT( util::FS::FileExists( path ), "map file \"" + path + "\" not found" );

	auto* game = g_engine->GetGame();
	g_engine->GetUI()->ShowLoader(
		"Loading map", LCH( this ) {
			CancelRequests();
			return false;
		}
	);
	ASSERT( m_state, "state not set" );
	m_state->m_settings.global.map.type = ::game::settings::MapSettings::MT_MAPFILE;
	m_state->m_settings.global.map.filename = path;
	m_map_data.filename = util::FS::GetBaseName( path );
	m_map_data.last_directory = util::FS::GetDirName( path );
	if ( m_mt_ids.init ) {
		game->MT_Cancel( m_mt_ids.init );
	}
	if ( m_mt_ids.get_map_data ) {
		game->MT_Cancel( m_mt_ids.get_map_data );
		m_mt_ids.get_map_data = 0;
	}
	m_mt_ids.init = game->MT_Init( m_state );
}

void Game::SaveMap( const std::string& path ) {

	auto* game = g_engine->GetGame();
	g_engine->GetUI()->ShowLoader( "Saving game" );
	if ( m_mt_ids.save_map ) {
		game->MT_Cancel( m_mt_ids.save_map );

	}
	m_mt_ids.save_map = game->MT_SaveMap( path );
}

void Game::ConfirmExit( ::ui::ui_handler_t on_confirm ) {
#ifdef DEBUG
	if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
		on_confirm();
		return;
	}
#endif
	NEWV( popup, ui::popup::PleaseDontGo, this, on_confirm );
	m_map_control.edge_scrolling.timer.Stop();
	popup->Open();
}

types::texture::Texture* Game::GetTerrainTexture() const {
	return m_textures.terrain;
}

void Game::SetEditorTool( ::game::map_editor::tool_type_t editor_tool ) {
	ASSERT( m_is_map_editing_allowed, "map editing not allowed" );
	if ( m_editor_tool != editor_tool ) {
		m_editor_tool = editor_tool;
	}
}

const ::game::map_editor::tool_type_t Game::GetEditorTool() const {
	ASSERT( m_is_map_editing_allowed, "map editing not allowed" );
	return m_editor_tool;
}

void Game::SetEditorBrush( ::game::map_editor::brush_type_t editor_brush ) {
	ASSERT( m_is_map_editing_allowed, "map editing not allowed" );
	if ( m_editor_brush != editor_brush ) {
		m_editor_brush = editor_brush;
	}
}

const ::game::map_editor::brush_type_t Game::GetEditorBrush() const {
	ASSERT( m_is_map_editing_allowed, "map editing not allowed" );
	return m_editor_brush;
}

void Game::DefineSlot(
	const size_t slot_index,
	const types::Color& color,
	const bool is_progenitor
) {
	if ( m_slots.find( slot_index ) == m_slots.end() ) {
		Log( "Initializing slot: " + std::to_string( slot_index ) );
		m_slots.insert(
			{
				slot_index,
				new Slot(
					m_badge_defs,
					m_ism,
					slot_index,
					color,
					is_progenitor
						? Slot::SF_PROGENITOR
						: Slot::SF_NONE
				)
			}
		);
	}
}

void Game::ShowAnimation( AnimationDef* def, const size_t animation_id, const types::Vec3& render_coords ) {
	ASSERT( m_animations.find( animation_id ) == m_animations.end(), "animation id already exists" );
	m_animations.insert(
		{
			animation_id,
			new Animation( animation_id, def, render_coords ),
		}
	);
}

void Game::DefineUnit( const ::game::unit::Def* def ) {
	auto unitdef_it = m_unitdefs.find( def->m_id );
	ASSERT( unitdef_it == m_unitdefs.end(), "unit def already exists" );

	Log( "Initializing unit definition: " + def->m_id );

	m_unitdefs.insert(
		{
			def->m_id,
			new UnitDef(
				m_ism,
				def
			)
		}
	);
}

void Game::DefineAnimation( const ::game::animation::Def* def ) {
	auto animation_it = m_animationdefs.find( def->m_id );
	ASSERT( animation_it == m_animationdefs.end(), "animation def already exists" );

	Log( "Initializing animation definition: " + def->m_id );

	m_animationdefs.insert(
		{
			def->m_id,
			new AnimationDef(
				m_ism,
				def
			)
		}
	);
}

void Game::SpawnUnit(
	const size_t unit_id,
	const std::string& unitdef_id,
	const size_t slot_index,
	const types::Vec2< size_t >& tile_coords,
	const types::Vec3& render_coords,
	const ::game::unit::movement_t movement,
	const ::game::unit::morale_t morale,
	const std::string& morale_string,
	const ::game::unit::health_t health
) {

	ASSERT( m_unitdefs.find( unitdef_id ) != m_unitdefs.end(), "unitdef not found" );
	ASSERT( m_slots.find( slot_index ) != m_slots.end(), "slot not found" );
	ASSERT( m_units.find( unit_id ) == m_units.end(), "unit id already exists" );

	auto* unitdef = m_unitdefs.at( unitdef_id );
	auto* slot_state = m_slots.at( slot_index );
	auto* tile = GetTile( tile_coords );

	auto* unit = m_units.insert(
		{
			unit_id,
			new Unit(
				m_badge_defs,
				unit_id,
				unitdef,
				slot_state,
				tile,
				{
					render_coords.x,
					render_coords.y,
					render_coords.z
				},
				slot_index == m_slot_index,
				movement,
				morale,
				morale_string,
				health
			)
		}
	).first->second;

	types::mesh::Rectangle* mesh = nullptr;
	types::texture::Texture* texture = nullptr;

	RenderTile( tile );

	if ( unit->IsActive() ) {
		const bool was_selectables_empty = m_selectables.units.empty();
		AddSelectable( unit );
		if ( was_selectables_empty ) {
			SelectNextUnitMaybe();
		}
	}
}

void Game::DespawnUnit( const size_t unit_id ) {
	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );

	auto* unit = it->second;

	const bool is_owned = unit->IsOwned();
	const bool need_tile_refresh = m_selected_tile == unit->GetTile();

	m_units.erase( it );
	delete unit;

	if ( m_selected_unit == unit ) {
		m_selected_unit = nullptr;
	}

	if ( need_tile_refresh ) {
		m_ui.bottom_bar->PreviewTile( m_selected_tile, 0 );
	}

	if ( is_owned ) {
		RemoveSelectable( unit );
	}
}

void Game::RefreshUnit( Unit* unit ) {
	const auto was_active = unit->IsActive();
	unit->Refresh();
	UpdateSelectable( unit );
	if ( m_selected_unit == unit && was_active ) {
		if ( !unit->IsActive() ) {
			SelectNextUnitOrSwitchToTileSelection();
		}
		else {
			unit->StartBadgeBlink();
		}
	}
}

void Game::MoveUnit( Unit* unit, Tile* dst_tile, const types::Vec3& dst_render_coords ) {

	auto* src_tile = unit->GetTile();

	unit->MoveTo( dst_tile, dst_render_coords );

	// TODO: animation

	if ( m_selected_unit == unit ) {
		m_selected_tile = m_selected_unit->GetTile();
	}

	// update old tile
	RenderTile( src_tile );

	// update unit
	RefreshUnit( unit );

	// update new tile
	RenderTile( dst_tile );

	if ( m_selected_unit == unit ) {
		m_ui.bottom_bar->PreviewTile( m_selected_tile, m_selected_unit->GetId() );
		ScrollToTile( m_selected_tile, false );
	}

}

void Game::ProcessRequest( const ::game::FrontendRequest* request ) {
	//Log( "Received frontend request (type=" + std::to_string( request->type ) + ")" ); // spammy
	const auto f_exit = [ this, request ]() -> void {
		const auto quit_reason = request->data.quit.reason
			? *request->data.quit.reason
			: "";
		ExitGame(
			[ this, quit_reason ]() -> void {
#ifdef DEBUG
				if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
					g_engine->ShutDown();
				}
				else
#endif
				{
					ReturnToMainMenu( quit_reason );
				}
			}
		);
	};
	switch ( request->type ) {
		case ::game::FrontendRequest::FR_QUIT: {
			f_exit();
			break;
		}
		case ::game::FrontendRequest::FR_ERROR: {
			Log( *request->data.error.stacktrace );
			g_engine->GetUI()->ShowError(
				*request->data.error.what, UH( f_exit ) {
					f_exit();
				}
			);
		}
		case ::game::FrontendRequest::FR_GLOBAL_MESSAGE: {
			AddMessage( *request->data.global_message.message );
			break;
		}
		case ::game::FrontendRequest::FR_TURN_STATUS: {
			m_turn_status = request->data.turn_status.status;
			ASSERT( m_ui.bottom_bar, "bottom bar not initialized" );
			m_ui.bottom_bar->SetTurnStatus( m_turn_status );
			bool is_turn_active = m_turn_status == ::game::turn::TS_TURN_ACTIVE || m_turn_status == ::game::turn::TS_TURN_COMPLETE;
			if ( m_is_turn_active != is_turn_active ) {
				m_is_turn_active = is_turn_active;
				if ( m_is_turn_active ) {
					m_selected_unit = nullptr;
					SelectNextUnitMaybe();
				}
				else {
					if ( m_selected_unit ) {
						DeselectTileOrUnit();
						//GetTileAtCoords( ::game::TQP_TILE_SELECT, m_selected_tile_data.tile_position ); // TODO ?
						m_selected_unit = nullptr;
					}
				}
			}
			break;
		}
		case ::game::FrontendRequest::FR_TURN_ADVANCE: {
			m_turn_id = request->data.turn_advance.turn_id;
			break;
		}
		case ::game::FrontendRequest::FR_SLOT_DEFINE: {
			for ( const auto& d : *request->data.slot_define.slotdefs ) {
				const auto& c = d.color;
				DefineSlot( d.slot_index, types::Color( c.r, c.g, c.b, c.a ), d.is_progenitor );
			}
			break;
		}
		case ::game::FrontendRequest::FR_ANIMATION_DEFINE: {
			types::Buffer buf( *request->data.unit_define.serialized_unitdef );
			const auto* animationdef = ::game::animation::Def::Unserialize( buf );
			DefineAnimation( animationdef );
			delete animationdef;
			break;
		}
		case ::game::FrontendRequest::FR_ANIMATION_SHOW: {
			const auto& d = request->data.animation_show;

			const auto animationdef_it = m_animationdefs.find( *d.animation_id );
			ASSERT( animationdef_it != m_animationdefs.end(), "animation id not found" );
			auto* animationdef = animationdef_it->second;

			const auto& c = d.render_coords;
			ShowAnimation(
				animationdef, d.running_animation_id, {
					c.x,
					c.y,
					c.z
				}
			);

			break;
		}
		case ::game::FrontendRequest::FR_UNIT_DEFINE: {
			types::Buffer buf( *request->data.unit_define.serialized_unitdef );
			const auto* unitdef = ::game::unit::Def::Unserialize( buf );
			DefineUnit( unitdef );
			delete unitdef;
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_SPAWN: {
			const auto& d = request->data.unit_spawn;
			const auto& tc = d.tile_coords;
			const auto& rc = d.render_coords;
			SpawnUnit(
				d.unit_id,
				*d.unitdef_id,
				d.slot_index,
				{
					tc.x,
					tc.y
				},
				{
					rc.x,
					rc.y,
					rc.z
				},
				d.movement,
				d.morale,
				*d.morale_string,
				d.health
			);
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_DESPAWN: {
			DespawnUnit( request->data.unit_despawn.unit_id );
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_REFRESH: {
			const auto& d = request->data.unit_refresh;
			auto* unit = m_units.at( d.unit_id );
			unit->SetMovement( d.movement );
			unit->SetHealth( d.health );
			const auto& c = unit->GetTile()->GetCoords();
			if ( d.tile_coords.x != c.x || d.tile_coords.y != c.y ) {
				MoveUnit(
					unit,
					GetTile(
						{
							d.tile_coords.x,
							d.tile_coords.y
						}
					), {
						d.render_coords.x,
						d.render_coords.y,
						d.render_coords.z,
					}
				);
			}
			else {
				RefreshUnit( unit );
				RenderTile( unit->GetTile() );
			}
			if ( unit->IsActive() ) {
				AddSelectable( unit );
			}
			else {
				RemoveSelectable( unit );
				if ( m_selected_unit == unit ) {
					SelectNextUnitOrSwitchToTileSelection();
				}
			}
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_MOVE: {
			const auto& d = request->data.unit_move;
			ASSERT( m_units.find( d.unit_id ) != m_units.end(), "unit id not found" );
			auto* unit = m_units.at( d.unit_id );
			auto* tile = GetTile(
				{
					d.tile_coords.x,
					d.tile_coords.y
				}
			);
			const auto& rc = d.render_coords;
			unit->SetMovement( d.movement_left );
			MoveUnit(
				unit, tile, {
					rc.x,
					rc.y,
					rc.z
				}
			);
			break;
		}
		default: {
			THROW( "unexpected frontend request type: " + std::to_string( request->type ) );
		}
	}
}

void Game::SendBackendRequest( const ::game::BackendRequest* request ) {
	m_pending_backend_requests.push_back( *request );
}

void Game::ActivateTurn() {

}

void Game::DeactivateTurn() {

}

void Game::UpdateMapData( const types::Vec2< size_t >& map_size ) {

	ASSERT( m_tiles.empty(), "tile states already set" );

	m_map_data.width = map_size.x;
	m_map_data.height = map_size.y;
	m_map_data.range.min = {
		-(float)( m_map_data.width - 1 ) * ::game::map::s_consts.tile.radius.x / 2,
		-(float)( m_map_data.height - 1 ) * ::game::map::s_consts.tile.radius.y / 2,
	};
	m_map_data.range.max = {
		(float)( m_map_data.width - 1 ) * ::game::map::s_consts.tile.radius.x / 2,
		(float)( m_map_data.height - 1 ) * ::game::map::s_consts.tile.radius.y / 2,
	};
	m_map_data.range.percent_to_absolute.x.SetRange(
		{
			{ 0.0f,                                                         1.0f },
			{ m_map_data.range.min.x - ::game::map::s_consts.tile.radius.x, m_map_data.range.max.x + ::game::map::s_consts.tile.radius.x }
		}
	);
	m_map_data.range.percent_to_absolute.y.SetRange(
		{
			{ 0.0f,                                                         1.0f },
			{ m_map_data.range.min.y - ::game::map::s_consts.tile.radius.y, m_map_data.range.max.y + ::game::map::s_consts.tile.radius.y }
		}
	);

	m_tiles.reserve( m_map_data.width * m_map_data.height / 2 ); // / 2 because smac coordinate system
	for ( size_t y = 0 ; y < m_map_data.height ; y++ ) {
		for ( size_t x = y & 1 ; x < m_map_data.width ; x += 2 ) {
			m_tiles.insert(
				{
					y * m_map_data.width + x,
					Tile{
						{
							x,
							y
						}
					}
				}
			);
		}
	}

	// link tiles to neighbours
	for ( size_t y = 0 ; y < m_map_data.height ; y++ ) {
		for ( size_t x = y & 1 ; x < m_map_data.width ; x += 2 ) {
			auto* ts = GetTile(
				{
					x,
					y
				}
			);

			ts->W = ( x >= 2 )
				? GetTile( x - 2, y )
				: GetTile( m_map_data.width - 1 - ( 1 - ( y % 2 ) ), y );
			ts->NW = ( y >= 1 )
				? ( ( x >= 1 )
					? GetTile( x - 1, y - 1 )
					: GetTile( m_map_data.width - 1, y - 1 )
				)
				: ts;
			ts->N = ( y >= 2 )
				? GetTile( x, y - 2 )
				: ts;
			ts->NE = ( y >= 1 )
				? ( ( x < m_map_data.width - 1 )
					? GetTile( x + 1, y - 1 )
					: GetTile( 0, y - 1 )
				)
				: ts;
			ts->E = ( x < m_map_data.width - 2 )
				? GetTile( x + 2, y )
				: GetTile( y % 2, y );
			ts->SE = ( y < m_map_data.height - 1 )
				? ( ( x < m_map_data.width - 1 )
					? GetTile( x + 1, y + 1 )
					: GetTile( 0, y + 1 )
				)
				: ts;
			ts->S = ( y < m_map_data.height - 2 )
				? GetTile( x, y + 2 )
				: ts;
			ts->SW = ( y < m_map_data.height - 1 )
				? ( ( x >= 1 )
					? GetTile( x - 1, y + 1 )
					: GetTile( m_map_data.width - 1, y + 1 )
				)
				: ts;
		}
	}

}

void Game::Initialize(
	types::texture::Texture* terrain_texture,
	types::mesh::Render* terrain_mesh,
	types::mesh::Data* terrain_data_mesh,
	const std::unordered_map< std::string, ::game::map::sprite_actor_t >& sprite_actors,
	const std::unordered_map< size_t, std::pair< std::string, types::Vec3 > >& sprite_instances,
	const std::vector< ::game::map::tile::Tile >* tiles,
	const std::vector< ::game::map::tile::TileState >* tile_states
) {
	ASSERT( !m_is_initialized, "already initialized" );

	auto* ui = g_engine->GetUI();

	if ( ui->HasPopup() ) {
		ui->CloseLastPopup( true );
	}

	Log( "Initializing game" );

	NEW( m_world_scene, scene::Scene, "Game", scene::SCENE_TYPE_ORTHO );
	NEW( m_ism, InstancedSpriteManager, m_world_scene );
	NEW( m_badge_defs, BadgeDefs, m_ism );

	NEW( m_camera, scene::Camera, scene::Camera::CT_ORTHOGRAPHIC );
	m_camera_angle = INITIAL_CAMERA_ANGLE;
	UpdateCameraAngle();

	m_world_scene->SetCamera( m_camera );

	// don't set exact 45 degree angles for lights, it will produce weird straight lines because of shadows
	{
		NEW( m_light_a, scene::Light, scene::Light::LT_AMBIENT_DIFFUSE );
		m_light_a->SetPosition(
			{
				48.227f,
				20.412f,
				57.65f
			}
		);
		m_light_a->SetColor(
			{
				0.8f,
				0.9f,
				1.0f,
				0.8f
			}
		);
		m_world_scene->AddLight( m_light_a );
	}
	{
		NEW( m_light_b, scene::Light, scene::Light::LT_AMBIENT_DIFFUSE );
		m_light_b->SetPosition(
			{
				22.412f,
				62.227f,
				43.35f
			}
		);
		m_light_b->SetColor(
			{
				1.0f,
				0.9f,
				0.8f,
				0.8f
			}
		);
		m_world_scene->AddLight( m_light_b );
	}

	g_engine->GetGraphics()->AddScene( m_world_scene );

	ASSERT( !m_textures.terrain, "terrain texture already set" );
	m_textures.terrain = terrain_texture;

	ASSERT( !m_actors.terrain, "terrain actor already set" );
	NEWV( terrain_actor, scene::actor::Mesh, "MapTerrain", terrain_mesh );
	terrain_actor->SetTexture( m_textures.terrain );
	terrain_actor->SetPosition( ::game::map::s_consts.map_position );
	terrain_actor->SetAngle( ::game::map::s_consts.map_rotation );
	terrain_actor->SetDataMesh( terrain_data_mesh );
	NEW( m_actors.terrain, scene::actor::Instanced, terrain_actor );
	m_actors.terrain->AddInstance( {} ); // default instance
	m_world_scene->AddActor( m_actors.terrain );

	Log( "Sprites count: " + std::to_string( sprite_actors.size() ) );
	Log( "Sprites instances: " + std::to_string( sprite_instances.size() ) );
	for ( auto& a : sprite_actors ) {
		GetTerrainInstancedSprite( a.second );
	}
	for ( auto& instance : sprite_instances ) {
		auto* actor = m_ism->GetInstancedSpriteByKey( instance.second.first )->actor;
		ASSERT( actor, "sprite actor not found" );
		ASSERT( !actor->HasInstance( instance.first ), "actor instance already exists" );
		actor->SetInstance( instance.first, instance.second.second );
	}

	// process tiles

	ASSERT( tiles, "tiles not set" );
	ASSERT( tiles->size() == m_map_data.width * m_map_data.height, "tiles count mismatch" ); // TODO: /2
	ASSERT( tile_states, "tile states not set" );
	ASSERT( tile_states->size() == m_map_data.width * m_map_data.height, "tile states count mismatch" ); // TODO: /2

	for ( size_t y = 0 ; y < m_map_data.height ; y++ ) {
		for ( size_t x = y & 1 ; x < m_map_data.width ; x += 2 ) {

			auto* tile = GetTile( x, y );
			const auto& t = tiles->at( y * m_map_data.width + x / 2 );
			const auto& ts = tile_states->at( y * m_map_data.width + x / 2 );

			Log( "Processing tile state: " + ts.coord.ToString() );

			::game::map::tile::tile_layer_type_t lt = ( t.is_water_tile
				? ::game::map::tile::LAYER_WATER
				: ::game::map::tile::LAYER_LAND
			);
			const auto& layer = ts.layers[ lt ];

			::game::map::tile::tile_vertices_t selection_coords = {};
			::game::map::tile::tile_vertices_t preview_coords = {};

#define x( _k ) selection_coords._k = layer.coords._k
			x( center );
			x( left );
			x( top );
			x( right );
			x( bottom );
#undef x

			if ( !t.is_water_tile && ts.is_coastline_corner ) {
				if ( t.W->is_water_tile ) {
					selection_coords.left = ts.layers[ ::game::map::tile::LAYER_WATER ].coords.left;
				}
				if ( t.N->is_water_tile ) {
					selection_coords.top = ts.layers[ ::game::map::tile::LAYER_WATER ].coords.top;
				}
				if ( t.E->is_water_tile ) {
					selection_coords.right = ts.layers[ ::game::map::tile::LAYER_WATER ].coords.right;
				}
				if ( t.S->is_water_tile ) {
					selection_coords.bottom = ts.layers[ ::game::map::tile::LAYER_WATER ].coords.bottom;
				}
			}

			lt = ( ( t.is_water_tile || ts.is_coastline_corner )
				? ::game::map::tile::LAYER_WATER
				: ::game::map::tile::LAYER_LAND
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

			std::vector< ::game::map::tile::tile_layer_type_t > layers = {};
			if ( t.is_water_tile ) {
				layers.push_back( ::game::map::tile::LAYER_LAND );
				layers.push_back( ::game::map::tile::LAYER_WATER_SURFACE );
				layers.push_back( ::game::map::tile::LAYER_WATER_SURFACE_EXTRA ); // TODO: only near coastlines?
				layers.push_back( ::game::map::tile::LAYER_WATER );
			}
			else {
				if ( ts.is_coastline_corner ) {
					layers.push_back( ::game::map::tile::LAYER_WATER_SURFACE );
					layers.push_back( ::game::map::tile::LAYER_WATER_SURFACE_EXTRA );
					layers.push_back( ::game::map::tile::LAYER_WATER );
				}
				else {
					layers.push_back( ::game::map::tile::LAYER_LAND );
				}
			}

			// looks a bit too bright without lighting otherwise
			const float tint_modifier = 0.7f;

			std::vector< types::mesh::Render* > preview_meshes = {};

			preview_meshes.reserve( layers.size() );
			for ( auto i = 0 ; i < layers.size() ; i++ ) {
				const auto& lt = layers[ i ];

				NEWV( mesh, types::mesh::Render, 5, 4 );

				auto& layer = ts.layers[ lt ];

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

				preview_meshes.push_back( mesh );
			}

			std::vector< std::string > sprites = {};
			for ( auto& s : ts.sprites ) {
				sprites.push_back( s.actor );
			}

			std::vector< std::string > info_lines = {};

			auto e = *t.elevation.center;
			if ( t.is_water_tile ) {
				if ( e < ::game::map::tile::ELEVATION_LEVEL_TRENCH ) {
					info_lines.push_back( "Ocean Trench" );
				}
				else if ( e < ::game::map::tile::ELEVATION_LEVEL_OCEAN ) {
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
				switch ( t.rockiness ) {
					case ::game::map::tile::ROCKINESS_FLAT: {
						tilestr += "Flat";
						break;
					}
					case ::game::map::tile::ROCKINESS_ROLLING: {
						tilestr += "Rolling";
						break;
					}
					case ::game::map::tile::ROCKINESS_ROCKY: {
						tilestr += "Rocky";
						break;
					}
				}
				tilestr += " & ";
				switch ( t.moisture ) {
					case ::game::map::tile::MOISTURE_ARID: {
						tilestr += "Arid";
						break;
					}
					case ::game::map::tile::MOISTURE_MOIST: {
						tilestr += "Moist";
						break;
					}
					case ::game::map::tile::MOISTURE_RAINY: {
						tilestr += "Rainy";
						break;
					}
				}
				info_lines.push_back( tilestr );
			}

#define FEATURE( _feature, _line ) \
            if ( t.features & ::game::map::tile::_feature ) { \
                info_lines.push_back( _line ); \
            }

			if ( t.is_water_tile ) {
				FEATURE( FEATURE_XENOFUNGUS, "Sea Fungus" )
			}
			else {
				FEATURE( FEATURE_XENOFUNGUS, "Xenofungus" )
			}

			switch ( t.bonus ) {
				case ::game::map::tile::BONUS_NUTRIENT: {
					info_lines.push_back( "Nutrient bonus" );
					break;
				}
				case ::game::map::tile::BONUS_ENERGY: {
					info_lines.push_back( "Energy bonus" );
					break;
				}
				case ::game::map::tile::BONUS_MINERALS: {
					info_lines.push_back( "Minerals bonus" );
					break;
				}
				default: {
					// nothing
				}
			}

			if ( t.is_water_tile ) {
				FEATURE( FEATURE_GEOTHERMAL, "Geothermal" )
			}
			else {
				FEATURE( FEATURE_RIVER, "River" )
				FEATURE( FEATURE_JUNGLE, "Jungle" )
				FEATURE( FEATURE_DUNES, "Dunes" )
				FEATURE( FEATURE_URANIUM, "Uranium" )
			}
			FEATURE( FEATURE_MONOLITH, "Monolith" )

#undef FEATURE

#define TERRAFORMING( _terraforming, _line ) \
            if ( t.terraforming & ::game::map::tile::_terraforming ) { \
                info_lines.push_back( _line ); \
            }

			if ( t.is_water_tile ) {
				TERRAFORMING( TERRAFORMING_FARM, "Kelp Farm" );
				TERRAFORMING( TERRAFORMING_SOLAR, "Tidal Harness" );
				TERRAFORMING( TERRAFORMING_MINE, "Mining Platform" );

				TERRAFORMING( TERRAFORMING_SENSOR, "Sensor Buoy" );
			}
			else {
				TERRAFORMING( TERRAFORMING_FOREST, "Forest" );
				TERRAFORMING( TERRAFORMING_FARM, "Farm" );
				TERRAFORMING( TERRAFORMING_SOIL_ENRICHER, "Soil Enricher" );
				TERRAFORMING( TERRAFORMING_MINE, "Mine" );
				TERRAFORMING( TERRAFORMING_SOLAR, "Solar Collector" );

				TERRAFORMING( TERRAFORMING_CONDENSER, "Condenser" );
				TERRAFORMING( TERRAFORMING_MIRROR, "Echelon Mirror" );
				TERRAFORMING( TERRAFORMING_BOREHOLE, "Thermal Borehole" );

				TERRAFORMING( TERRAFORMING_ROAD, "Road" );
				TERRAFORMING( TERRAFORMING_MAG_TUBE, "Mag Tube" );

				TERRAFORMING( TERRAFORMING_SENSOR, "Sensor Array" );
				TERRAFORMING( TERRAFORMING_BUNKER, "Bunker" );
				TERRAFORMING( TERRAFORMING_AIRBASE, "Airbase" );
			}

#undef TERRAFORMING

			// combine into printable lines
			std::string info_line = "";
			std::string info_line_new = "";
			constexpr size_t max_length = 16; // TODO: determine width from actual text because different symbols are different

			std::vector< std::string > preview_lines = {};
			for ( auto& line : info_lines ) {
				info_line_new = info_line + ( info_line.empty()
					? ""
					: ", "
				) + line;
				if ( info_line_new.size() > max_length ) {
					preview_lines.push_back( info_line );
					info_line = line;
				}
				else {
					info_line = info_line_new;
				}
			}
			if ( !info_line.empty() ) {
				preview_lines.push_back( info_line );
			}

			tile->SetCoords( layer.coords.center );
			tile->SetSelectionCoords( selection_coords );
			tile->SetPreviewMeshes( preview_meshes );
			tile->SetPreviewLines( preview_lines );
			tile->SetSprites( sprites );
		}
	}


	// UI
	NEW( m_ui.theme, ui::style::Theme );
	ui->AddTheme( m_ui.theme );
	NEW( m_ui.bottom_bar, ui::BottomBar, this );
	ui->AddObject( m_ui.bottom_bar );

	m_viewport.bottom_bar_overlap = 32; // it has transparent area on top so let map render through it

	// map event handlers

	auto* game = g_engine->GetGame();

	m_handlers.keydown_before = ui->AddGlobalEventHandler(
		::ui::event::EV_KEY_DOWN, EH( this, ui ) {
			if (
				ui->HasPopup() &&
					!data->key.modifiers &&
					data->key.code == ::ui::event::K_ESCAPE
				) {
				ui->CloseLastPopup();
				return true;
			}
			return false;
		}, ::ui::UI::GH_BEFORE
	);

	m_handlers.keydown_after = ui->AddGlobalEventHandler(
		::ui::event::EV_KEY_DOWN, EH( this, ui, game ) {

			if ( ui->HasPopup() ) {
				return false;
			}

			m_ui.bottom_bar->CloseMenus();

			if ( !data->key.modifiers ) {
				if ( m_selected_tile ) {

					bool is_tile_selected = false;
					::game::map::tile::direction_t td = ::game::map::tile::D_NONE;

					switch ( data->key.code ) {
#define X( _key, _altkey, _direction ) \
                        case ::ui::event::_key: \
                        case ::ui::event::_altkey: { \
                            td = ::game::map::tile::_direction; \
                            is_tile_selected = true; \
                            break; \
                        }
						X( K_LEFT, K_KP_LEFT, D_W )
						X( K_UP, K_KP_UP, D_N )
						X( K_RIGHT, K_KP_RIGHT, D_E )
						X( K_DOWN, K_KP_DOWN, D_S )
						X( K_HOME, K_KP_LEFT_UP, D_NW )
						X( K_END, K_KP_LEFT_DOWN, D_SW )
						X( K_PAGEUP, K_KP_RIGHT_UP, D_NE )
						X( K_PAGEDOWN, K_KP_RIGHT_DOWN, D_SE )
#undef X
						case ::ui::event::K_TAB: {
							SelectNextUnitMaybe();
							break;
						}
						case ::ui::event::K_SPACE: {
							if ( m_selected_unit ) {
								const auto event = ::game::event::SkipUnitTurn( m_slot_index, m_selected_unit->GetId() );
								game->MT_AddEvent( &event );
							}
							break;
						}
						case ::ui::event::K_ENTER: {
							if ( m_turn_status == ::game::turn::TS_TURN_COMPLETE ) {
								CompleteTurn();
							}
							break;
						}
						default: {
							// nothing
						}
					}

					if ( is_tile_selected ) {
						auto* tile = m_selected_tile->GetNeighbour( td );

						switch ( m_tile_at_query_purpose ) {
							case ::game::TQP_TILE_SELECT: {
								SelectTileOrUnit( tile );
								ScrollToTile( tile, false );
								return true;
							}
							case ::game::TQP_UNIT_SELECT: {
								// try moving unit to tile
								if ( m_selected_unit ) {
									const auto& ts = m_selected_unit->GetTile()->GetNeighbour( td );
									std::unordered_map< size_t, Unit* > foreign_units = {};
									for ( const auto& it : ts->GetUnits() ) {
										const auto& unit = it.second;
										if ( !unit->IsOwned() ) { // TODO: pacts
											// TODO: skip units of treaty/truce faction?
											foreign_units.insert( it );
										}
									}
									if ( foreign_units.empty() ) {
										// move
										const auto event = ::game::event::MoveUnit( m_slot_index, m_selected_unit->GetId(), td );
										game->MT_AddEvent( &event );
									}
									else {
										// attack
										// TODO: select the most powerful defender for attacker
										const auto& target_unit = foreign_units.at( Tile::GetUnitsOrder( foreign_units ).front() );
										const auto event = ::game::event::AttackUnit( m_slot_index, m_selected_unit->GetId(), target_unit->GetId() );
										game->MT_AddEvent( &event );
									}

								}
								return true;
							}
							default: {
								// nothing
							}
						}
					}
				}

				if ( data->key.key == 'z' ) {
					m_map_control.key_zooming = 1;
					return true;
				}
				if ( data->key.key == 'x' ) {
					m_map_control.key_zooming = -1;
					return true;
				}

				if ( data->key.code == ::ui::event::K_ESCAPE ) {
					if ( !g_engine->GetUI()->HasPopup() ) { // close all other popups first (including same one)
						ConfirmExit(
							UH( this ) {
								ExitGame(
									[]() -> void {
										g_engine->ShutDown();
									}
								);
							}
						);
					}
					return true;
				}
			}
			else if ( m_is_map_editing_allowed && data->key.code == ::ui::event::K_CTRL ) {
				m_is_editing_mode = true;
			}

			return false;
		}, ::ui::UI::GH_AFTER
	);

	m_handlers.keyup = ui->AddGlobalEventHandler(
		::ui::event::EV_KEY_UP, EH( this ) {
			if ( data->key.key == 'z' || data->key.key == 'x' ) {
				if ( m_map_control.key_zooming ) {
					m_map_control.key_zooming = 0;
					m_scroller.Stop();
				}
			}
			else if ( m_is_map_editing_allowed && data->key.code == ::ui::event::K_CTRL ) {
				m_is_editing_mode = false;
				m_editing_draw_timer.Stop();
				m_editor_draw_mode = ::game::map_editor::DM_NONE;
			}
			return false;
		}, ::ui::UI::GH_BEFORE
	);

	m_handlers.mousedown = ui->AddGlobalEventHandler(
		::ui::event::EV_MOUSE_DOWN, EH( this, ui ) {
			if ( ui->HasPopup() ) {
				return false;
			}

			m_ui.bottom_bar->CloseMenus();

			if ( m_is_map_editing_allowed && m_is_editing_mode ) {
				switch ( data->mouse.button ) {
					case ::ui::event::M_LEFT: {
						m_editor_draw_mode = ::game::map_editor::DM_INC;
						break;
					}
					case ::ui::event::M_RIGHT: {
						m_editor_draw_mode = ::game::map_editor::DM_DEC;
						break;
					}
					default: {
						m_editor_draw_mode = ::game::map_editor::DM_NONE;
					}

				}
				SelectTileAtPoint( ::game::TQP_TILE_SELECT, data->mouse.absolute.x, data->mouse.absolute.y ); // async
				m_editing_draw_timer.SetInterval( Game::s_consts.map_editing.draw_frequency_ms ); // keep drawing until mouseup
			}
			else {
				switch ( data->mouse.button ) {
					case ::ui::event::M_LEFT: {
						SelectTileAtPoint(
							::game::TQP_UNIT_SELECT,
							data->mouse.absolute.x,
							data->mouse.absolute.y
						); // async
						break;
					}
					case ::ui::event::M_MIDDLE: {
						m_scroller.Stop();
						m_map_control.is_dragging = true;
						m_map_control.last_drag_position = {
							m_clamp.x.Clamp( data->mouse.absolute.x ),
							m_clamp.y.Clamp( data->mouse.absolute.y )
						};
						break;
					}
				}
			}
			return true;
		}, ::ui::UI::GH_AFTER
	);

	m_handlers.mousemove = ui->AddGlobalEventHandler(
		::ui::event::EV_MOUSE_MOVE, EH( this, ui ) {
			m_map_control.last_mouse_position = {
				GetFixedX( data->mouse.absolute.x ),
				(float)data->mouse.absolute.y
			};

			if ( ui->HasPopup() ) {
				return false;
			}

			if ( m_map_control.is_dragging ) {
				types::Vec2< float > current_drag_position = {
					m_clamp.x.Clamp( data->mouse.absolute.x ),
					m_clamp.y.Clamp( data->mouse.absolute.y )
				};
				types::Vec2< float > drag = current_drag_position - m_map_control.last_drag_position;

				m_camera_position.x += (float)drag.x;
				m_camera_position.y += (float)drag.y;
				UpdateCameraPosition();

				m_map_control.last_drag_position = current_drag_position;
			}
			else if ( !m_ui.bottom_bar->IsMouseDraggingMiniMap() ) {
				if ( g_engine->GetGraphics()->IsFullscreen() ) { // edge scrolling only usable in fullscreen
					const ssize_t edge_distance = m_viewport.is_fullscreen
						? Game::s_consts.map_scroll.static_scrolling.edge_distance_px.fullscreen
						: Game::s_consts.map_scroll.static_scrolling.edge_distance_px.windowed;
					if ( data->mouse.absolute.x < edge_distance ) {
						m_map_control.edge_scrolling.speed.x = Game::s_consts.map_scroll.static_scrolling.speed.x;
					}
					else if ( data->mouse.absolute.x >= m_viewport.window_width - edge_distance ) {
						m_map_control.edge_scrolling.speed.x = -Game::s_consts.map_scroll.static_scrolling.speed.x;
					}
					else {
						m_map_control.edge_scrolling.speed.x = 0;
					}
					if ( data->mouse.absolute.y <= edge_distance ) {
						m_map_control.edge_scrolling.speed.y = Game::s_consts.map_scroll.static_scrolling.speed.y;
					}
					else if ( data->mouse.absolute.y >= m_viewport.window_height - edge_distance ) {
						m_map_control.edge_scrolling.speed.y = -Game::s_consts.map_scroll.static_scrolling.speed.y;
					}
					else {
						m_map_control.edge_scrolling.speed.y = 0;
					}
				}
				else {
					m_map_control.edge_scrolling.speed = {
						0.0f,
						0.0f
					};
				}
				if ( m_map_control.edge_scrolling.speed ) {
					if ( !m_map_control.edge_scrolling.timer.IsRunning() ) {
						//Log( "Edge scrolling started" );
						m_map_control.edge_scrolling.timer.SetInterval( Game::s_consts.map_scroll.static_scrolling.scroll_step_ms );
					}
				}
				else {
					if ( m_map_control.edge_scrolling.timer.IsRunning() ) {
						//Log( "Edge scrolling stopped" );
						m_map_control.edge_scrolling.timer.Stop();
					}
				}
			}
			return true;
		}, ::ui::UI::GH_AFTER
	);

	m_handlers.mouseup = ui->AddGlobalEventHandler(
		::ui::event::EV_MOUSE_UP, EH( this ) {
			switch ( data->mouse.button ) {
				case ::ui::event::M_MIDDLE: {
					m_map_control.is_dragging = false;
					break;
				}
			}
			if ( m_is_map_editing_allowed && m_is_editing_mode ) {
				m_editing_draw_timer.Stop();
				m_editor_draw_mode = ::game::map_editor::DM_NONE;
			}
			return true;
		}, ::ui::UI::GH_AFTER
	);

	m_handlers.mousescroll = ui->AddGlobalEventHandler(
		::ui::event::EV_MOUSE_SCROLL, EH( this, ui ) {
			if ( ui->HasPopup() ) {
				return false;
			}

			SmoothScroll( m_map_control.last_mouse_position, data->mouse.scroll_y );
			return true;
		}, ::ui::UI::GH_AFTER
	);

	// other stuff

	m_clamp.x.SetDstRange(
		{
			-0.5f,
			0.5f
		}
	);
	m_clamp.y.SetDstRange(
		{
			-0.5f,
			0.5f
		}
	);

	// map should continue scrolling even if mouse is outside viewport
	m_clamp.x.SetOverflowAllowed( true );
	m_clamp.y.SetOverflowAllowed( true );

	UpdateViewport();

	// assume mouse starts at center
	m_map_control.last_mouse_position = {
		(float)m_viewport.window_width / 2,
		(float)m_viewport.window_height / 2
	};

	SetCameraPosition(
		{
			0.0f,
			-0.25f,
			0.1f
		}
	);

	UpdateCameraRange();
	UpdateCameraScale();

	g_engine->GetGraphics()->AddOnWindowResizeHandler(
		this, RH( this ) {
			UpdateViewport();
			UpdateCameraRange();
			UpdateMapInstances();
			UpdateMinimap();
		}
	);
	m_is_resize_handler_set = true;

	ResetMapState();

	m_is_initialized = true;
}

void Game::Deinitialize() {
	ASSERT( m_is_initialized, "not initialized" );

	CloseMenus();
	DeselectTileOrUnit();

	auto* ui = g_engine->GetUI();

	if ( m_ui.bottom_bar ) {
		ui->RemoveObject( m_ui.bottom_bar );
		ui->RemoveTheme( m_ui.theme );
		m_ui.bottom_bar = nullptr;
		DELETE( m_ui.theme );
		m_ui.theme = nullptr;
	}

	if ( m_is_resize_handler_set ) {
		g_engine->GetGraphics()->RemoveOnWindowResizeHandler( this );
		m_is_resize_handler_set = false;
	}

#define x( _k ) \
    if ( m_handlers._k ) { \
        ui->RemoveGlobalEventHandler( m_handlers._k ); \
        m_handlers._k = nullptr; \
    }
	x( keydown_before );
	x( keydown_after );
	x( keyup );
	x( mousedown );
	x( mousemove );
	x( mouseup );
	x( mousescroll );
#undef x

	if ( m_tile_at_request_id ) {
		ASSERT( m_actors.terrain, "tileat request pending but terrain actor not set" );
		CancelTileAtRequest();
	}

	if ( m_minimap_texture_request_id ) {
		ASSERT( m_actors.terrain, "minimap texture request pending but terrain actor not set" );
		m_actors.terrain->GetMeshActor()->CancelDataRequest( m_minimap_texture_request_id );
		m_minimap_texture_request_id = 0;
	}

	if ( m_actors.terrain ) {
		m_world_scene->RemoveActor( m_actors.terrain );
		DELETE( m_actors.terrain );
		m_actors.terrain = nullptr;
	}

	if ( m_textures.terrain ) {
		DELETE( m_textures.terrain );
		m_textures.terrain = nullptr;
	}

	for ( const auto& it : m_animations ) {
		delete it.second;
	}
	m_animations.clear();

	for ( const auto& it : m_animationdefs ) {
		delete it.second;
	}
	m_animationdefs.clear();

	for ( const auto& it : m_units ) {
		delete it.second;
	}
	m_units.clear();

	for ( const auto& it : m_unitdefs ) {
		delete it.second;
	}
	m_unitdefs.clear();

	for ( const auto& it : m_slots ) {
		delete it.second;
	}
	m_slots.clear();

	for ( auto& it : m_actors_map ) {
		m_world_scene->RemoveActor( it.second );
		DELETE( it.second );
	}
	m_actors_map.clear();

	if ( m_badge_defs ) {
		DELETE( m_badge_defs );
		m_badge_defs = nullptr;
	}

	if ( m_ism ) {
		DELETE( m_ism );
		m_ism = nullptr;
	}

	if ( m_world_scene ) {
		g_engine->GetGraphics()->RemoveScene( m_world_scene );
		m_world_scene = nullptr;
	}

#define x( _obj ) \
    if ( _obj ) { \
        DELETE( _obj ); \
        _obj = nullptr; \
    }
	x( m_camera );
	x( m_light_a );
	x( m_light_b );
#undef x

	m_textures.terrain = nullptr;

	m_is_initialized = false;

}

void Game::AddMessage( const std::string& text ) {
	if ( m_ui.bottom_bar ) {
		m_ui.bottom_bar->AddMessage( text );
	}
}

void Game::SendChatMessage( const std::string& text ) {
	ASSERT( !m_mt_ids.chat, "previous chat request still pending" );
	m_mt_ids.chat = g_engine->GetGame()->MT_Chat( text );
}

void Game::CompleteTurn() {
	m_is_turn_active = false;
	const auto event = ::game::event::CompleteTurn( m_slot_index, m_turn_id );
	g_engine->GetGame()->MT_AddEvent( &event );
}

void Game::UncompleteTurn() {
	m_is_turn_active = false;
	const auto event = ::game::event::UncompleteTurn( m_slot_index, m_turn_id );
	g_engine->GetGame()->MT_AddEvent( &event );
}

void Game::SelectTileAtPoint( const ::game::tile_query_purpose_t tile_query_purpose, const size_t x, const size_t y ) {
	//Log( "Looking up tile at " + std::to_string( x ) + "x" + std::to_string( y ) );
	GetTileAtScreenCoords( tile_query_purpose, x, m_viewport.window_height - y ); // async
}

void Game::SelectUnit( Unit* unit, const bool actually_select_unit ) {
	m_tile_at_query_purpose = ::game::TQP_UNIT_SELECT;
	DeselectTileOrUnit();
	if ( m_selected_unit != unit ) {

		unit->SetActiveOnTile();

		m_selected_unit = unit;
		m_selected_tile = m_selected_unit->GetTile();
		m_selected_unit->Show();

		m_ui.bottom_bar->PreviewTile( m_selected_tile, m_selected_unit->GetId() );
	}
	if ( actually_select_unit && m_selected_unit->IsActive() && m_is_turn_active ) {

		if ( m_selected_tile && !m_selected_tile->GetUnits().empty() ) {
			auto* most_important_unit = m_selected_tile->GetMostImportantUnit();
			if ( m_selected_unit != most_important_unit ) {
				most_important_unit->Hide();
			}
		}

		m_selected_unit->StartBadgeBlink();
		SetCurrentSelectable( m_selected_unit );
		Log( "Selected unit " + std::to_string( m_selected_unit->GetId() ) );
	}
}

void Game::SelectTileOrUnit( Tile* tile, const size_t selected_unit_id ) {

	ASSERT( m_tile_at_query_purpose != ::game::TQP_NONE, "tile query purpose not set" );

	DeselectTileOrUnit();

	m_selected_tile = tile;

	Unit* selected_unit = nullptr;
	if ( m_tile_at_query_purpose == ::game::TQP_UNIT_SELECT ) {
		const auto& units = m_selected_tile->GetUnits();
		if ( m_is_turn_active ) {
			if ( selected_unit_id ) {
				for ( const auto& it : units ) {
					if ( it.first == selected_unit_id ) {
						selected_unit = it.second;
						break;
					}
				}
			}
			if ( !selected_unit ) {
				selected_unit = m_selected_tile->GetMostImportantUnit();//GetFirstSelectableUnit( units );
			}
		}
		if ( !selected_unit ) {
			m_tile_at_query_purpose = ::game::TQP_TILE_SELECT;
		}
	}

	switch ( m_tile_at_query_purpose ) {
		case ::game::TQP_TILE_SELECT: {
			m_selected_unit = nullptr;
			Log( "Selected tile at " + m_selected_tile->GetCoords().ToString() + " ( " + m_selected_tile->GetRenderData().selection_coords.center.ToString() + " )" );
			ShowTileSelector();
			m_ui.bottom_bar->PreviewTile( m_selected_tile, selected_unit_id );
			break;
		}
		case ::game::TQP_UNIT_SELECT: {
			SelectUnit( selected_unit, true );
			break;
		}
		default:
			THROW( "unknown selection mode: " + std::to_string( m_tile_at_query_purpose ) );
	}

}

void Game::DeselectTileOrUnit() {

	HideTileSelector();
	if ( m_selected_unit ) {

		ASSERT( !m_selected_tile->GetUnits().empty(), "unit was selected but tile data has no units" );

		// reset to most important unit if needed
		auto* most_important_unit = m_selected_tile->GetMostImportantUnit();
		if ( m_selected_unit ) {
			if ( m_selected_unit == most_important_unit ) {
				m_selected_unit->StopBadgeBlink( true );
			}
			else {
				m_selected_unit->Hide();
				most_important_unit->Show();
			}
			m_selected_unit = nullptr;
		}
	}

	m_ui.bottom_bar->HideTilePreview();
}

Unit* Game::GetFirstSelectableUnit( const std::unordered_map< size_t, Unit* >& units ) const {
	for ( const auto& it : units ) {
		auto* unit = it.second;
		if ( unit->IsActive() ) {
			return unit;
		}
	}
	return nullptr;
}

void Game::AddActor( actor::Actor* actor ) {
	ASSERT( m_actors_map.find( actor ) == m_actors_map.end(), "world actor already added" );
	NEWV( instanced, scene::actor::Instanced, actor );
	instanced->AddInstance( {} ); // default instance
	m_world_scene->AddActor( instanced );
	m_actors_map[ actor ] = instanced;
}

void Game::RemoveActor( actor::Actor* actor ) {
	auto it = m_actors_map.find( actor );
	ASSERT( it != m_actors_map.end(), "world actor not found" );
	m_world_scene->RemoveActor( it->second );
	DELETE( it->second );
	m_actors_map.erase( it );
}

const types::Vec2< float > Game::GetTileWindowCoordinates( const Tile* tile ) const {
	const auto& c = tile->GetRenderData().coords;
	return {
		c.x * m_viewport.window_aspect_ratio * m_camera_position.z,
		( c.y - ( c.z - 2.0f ) ) * m_viewport.ratio.y * m_camera_position.z / 1.414f
	};
}

void Game::ScrollTo( const types::Vec3& target ) {
	if ( m_scroller.IsRunning() && true /* TODO: m_selected_tile_data.scroll_adaptively */ ) {
		const auto& target = m_scroller.GetTargetPosition();
		if ( target.z == m_camera_position.z ) {
			m_camera_position = m_scroller.GetTargetPosition();
			UpdateCameraPosition();
		}
	}
	m_scroller.Scroll( m_camera_position, target );
}

void Game::ScrollToTile( const Tile* tile, bool center_on_tile ) {

	auto tc = GetTileWindowCoordinates( tile );

	if ( center_on_tile ) {
		const float tile_x_shifted = m_camera_position.x > 0
			? tc.x - ( m_camera_range.max.x - m_camera_range.min.x )
			: tc.x + ( m_camera_range.max.x - m_camera_range.min.x );
		if (
			fabs( tile_x_shifted - -m_camera_position.x )
				<
					fabs( tc.x - -m_camera_position.x )
			) {
			// smaller distance if going other side
			tc.x = tile_x_shifted;
		}

		ScrollTo(
			{
				-tc.x,
				-tc.y,
				m_camera_position.z
			}
		);
	}
	else {
		types::Vec2< float > uc = {
			GetFixedX( tc.x + m_camera_position.x ),
			tc.y + m_camera_position.y
		};

		types::Vec2< float > scroll_by = {
			0,
			0
		};

		//Log( "Resolved tile coordinates to " + uc.ToString() + " ( camera: " + m_camera_position.ToString() + " )" );

		// tile size
		types::Vec2< float > ts = {
			::game::map::s_consts.tile.scale.x * m_camera_position.z,
			::game::map::s_consts.tile.scale.y * m_camera_position.z
		};
		// edge size
		types::Vec2< float > es = {
			0.5f - ts.x,
			0.5f - ts.y
		};

		if ( uc.x < -es.x ) {
			scroll_by.x = ts.x - 0.5f - uc.x;
		}
		else if ( uc.x > es.x ) {
			scroll_by.x = 0.5f - ts.x - uc.x;
		}
		if ( uc.y < -es.y ) {
			scroll_by.y = ts.y - 0.5f - uc.y;
		}
		else if ( uc.y > es.y ) {
			scroll_by.y = 0.5f - ts.y - uc.y;
		}

		if ( scroll_by ) {
			//Log( "Scroll by " + scroll_by.ToString() );
			FixCameraX();
			ScrollTo(
				{
					m_camera_position.x + scroll_by.x,
					m_camera_position.y + scroll_by.y,
					m_camera_position.z
				}
			);
		}
	}
}

void Game::CancelTileAtRequest() {
	ASSERT( m_tile_at_request_id, "tileat request not found" );
	m_actors.terrain->GetMeshActor()->CancelDataRequest( m_tile_at_request_id );
	m_tile_at_request_id = 0;
}

void Game::GetTileAtScreenCoords( const ::game::tile_query_purpose_t tile_query_purpose, const size_t screen_x, const size_t screen_inverse_y ) {
	if ( m_tile_at_request_id ) {
		m_actors.terrain->GetMeshActor()->CancelDataRequest( m_tile_at_request_id );
	}
	ASSERT( tile_query_purpose != ::game::TQP_NONE, "tile query purpose is not set" );
	m_tile_at_query_purpose = tile_query_purpose;
	m_tile_at_request_id = m_actors.terrain->GetMeshActor()->GetDataAt( screen_x, screen_inverse_y );
}

const bool Game::IsTileAtRequestPending() const {
	return m_tile_at_request_id;
}

const Game::tile_at_result_t Game::GetTileAtScreenCoordsResult() {
	if ( m_tile_at_request_id ) {
		auto result = m_actors.terrain->GetMeshActor()->GetDataResponse( m_tile_at_request_id );
		if ( result.first ) {
			m_tile_at_request_id = 0;

			if ( result.second ) {
				auto data = *result.second;
				if ( data ) { // some tile was clicked

					data--; // we used +1 increment to differentiate 'tile at 0,0' from 'no tiles'

					return {
						true,
						{
							data % m_map_data.width,
							data / m_map_data.width
						}
					};
				}
			}
		}
	}
	// no data
	return {};
}

void Game::GetMinimapTexture( scene::Camera* camera, const types::Vec2< size_t > texture_dimensions ) {
	if ( m_minimap_texture_request_id ) {
		Log( "Canceling minimap texture request" );
		m_actors.terrain->GetMeshActor()->CancelDataRequest( m_minimap_texture_request_id );
	}
	Log( "Requesting minimap texture" );
	m_minimap_texture_request_id = m_actors.terrain->GetMeshActor()->CaptureToTexture( camera, texture_dimensions );
}

types::texture::Texture* Game::GetMinimapTextureResult() {
	if ( m_minimap_texture_request_id ) {
		auto result = m_actors.terrain->GetMeshActor()->GetCaptureToTextureResponse( m_minimap_texture_request_id );
		if ( result ) {
			Log( "Received minimap texture" );
			m_minimap_texture_request_id = 0;
			return result;
		}
	}
	// no texture (yet)
	return nullptr;
}

void Game::UpdateMinimap() {
	NEWV( camera, scene::Camera, scene::Camera::CT_ORTHOGRAPHIC );

	auto mm = m_ui.bottom_bar->GetMinimapDimensions();
	// 'black grid' artifact workaround
	// TODO: find reason and fix properly, maybe just keep larger internal viewport
	types::Vec2< float > scale = {
		(float)m_viewport.window_width / mm.x,
		(float)m_viewport.window_height / mm.y
	};

	mm.x *= scale.x;
	mm.y *= scale.y;

	const float sx = (float)mm.x / (float)m_map_data.width / (float)::game::map::s_consts.tc.texture_pcx.dimensions.x;
	const float sy = (float)mm.y / (float)m_map_data.height / (float)::game::map::s_consts.tc.texture_pcx.dimensions.y;
	const float sz = ( sx + sy ) / 2;
	const float ss = ( (float)mm.y / (float)m_viewport.window_height );
	const float sxy = (float)scale.x / scale.y;

	camera->SetAngle( m_camera->GetAngle() );
	camera->SetScale(
		{
			sx * ss * sxy / scale.x,
			sy * ss * 1.38f / scale.y,
			0.01f
		}
	);

	camera->SetPosition(
		{
			ss * sxy,
			1.0f - ss * 0.48f,
			0.5f
		}
	);

	GetMinimapTexture(
		camera, {
			mm.x,
			mm.y
		}
	);
}

void Game::ResetMapState() {
	UpdateCameraPosition();
	UpdateMapInstances();
	UpdateUICamera();
	UpdateMinimap();

	// select tile at center
	types::Vec2< size_t > coords = {
		m_map_data.width / 2,
		m_map_data.height / 2
	};
	if ( ( coords.y % 2 ) != ( coords.x % 2 ) ) {
		coords.y++;
	}

	m_tile_at_query_purpose = ::game::TQP_TILE_SELECT;
	SelectTileOrUnit( GetTile( coords ) );
}

void Game::SmoothScroll( const float scroll_value ) {
	SmoothScroll(
		{
			(float)m_viewport.width / 2,
			(float)m_viewport.height / 2
		}, scroll_value
	);
}

void Game::SmoothScroll( const types::Vec2< float > position, const float scroll_value ) {

	float speed = Game::s_consts.map_scroll.smooth_scrolling.zoom_speed * m_camera_position.z;

	float new_z = m_camera_position.z + scroll_value * speed;

	if ( new_z < m_camera_range.min.z ) {
		new_z = m_camera_range.min.z;
	}
	if ( new_z > m_camera_range.max.z ) {
		new_z = m_camera_range.max.z;
	}

	float diff = m_camera_position.z / new_z;

	types::Vec2< float > m = {
		m_clamp.x.Clamp( position.x ),
		m_clamp.y.Clamp( position.y )
	};

	ScrollTo(
		{
			( m_camera_position.x - m.x ) / diff + m.x,
			( m_camera_position.y - m.y ) / diff + m.y,
			new_z
		}
	);
}

util::random::Random* Game::GetRandom() const {
	return m_random;
}

void Game::CloseMenus() {
	if ( m_ui.bottom_bar ) {
		m_ui.bottom_bar->CloseMenus();
	}
}

void Game::ExitGame( const f_exit_game on_game_exit ) {
	auto* ui = g_engine->GetUI();
	auto* game = g_engine->GetGame();
	ui->ShowLoader( "Exiting game" );
	m_on_game_exit = on_game_exit;
	CancelRequests();
	if ( !m_mt_ids.reset ) {
		m_mt_ids.reset = game->MT_Reset();
	}
}

void Game::AddSelectable( Unit* unit ) {
	const auto& it = std::find( m_selectables.units.begin(), m_selectables.units.end(), unit );
	if ( it == m_selectables.units.end() ) {
		if ( m_selectables.units.empty() ) {
			m_selectables.selected_id_index = 0;
		}
		m_selectables.units.push_back( unit );
	}
}

void Game::RemoveSelectable( Unit* unit ) {
	const auto& it = std::find( m_selectables.units.begin(), m_selectables.units.end(), unit );
	if ( it != m_selectables.units.end() ) {
		const size_t removed_index = it - m_selectables.units.begin();
		m_selectables.units.erase( it );
		if ( m_selectables.selected_id_index > 0 && m_selectables.selected_id_index >= removed_index ) {
			m_selectables.selected_id_index--;
		}
		if ( m_selected_unit == unit ) {
			m_selected_unit = nullptr;
			SelectNextUnitOrSwitchToTileSelection();
		}
	}
}

void Game::UpdateSelectable( Unit* unit ) {
	if ( unit->IsOwned() ) {
		if ( unit->IsActive() ) {
			AddSelectable( unit );
		}
		else {
			RemoveSelectable( unit );
		}
	}
}

void Game::SetCurrentSelectable( Unit* unit ) {
	const auto& it = std::find( m_selectables.units.begin(), m_selectables.units.end(), unit );
	ASSERT( it != m_selectables.units.end(), "selectable not found" );
	m_selectables.selected_id_index = it - m_selectables.units.begin();
}

Unit* Game::GetCurrentSelectable() {
	if ( !m_selectables.units.empty() ) {
		return m_selectables.units.at( m_selectables.selected_id_index );
	}
	return nullptr;
}

Unit* Game::GetNextSelectable() {
	if ( !m_selectables.units.empty() ) {
		m_selectables.selected_id_index++;
		if ( m_selectables.selected_id_index >= m_selectables.units.size() ) {
			m_selectables.selected_id_index = 0;
		}
		return GetCurrentSelectable();
	}
	return nullptr;
}

const bool Game::SelectNextUnitMaybe() {
	if ( !m_is_turn_active ) {
		return false;
	}
	auto* selected_unit = m_selected_unit
		? GetNextSelectable()
		: GetCurrentSelectable();
	if ( selected_unit ) {
		Log( "Selecting unit " + std::to_string( selected_unit->GetId() ) );
		SelectUnit( selected_unit, true );
		ScrollToTile( m_selected_unit->GetTile(), true );
		return true;
	}
	return false;
}

void Game::SelectNextUnitOrSwitchToTileSelection() {
	if ( !SelectNextUnitMaybe() ) {
		SelectTileOrUnit(
			m_selected_tile, m_selected_unit
				? m_selected_unit->GetId()
				: 0
		);
	}
}

void Game::CancelRequests() {
	auto* game = g_engine->GetGame();
	if ( m_mt_ids.init ) {
		game->MT_Cancel( m_mt_ids.init );
		m_mt_ids.init = 0;
		// WHY?
		// is it even needed?
		/*ASSERT( !m_mt_ids.ping, "ping already active" );
		g_engine->GetUI()->SetLoaderText( "Canceling" );
		m_mt_ids.ping = game->MT_Ping();*/
	}
	if ( m_mt_ids.get_map_data ) {
		game->MT_Cancel( m_mt_ids.get_map_data );
		m_mt_ids.get_map_data = 0;
	}
	if ( m_mt_ids.reset ) {
		game->MT_Cancel( m_mt_ids.reset );
		m_mt_ids.reset = 0;
	}
	for ( const auto& mt_id : m_mt_ids.select_tile ) {
		game->MT_Cancel( mt_id );
	}
	m_mt_ids.select_tile.clear();
	if ( m_mt_ids.get_frontend_requests ) {
		game->MT_Cancel( m_mt_ids.get_frontend_requests );
		m_mt_ids.get_frontend_requests = 0;
	}
	if ( m_mt_ids.send_backend_requests ) {
		game->MT_Cancel( m_mt_ids.send_backend_requests );
		m_mt_ids.send_backend_requests = 0;
	}
	// TODO: cancel other requests?
}

void Game::CancelGame() {
	ExitGame(
		[ this ]() -> void {
			if ( m_state ) {
				auto* connection = m_state->GetConnection();
				if ( connection && connection->IsConnected() ) {
					connection->Disconnect();
				}
			}
			if ( m_on_cancel ) {
				m_on_cancel();
				m_on_cancel = nullptr;
			}
			g_engine->GetScheduler()->RemoveTask( this );
		}
	);
}

void Game::ReturnToMainMenu( const std::string reason ) {
	NEWV( task, task::mainmenu::MainMenu );
	if ( !reason.empty() ) {
		task->ShowErrorOnStart( reason );
	}
	g_engine->GetScheduler()->RemoveTask( this );
	g_engine->GetScheduler()->AddTask( task );
}

Tile* Game::GetTile( const size_t x, const size_t y ) {
	ASSERT( !m_tiles.empty(), "tile states not set" );
	ASSERT( x < m_map_data.width, "tile x overflow" );
	ASSERT( y < m_map_data.height, "tile y overflow" );
	return &m_tiles.at( y * m_map_data.width + x );
}

Tile* Game::GetTile( const types::Vec2< size_t >& coords ) {
	return GetTile( coords.x, coords.y );
}

void Game::ShowTileSelector() {
	HideTileSelector();
	NEW( m_actors.tile_selection, actor::TileSelection, m_selected_tile->GetRenderData().selection_coords );
	AddActor( m_actors.tile_selection );
}

void Game::HideTileSelector() {
	if ( m_actors.tile_selection ) {
		RemoveActor( m_actors.tile_selection );
		m_actors.tile_selection = nullptr;
	}
}

void Game::RenderTile( Tile* tile ) {
	tile->Render(
		m_selected_unit
			? m_selected_unit->GetId()
			: 0
	);
	if ( m_selected_unit && m_selected_unit->IsActive() ) {
		m_selected_unit->StartBadgeBlink();
	}
}

}
}
