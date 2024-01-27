#include "Game.h"

#include "engine/Engine.h"
#include "../mainmenu/MainMenu.h"
#include "graphics/Graphics.h"
#include "util/FS.h"
#include "ui/popup/PleaseDontGo.h"

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

	if ( !m_textures.source.ter1_pcx ) {
		m_textures.source.ter1_pcx = g_engine->GetTextureLoader()->LoadTextureTCs(
			"ter1.pcx", {
				Color::RGB( 152, 24, 228 ), // remove transparency color
				Color::RGB( 100, 16, 156 ), // remove second transparency color
				Color::RGB( 24, 184, 228 ), // remove frame
				Color::RGB( 253, 189, 118 ) // remove drawn shadows too (we'll have our own)
			}
		);
	}

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
						const auto& actor = a.second;
						GetTerrainSpriteActor( actor.name, actor.tex_coords, actor.z_index );
					}
				}

				if ( response.data.edit_map.sprites.instances_to_remove ) {
					Log( "Need to remove " + std::to_string( response.data.edit_map.sprites.instances_to_remove->size() ) + " instances" );
					for ( auto& i : *response.data.edit_map.sprites.instances_to_remove ) {
						auto* actor = GetTerrainSpriteActorByKey( i.second );
						ASSERT( actor, "sprite actor not found" );
						ASSERT( actor->HasInstance( i.first ), "actor instance not found" );
						actor->RemoveInstance( i.first );
					}
				}

				if ( response.data.edit_map.sprites.instances_to_add ) {
					Log( "Need to add " + std::to_string( response.data.edit_map.sprites.instances_to_add->size() ) + " instances" );
					for ( auto& i : *response.data.edit_map.sprites.instances_to_add ) {
						const auto& instance = i.second;
						auto* actor = GetTerrainSpriteActorByKey( instance.first );
						ASSERT( actor, "sprite actor not found" );
						ASSERT( !actor->HasInstance( i.first ), "actor instance already exists" );
						actor->SetInstance( i.first, instance.second );
					}
				}

				/*
				result.sprites_to_add = *response.data.select_tile.sprites_to_add;
				result.instances_to_remove = *response.data.select_tile.instances_to_remove;
				result.instances_to_add = *response.data.select_tile.instances_to_add;
				*/
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

	// poll game thread for events
	if ( m_mt_ids.get_events ) {
		auto response = game->MT_GetResponse( m_mt_ids.get_events );
		if ( response.result != ::game::R_NONE ) {
			ASSERT( response.result == ::game::R_SUCCESS, "unexpected game events response" );
			m_mt_ids.get_events = 0;
			const auto* events = response.data.get_events.events;
			if ( events ) {
				Log( "got " + std::to_string( events->size() ) + " game events" );

				for ( const auto& event : *events ) {
					ProcessEvent( event );
					if ( m_on_game_exit ) {
						break; // exiting game
					}
				}
			}
			game->MT_DestroyResponse( response );
			if ( !m_on_game_exit ) {
				m_mt_ids.get_events = game->MT_GetEvents();
			}
		}
	}
	else {
		m_mt_ids.get_events = game->MT_GetEvents();
	}

	if ( m_is_initialized ) {

		if ( m_is_map_editing_allowed && m_editing_draw_timer.HasTicked() ) {
			if ( m_is_editing_mode && !IsTileAtRequestPending() ) {
				SelectTileAtPoint( m_map_control.last_mouse_position.x, m_map_control.last_mouse_position.y ); // async
			}
		}

		// response for clicked tile (if click happened)
		const auto tile_at = GetTileAtScreenCoordsResult();
		if ( tile_at.is_set ) {
			GetTileAtCoords( tile_at.tile_pos );
		}

		const auto tile_data = GetTileAtCoordsResult();
		if ( tile_data.is_set ) {
			if ( m_is_map_editing_allowed && m_is_editing_mode ) {
				if ( !m_mt_ids.edit_map ) { // TODO: need to queue?
					m_mt_ids.edit_map = game->MT_EditMap( tile_data.tile_position, m_editor_tool, m_editor_brush, m_editor_draw_mode );
				}
				SelectTile( tile_data );
			}
			else {
				SelectTile( tile_data );
				ScrollToTile( tile_data );
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

scene::actor::Instanced* Game::GetTerrainSpriteActor( const std::string& name, const ::game::map::Consts::pcx_texture_coordinates_t& tex_coords, const float z_index ) {

	const auto key = name + " " + tex_coords.ToString();

	const auto& it = m_instanced_sprites.find( key );
	if ( it == m_instanced_sprites.end() ) {

		Log( "Creating instanced sprite actor: " + key );

		NEWV(
			sprite,
			scene::actor::Sprite,
			name,
			{
				::game::map::s_consts.tile.scale.x,
				::game::map::s_consts.tile.scale.y * ::game::map::s_consts.sprite.y_scale
			},
			m_textures.source.ter1_pcx,
			{
				{
					(float)1.0f / m_textures.source.ter1_pcx->m_width * ( tex_coords.x + 1 ),
					(float)1.0f / m_textures.source.ter1_pcx->m_height * ( tex_coords.y + 1 )
				},
				{
					(float)1.0f / m_textures.source.ter1_pcx->m_width * ( tex_coords.x + ::game::map::s_consts.tc.ter1_pcx.dimensions.x - 4 ),
					(float)1.0f / m_textures.source.ter1_pcx->m_height * ( tex_coords.y + ::game::map::s_consts.tc.ter1_pcx.dimensions.y - 4 )
				}
			}
		);
		NEWV( instanced, scene::actor::Instanced, sprite );
		instanced->SetZIndex( z_index ); // needs to be higher than map terrain z position
		m_world_scene->AddActor( instanced );
		m_instanced_sprites[ key ] = {
			name,
			tex_coords,
			instanced
		};
		return instanced;
	}
	else {
		return it->second.actor;
	}
}

scene::actor::Instanced* Game::GetTerrainSpriteActorByKey( const std::string& key ) {
	const auto& it = m_instanced_sprites.find( key );
	ASSERT( it != m_instanced_sprites.end(), "sprite actor '" + key + "' not found" );
	return it->second.actor;
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

void Game::ProcessEvent( const ::game::Event& event ) {
	Log( "Received event (type=" + std::to_string( event.type ) + ")" );
	switch ( event.type ) {
		case ::game::Event::ET_QUIT: {
			ExitGame(
				[ this, event ]() -> void {
					ReturnToMainMenu(
						event.data.quit.reason
							? *event.data.quit.reason
							: ""
					);
				}
			);
			break;
		}
		case ::game::Event::ET_GLOBAL_MESSAGE: {
			AddMessage( *event.data.global_message.message );
			break;
		}
		default: {
			THROW( "unexpected event type: " + std::to_string( event.type ) );
		}
	}
}

void Game::UpdateMapData( const types::Vec2< size_t >& map_size ) {
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
		GetTerrainSpriteActor( a.second.name, a.second.tex_coords, a.second.z_index );
	}
	for ( auto& instance : sprite_instances ) {
		auto* actor = GetTerrainSpriteActorByKey( instance.second.first );
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
		UIEvent::EV_KEY_DOWN, EH( this, ui ) {

			if ( ui->HasPopup() ) {
				return false;
			}

			if ( !data->key.modifiers ) {
				if ( m_selected_tile_data.is_set ) {

					// move tile selector
					bool is_tile_selected = true;
					::game::tile_direction_t td = ::game::TD_NONE;
					if ( data->key.code == UIEvent::K_LEFT || data->key.code == UIEvent::K_KP_LEFT ) {
						td = ::game::TD_W;
					}
					else if ( data->key.code == UIEvent::K_UP || data->key.code == UIEvent::K_KP_UP ) {
						td = ::game::TD_N;
					}
					else if ( data->key.code == UIEvent::K_RIGHT || data->key.code == UIEvent::K_KP_RIGHT ) {
						td = ::game::TD_E;
					}
					else if ( data->key.code == UIEvent::K_DOWN || data->key.code == UIEvent::K_KP_DOWN ) {
						td = ::game::TD_S;
					}
					else if ( data->key.code == UIEvent::K_HOME || data->key.code == UIEvent::K_KP_LEFT_UP ) {
						td = ::game::TD_NW;
					}
					else if ( data->key.code == UIEvent::K_END || data->key.code == UIEvent::K_KP_LEFT_DOWN ) {
						td = ::game::TD_SW;
					}
					else if ( data->key.code == UIEvent::K_PAGEUP || data->key.code == UIEvent::K_KP_RIGHT_UP ) {
						td = ::game::TD_NE;
					}
					else if ( data->key.code == UIEvent::K_PAGEDOWN || data->key.code == UIEvent::K_KP_RIGHT_DOWN ) {
						td = ::game::TD_SE;
					}
					else {
						is_tile_selected = false; // not selected
					}

					if ( is_tile_selected ) {

						GetTileAtCoords( m_selected_tile_data.tile_position, td );

						return true;
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
				SelectTileAtPoint( data->mouse.absolute.x, data->mouse.absolute.y ); // async
				m_editing_draw_timer.SetInterval( Game::s_consts.map_editing.draw_frequency_ms ); // keep drawing until mouseup
			}
			else {
				switch ( data->mouse.button ) {
					case UIEvent::M_LEFT: {
						SelectTileAtPoint( data->mouse.absolute.x, data->mouse.absolute.y ); // async
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
						Log( "Edge scrolling started" );
						m_map_control.edge_scrolling.timer.SetInterval( Game::s_consts.map_scroll.static_scrolling.scroll_step_ms );
					}
				}
				else {
					if ( m_map_control.edge_scrolling.timer.IsRunning() ) {
						Log( "Edge scrolling stopped" );
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
	DeselectTile();

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
	}

	for ( auto& it : m_actors_map ) {
		m_world_scene->RemoveActor( it.second );
		DELETE( it.second );
	}
	m_actors_map.clear();

	for ( auto& sprite : m_instanced_sprites ) {
		m_world_scene->RemoveActor( sprite.second.actor );
		DELETE( sprite.second.actor );
	}
	m_instanced_sprites.clear();

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
	x( m_world_scene );
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

void Game::SelectTileAtPoint( const size_t x, const size_t y ) {
	//Log( "Looking up tile at " + std::to_string( x ) + "x" + std::to_string( y ) );
	GetTileAtScreenCoords( x, m_viewport.window_height - y ); // async
}

void Game::SelectTile( const tile_data_t& tile_data ) {
	DeselectTile();

	NEW( m_actors.tile_selection, actor::TileSelection, tile_data.selection_coords );
	AddActor( m_actors.tile_selection );

	m_ui.bottom_bar->PreviewTile( tile_data );

	m_selected_tile_data = tile_data;

	//Log( "Selected tile at " + m_selected_tile_data.tile_position.ToString() );
}

void Game::DeselectTile() {
	if ( m_actors.tile_selection ) {
		RemoveActor( m_actors.tile_selection );
		m_actors.tile_selection = nullptr;
	}

	m_ui.bottom_bar->HideTilePreview();
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

void Game::GetTileAtScreenCoords( const size_t screen_x, const size_t screen_inverse_y ) {
	if ( m_tile_at_request_id ) {
		m_actors.terrain->GetMeshActor()->CancelDataRequest( m_tile_at_request_id );
	}
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

void Game::GetTileAtCoords( const Vec2< size_t >& tile_pos, const ::game::tile_direction_t tile_direction ) {
	auto* game = g_engine->GetGame();
	if ( m_mt_ids.select_tile ) {
		game->MT_Cancel( m_mt_ids.select_tile );
	}
	m_mt_ids.select_tile = game->MT_SelectTile( tile_pos, tile_direction );
}

tile_data_t Game::GetTileAtCoordsResult() {
	if ( m_mt_ids.select_tile ) {
		auto* game = g_engine->GetGame();
		auto response = game->MT_GetResponse( m_mt_ids.select_tile );
		if ( response.result != ::game::R_NONE ) {
			m_mt_ids.select_tile = 0;

			tile_data_t result = {};
			result.is_set = true;
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

			return result;
		}
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

void Game::CancelRequests() {
	auto* game = g_engine->GetGame();
	if ( m_mt_ids.init ) {
		game->MT_Cancel( m_mt_ids.init );
		m_mt_ids.init = 0;
		// WHY?
		ASSERT( !m_mt_ids.ping, "ping already active" );
		g_engine->GetUI()->SetLoaderText( "Canceling" );
		m_mt_ids.ping = game->MT_Ping();
	}
	if ( m_mt_ids.get_map_data ) {
		game->MT_Cancel( m_mt_ids.get_map_data );
		m_mt_ids.get_map_data = 0;
	}
	if ( m_mt_ids.reset ) {
		game->MT_Cancel( m_mt_ids.reset );
		m_mt_ids.reset = 0;
	}
	if ( m_mt_ids.get_events ) {
		game->MT_Cancel( m_mt_ids.get_events );
		m_mt_ids.get_events = 0;
	}
	// TODO: cancel other requests?
}

void Game::CancelGame() {
	ExitGame(
		[ this ]() -> void {
			ASSERT( m_state, "state not set" );
			auto* connection = m_state->GetConnection();
			if ( connection && connection->IsConnected() ) {
				connection->Disconnect();
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

}
}
