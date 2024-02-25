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
						auto* actor = GetInstancedSpriteByKey( i.second ).actor;
						ASSERT( actor, "sprite actor not found" );
						ASSERT( actor->HasInstance( i.first ), "actor instance not found" );
						actor->RemoveInstance( i.first );
					}
				}

				if ( response.data.edit_map.sprites.instances_to_add ) {
					Log( "Need to add " + std::to_string( response.data.edit_map.sprites.instances_to_add->size() ) + " instances" );
					for ( auto& i : *response.data.edit_map.sprites.instances_to_add ) {
						const auto& instance = i.second;
						auto* actor = GetInstancedSpriteByKey( instance.first ).actor;
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
				SelectTileAtPoint( ::game::TQP_TILE_SELECT, m_map_control.last_mouse_position.x, m_map_control.last_mouse_position.y ); // async
			}
		}

		// response for clicked tile (if click happened)
		const auto tile_at = GetTileAtScreenCoordsResult();
		if ( tile_at.is_set ) {
			ASSERT( m_tile_at_query_purpose != ::game::TQP_NONE, "tile preferred mode not set" );
			GetTileAtCoords( m_tile_at_query_purpose, tile_at.tile_pos );
		}

		const auto tile_data = GetTileAtCoordsResult();
		if ( tile_data.is_set ) {
			if ( m_is_map_editing_allowed && m_is_editing_mode ) {
				if ( !m_mt_ids.edit_map ) { // TODO: need to queue?
					m_mt_ids.edit_map = game->MT_EditMap( tile_data.tile_position, m_editor_tool, m_editor_brush, m_editor_draw_mode );
				}
				ASSERT( tile_data.purpose == ::game::TQP_TILE_SELECT, "only tile selections allowed in map editor" );
				SelectTileOrUnit( tile_data );
			}
			else {
				SelectTileOrUnit( tile_data );
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

		if ( m_selected_unit_state ) {
			auto& badge = m_selected_unit_state->render.badge;
			while ( badge.blink.timer.HasTicked() ) {
				auto& ba = badge.def->instanced_sprite->actor;
				if ( ba->HasInstance( badge.instance_id ) ) {
					UnrenderUnitBadge( *m_selected_unit_state );
				}
				else {
					RenderUnitBadge( *m_selected_unit_state );
				}
			}
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

types::Texture* Game::GetRepaintedSourceTexture( const std::string& name, const types::Texture* original, const types::Texture::repaint_rules_t& rules ) {
	const auto it = m_textures.repainted_source.find( name );
	if ( it != m_textures.repainted_source.end() ) {
		return it->second;
	}
	NEWV( texture, types::Texture, original->m_name, original->m_width, original->m_height );
	texture->RepaintFrom( original, rules );
	m_textures.repainted_source.insert(
		{
			name,
			texture
		}
	);
	return texture;
}

Game::instanced_sprite_t& Game::GetInstancedSprite(
	const std::string& name,
	types::Texture* texture,
	const ::game::map::Consts::pcx_texture_coordinates_t& src_xy,
	const ::game::map::Consts::pcx_texture_coordinates_t& src_wh,
	const ::game::map::Consts::pcx_texture_coordinates_t& src_cxy,
	const types::Vec2< float > dst_wh,
	const float z_index
) {

	const auto key = name + " " + src_xy.ToString() + " " + src_wh.ToString();

	auto it = m_instanced_sprites.find( key );
	if ( it == m_instanced_sprites.end() ) {

		Log( "Creating instanced sprite: " + key );

		NEWV(
			sprite,
			scene::actor::Sprite,
			name,
			{
				dst_wh.x,
				dst_wh.y
			},
			texture,
			{
				{
					(float)1.0f / texture->m_width * ( src_xy.x ),
					(float)1.0f / texture->m_height * ( src_xy.y )
				},
				{
					(float)1.0f / texture->m_width * ( src_xy.x + src_wh.x ),
					(float)1.0f / texture->m_height * ( src_xy.y + src_wh.y )
				}
			}
		);
		NEWV( instanced, scene::actor::Instanced, sprite );
		instanced->SetZIndex( z_index ); // needs to be higher than map terrain z position
		m_world_scene->AddActor( instanced );
		it = m_instanced_sprites.insert(
			{
				key,
				{
					key,
					name,
					src_xy,
					src_wh,
					src_cxy,
					instanced
				}
			}
		).first;
	}
	return it->second;
}

Game::instanced_sprite_t& Game::GetInstancedSpriteByKey( const std::string& key ) {
	const auto& it = m_instanced_sprites.find( key );
	ASSERT( it != m_instanced_sprites.end(), "sprite actor '" + key + "' not found" );
	return it->second;
}

Game::instanced_sprite_t& Game::GetTerrainInstancedSprite( const ::game::map::Map::sprite_actor_t& actor ) {
	return GetInstancedSprite(
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

Game::instanced_sprite_t* Game::GetRepaintedInstancedSprite( const std::string& name, const Game::instanced_sprite_t& original, const types::Texture::repaint_rules_t& rules ) {
	const auto& key = name;

	auto it = m_instanced_sprites.find( key );
	if ( it == m_instanced_sprites.end() ) {

		Log( "Creating repainted instanced sprite: " + key );

		const auto* original_sprite = original.actor->GetSpriteActor();
		auto* texture = GetRepaintedSourceTexture( name, original_sprite->GetTexture(), rules );

		NEWV(
			sprite,
			scene::actor::Sprite,
			name,
			original_sprite->GetDimensions(),
			texture,
			original_sprite->GetTexCoords()
		);
		sprite->SetRenderFlags( original_sprite->GetRenderFlags() );
		NEWV( instanced, scene::actor::Instanced, sprite );
		instanced->SetZIndex( original.actor->GetZIndex() );
		m_world_scene->AddActor( instanced );
		it = m_instanced_sprites.insert(
			{
				key,
				{
					key,
					name,
					original.xy,
					original.wh,
					original.cxy,
					instanced
				}
			}
		).first;

	}
	return &it->second;
}

void Game::RemoveInstancedSpriteByKey( const std::string& key ) {
	const auto& it = m_instanced_sprites.find( key );
	ASSERT( it != m_instanced_sprites.end(), "instanced sprite not found: " + key );
	Log( "Removing instanced sprite: " + key );
	const auto& sprite = it->second;
	m_world_scene->RemoveActor( sprite.actor );
	m_instanced_sprites.erase( it );
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
	auto slot_it = m_slot_states.find( slot_index );
	if ( slot_it == m_slot_states.end() ) {
		Log( "Initializing slot state: " + std::to_string( slot_index ) );
		slot_it = m_slot_states.insert(
			{
				slot_index,
				{
					slot_index,
					{},
					{}
				}
			}
		).first;
	}
	auto& slot = slot_it->second;
	if ( slot.color != color || slot.badges.empty() ) {
		const auto badges_key = "Badge_" + std::to_string( slot_index );
		const auto& c = color.value;
		const types::Texture::repaint_rules_t& rules = {
			{ // active
				types::Color::RGB( 252, 0, 0 ),
				types::Color( c.red, c.green, c.blue ).GetRGBA()
			},
			{ // greyed out
				types::Color::RGB( 125, 0, 0 ),
				types::Color( ( 0.5f + c.red / 2 ) / 2, ( 0.5f + c.green / 2 ) / 2, ( 0.5f + c.blue / 2 ) / 2 ).GetRGBA()
			}
		};

		// fake badges should be slightly grayer
		auto fake_badge_rules = rules;
		for ( auto& rule : fake_badge_rules ) {
			rule.second = types::Color( ( 0.6f + c.red / 2.5 ) / 2, ( 0.6f + c.green / 2.5 ) / 2, ( 0.6f + c.blue / 2.5 ) / 2 ).GetRGBA();
		}

		if ( !slot.badges.empty() ) {
			for ( const auto& it : slot.badges ) {
				const auto& badgedef = it.second;
				RemoveInstancedSpriteByKey( badgedef.normal.instanced_sprite->key );
				RemoveInstancedSpriteByKey( badgedef.greyedout.instanced_sprite->key );
			}
			slot.badges.clear();
		}
		const badge_type_t badge_type = is_progenitor
			? BT_PROGENITOR
			: BT_DEFAULT;
		for ( auto morale = ::game::unit::Unit::MORALE_MIN ; morale <= ::game::unit::Unit::MORALE_MAX ; morale++ ) {
			auto& badgedef = slot.badges[ morale ];
			badgedef.normal.instanced_sprite = GetRepaintedInstancedSprite(
				badges_key + "_" + std::to_string( morale ) + "_NORMAL",
				*m_unitbadge_sprites.at( badge_type | BT_NORMAL ).at( morale ),
				rules
			);
			badgedef.normal.next_instance_id = 1;
			badgedef.greyedout.instanced_sprite = GetRepaintedInstancedSprite(
				badges_key + "_" + std::to_string( morale ) + "_GREYEDOUT",
				*m_unitbadge_sprites.at( badge_type | BT_GREYEDOUT ).at( morale ),
				rules
			);
			badgedef.greyedout.next_instance_id = 1;
		}
		slot.fake_badge.instanced_sprite = GetRepaintedInstancedSprite(
			badges_key + "_FAKE",
			*m_fake_badge,
			fake_badge_rules
		);
		slot.fake_badge.next_instance_id = 1;
	}
	slot.color = color;
}

void Game::DefineUnit( const ::game::unit::Def* unitdef ) {
	auto unitdef_it = m_unitdef_states.find( unitdef->m_id );
	ASSERT( unitdef_it == m_unitdef_states.end(), "unit def already exists" );

	Log( "Initializing unitdef state: " + unitdef->m_id );
	unitdef_state_t unitdef_state = {
		unitdef->m_id,
		unitdef->m_name,
		unitdef->m_type,
	};

	switch ( unitdef->m_type ) {
		case ::game::unit::Def::DT_STATIC: {
			const auto* def = (::game::unit::StaticDef*)unitdef;

			switch ( def->m_render->m_type ) {

				case ::game::unit::Render::RT_SPRITE: {
					const auto* render = (::game::unit::SpriteRender*)def->m_render;

					unitdef_state.static_.movement_type = def->m_movement_type;
					unitdef_state.static_.movement_per_turn = def->m_movement_per_turn;
					unitdef_state.static_.render.is_sprite = true;

					const auto name = "Unit_" + def->m_id;
					auto* texture = GetSourceTexture( render->m_file );
					const ::game::map::Consts::pcx_texture_coordinates_t& src_wh = {
						render->m_w,
						render->m_h,
					};
					const Vec2< float >& dst_wh = {
						::game::map::s_consts.tile.scale.x,
						::game::map::s_consts.tile.scale.y * ::game::map::s_consts.sprite.y_scale
					};
					const auto zindex = 0.5f;

					unitdef_state.static_.render.morale_based_xshift = render->m_morale_based_xshift;
					if ( unitdef_state.static_.render.morale_based_xshift ) {
						NEW( unitdef_state.static_.render.morale_based_sprites, morale_based_sprite_states_t );
						for ( ::game::unit::Unit::morale_t morale = ::game::unit::Unit::MORALE_MIN ; morale <= ::game::unit::Unit::MORALE_MAX ; morale++ ) {
							const uint32_t xshift = unitdef_state.static_.render.morale_based_xshift * ( morale - ::game::unit::Unit::MORALE_MIN );
							unitdef_state.static_.render.morale_based_sprites->insert(
								{
									morale,
									{
										&GetInstancedSprite(
											name + "_" + std::to_string( morale ), texture, {
												render->m_x + xshift,
												render->m_y,
											},
											src_wh,
											{
												render->m_cx + xshift,
												render->m_cy,
											},
											dst_wh,
											zindex
										),
									}
								}
							);
						}
					}
					else {
						unitdef_state.static_.render.sprite.instanced_sprite = &GetInstancedSprite(
							name, texture, {
								render->m_x,
								render->m_y,
							},
							src_wh,
							{
								render->m_cx,
								render->m_cy,
							},
							dst_wh,
							zindex
						);
					}
					break;
				}
				default:
					THROW( "unknown unit render type: " + std::to_string( def->m_render->m_type ) );
			}
			break;
		}
		default:
			THROW( "unknown unit def type: " + std::to_string( unitdef->m_type ) );
	}
	m_unitdef_states.insert(
		{
			unitdef->m_id,
			unitdef_state
		}
	);
}

void Game::SpawnUnit(
	const size_t unit_id,
	const std::string& unitdef_id,
	const size_t slot_index,
	const Vec2< size_t >& tile_coords,
	const Vec3& render_coords,
	const bool is_active,
	const ::game::unit::Unit::movement_t movement,
	const ::game::unit::Unit::morale_t morale,
	const ::game::unit::Unit::health_t health
) {

	ASSERT( m_unitdef_states.find( unitdef_id ) != m_unitdef_states.end(), "unitdef not found" );
	ASSERT( m_slot_states.find( slot_index ) != m_slot_states.end(), "slot not found" );
	ASSERT( m_unit_states.find( unit_id ) == m_unit_states.end(), "unit id already exists" );

	auto& slot_state = m_slot_states.at( slot_index );
	auto& unit_state = m_unit_states.insert(
		{
			unit_id,
			{
				unit_id,
				&m_unitdef_states.at( unitdef_id ),
				&slot_state,
				&GetTileState( tile_coords.x, tile_coords.y ),
				{
					{
						render_coords.x,
						render_coords.y,
						render_coords.z
					},
					false,
					0,
					{
						nullptr, // will be set later
						0,
						{
							&m_healthbar_sprites.at( floor( health * s_consts.badges.healthbars.resolution ) ),
							0
						}
					},
					{
						false,
						0
					}
				},
				is_active,
				movement,
				morale,
				health
			}
		}
	).first->second;

	unit_state.render.badge.def = GetUnitBadgeDef( unit_state );

	ASSERT( unit_state.tile->units.find( unit_id ) == unit_state.tile->units.end(), "unit id already exists" );
	unit_state.tile->units.insert(
		{
			unit_id,
			&unit_state
		}
	);

	RenderTile( *unit_state.tile );
}

void Game::DespawnUnit( const size_t unit_id ) {
	const auto& it = m_unit_states.find( unit_id );
	ASSERT( it != m_unit_states.end(), "unit id not found" );

	auto& unit_state = it->second;

	auto& tile = unit_state.tile;
	ASSERT( tile, "unit tile not set" );
	ASSERT( tile->units.find( unit_id ) != tile->units.end(), "unit id does not exist" );
	tile->units.erase( unit_id );
	RenderTile( *tile );

	m_unit_states.erase( it );

}

void Game::RefreshUnit( unit_state_t* unit_state ) {
	if ( unit_state->is_active && unit_state->movement < ::game::unit::Unit::MINIMUM_MOVEMENT_TO_KEEP ) {
		// no more moves left, unselect
		// TODO: select next unit
		unit_state->is_active = false;
		unit_state->render.badge.blink.timer.Stop();
		unit_state->render.badge.def = GetUnitBadgeDef( *unit_state );
		if ( m_selected_tile_data.purpose == ::game::TQP_UNIT_SELECT ) {
			//m_selected_tile_data.purpose = ::game::TQP_TILE_SELECT;
			GetTileAtCoords( ::game::TQP_TILE_SELECT, unit_state->tile->coords );
		}
	}
	RenderTile( *unit_state->tile );
}

void Game::MoveUnit( unit_state_t* unit, tile_state_t* dst_tile, const types::Vec3& dst_render_coords ) {
	auto* src_tile = unit->tile;
	ASSERT( src_tile, "unit tile not set" );

	ASSERT( src_tile->units.find( unit->unit_id ) != src_tile->units.end(), "unit id not found in src tile" );
	src_tile->units.erase( unit->unit_id );

	ASSERT( dst_tile->units.find( unit->unit_id ) == dst_tile->units.end(), "unit id already exists in dst tile" );
	dst_tile->units.insert(
		{
			unit->unit_id,
			unit
		}
	);

	unit->tile = dst_tile;

	unit->render.coords = dst_render_coords;

	// TODO: animation

	// update old tile
	RenderTile( *src_tile );

	// update unit and new tile
	RefreshUnit( unit );

	if ( unit->unit_id == m_currently_moving_unit_id ) {
		if ( unit->is_active ) {
			// update ui and keep same unit selected
			::game::tile_query_metadata_t metadata = {};
			metadata.unit_move.unit_id = unit->unit_id;
			GetTileAtCoords( ::game::TQP_UNIT_SELECT, dst_tile->coords, ::game::map::Tile::D_NONE, metadata );
		}
		m_currently_moving_unit_id = 0;
	}
}

void Game::ProcessEvent( const ::game::Event& event ) {
	//Log( "Received event (type=" + std::to_string( event.type ) + ")" ); // spammy
	const auto f_exit = [ this, &event ]() -> void {
		ExitGame(
			[ this, event ]() -> void {
#ifdef DEBUG
				if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
					g_engine->ShutDown();
				}
				else
#endif
				{
					ReturnToMainMenu(
						event.data.quit.reason
							? *event.data.quit.reason
							: ""
					);
				}
			}
		);
	};
	switch ( event.type ) {
		case ::game::Event::ET_QUIT: {
			f_exit();
			break;
		}
		case ::game::Event::ET_ERROR: {
			Log( *event.data.error.stacktrace );
			g_engine->GetUI()->ShowError(
				*event.data.error.what, UH( f_exit ) {
					f_exit();
				}
			);
		}
		case ::game::Event::ET_GLOBAL_MESSAGE: {
			AddMessage( *event.data.global_message.message );
			break;
		}
		case ::game::Event::ET_TURN_COMPLETE_STATUS: {
			ASSERT( m_ui.bottom_bar, "bottom bar not initialized" );
			m_ui.bottom_bar->SetTurnCompleteStatus( event.data.turn_complete_status.is_turn_complete );
			break;
		}
		case ::game::Event::ET_SLOT_DEFINE: {
			for ( const auto& d : *event.data.slot_define.slotdefs ) {
				const auto& c = d.color;
				DefineSlot( d.slot_index, types::Color( c.r, c.g, c.b, c.a ), d.is_progenitor );
			}
			break;
		}
		case ::game::Event::ET_UNIT_DEFINE: {
			types::Buffer buf( *event.data.unit_define.serialized_unitdef );
			const auto* unitdef = ::game::unit::Def::Unserialize( buf );
			DefineUnit( unitdef );
			delete unitdef;
			break;
		}
		case ::game::Event::ET_UNIT_SPAWN: {
			const auto& d = event.data.unit_spawn;
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
				d.is_active,
				d.movement,
				d.morale,
				d.health
			);
			break;
		}
		case ::game::Event::ET_UNIT_DESPAWN: {
			DespawnUnit( event.data.unit_despawn.unit_id );
			break;
		}
		case ::game::Event::ET_UNIT_REFRESH: {
			const auto& d = event.data.unit_refresh;
			auto& unit_state = m_unit_states.at( d.unit_id );
			unit_state.movement = d.movement_left;
			RefreshUnit( &unit_state );
			break;
		}
		case ::game::Event::ET_UNIT_MOVE: {
			const auto& d = event.data.unit_move;
			ASSERT( m_unit_states.find( d.unit_id ) != m_unit_states.end(), "unit id not found" );
			auto& unit_state = m_unit_states.at( d.unit_id );
			unit_state.movement = d.movement_left;
			auto& tile_state = GetTileState( d.tile_coords.x, d.tile_coords.y );
			const auto& rc = d.render_coords;
			MoveUnit(
				&unit_state, &tile_state, {
					rc.x,
					rc.y,
					rc.z
				}
			);
			break;
		}
		default: {
			THROW( "unexpected event type: " + std::to_string( event.type ) );
		}
	}
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

	// TODO: optimize for SMAC coordinate system
	m_tile_states.resize( m_map_data.width * m_map_data.height );
	for ( size_t y = 0 ; y < m_map_data.height ; y++ ) {
		for ( size_t x = 0 ; x < m_map_data.width ; x++ ) {
			GetTileState( x, y ).coords = {
				x,
				y
			};
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
		auto* actor = GetInstancedSpriteByKey( instance.second.first ).actor;
		ASSERT( actor, "sprite actor not found" );
		ASSERT( !actor->HasInstance( instance.first ), "actor instance already exists" );
		actor->SetInstance( instance.first, instance.second.second );
	}

	{
		// load badge sprites
		ASSERT( m_unitbadge_sprites.empty(), "badge sprites already initialized" );
		const uint8_t w = 23;
		const uint8_t h = 30;
		const uint8_t margin = 1;
		auto* texture = GetSourceTexture( "flags.pcx" );
		const types::Color transparent( 0.0f, 0.0f, 0.0f, 0.0f );
		uint32_t x, y;
		for ( auto badge_type = BT_PROGENITOR ; badge_type <= BT_DEFAULT ; badge_type++ ) {
			for ( auto badge_mode = BT_NORMAL ; badge_mode <= BT_GREYEDOUT ; badge_mode++ ) {
				const uint8_t row = badge_type | badge_mode;
				auto& spritemap = m_unitbadge_sprites[ row ];
				for ( auto morale = ::game::unit::Unit::MORALE_MIN ; morale <= ::game::unit::Unit::MORALE_MAX ; morale++ ) {
					x = ( morale - ::game::unit::Unit::MORALE_MIN ) * ( w + margin ) + margin;
					y = row * ( h + margin ) + margin;

					// cut holes for healthbars
					texture->Fill( x + 6, y + 6, x + 7, y + 26, transparent );

					auto* sprite = &GetInstancedSprite(
						"Badge_" + std::to_string( badge_type ) + "_" + std::to_string( badge_mode ),
						texture,
						{
							x,
							y,
						},
						{
							w,
							h,
						},
						{
							x + w / 2,
							y + h / 2,
						},
						{
							::game::map::s_consts.tile.scale.x * s_consts.badges.scale.x,
							::game::map::s_consts.tile.scale.y * s_consts.badges.scale.y * ::game::map::s_consts.sprite.y_scale
						},
						0.55f
					);
					//sprite->actor->GetSpriteActor()->SetRenderFlags( actor::Actor::RF_SPRITES_DEPTH );

					spritemap.insert(
						{
							morale,
							sprite
						}
					);
				}
			}
		}

		// load fake badge sprite
		ASSERT( !m_fake_badge, "fake badge already initialized" );
		m_fake_badge = &GetInstancedSprite(
			"Badge_FAKE",
			texture,
			{
				21,
				5,
			},
			{
				3,
				17,
			},
			{
				22,
				12,
			},
			{
				::game::map::s_consts.tile.scale.x * s_consts.badges.fake_badges.scale.x,
				::game::map::s_consts.tile.scale.y * s_consts.badges.fake_badges.scale.y * ::game::map::s_consts.sprite.y_scale
			},
			0.52f
		);
	}
	{
		// load healthbar sprites
		ASSERT( m_healthbar_textures.empty(), "healthbar textures already initialized" );
		ASSERT( m_healthbar_sprites.empty(), "healthbar sprites already initialized" );

		const auto res = s_consts.badges.healthbars.resolution;

		const float stepval = 1.0f / ( res );
		types::Color black( 0.0f, 0.0f, 0.0f );
		types::Color color( 1.0f - stepval / 2, stepval / 2, 0.0f );

		m_healthbar_textures.reserve( res );
		m_healthbar_sprites.resize( res );
		for ( auto step = 0 ; step < res ; step++ ) {

			// generate healthbar texture
			auto* texture = new types::Texture( "HealthBar_" + std::to_string( step ), 1, res );

			texture->Fill( 0, 0, 0, step, color );
			if ( step < res - 1 ) {
				texture->Fill( 0, step + 1, 0, res - 1, black );
			}

			auto* sprite = &GetInstancedSprite(
				"Badge_Healthbar_" + std::to_string( step ),
				texture,
				{
					0,
					0,
				},
				{
					1,
					res,
				},
				{
					0,
					0,
				},
				{
					::game::map::s_consts.tile.scale.x * s_consts.badges.healthbars.scale.x,
					::game::map::s_consts.tile.scale.y * s_consts.badges.healthbars.scale.y * ::game::map::s_consts.sprite.y_scale
				},
				0.54f
			);
			//sprite->actor->GetSpriteActor()->SetRenderFlags( actor::Actor::RF_SPRITES_DEPTH );

			m_healthbar_textures.push_back( texture );
			m_healthbar_sprites.at( step ).instanced_sprite = sprite;
			color.value.red -= stepval;
			color.value.green += stepval;
		}
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

					bool is_tile_selected = true;
					::game::map::Tile::direction_t td = ::game::map::Tile::D_NONE;

					switch ( data->key.code ) {
#define X( _key, _altkey, _direction ) \
                        case UIEvent::_key: \
                        case UIEvent::_altkey: { \
                            td = ::game::map::Tile::_direction; \
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
						default: {
							is_tile_selected = false; // not selected
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
								ASSERT( m_selected_unit_data, "no unit data selected" );
								m_currently_moving_unit_id = m_selected_unit_data->id;
								const auto event = ::game::event::MoveUnit( m_slot_index, m_currently_moving_unit_id, td );
								g_engine->GetGame()->MT_AddGameEvent( &event );
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
						SelectTileAtPoint( ::game::TQP_UNIT_SELECT, data->mouse.absolute.x, data->mouse.absolute.y ); // async
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

	for ( const auto& texture : m_healthbar_textures ) {
		delete texture;
	}
	m_healthbar_textures.clear();

	m_slot_states.clear();

	for ( const auto& it : m_textures.repainted_source ) {
		DELETE( it.second );
	}
	m_textures.repainted_source.clear();

	for ( auto& it : m_unitdef_states ) {
		if ( it.second.m_type == ::game::unit::Def::DT_STATIC ) {
			if ( it.second.static_.render.morale_based_xshift ) {
				DELETE( it.second.static_.render.morale_based_sprites );
			}
		}
	}
	m_unitdef_states.clear();

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

void Game::SelectTileAtPoint( const ::game::tile_query_purpose_t tile_query_purpose, const size_t x, const size_t y ) {
	//Log( "Looking up tile at " + std::to_string( x ) + "x" + std::to_string( y ) );
	GetTileAtScreenCoords( tile_query_purpose, x, m_viewport.window_height - y ); // async
}

void Game::SelectUnit( const unit_data_t& unit_data, const bool actually_select_unit ) {
	m_selected_unit_data = &unit_data;
	ASSERT( m_unit_states.find( m_selected_unit_data->id ) != m_unit_states.end(), "unit id not found" );
	auto* unit_state = &m_unit_states.at( m_selected_unit_data->id );
	if ( m_selected_unit_state != unit_state ) {
		if ( m_selected_unit_state ) {
			UnrenderUnit( *m_selected_unit_state );
		}
		m_selected_unit_state = unit_state;
		RenderUnit( *m_selected_unit_state );
	}
	if ( actually_select_unit ) {
		UnrenderUnitBadge( *m_selected_unit_state );
		m_selected_unit_state->render.badge.blink.timer.SetInterval( s_consts.badges.blink.interval_ms );
		Log( "Selected unit " + std::to_string( m_selected_unit_data->id ) );
	}
}

void Game::SelectTileOrUnit( const tile_data_t& tile_data ) {

	ASSERT( tile_data.purpose != ::game::TQP_NONE, "tile query purpose not set" );

	DeselectTileOrUnit();

	m_selected_tile_data = tile_data;

	const unit_data_t* selected_unit = nullptr;
	if ( m_selected_tile_data.purpose == ::game::TQP_UNIT_SELECT ) {
		const auto& md = m_selected_tile_data.metadata.unit_move;
		if ( md.unit_id ) {
			for ( const auto& unit : m_selected_tile_data.units ) {
				if ( unit.id == md.unit_id ) {
					selected_unit = &unit;
					break;
				}
			}
			ASSERT( selected_unit, "unit id not found" );
		}
		else {
			selected_unit = GetFirstSelectableUnit( m_selected_tile_data.units );
			if ( !selected_unit ) {
				m_selected_tile_data.purpose = ::game::TQP_TILE_SELECT;
			}
		}
	}

	m_ui.bottom_bar->PreviewTile( m_selected_tile_data );

	switch ( m_selected_tile_data.purpose ) {
		case ::game::TQP_TILE_SELECT: {
			NEW( m_actors.tile_selection, actor::TileSelection, m_selected_tile_data.selection_coords );
			AddActor( m_actors.tile_selection );
			Log( "Selected tile at " + m_selected_tile_data.tile_position.ToString() + " ( " + m_selected_tile_data.selection_coords.center.ToString() + " )" );
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

		m_selected_unit_state->render.badge.blink.timer.Stop();
		RenderUnitBadge( *m_selected_unit_state );

		ASSERT( !m_selected_tile_data.units.empty(), "unit was selected but tile data has no units" );
		// reset to most important unit if needed
		const auto unit_id = m_selected_tile_data.units.front().id;
		ASSERT( m_unit_states.find( unit_id ) != m_unit_states.end(), "unit id not found" );
		auto* most_important_unit = &m_unit_states.at( unit_id );
		if ( m_selected_unit_state && m_selected_unit_state != most_important_unit ) {
			UnrenderUnit( *m_selected_unit_state );
			RenderUnit( *most_important_unit );
		}

		m_selected_unit_state = nullptr;
	}

	m_ui.bottom_bar->HideTilePreview();
}

const unit_data_t* Game::GetFirstSelectableUnit( const std::vector< unit_data_t >& units ) const {
	for ( const auto& unit : units ) {
		ASSERT( m_unit_states.find( unit.id ) != m_unit_states.end(), "unit id not found" );
		const auto& unit_state = m_unit_states.at( unit.id );
		if ( unit_state.slot->slot_index == m_slot_index && unit_state.is_active ) {
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
	if ( m_mt_ids.select_tile ) {
		game->MT_Cancel( m_mt_ids.select_tile );
	}
	ASSERT( tile_query_purpose != ::game::TQP_NONE, "tile query purpose not set" );
	m_mt_ids.select_tile = game->MT_SelectTile( tile_query_purpose, tile_pos, tile_direction, tile_query_metadata );
}

tile_data_t Game::GetTileAtCoordsResult() {
	if ( m_mt_ids.select_tile ) {
		auto* game = g_engine->GetGame();
		auto response = game->MT_GetResponse( m_mt_ids.select_tile );
		if ( response.result != ::game::R_NONE ) {
			m_mt_ids.select_tile = 0;

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

			unit_states_t units = {};
			for ( const auto& unit_id : *response.data.select_tile.unit_ids ) {
				ASSERT( m_unit_states.find( unit_id ) != m_unit_states.end(), "unit id not found" );
				units.insert(
					{
						unit_id,
						&m_unit_states.at( unit_id )
					}
				);
			}
			const auto units_order = GetUnitsOrder( units );

			for ( const auto& unit_id : units_order ) {
				ASSERT( m_unit_states.find( unit_id ) != m_unit_states.end(), "unit id not found" );
				const auto& unit_state = m_unit_states.at( unit_id );

				types::mesh::Rectangle* mesh = nullptr;
				types::Texture* texture = nullptr;

				const auto& def = unit_state.def;
				ASSERT ( def->m_type == ::game::unit::Def::DT_STATIC, "only static units are supported for now" );
				const auto& render = def->static_.render;
				ASSERT( render.is_sprite, "only sprite units are supported for now" );
				const auto& sprite_state = render.morale_based_xshift
					? render.morale_based_sprites->at( unit_state.morale )
					: render.sprite;

				const auto& f_meshtex = []( const Game::instanced_sprite_t* sprite ) -> meshtex_t {
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
				std::string short_power_string = "";
				if ( def->m_id == "SporeLauncher" ) {
					short_power_string += "(?)";
				}
				else {
					short_power_string += "?";
				}
				short_power_string += " - ? - " + FloatToString( def->static_.movement_per_turn );

				std::string moves_string = "";
				if ( def->static_.movement_type != ::game::unit::StaticDef::MT_IMMOVABLE ) {
					moves_string = "Moves: " + FloatToString( unit_state.movement );
				}

				result.units.push_back(
					{
						unit_state.unit_id,
						f_meshtex( sprite_state.instanced_sprite ),
						f_meshtex( unit_state.render.badge.def->instanced_sprite ),
						f_meshtex( unit_state.render.badge.healthbar.def->instanced_sprite ),
						def->m_name,
						short_power_string,
						::game::unit::Unit::GetMoraleString( unit_state.morale ),
						moves_string,
					}
				);
			}
			result.metadata = response.data.select_tile.metadata;

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

Game::sprite_state_t* Game::GetUnitBadgeDef( const unit_state_t& unit_state ) const {
	const auto& slot_state = unit_state.slot;
	const bool is_active = slot_state->slot_index == m_slot_index && unit_state.is_active;
	return is_active
		? &slot_state->badges.at( unit_state.morale ).normal
		: &slot_state->badges.at( unit_state.morale ).greyedout;
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

void Game::RenderUnit( unit_state_t& unit_state ) {

	if ( unit_state.render.is_rendered ) {
		return;
	}

	Log( "Rendering unit " + std::to_string( unit_state.unit_id ) );

	auto* unitdef_state = unit_state.def;
	auto* slot_state = unit_state.slot;
	const auto& c = unit_state.render.coords;

	ASSERT( unitdef_state->m_type == ::game::unit::Def::DT_STATIC, "only static unitdefs are supported for now" );
	ASSERT( unitdef_state->static_.render.is_sprite, "only sprite unitdefs are supported for now" );

	auto& sprite = unitdef_state->static_.render.morale_based_xshift
		? unitdef_state->static_.render.morale_based_sprites->at( unit_state.morale )
		: unitdef_state->static_.render.sprite;
	unit_state.render.instance_id = sprite.next_instance_id++;
	sprite.instanced_sprite->actor->SetInstance(
		unit_state.render.instance_id, {
			c.x,
			c.y,
			c.z
		}
	);

	unit_state.render.badge.instance_id = unit_state.render.badge.def->next_instance_id++;
	unit_state.render.badge.healthbar.instance_id = unit_state.render.badge.healthbar.def->next_instance_id++;

	RenderUnitBadge( unit_state );

	unit_state.render.is_rendered = true;
}

void Game::RenderUnitBadge( const unit_state_t& unit_state ) {
	const auto& c = unit_state.render.coords;
	unit_state.render.badge.def->instanced_sprite->actor->SetInstance(
		unit_state.render.badge.instance_id, {
			c.x + ::game::map::s_consts.tile.scale.x * s_consts.badges.offset.x,
			c.y - ::game::map::s_consts.tile.scale.y * s_consts.badges.offset.y * ::game::map::s_consts.sprite.y_scale,
			c.z
		}
	);
	unit_state.render.badge.healthbar.def->instanced_sprite->actor->SetInstance(
		unit_state.render.badge.healthbar.instance_id, {
			c.x + ::game::map::s_consts.tile.scale.x * s_consts.badges.healthbars.offset.x,
			c.y - ::game::map::s_consts.tile.scale.y * s_consts.badges.healthbars.offset.y * ::game::map::s_consts.sprite.y_scale,
			c.z
		}
	);
}

void Game::RenderUnitFakeBadge( unit_state_t& unit_state, const size_t offset ) {

	Log( "Rendering unit fake badge " + std::to_string( unit_state.unit_id ) );

	ASSERT( !unit_state.render.fake_badge.is_rendered, "unit fake badge already rendered" );

	auto& slot_state = unit_state.slot;

	const auto& c = unit_state.render.coords;

	unit_state.render.fake_badge.instance_id = slot_state->fake_badge.next_instance_id++;
	slot_state->fake_badge.instanced_sprite->actor->SetInstance(
		unit_state.render.fake_badge.instance_id, {
			c.x + ::game::map::s_consts.tile.scale.x * s_consts.badges.fake_badges.offset.x + s_consts.badges.fake_badges.step_x * offset,
			c.y - ::game::map::s_consts.tile.scale.y * s_consts.badges.fake_badges.offset.y * ::game::map::s_consts.sprite.y_scale,
			c.z
		}
	);
	unit_state.render.fake_badge.is_rendered = true;
}

void Game::UnrenderUnit( unit_state_t& unit_state ) {

	if ( !unit_state.render.is_rendered ) {
		return;
	}

	Log( "Unrendering unit " + std::to_string( unit_state.unit_id ) );

	const auto& unitdef_state = unit_state.def;

	ASSERT( unitdef_state->m_type == ::game::unit::Def::DT_STATIC, "only static unitdefs are supported for now" );
	ASSERT( unitdef_state->static_.render.is_sprite, "only sprite unitdefs are supported for now" );

	if ( m_selected_unit_state == &unit_state ) {
		m_selected_unit_state = nullptr;
	}

	auto& sprite = unitdef_state->static_.render.morale_based_xshift
		? unitdef_state->static_.render.morale_based_sprites->at( unit_state.morale )
		: unitdef_state->static_.render.sprite;
	sprite.instanced_sprite->actor->RemoveInstance( unit_state.render.instance_id );

	UnrenderUnitBadge( unit_state );

	unit_state.render.is_rendered = false;
}

void Game::UnrenderUnitBadge( const unit_state_t& unit_state ) {
	unit_state.render.badge.def->instanced_sprite->actor->RemoveInstance( unit_state.render.badge.instance_id );
	unit_state.render.badge.healthbar.def->instanced_sprite->actor->RemoveInstance( unit_state.render.badge.healthbar.instance_id );
}

void Game::UnrenderUnitFakeBadge( unit_state_t& unit_state ) {

	Log( "Unrendering unit fake badge " + std::to_string( unit_state.unit_id ) );

	ASSERT( unit_state.render.fake_badge.is_rendered, "unit fake badge not rendered" );

	auto& slot = unit_state.slot;

	slot->fake_badge.instanced_sprite->actor->RemoveInstance( unit_state.render.fake_badge.instance_id );

	unit_state.render.fake_badge.is_rendered = false;
}

std::vector< size_t > Game::GetUnitsOrder( const unit_states_t& units ) const {
	typedef int16_t weight_t;
	std::map< weight_t, std::vector< size_t > > weights; // { weight, units }

	for ( auto& it : units ) {
		const auto unit_id = it.first;
		const auto& unit = it.second;
		weight_t weight = 0;

		// active units have priority
		if ( unit->is_active ) {
			weight += 200;
		}

		// fungal towers are like bases so have priority
		// TODO: use real unit properties
		if ( unit->def->m_id == "FungalTower" ) {
			weight += 100;
		}

		// non-artillery units have priority
		// TODO: use real unit properties
		if ( unit->def->m_id != "SporeLauncher" ) {
			weight += 60;
		}

		// higher morale has priority
		weight += unit->morale * 2;

		// health is also somewhat important
		weight += unit->health * 10;

		weights[ -weight ].push_back( unit_id ); // negative because we need reverse order

	}

	std::vector< size_t > result = {};
	for ( const auto& it : weights ) {
		for ( const auto& unit_id : it.second ) {
			result.push_back( unit_id );
		}
	}

	return result;
}

void Game::RenderTile( tile_state_t& tile_state ) {

	Log( "Rendering tile [" + std::to_string( tile_state.coords.x ) + "," + std::to_string( tile_state.coords.y ) + "]" );

	// unrender unit
	if ( tile_state.render.currently_rendered_unit ) {
		UnrenderUnit( *tile_state.render.currently_rendered_unit );
		tile_state.render.currently_rendered_unit = nullptr;
	}

	// unrender fake badges
	for ( const auto& unit : tile_state.render.currently_rendered_fake_badges ) {
		UnrenderUnitFakeBadge( *unit );
	}
	tile_state.render.currently_rendered_fake_badges.clear();

	if ( !tile_state.units.empty() ) {
		const auto units_order = GetUnitsOrder( tile_state.units );
		ASSERT( !units_order.empty(), "units order is empty" );

		// for now just keep most important unit rendered
		const auto most_important_unit_id = units_order.front();

		// also display badges from stacked units that are not visible themselves
		ASSERT( tile_state.render.currently_rendered_fake_badges.empty(), "orphan badges already rendered" );
		size_t fake_badge_idx = 1;
		std::vector< unit_state_t* > fake_badges = {};
		for ( const auto& unit_id : units_order ) {
			const auto& unit = tile_state.units.at( unit_id );
			if ( unit_id == most_important_unit_id ) {
				// render full unit
				tile_state.render.currently_rendered_unit = unit;
				RenderUnit( *tile_state.render.currently_rendered_unit );
			}
			else {
				// render fake badge
				tile_state.render.currently_rendered_fake_badges.push_back( unit );
				fake_badges.push_back( unit );
				if ( fake_badge_idx++ > s_consts.badges.fake_badges.max_per_tile ) {
					break;
				}
			}
		}
		size_t idx;
		for ( size_t i = 0 ; i < fake_badges.size() ; i++ ) { // order is important
			idx = fake_badges.size() - i - 1;
			RenderUnitFakeBadge( *fake_badges.at( idx ), i );
		}
	}
}

Game::tile_state_t& Game::GetTileState( const size_t x, const size_t y ) {
	ASSERT( !m_tile_states.empty(), "tile states not set" );
	ASSERT( x < m_map_data.width, "tile x overflow" );
	ASSERT( y < m_map_data.height, "tile y overflow" );
	return m_tile_states.at( y * m_map_data.width + x );
}

const std::string Game::FloatToString( const float value ) {
	std::string result = std::to_string( value );
	result.erase( result.find_last_not_of( '0' ) + 1, std::string::npos );
	result.erase( result.find_last_not_of( '.' ) + 1, std::string::npos );
	return result;
}

}
}
