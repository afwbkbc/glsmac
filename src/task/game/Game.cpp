#include "Game.h"

#include <algorithm>
#include <map>

#include "engine/Engine.h"
#include "../mainmenu/MainMenu.h"
#include "graphics/Graphics.h"
#include "util/FS.h"
#include "ui/popup/PleaseDontGo.h"

#include "game/unit/StaticDef.h"
#include "game/unit/SpriteRender.h"

#include "Types.h"

#include "game/event/MoveUnit.h"
#include "game/event/SkipUnitTurn.h"
#include "game/event/CompleteTurn.h"
#include "game/event/UncompleteTurn.h"

#define INITIAL_CAMERA_ANGLE { -M_PI * 0.5, M_PI * 0.75, 0 }

namespace task {
namespace game {

const Game::consts_t Game::s_consts = {};

Game::Game( ::game::State* state, ui_handler_t on_start, ui_handler_t on_cancel )
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
	NEW( m_random, util::Random );

	auto* ui = g_engine->GetUI();
	auto* game = g_engine->GetGame();
	auto* config = g_engine->GetConfig();

	if ( m_state->IsMaster() ) {
#ifdef DEBUG
		if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_MAP_FILE ) ) {
			m_state->m_settings.global.map.type = ::game::MapSettings::MT_MAPFILE;
			m_state->m_settings.global.map.filename = config->GetQuickstartMapFile();
		}
#endif

		if ( m_state->m_settings.global.map.type == ::game::MapSettings::MT_MAPFILE ) {
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
						*response.data.get_map_data->sprites.instances
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

	// poll game thread for frontend requests
	if ( m_mt_ids.get_frontend_requests ) {
		auto response = game->MT_GetResponse( m_mt_ids.get_frontend_requests );
		if ( response.result != ::game::R_NONE ) {
			ASSERT( response.result == ::game::R_SUCCESS, "unexpected frontend requests response" );
			m_mt_ids.get_frontend_requests = 0;
			const auto* requests = response.data.get_frontend_requests.requests;
			if ( requests ) {
				Log( "got " + std::to_string( requests->size() ) + " frontend requests" );

				for ( const auto& request : *requests ) {
					ProcessRequest( request );
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
			GetTileAtCoords( m_tile_at_query_purpose, tile_at.tile_pos );
		}

		const auto select_tile_mt_ids = m_mt_ids.select_tile; // copy for iteration
		for ( const auto& mt_id : select_tile_mt_ids ) {
			const auto tile_data = GetTileAtCoordsResult( mt_id );
			if ( tile_data.is_set ) {
				if ( m_is_map_editing_allowed && m_is_editing_mode ) {
					if ( !m_mt_ids.edit_map ) { // TODO: need to queue?
						m_mt_ids.edit_map = game->MT_EditMap( tile_data.tile_position, m_editor_tool, m_editor_brush, m_editor_draw_mode );
					}
					ASSERT( tile_data.purpose == ::game::TQP_TILE_SELECT, "only tile selections allowed in map editor" );
					SelectTileOrUnit( tile_data, 0 );
				}
				else {
					size_t selected_unit_id = 0;
					bool need_scroll = false;
					if ( tile_data.purpose == ::game::TQP_UNIT_SELECT ) {
						selected_unit_id = tile_data.metadata.unit_select.unit_id;
						need_scroll = !tile_data.metadata.unit_select.no_scroll_after;
					}
					else if ( tile_data.purpose == ::game::TQP_TILE_SELECT ) {
						need_scroll = !tile_data.metadata.tile_select.no_scroll_after;
					}
					SelectTileOrUnit( tile_data, selected_unit_id );
					if ( need_scroll ) {
						ScrollToTile( tile_data );
					}
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

		if ( m_selected_unit_state ) {
			m_selected_unit_state->Iterate();
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

types::Texture* Game::GetSourceTexture( const std::string& name ) {
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

InstancedSprite* Game::GetTerrainInstancedSprite( const ::game::map::Map::sprite_actor_t& actor ) {
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

void Game::CenterAtCoordinatePercents( const Vec2< float > position_percents ) {
	const Vec2< float > position = {
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

void Game::SetCameraPosition( const Vec3 camera_position ) {
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
	std::vector< Vec3 > instances;

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
	ASSERT( FS::FileExists( path ), "map file \"" + path + "\" not found" );

	auto* game = g_engine->GetGame();
	g_engine->GetUI()->ShowLoader(
		"Loading map", LCH( this ) {
			CancelRequests();
			return false;
		}
	);
	ASSERT( m_state, "state not set" );
	m_state->m_settings.global.map.type = ::game::MapSettings::MT_MAPFILE;
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

types::Texture* Game::GetTerrainTexture() const {
	return m_textures.terrain;
}

void Game::SetEditorTool( ::game::map_editor::MapEditor::tool_type_t editor_tool ) {
	ASSERT( m_is_map_editing_allowed, "map editing not allowed" );
	if ( m_editor_tool != editor_tool ) {
		m_editor_tool = editor_tool;
	}
}

const ::game::map_editor::MapEditor::tool_type_t Game::GetEditorTool() const {
	ASSERT( m_is_map_editing_allowed, "map editing not allowed" );
	return m_editor_tool;
}

void Game::SetEditorBrush( ::game::map_editor::MapEditor::brush_type_t editor_brush ) {
	ASSERT( m_is_map_editing_allowed, "map editing not allowed" );
	if ( m_editor_brush != editor_brush ) {
		m_editor_brush = editor_brush;
	}
}

const ::game::map_editor::MapEditor::brush_type_t Game::GetEditorBrush() const {
	ASSERT( m_is_map_editing_allowed, "map editing not allowed" );
	return m_editor_brush;
}

void Game::DefineSlot(
	const size_t slot_index,
	const types::Color& color,
	const bool is_progenitor
) {
	if ( m_slot_states.find( slot_index ) == m_slot_states.end() ) {
		Log( "Initializing slot state: " + std::to_string( slot_index ) );
		m_slot_states.insert(
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

void Game::DefineUnit( const ::game::unit::Def* def ) {
	auto unitdef_it = m_unitdef_states.find( def->m_id );
	ASSERT( unitdef_it == m_unitdef_states.end(), "unit def already exists" );

	Log( "Initializing unitdef state: " + def->m_id );

	m_unitdef_states.insert(
		{
			def->m_id,
			new UnitDef(
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
	const Vec2< size_t >& tile_coords,
	const Vec3& render_coords,
	const ::game::unit::Unit::movement_t movement,
	const ::game::unit::Unit::morale_t morale,
	const ::game::unit::Unit::health_t health
) {

	ASSERT( m_unitdef_states.find( unitdef_id ) != m_unitdef_states.end(), "unitdef not found" );
	ASSERT( m_slot_states.find( slot_index ) != m_slot_states.end(), "slot not found" );
	ASSERT( m_unit_states.find( unit_id ) == m_unit_states.end(), "unit id already exists" );

	auto* unitdef = m_unitdef_states.at( unitdef_id );
	auto* slot_state = m_slot_states.at( slot_index );
	auto* tile = GetTileState( tile_coords.x, tile_coords.y );

	auto* unit_state = m_unit_states.insert(
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
				health
			)
		}
	).first->second;

	tile->Render(
		m_selected_unit_state
			? m_selected_unit_state->GetId()
			: 0
	);

	if ( unit_state->IsActive() ) {
		const bool was_selectables_empty = m_selectables.unit_states.empty();
		AddSelectable( unit_state );
		if ( was_selectables_empty ) {
			SelectNextUnitMaybe();
		}
	}
}

void Game::DespawnUnit( const size_t unit_id ) {
	const auto& it = m_unit_states.find( unit_id );
	ASSERT( it != m_unit_states.end(), "unit id not found" );

	auto* unit_state = it->second;

	if ( unit_state->IsActive() ) {
		RemoveSelectable( unit_state );
	}

	auto* tile = unit_state->GetTile();

	delete unit_state;
	m_unit_states.erase( it );

	tile->Render(
		m_selected_unit_state
			? m_selected_unit_state->GetId()
			: 0
	);
}

void Game::RefreshUnit( Unit* unit_state ) {
	const auto was_active = unit_state->IsActive();
	unit_state->Refresh();
	if ( m_selected_unit_state == unit_state && was_active ) {
		if ( !unit_state->IsActive() ) {
			UpdateSelectable( unit_state );
			if ( m_selected_tile_data.purpose == ::game::TQP_UNIT_SELECT ) {
				// update next tile
				::game::tile_query_metadata_t metadata = {};
				metadata.tile_select.try_next_unit = true; // try next unit after updating tile
				GetTileAtCoords(
					::game::TQP_TILE_SELECT,
					unit_state->GetTile()->GetCoords(),
					::game::map::Tile::D_NONE,
					metadata
				);
			}
			m_selected_unit_state = nullptr;
		}
		else {
			// keep same unit selected
			::game::tile_query_metadata_t metadata = {};
			metadata.unit_select.unit_id = unit_state->GetId();
			GetTileAtCoords(
				::game::TQP_UNIT_SELECT,
				unit_state->GetTile()->GetCoords(),
				::game::map::Tile::D_NONE,
				metadata
			);
//			m_selected_unit_state = nullptr;
		}
	}
}

void Game::MoveUnit( Unit* unit, Tile* dst_tile, const types::Vec3& dst_render_coords ) {

	auto* src_tile = unit->GetTile();

	unit->MoveTo( dst_tile, dst_render_coords );

	// TODO: animation

	// update old tile
	src_tile->Render(
		m_selected_unit_state
			? m_selected_unit_state->GetId()
			: 0
	);

	// update unit
	RefreshUnit( unit );

	// update new tile
	dst_tile->Render(
		m_selected_unit_state
			? m_selected_unit_state->GetId()
			: 0
	);

	// update ui
	// TODO: refactor and get rid of m_selected_tile_data?
	if (
		unit != m_selected_unit_state &&
			m_selected_tile_data.is_set && (
			m_selected_tile_data.tile_position == src_tile->GetCoords() ||
				m_selected_tile_data.tile_position == dst_tile->GetCoords()
		)
		) {
		::game::tile_query_metadata_t metadata = {};
		switch ( m_selected_tile_data.purpose ) {
			case ::game::TQP_TILE_SELECT: {
				metadata.tile_select.no_scroll_after = true;
				break;
			}
			case ::game::TQP_UNIT_SELECT: {
				metadata.unit_select.no_scroll_after = true;
				break;
			}
			default: {
				//
			}
		}

		GetTileAtCoords(
			m_selected_tile_data.purpose,
			m_selected_tile_data.tile_position,
			::game::map::Tile::D_NONE,
			metadata
		);
	}

}

void Game::ProcessRequest( const ::game::FrontendRequest& request ) {
	//Log( "Received frontend request (type=" + std::to_string( request.type ) + ")" ); // spammy
	const auto f_exit = [ this, &request ]() -> void {
		ExitGame(
			[ this, request ]() -> void {
#ifdef DEBUG
				if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
					g_engine->ShutDown();
				}
				else
#endif
				{
					ReturnToMainMenu(
						request.data.quit.reason
							? *request.data.quit.reason
							: ""
					);
				}
			}
		);
	};
	switch ( request.type ) {
		case ::game::FrontendRequest::FR_QUIT: {
			f_exit();
			break;
		}
		case ::game::FrontendRequest::FR_ERROR: {
			Log( *request.data.error.stacktrace );
			g_engine->GetUI()->ShowError(
				*request.data.error.what, UH( f_exit ) {
					f_exit();
				}
			);
		}
		case ::game::FrontendRequest::FR_GLOBAL_MESSAGE: {
			AddMessage( *request.data.global_message.message );
			break;
		}
		case ::game::FrontendRequest::FR_TURN_ACTIVE_STATUS: {
			const auto& d = request.data.turn_active_status;
			if ( m_is_turn_active != d.is_turn_active ) {
				m_is_turn_active = d.is_turn_active;
				ASSERT( m_ui.bottom_bar, "bottom bar not initialized" );
				m_ui.bottom_bar->SetTurnActiveStatus( d.is_turn_active );
				if ( m_is_turn_active ) {
					m_selected_unit_state = nullptr;
					SelectNextUnitMaybe();
				}
				else {
					if ( m_selected_unit_state ) {
						DeselectTileOrUnit();
						GetTileAtCoords( ::game::TQP_TILE_SELECT, m_selected_tile_data.tile_position );
						m_selected_unit_state = nullptr;
					}
				}
			}
			break;
		}
		case ::game::FrontendRequest::FR_TURN_COMPLETE_STATUS: {
			ASSERT( m_ui.bottom_bar, "bottom bar not initialized" );
			const auto& d = request.data.turn_complete_status;
			m_ui.bottom_bar->SetTurnCompleteStatus( d.is_turn_complete, d.play_sound );
			break;
		}
		case ::game::FrontendRequest::FR_SLOT_DEFINE: {
			for ( const auto& d : *request.data.slot_define.slotdefs ) {
				const auto& c = d.color;
				DefineSlot( d.slot_index, types::Color( c.r, c.g, c.b, c.a ), d.is_progenitor );
			}
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_DEFINE: {
			types::Buffer buf( *request.data.unit_define.serialized_unitdef );
			const auto* unitdef = ::game::unit::Def::Unserialize( buf );
			DefineUnit( unitdef );
			delete unitdef;
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_SPAWN: {
			const auto& d = request.data.unit_spawn;
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
				d.health
			);
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_DESPAWN: {
			DespawnUnit( request.data.unit_despawn.unit_id );
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_REFRESH: {
			const auto& d = request.data.unit_refresh;
			auto* unit_state = m_unit_states.at( d.unit_id );
			unit_state->SetMovement( d.movement_left );
			RefreshUnit( unit_state );
			unit_state->GetTile()->Render(
				m_selected_unit_state
					? m_selected_unit_state->GetId()
					: 0
			);
			if ( m_selected_unit_state == unit_state && !unit_state->IsActive() ) {
				RemoveSelectable( unit_state );
				SelectNextUnitOrSwitchToTileSelection();
			}
			break;
		}
		case ::game::FrontendRequest::FR_UNIT_MOVE: {
			const auto& d = request.data.unit_move;
			ASSERT( m_unit_states.find( d.unit_id ) != m_unit_states.end(), "unit id not found" );
			auto* unit_state = m_unit_states.at( d.unit_id );
			auto* tile_state = GetTileState( d.tile_coords.x, d.tile_coords.y );
			const auto& rc = d.render_coords;
			unit_state->SetMovement( d.movement_left );
			MoveUnit(
				unit_state, tile_state, {
					rc.x,
					rc.y,
					rc.z
				}
			);
			break;
		}
		default: {
			THROW( "unexpected frontend request type: " + std::to_string( request.type ) );
		}
	}
}

void Game::ActivateTurn() {

}

void Game::DeactivateTurn() {

}

void Game::UpdateMapData( const types::Vec2< size_t >& map_size ) {

	ASSERT( m_tile_states.empty(), "tile states already set" );

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

	m_tile_states.reserve( m_map_data.width * m_map_data.height / 2 ); // / 2 because smac coordinate system
	for ( size_t y = 0 ; y < m_map_data.height ; y++ ) {
		for ( size_t x = y & 1 ; x < m_map_data.width ; x += 2 ) {
			m_tile_states.insert(
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

}

void Game::Initialize(
	types::Texture* terrain_texture,
	types::mesh::Render* terrain_mesh,
	types::mesh::Data* terrain_data_mesh,
	const std::unordered_map< std::string, ::game::map::Map::sprite_actor_t >& sprite_actors,
	const std::unordered_map< size_t, std::pair< std::string, Vec3 > >& sprite_instances
) {
	ASSERT( !m_is_initialized, "already initialized" );

	auto* ui = g_engine->GetUI();

	if ( ui->HasPopup() ) {
		ui->CloseLastPopup( true );
	}

	Log( "Initializing game" );

	NEW( m_world_scene, Scene, "Game", SCENE_TYPE_ORTHO );
	NEW( m_ism, InstancedSpriteManager, m_world_scene );
	NEW( m_badge_defs, BadgeDefs, m_ism );

	NEW( m_camera, Camera, Camera::CT_ORTHOGRAPHIC );
	m_camera_angle = INITIAL_CAMERA_ANGLE;
	UpdateCameraAngle();

	m_world_scene->SetCamera( m_camera );

	// don't set exact 45 degree angles for lights, it will produce weird straight lines because of shadows
	{
		NEW( m_light_a, Light, Light::LT_AMBIENT_DIFFUSE );
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
		NEW( m_light_b, Light, Light::LT_AMBIENT_DIFFUSE );
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

	// UI
	ui->AddTheme( &m_ui.theme );
	NEW( m_ui.bottom_bar, ui::BottomBar, this );
	ui->AddObject( m_ui.bottom_bar );

	m_viewport.bottom_bar_overlap = 32; // it has transparent area on top so let map render through it

	// map event handlers

	auto* game = g_engine->GetGame();

	m_handlers.keydown_before = ui->AddGlobalEventHandler(
		UIEvent::EV_KEY_DOWN, EH( this, ui ) {
			if (
				ui->HasPopup() &&
					!data->key.modifiers &&
					data->key.code == UIEvent::K_ESCAPE
				) {
				ui->CloseLastPopup();
				return true;
			}
			return false;
		}, UI::GH_BEFORE
	);

	m_handlers.keydown_after = ui->AddGlobalEventHandler(
		UIEvent::EV_KEY_DOWN, EH( this, ui, game ) {

			if ( ui->HasPopup() ) {
				return false;
			}

			m_ui.bottom_bar->CloseMenus();

			if ( !data->key.modifiers ) {
				if ( m_selected_tile_data.is_set ) {

					bool is_tile_selected = false;
					::game::map::Tile::direction_t td = ::game::map::Tile::D_NONE;

					switch ( data->key.code ) {
#define X( _key, _altkey, _direction ) \
                        case UIEvent::_key: \
                        case UIEvent::_altkey: { \
                            td = ::game::map::Tile::_direction; \
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
						case UIEvent::K_TAB: {
							SelectNextUnitMaybe();
							break;
						}
						case UIEvent::K_SPACE: {
							if ( m_selected_unit_state ) {
								const auto event = ::game::event::SkipUnitTurn( m_slot_index, m_selected_unit_state->GetId() );
								game->MT_AddEvent( &event );
							}
							break;
						}
						default: {
							// nothing
						}
					}

					if ( is_tile_selected ) {
						switch ( m_selected_tile_data.purpose ) {
							case ::game::TQP_TILE_SELECT: {
								// move tile selector
								GetTileAtCoords( ::game::TQP_TILE_SELECT, m_selected_tile_data.tile_position, td );
								return true;
							}
							case ::game::TQP_UNIT_SELECT: {
								// try moving unit to tile
								if ( m_selected_unit_state ) {
									const auto event = ::game::event::MoveUnit( m_slot_index, m_selected_unit_state->GetId(), td );
									game->MT_AddEvent( &event );
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

				if ( data->key.code == UIEvent::K_ESCAPE ) {
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
			else if ( m_is_map_editing_allowed && data->key.code == UIEvent::K_CTRL ) {
				m_is_editing_mode = true;
			}

			return false;
		}, UI::GH_AFTER
	);

	m_handlers.keyup = ui->AddGlobalEventHandler(
		UIEvent::EV_KEY_UP, EH( this ) {
			if ( data->key.key == 'z' || data->key.key == 'x' ) {
				if ( m_map_control.key_zooming ) {
					m_map_control.key_zooming = 0;
					m_scroller.Stop();
				}
			}
			else if ( m_is_map_editing_allowed && data->key.code == UIEvent::K_CTRL ) {
				m_is_editing_mode = false;
				m_editing_draw_timer.Stop();
				m_editor_draw_mode = ::game::map_editor::MapEditor::DM_NONE;
			}
			return false;
		}, UI::GH_BEFORE
	);

	m_handlers.mousedown = ui->AddGlobalEventHandler(
		UIEvent::EV_MOUSE_DOWN, EH( this, ui ) {
			if ( ui->HasPopup() ) {
				return false;
			}

			m_ui.bottom_bar->CloseMenus();

			if ( m_is_map_editing_allowed && m_is_editing_mode ) {
				switch ( data->mouse.button ) {
					case UIEvent::M_LEFT: {
						m_editor_draw_mode = ::game::map_editor::MapEditor::DM_INC;
						break;
					}
					case UIEvent::M_RIGHT: {
						m_editor_draw_mode = ::game::map_editor::MapEditor::DM_DEC;
						break;
					}
					default: {
						m_editor_draw_mode = ::game::map_editor::MapEditor::DM_NONE;
					}

				}
				SelectTileAtPoint( ::game::TQP_TILE_SELECT, data->mouse.absolute.x, data->mouse.absolute.y ); // async
				m_editing_draw_timer.SetInterval( Game::s_consts.map_editing.draw_frequency_ms ); // keep drawing until mouseup
			}
			else {
				switch ( data->mouse.button ) {
					case UIEvent::M_LEFT: {
						SelectTileAtPoint(
							::game::TQP_UNIT_SELECT,
							data->mouse.absolute.x,
							data->mouse.absolute.y
						); // async
						break;
					}
					case UIEvent::M_RIGHT: {
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
		}, UI::GH_AFTER
	);

	m_handlers.mousemove = ui->AddGlobalEventHandler(
		UIEvent::EV_MOUSE_MOVE, EH( this, ui ) {
			m_map_control.last_mouse_position = {
				GetFixedX( data->mouse.absolute.x ),
				(float)data->mouse.absolute.y
			};

			if ( ui->HasPopup() ) {
				return false;
			}

			if ( m_map_control.is_dragging ) {

				Vec2< float > current_drag_position = {
					m_clamp.x.Clamp( data->mouse.absolute.x ),
					m_clamp.y.Clamp( data->mouse.absolute.y )
				};
				Vec2< float > drag = current_drag_position - m_map_control.last_drag_position;

				m_camera_position.x += (float)drag.x;
				m_camera_position.y += (float)drag.y;
				UpdateCameraPosition();

				m_map_control.last_drag_position = current_drag_position;
			}
			else if ( !m_ui.bottom_bar->IsMouseDraggingMiniMap() ) {
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
		}, UI::GH_AFTER
	);

	m_handlers.mouseup = ui->AddGlobalEventHandler(
		UIEvent::EV_MOUSE_UP, EH( this ) {
			switch ( data->mouse.button ) {
				case UIEvent::M_RIGHT: {
					m_map_control.is_dragging = false;
					break;
				}
			}
			if ( m_is_map_editing_allowed && m_is_editing_mode ) {
				m_editing_draw_timer.Stop();
				m_editor_draw_mode = ::game::map_editor::MapEditor::DM_NONE;
			}
			return true;
		}, UI::GH_AFTER
	);

	m_handlers.mousescroll = ui->AddGlobalEventHandler(
		UIEvent::EV_MOUSE_SCROLL, EH( this, ui ) {
			if ( ui->HasPopup() ) {
				return false;
			}

			SmoothScroll( m_map_control.last_mouse_position, data->mouse.scroll_y );
			return true;
		}, UI::GH_AFTER
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
		ui->RemoveTheme( &m_ui.theme );
		m_ui.bottom_bar = nullptr;
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

	for ( const auto& it : m_unit_states ) {
		delete it.second;
	}
	m_unit_states.clear();

	for ( const auto& it : m_unitdef_states ) {
		delete it.second;
	}
	m_unitdef_states.clear();

	for ( const auto& it : m_slot_states ) {
		delete it.second;
	}
	m_slot_states.clear();

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
	const auto event = ::game::event::CompleteTurn( m_slot_index );
	g_engine->GetGame()->MT_AddEvent( &event );
}

void Game::UncompleteTurn() {
	const auto event = ::game::event::UncompleteTurn( m_slot_index );
	g_engine->GetGame()->MT_AddEvent( &event );
}

void Game::SelectTileAtPoint( const ::game::tile_query_purpose_t tile_query_purpose, const size_t x, const size_t y ) {
	//Log( "Looking up tile at " + std::to_string( x ) + "x" + std::to_string( y ) );
	GetTileAtScreenCoords( tile_query_purpose, x, m_viewport.window_height - y ); // async
}

void Game::SelectUnit( const unit_data_t& unit_data, const bool actually_select_unit ) {
	ASSERT( m_unit_states.find( unit_data.id ) != m_unit_states.end(), "unit id not found" );
	auto* unit_state = m_unit_states.at( unit_data.id );
	if ( m_selected_unit_state != unit_state ) {
		if ( m_selected_unit_state ) {
			m_selected_unit_state->Hide();
			m_selected_unit_state = nullptr;
		}

		unit_state->SetActiveOnTile();

		m_selected_unit_state = unit_state;
		m_selected_unit_state->Show();
	}
	if ( actually_select_unit && m_selected_unit_state->IsActive() && m_is_turn_active ) {

		if ( m_selected_unit_state->GetId() != m_selected_tile_data.units.front().id ) {
			m_unit_states.at( m_selected_tile_data.units.front().id )->Hide();
		}

		m_selected_unit_state->StartBadgeBlink();
		SetCurrentSelectable( m_selected_unit_state );
		Log( "Selected unit " + std::to_string( m_selected_unit_state->GetId() ) );
	}
}

void Game::SelectTileOrUnit( const tile_data_t& tile_data, const size_t selected_unit_id ) {

	ASSERT( tile_data.purpose != ::game::TQP_NONE, "tile query purpose not set" );

	DeselectTileOrUnit();

	m_selected_tile_data = tile_data;

	const unit_data_t* selected_unit = nullptr;
	if ( m_selected_tile_data.purpose == ::game::TQP_UNIT_SELECT ) {
		if ( m_is_turn_active ) {
			const auto& md = m_selected_tile_data.metadata.unit_select;
			if ( md.unit_id ) {
				for ( const auto& unit : m_selected_tile_data.units ) {
					if ( unit.id == md.unit_id ) {
						selected_unit = &unit;
						break;
					}
				}
			}
			if ( !selected_unit ) {
				selected_unit = GetFirstSelectableUnit( m_selected_tile_data.units );
			}
		}
		if ( !selected_unit ) {
			m_selected_tile_data.purpose = ::game::TQP_TILE_SELECT;
		}
	}

	m_ui.bottom_bar->PreviewTile( m_selected_tile_data, selected_unit_id );

	switch ( m_selected_tile_data.purpose ) {
		case ::game::TQP_TILE_SELECT: {
			m_selected_unit_state = nullptr;
			Log( "Selected tile at " + m_selected_tile_data.tile_position.ToString() + " ( " + m_selected_tile_data.selection_coords.center.ToString() + " )" );
			if ( m_selected_tile_data.metadata.tile_select.try_next_unit && SelectNextUnitMaybe() ) {
				// no need for tile selector because we selected next unit
				break;
			}
			else {
				// show tile selector
				NEW( m_actors.tile_selection, actor::TileSelection, m_selected_tile_data.selection_coords );
				AddActor( m_actors.tile_selection );
			}
			break;
		}
		case ::game::TQP_UNIT_SELECT: {
			SelectUnit( *selected_unit, true );
			break;
		}
		default:
			THROW( "unknown selection mode: " + std::to_string( m_selected_tile_data.purpose ) );
	}

}

void Game::DeselectTileOrUnit() {

	if ( m_actors.tile_selection ) {
		RemoveActor( m_actors.tile_selection );
		m_actors.tile_selection = nullptr;
	}
	if ( m_selected_unit_state ) {

		ASSERT( !m_selected_tile_data.units.empty(), "unit was selected but tile data has no units" );

		// reset to most important unit if needed
		size_t unit_id = m_selected_tile_data.units.front().id;
		ASSERT( m_unit_states.find( unit_id ) != m_unit_states.end(), "unit id not found" );

		auto* most_important_unit = m_unit_states.at( unit_id );

		m_selected_unit_state->StopBadgeBlink( m_selected_unit_state == most_important_unit );
		if ( m_selected_unit_state != most_important_unit ) {
			m_selected_unit_state->Hide();
			most_important_unit->Show();
		}
		m_selected_unit_state = nullptr;
	}

	m_ui.bottom_bar->HideTilePreview();
}

const unit_data_t* Game::GetFirstSelectableUnit( const std::vector< unit_data_t >& units ) const {
	for ( const auto& unit : units ) {
		ASSERT( m_unit_states.find( unit.id ) != m_unit_states.end(), "unit id not found" );
		const auto* unit_state = m_unit_states.at( unit.id );
		if ( unit_state->IsActive() ) {
			return &unit;
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

const Vec2< float > Game::GetTileWindowCoordinates( const Vec3& tile_coords ) {
	return {
		tile_coords.x * m_viewport.window_aspect_ratio * m_camera_position.z,
		( tile_coords.y - std::max( 0.0f, tile_coords.z ) ) * m_viewport.ratio.y * m_camera_position.z / 1.414f
	};
}

void Game::ScrollTo( const Vec3& target ) {
	if ( m_scroller.IsRunning() && m_selected_tile_data.scroll_adaptively ) {
		const auto& target = m_scroller.GetTargetPosition();
		if ( target.z == m_camera_position.z ) {
			m_camera_position = m_scroller.GetTargetPosition();
			UpdateCameraPosition();
		}
	}
	m_scroller.Scroll( m_camera_position, target );
}

void Game::ScrollToTile( const tile_data_t& tile_data ) {

	if ( tile_data.scroll_adaptively ) {

		const auto tc = GetTileWindowCoordinates( tile_data.coords );

		Vec2< float > uc = {
			GetFixedX( tc.x + m_camera_position.x ),
			tc.y + m_camera_position.y
		};

		Vec2< float > scroll_by = {
			0,
			0
		};

		//Log( "Resolved tile coordinates to " + uc.ToString() + " ( camera: " + m_camera_position.ToString() + " )" );

		// tile size
		Vec2< float > ts = {
			::game::map::s_consts.tile.scale.x * m_camera_position.z,
			::game::map::s_consts.tile.scale.y * m_camera_position.z
		};
		// edge size
		Vec2< float > es = {
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
	else {

		Vec2< float > tc = {
			tile_data.coords.x * m_viewport.window_aspect_ratio * m_camera_position.z,
			( tile_data.coords.y - std::max( 0.0f, tile_data.coords.z ) ) * m_viewport.ratio.y * m_camera_position.z / 1.414f
		};

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

void Game::GetTileAtCoords(
	const ::game::tile_query_purpose_t tile_query_purpose,
	const Vec2< size_t >& tile_pos,
	const ::game::map::Tile::direction_t tile_direction,
	const ::game::tile_query_metadata_t& tile_query_metadata
) {
	auto* game = g_engine->GetGame();
	ASSERT( tile_query_purpose != ::game::TQP_NONE, "tile query purpose not set" );
	m_mt_ids.select_tile.insert( game->MT_SelectTile( tile_query_purpose, tile_pos, tile_direction, tile_query_metadata ) );
}

tile_data_t Game::GetTileAtCoordsResult( const mt_id_t mt_id ) {
	ASSERT( m_mt_ids.select_tile.find( mt_id ) != m_mt_ids.select_tile.end(), "select_tile mt_id not found" );
	auto* game = g_engine->GetGame();
	auto response = game->MT_GetResponse( mt_id );
	if ( response.result != ::game::R_NONE ) {
		m_mt_ids.select_tile.erase( mt_id );

		tile_data_t result = {};
		result.is_set = true;
		result.purpose = response.data.select_tile.purpose;
		result.tile_position = {
			response.data.select_tile.tile_x,
			response.data.select_tile.tile_y
		};
		result.coords = {
			response.data.select_tile.coords.x,
			response.data.select_tile.coords.y,
			::game::map::s_consts.tile.elevation_to_vertex_z.Clamp( response.data.select_tile.elevation.center )
		};

#define xx( _k, _kk ) result.selection_coords._k._kk = response.data.select_tile.selection_coords._k._kk
#define x( _k ) { \
            xx( _k, x ); \
            xx( _k, y ); \
            xx( _k, z ); \
        }
		x( center );
		x( left );
		x( top );
		x( right );
		x( bottom );
#undef x
#undef xx

		ASSERT( response.data.select_tile.preview_meshes, "preview meshes not set" );
		result.preview_meshes = *response.data.select_tile.preview_meshes;
		// needed to avoid mesh deallocations
		DELETE( response.data.select_tile.preview_meshes );
		response.data.select_tile.preview_meshes = nullptr;

		ASSERT( response.data.select_tile.preview_lines, "preview lines not set" );
		result.preview_lines = *response.data.select_tile.preview_lines;

		result.sprites = *response.data.select_tile.sprites;

		result.scroll_adaptively = response.data.select_tile.scroll_adaptively;

		std::unordered_map< size_t, Unit* > units = {};
		for ( const auto& unit_id : *response.data.select_tile.unit_ids ) {
			const auto& it = m_unit_states.find( unit_id );
			if ( m_unit_states.find( unit_id ) != m_unit_states.end() ) {
				units.insert(
					{
						unit_id,
						it->second
					}
				);
			}
		}
		const auto units_order = Tile::GetUnitsOrder( units );

		for ( const auto& unit_id : units_order ) {
			ASSERT( m_unit_states.find( unit_id ) != m_unit_states.end(), "unit id not found" );
			const auto* unit_state = m_unit_states.at( unit_id );

			types::mesh::Rectangle* mesh = nullptr;
			types::Texture* texture = nullptr;

			const auto& f_meshtex = []( const InstancedSprite* sprite ) -> meshtex_t {
				auto* texture = sprite->actor->GetSpriteActor()->GetTexture();
				NEWV( mesh, types::mesh::Rectangle );
				mesh->SetCoords(
					{
						0.0f,
						0.0f
					},
					{
						1.0f,
						1.0f
					},
					{
						sprite->xy.x,
						sprite->xy.y
					},
					{
						sprite->xy.x + sprite->wh.x,
						sprite->xy.y + sprite->wh.y
					},
					{
						texture->m_width,
						texture->m_height
					},
					0.8f
				);
				return {
					mesh,
					texture,
				};
			};

			// TODO: use real unit properties

			result.units.push_back(
				{
					unit_state->GetId(),
					f_meshtex( unit_state->GetSprite()->instanced_sprite ),
					f_meshtex( unit_state->GetBadgeSprite()->instanced_sprite ),
					f_meshtex( unit_state->GetBadgeHealthbarSprite()->instanced_sprite ),
					unit_state->GetNameString(),
					unit_state->GetStatsString(),
					unit_state->GetMoraleString(),
					unit_state->GetMovesString(),
				}
			);
		}
		result.metadata = response.data.select_tile.metadata;

		return result;
	}
	// no data
	return {};
}

void Game::GetMinimapTexture( scene::Camera* camera, const Vec2< size_t > texture_dimensions ) {
	if ( m_minimap_texture_request_id ) {
		Log( "Canceling minimap texture request" );
		m_actors.terrain->GetMeshActor()->CancelDataRequest( m_minimap_texture_request_id );
	}
	Log( "Requesting minimap texture" );
	m_minimap_texture_request_id = m_actors.terrain->GetMeshActor()->CaptureToTexture( camera, texture_dimensions );
}

Texture* Game::GetMinimapTextureResult() {
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
	Vec2< float > scale = {
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
	Vec2< size_t > coords = {
		m_map_data.width / 2,
		m_map_data.height / 2
	};
	if ( ( coords.y % 2 ) != ( coords.x % 2 ) ) {
		coords.y++;
	}

	GetTileAtCoords(
		::game::TQP_TILE_SELECT,
		{
			coords.x,
			coords.y
		}
	);
}

void Game::SmoothScroll( const float scroll_value ) {
	SmoothScroll(
		{
			(float)m_viewport.width / 2,
			(float)m_viewport.height / 2
		}, scroll_value
	);
}

void Game::SmoothScroll( const Vec2< float > position, const float scroll_value ) {

	float speed = Game::s_consts.map_scroll.smooth_scrolling.zoom_speed * m_camera_position.z;

	float new_z = m_camera_position.z + scroll_value * speed;

	if ( new_z < m_camera_range.min.z ) {
		new_z = m_camera_range.min.z;
	}
	if ( new_z > m_camera_range.max.z ) {
		new_z = m_camera_range.max.z;
	}

	float diff = m_camera_position.z / new_z;

	Vec2< float > m = {
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

util::Random* Game::GetRandom() const {
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

void Game::AddSelectable( Unit* unit_state ) {
	const auto& it = std::find( m_selectables.unit_states.begin(), m_selectables.unit_states.end(), unit_state );
	if ( it == m_selectables.unit_states.end() ) {
		if ( m_selectables.unit_states.empty() ) {
			m_selectables.selected_id_index = 0;
		}
		m_selectables.unit_states.push_back( unit_state );
	}
}

void Game::RemoveSelectable( Unit* unit_state ) {
	const auto& it = std::find( m_selectables.unit_states.begin(), m_selectables.unit_states.end(), unit_state );
	if ( it != m_selectables.unit_states.end() ) {
		const size_t removed_index = it - m_selectables.unit_states.begin();
		m_selectables.unit_states.erase( it );
		if ( m_selectables.selected_id_index > 0 && m_selectables.selected_id_index >= removed_index ) {
			m_selectables.selected_id_index--;
		}
	}
}

void Game::UpdateSelectable( Unit* unit_state ) {
	if ( unit_state->IsOwned() ) {
		if ( unit_state->IsActive() ) {
			AddSelectable( unit_state );
		}
		else {
			RemoveSelectable( unit_state );
		}
	}
}

void Game::SetCurrentSelectable( Unit* unit_state ) {
	const auto& it = std::find( m_selectables.unit_states.begin(), m_selectables.unit_states.end(), unit_state );
	ASSERT( it != m_selectables.unit_states.end(), "selectable not found" );
	m_selectables.selected_id_index = it - m_selectables.unit_states.begin();
}

Unit* Game::GetCurrentSelectable() {
	if ( !m_selectables.unit_states.empty() ) {
		return m_selectables.unit_states.at( m_selectables.selected_id_index );
	}
	return nullptr;
}

Unit* Game::GetNextSelectable() {
	if ( !m_selectables.unit_states.empty() ) {
		m_selectables.selected_id_index++;
		if ( m_selectables.selected_id_index >= m_selectables.unit_states.size() ) {
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
	auto* selected_unit = m_selected_unit_state
		? GetNextSelectable()
		: GetCurrentSelectable();
	if ( selected_unit ) {
		Log( "Tab-selecting unit " + std::to_string( selected_unit->GetId() ) );
		::game::tile_query_metadata_t metadata = {};
		metadata.unit_select.unit_id = selected_unit->GetId();
		GetTileAtCoords(
			::game::TQP_UNIT_SELECT,
			selected_unit->GetTile()->GetCoords(),
			::game::map::Tile::D_NONE,
			metadata
		);
		return true;
	}
	return false;
}

void Game::SelectNextUnitOrSwitchToTileSelection() {
	if ( !SelectNextUnitMaybe() ) {
		const auto& coords = m_selected_unit_state->GetTile()->GetCoords();
		DeselectTileOrUnit();
		GetTileAtCoords( ::game::TQP_TILE_SELECT, coords );
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

Tile* Game::GetTileState( const size_t x, const size_t y ) {
	ASSERT( !m_tile_states.empty(), "tile states not set" );
	ASSERT( x < m_map_data.width, "tile x overflow" );
	ASSERT( y < m_map_data.height, "tile y overflow" );
	return &m_tile_states.at( y * m_map_data.width + x );
}

}
}
