#include "World.h"

#include "engine/Engine.h"

#include "../mainmenu/MainMenu.h"

#include "graphics/Graphics.h"

#include "util/FS.h"

#include "map_generator/SimplePerlin.h"

//#include "map_generator/Test.h"

// TODO: move to settings
#define MAP_ROTATE_SPEED 2.0f

#define INITIAL_CAMERA_ANGLE { -M_PI * 0.5, M_PI * 0.75, 0 }

#ifdef DEBUG
#define MAP_FILENAME "./tmp/lastmap.gsm"
#define MAP_DUMP_FILENAME "./tmp/lastmap.gsmd"
#define MAP_SEED_FILENAME "./tmp/lastmap.seed"
#endif

namespace game {
namespace world {

const World::consts_t World::s_consts = {};

World::World( const Settings& settings )
	: m_settings( settings )
{
	
}

World::~World() {
	
}

void World::Start() {

	NEW( m_random, Random );

#ifdef DEBUG
	const auto* config = g_engine->GetConfig();
	if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_SEED ) ) {
		m_random->SetState( config->GetQuickstartSeed() );
	}
#endif
	
	Log( "World seed: " + m_random->GetStateString() );

	NEW( m_world_scene, Scene, "World", SCENE_TYPE_ORTHO );
	
	NEW( m_camera, Camera, Camera::CT_ORTHOGRAPHIC );
	m_camera_angle = INITIAL_CAMERA_ANGLE;
	UpdateCameraAngle();

	m_world_scene->SetCamera( m_camera );
	
	// don't set exact 45 degree angles for lights, it will produce weird straight lines because of shadows
	{
		NEW( m_light_a, Light, Light::LT_AMBIENT_DIFFUSE );
		m_light_a->SetPosition( { 48.227f, 20.412f, 57.65f } );
		m_light_a->SetColor( { 0.8f, 0.9f, 1.0f, 0.8f } );
		m_world_scene->AddLight( m_light_a );
	}
	{
		NEW( m_light_b, Light, Light::LT_AMBIENT_DIFFUSE );
		m_light_b->SetPosition( { 22.412f, 62.227f, 43.35f } );
		m_light_b->SetColor( { 1.0f, 0.9f, 0.8f, 0.8f } );
		m_world_scene->AddLight( m_light_b );
	}
	
	g_engine->GetGraphics()->AddScene( m_world_scene );	
	
	NEW( m_map, Map, m_random, m_world_scene );
	
	Tiles* tiles = nullptr;
	#ifdef DEBUG
	if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_MAPSIZE ) ) {
		const auto& map_size = config->GetQuickstartMapSize();
		NEW( tiles, Tiles, map_size.x, map_size.y, m_random );
	}
	else
	#endif
	{
		NEW( tiles, Tiles, 80, 40, m_random );
	}
	
#ifdef DEBUG
	// if crash happens - it's handy to have a seed to reproduce it
	FS::WriteFile( MAP_SEED_FILENAME, m_random->GetStateString() );
#endif
	
#ifdef DEBUG
	if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_MAPDUMP ) ) {
		const std::string& filename = config->GetQuickstartMapDump();
		ASSERT( FS::FileExists( filename ), "map dump file \"" + filename + "\" not found" );
		Log( (std::string) "Loading map dump from " + filename );
		m_map->SetTiles( tiles, false );
		m_map->Unserialize( Buffer( FS::ReadFile( filename ) ) );
	}
	else
#endif
	{
#ifdef DEBUG
		if ( config->HasDebugFlag( config::Config::DF_QUICKSTART_MAPFILE ) ) {
			const std::string& filename = config->GetQuickstartMapFile();
			ASSERT( FS::FileExists( filename ), "map file \"" + filename + "\" not found" );
			Log( (std::string) "Loading map from " + filename );
			tiles->Unserialize( Buffer( FS::ReadFile( filename ) ) );
		}
		else
#endif
		{
			map_generator::SimplePerlin generator;
			//map_generator::Test generator;
#ifdef DEBUG
			//map_generator::SimpleRandom generator;
			//map_generator::SimpleRandomNoLoops generator;
			//map_generator::SimpleRandomNoPointers generator;
			util::Timer timer;
			timer.Start();
#endif
			generator.Generate( tiles, m_random->GetUInt( 0, UINT32_MAX - 1 ) );
#ifdef DEBUG
			Log( "Map generation took " + std::to_string( timer.GetElapsed().count() ) + "ms" );
			// if crash happens - it's handy to have a map file to reproduce it
			if ( !config->HasDebugFlag( config::Config::DF_QUICKSTART_MAPFILE ) ) { // no point saving if we just loaded it
				Log( (std::string) "Saving map to " + MAP_FILENAME );
				FS::WriteFile( MAP_FILENAME, tiles->Serialize().ToString() );
			}
#endif
		}
		m_map->SetTiles( tiles );
#ifdef DEBUG
		// also handy to have dump of generated map
		if ( !config->HasDebugFlag( config::Config::DF_QUICKSTART_MAPDUMP ) ) { // no point saving if we just loaded it
			Log( (std::string) "Saving map dump to " + MAP_DUMP_FILENAME );
			FS::WriteFile( MAP_DUMP_FILENAME, m_map->Serialize().ToString() );
		}
#endif
	}

	// init map editor
	NEW( m_map_editor, map_editor::MapEditor, this );
	
	auto* ui = g_engine->GetUI();
	
	// UI
	ui->AddTheme( &m_ui.theme );
	NEW( m_ui.bottom_bar, ui::BottomBar, this );
	ui->AddObject( m_ui.bottom_bar );

	m_viewport.bottom_bar_overlap = 32; // it has transparent area on top so let map render through it
	
	// map event handlers
	m_handlers.keydown = ui->AddGlobalEventHandler( UIEvent::EV_KEY_DOWN, EH( this ) {
		if ( !data->key.modifiers ) {
			
			if ( m_selected_tile_info.tile && m_selected_tile_info.ts ) {
				
				// move tile selector
				bool is_tile_moved = true;
				if ( data->key.code == UIEvent::K_LEFT || data->key.code == UIEvent::K_KP_LEFT ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->W;
					m_selected_tile_info.ts = m_selected_tile_info.ts->W;
				}
				else if ( data->key.code == UIEvent::K_UP || data->key.code == UIEvent::K_KP_UP ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->N;
					m_selected_tile_info.ts = m_selected_tile_info.ts->N;
				}
				else if ( data->key.code == UIEvent::K_RIGHT || data->key.code == UIEvent::K_KP_RIGHT ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->E;
					m_selected_tile_info.ts = m_selected_tile_info.ts->E;
				}
				else if ( data->key.code == UIEvent::K_DOWN || data->key.code == UIEvent::K_KP_DOWN ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->S;
					m_selected_tile_info.ts = m_selected_tile_info.ts->S;
				}
				else if ( data->key.code == UIEvent::K_HOME || data->key.code == UIEvent::K_KP_LEFT_UP ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->NW;
					m_selected_tile_info.ts = m_selected_tile_info.ts->NW;
				}
				else if (
					data->key.code == UIEvent::K_END || data->key.code == UIEvent::K_KP_LEFT_DOWN ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->SW;
					m_selected_tile_info.ts = m_selected_tile_info.ts->SW;
				}
				else if (
					data->key.code == UIEvent::K_PAGEUP || data->key.code == UIEvent::K_KP_RIGHT_UP ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->NE;
					m_selected_tile_info.ts = m_selected_tile_info.ts->NE;
				}
				else if (
					data->key.code == UIEvent::K_PAGEDOWN || data->key.code == UIEvent::K_KP_RIGHT_DOWN ) {
					m_selected_tile_info.tile = m_selected_tile_info.tile->SE;
					m_selected_tile_info.ts = m_selected_tile_info.ts->SE;
				}
				else {
					is_tile_moved = false; // not moved
				}
				
				if ( is_tile_moved ) {
					// moved
					SelectTile( m_selected_tile_info );
					
					auto tc = GetTileWindowCoordinates( m_selected_tile_info.ts );
					
					Vec2< float > uc = {
						GetFixedX( tc.x + m_camera_position.x ),
						tc.y + m_camera_position.y
					};
					
					Vec2< float > scroll_by = { 0, 0 };
					
					//Log( "Resolved tile coordinates to " + uc.ToString() + " ( camera: " + m_camera_position.ToString() + " )" );
					
					// tile size
					Vec2< float > ts = {
						Map::s_consts.tile.scale.x * m_camera_position.z,
						Map::s_consts.tile.scale.y * m_camera_position.z
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
						FixCameraX();
						ScrollTo({
							m_camera_position.x + scroll_by.x,
							m_camera_position.y + scroll_by.y,
							m_camera_position.z
						});
					}
					
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
			
			// exit to main menu
			if ( data->key.code == UIEvent::K_ESCAPE ) {
				ReturnToMainMenu();
				return true;
			}
		}
		else if ( data->key.code == UIEvent::K_CTRL ) {
			if ( m_map_editor->IsEnabled() ) {
				m_is_editing_mode = true;
			}
		}
		
		return false;
	}, UI::GH_AFTER );
	
	m_handlers.keyup = ui->AddGlobalEventHandler( UIEvent::EV_KEY_UP, EH( this ) {
		if ( data->key.key == 'z' || data->key.key == 'x' ) {
			if ( m_map_control.key_zooming ) {
				m_map_control.key_zooming = 0;
				m_scroller.Stop();
			}
		}
		else if ( data->key.code == UIEvent::K_CTRL ) {
			if ( m_map_editor->IsEnabled() ) {
				m_is_editing_mode = false;
				m_editing_draw_timer.Stop();
				m_editing_draw_mode = map_editor::MapEditor::DM_NONE;
			}
		}
		return false;
	}, UI::GH_BEFORE );
	
	m_handlers.mousedown = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_DOWN, EH( this ) {
		if ( m_is_editing_mode ) {
			switch ( data->mouse.button ) {
				case UIEvent::M_LEFT: {
					m_editing_draw_mode = map_editor::MapEditor::DM_INC;
					break;
				}
				case UIEvent::M_RIGHT: {
					m_editing_draw_mode = map_editor::MapEditor::DM_DEC;
					break;
				}
				default: {
					m_editing_draw_mode = map_editor::MapEditor::DM_NONE;
				}
					
			}
			SelectTileAtPoint( data->mouse.absolute.x, data->mouse.absolute.y ); // async
			m_editing_draw_timer.SetInterval( World::s_consts.map_editing.draw_frequency_ms ); // keep drawing until mouseup
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
					m_map_control.last_drag_position = { m_clamp.x.Clamp( data->mouse.absolute.x ), m_clamp.y.Clamp( data->mouse.absolute.y ) };
					break;
				}
#ifdef DEBUG
				case UIEvent::M_MIDDLE: {
					m_map_control.is_rotating = true;
					m_map_control.last_rotate_position = { m_clamp.x.Clamp( data->mouse.absolute.x ), m_clamp.y.Clamp( data->mouse.absolute.y ) };
					break;
				}
#endif
			}
		}
		return true;
	}, UI::GH_AFTER );
	
	m_handlers.mousemove = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_MOVE, EH( this ) {
		m_map_control.last_mouse_position = {
			GetFixedX( data->mouse.absolute.x ),
			(float)data->mouse.absolute.y
		};
		if ( m_map_control.is_dragging ) {
				
			Vec2<float> current_drag_position = { m_clamp.x.Clamp( data->mouse.absolute.x ), m_clamp.y.Clamp( data->mouse.absolute.y ) };
			Vec2<float> drag = current_drag_position - m_map_control.last_drag_position;
			
			m_camera_position.x += (float) drag.x;
			m_camera_position.y += (float) drag.y;
			UpdateCameraPosition();
			
			m_map_control.last_drag_position = current_drag_position;
		}
#ifdef DEBUG
		else if ( m_map_control.is_rotating ) {
			Vec2<float> current_rotate_position = { m_clamp.x.Clamp( data->mouse.absolute.x ), m_clamp.y.Clamp( data->mouse.absolute.y ) };
			Vec2<float> rotate = current_rotate_position - m_map_control.last_rotate_position;
			
			for (auto& actor : m_map->GetActors() ) {
				auto newz = actor->GetAngleZ() + ( (float) rotate.x * MAP_ROTATE_SPEED );
				auto newy = std::max( -0.5f, std::min( 0.5f, actor->GetAngleY() + ( (float) rotate.y * MAP_ROTATE_SPEED ) ) );
				actor->SetAngleZ( newz );
				actor->SetAngleY( newy );
			}
			m_map_control.last_rotate_position = current_rotate_position;
		}
#endif
		else if ( !m_ui.bottom_bar->IsMouseDraggingMiniMap() ) {
			const ssize_t edge_distance = m_viewport.is_fullscreen
				? World::s_consts.map_scroll.static_scrolling.edge_distance_px.fullscreen
				: World::s_consts.map_scroll.static_scrolling.edge_distance_px.windowed
			;
			if ( data->mouse.absolute.x < edge_distance ) {
				m_map_control.edge_scrolling.speed.x = World::s_consts.map_scroll.static_scrolling.speed.x;
			}
			else if ( data->mouse.absolute.x >= m_viewport.window_width - edge_distance ) {
				m_map_control.edge_scrolling.speed.x = -World::s_consts.map_scroll.static_scrolling.speed.x;
			}
			else {
				m_map_control.edge_scrolling.speed.x = 0;
			}
			if ( data->mouse.absolute.y <= edge_distance ) {
				m_map_control.edge_scrolling.speed.y = World::s_consts.map_scroll.static_scrolling.speed.y;
			}
			else if ( data->mouse.absolute.y >= m_viewport.window_height - edge_distance ) {
				m_map_control.edge_scrolling.speed.y = -World::s_consts.map_scroll.static_scrolling.speed.y;
			}
			else {
				m_map_control.edge_scrolling.speed.y = 0;
			}
			if ( m_map_control.edge_scrolling.speed ) {
				if ( !m_map_control.edge_scrolling.timer.IsRunning() ) {
					Log( "Edge scrolling started" );
					m_map_control.edge_scrolling.timer.SetInterval( World::s_consts.map_scroll.static_scrolling.scroll_step_ms );
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
	}, UI::GH_AFTER );
	
	m_handlers.mouseup = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) {
		switch ( data->mouse.button ) {
			case UIEvent::M_RIGHT: {
				m_map_control.is_dragging = false;
				break;
			}
#ifdef DEBUG
			case UIEvent::M_MIDDLE: {
				m_map_control.is_rotating = false;
				for (auto& actor : m_map->GetActors() ) {
					actor->SetAngle( { 0.0, 0.0, 0.0 } );
				}
				break;
			}
#endif
		}
		if ( m_is_editing_mode ) {
			m_editing_draw_timer.Stop();
			m_editing_draw_mode = map_editor::MapEditor::DM_NONE;
		}
		return true;
	}, UI::GH_AFTER );
	
	m_handlers.mousescroll = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_SCROLL, EH( this ) {
		SmoothScroll( m_map_control.last_mouse_position, data->mouse.scroll_y );
		return true;
	}, UI::GH_AFTER );
	
	// other stuff
	
	m_clamp.x.SetDstRange( { -0.5f, 0.5f } );
	m_clamp.y.SetDstRange( { -0.5f, 0.5f } );
	
	// map should continue scrolling even if mouse is outside viewport
	m_clamp.x.SetOverflowAllowed( true );
	m_clamp.y.SetOverflowAllowed( true );
	
	UpdateViewport();
	
	SetCameraPosition( { 0.0f, -0.25f, 0.1f } );
	
	UpdateCameraRange();
	UpdateCameraScale();
	
	g_engine->GetGraphics()->AddOnWindowResizeHandler( this, RH( this ) {
		UpdateViewport();
		UpdateCameraRange();
		UpdateMapInstances();
		UpdateMinimap();
	});
	
	UpdateMapInstances();
	
	UpdateUICamera();
	
	// assume mouse starts at center
	m_map_control.last_mouse_position = {
		(float)m_viewport.window_width / 2,
		(float)m_viewport.window_height / 2
	};
	
	// update minimap
	UpdateMinimap();
	
	// select tile at center
	Vec2< size_t > coords = { m_map->GetWidth() / 2, m_map->GetHeight() / 2 };
	if ( ( coords.y % 2 ) != ( coords.x % 2 ) ) {
		coords.y++;
	}
	SelectTile( m_map->GetTileAt( coords.x, coords.y ) );
	
}

void World::Stop() {
	
	CloseMenus();
	
	auto* ui = g_engine->GetUI();
	ui->RemoveObject( m_ui.bottom_bar );
	ui->RemoveTheme( &m_ui.theme );
	
	DELETE( m_map_editor );
	
	DELETE( m_map );
	
	g_engine->GetGraphics()->RemoveOnWindowResizeHandler( this );
	
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.keydown );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.keyup );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousedown );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousemove );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mouseup );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_handlers.mousescroll );
	
	g_engine->GetGraphics()->RemoveScene( m_world_scene );	
	DELETE( m_camera );
	DELETE( m_light_a );
	DELETE( m_light_b );

	for ( auto& it : m_actors_map ) {
		m_world_scene->RemoveActor( it.second );
		DELETE( it.second );
	}
	m_actors_map.clear();

	DELETE( m_world_scene );
	
	DELETE( m_random );
}

void World::Iterate() {
	
	if ( m_editing_draw_timer.HasTicked() ) {
		if ( m_is_editing_mode && !m_map->IsTileAtRequestPending() ) {
			SelectTileAtPoint( m_map_control.last_mouse_position.x, m_map_control.last_mouse_position.y ); // async
		}
	}
	
	// response for clicked tile (if click happened)
	auto tile_info = m_map->GetTileAtScreenCoordsResult();
	if ( tile_info.tile ) {
		if ( m_is_editing_mode ) {
			const auto tiles_to_reload = m_map_editor->Draw( tile_info.tile, m_editing_draw_mode );
			m_map->LoadTiles( tiles_to_reload );
			m_map->FixNormals( tiles_to_reload );
			SelectTile( tile_info );
		}
		else {
			SelectTile( tile_info );
			CenterAtTile( tile_info.ts );
		}
	}
	
	// update minimap (if it was updated)
	auto minimap_texture = m_map->GetMinimapTextureResult();
	if ( minimap_texture ) {
		m_ui.bottom_bar->SetMinimapTexture( minimap_texture );
		//UpdateMinimap(); // tmp
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

Map* World::GetMap() const {
	ASSERT( m_map, "m_map not set during GetMap()" );
	return m_map;
}

map_editor::MapEditor* World::GetMapEditor() const {
	return m_map_editor;
}

void World::CenterAtCoordinatePercents( const Vec2< float > position_percents ) {
	const auto* ms = m_map->GetMapState();
	const Vec2< float > position = {
		ms->range.percent_to_absolute.x.Clamp( position_percents.x ),
		ms->range.percent_to_absolute.y.Clamp( position_percents.y )
	};
	//Log( "Scrolling to percents " + position_percents.ToString() );
	m_camera_position = {
		GetFixedX( - position.x * m_camera_position.z / m_viewport.window_aspect_ratio ),
		- position.y * m_camera_position.z * m_viewport.ratio.y * 0.707f,
		m_camera_position.z
	};
	UpdateCameraPosition();
}

void World::SetCameraPosition( const Vec3 camera_position ) {
	if ( camera_position != m_camera_position ) {
		bool position_updated =
			( m_camera_position.x != camera_position.x ) ||
			( m_camera_position.y != camera_position.y )
		;
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

const float World::GetFixedX( float x ) const {
	if ( x < m_camera_range.min.x ) {
		x += m_camera_range.max.x - m_camera_range.min.x;
	}	
	else if ( x > m_camera_range.max.x ) {
		x -= m_camera_range.max.x - m_camera_range.min.x;
	}
	return x;
}

void World::FixCameraX() {
	m_camera_position.x = GetFixedX( m_camera_position.x );
}

void World::UpdateViewport() {
	auto* graphics = g_engine->GetGraphics();
	m_viewport.window_width = graphics->GetViewportWidth();
	m_viewport.window_height = graphics->GetViewportHeight();
	m_viewport.window_aspect_ratio = graphics->GetAspectRatio();
	m_viewport.max.x = std::max< ssize_t >( m_viewport.min.x, m_viewport.window_width );
	m_viewport.max.y = std::max< ssize_t >( m_viewport.min.y, m_viewport.window_height - m_ui.bottom_bar->GetHeight() + m_viewport.bottom_bar_overlap );
	m_viewport.ratio.x = (float) m_viewport.window_width / m_viewport.max.x;
	m_viewport.ratio.y = (float) m_viewport.window_height / m_viewport.max.y;
	m_viewport.width = m_viewport.max.x - m_viewport.min.x;
	m_viewport.height = m_viewport.max.y - m_viewport.min.y;
	m_viewport.aspect_ratio = (float) m_viewport.width / m_viewport.height;
	m_viewport.is_fullscreen = graphics->IsFullscreen();
	m_clamp.x.SetSrcRange( { (float)m_viewport.min.x, (float)m_viewport.max.x } );
	m_clamp.y.SetSrcRange( { (float)m_viewport.min.y, (float)m_viewport.max.y } );
}

void World::UpdateCameraPosition() {
	
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
	
	m_camera->SetPosition({
		( 0.5f + m_camera_position.x ) * m_viewport.window_aspect_ratio,
		( 0.5f + m_camera_position.y ) / m_viewport.ratio.y + Map::s_consts.tile_scale_z * m_camera_position.z / 1.414f, // TODO: why 1.414?
		( 0.5f + m_camera_position.y ) / m_viewport.ratio.y + m_camera_position.z
	});

	const auto* ms = m_map->GetMapState();

	m_ui.bottom_bar->SetMinimapSelection({
		1.0f - ms->range.percent_to_absolute.x.Unclamp( m_camera_position.x / m_camera_position.z * m_viewport.window_aspect_ratio ),
		1.0f - ms->range.percent_to_absolute.y.Unclamp( m_camera_position.y / m_camera_position.z / m_viewport.ratio.y / 0.707f )
	}, {
		2.0f / ( (float) ( m_map->GetWidth() ) * m_camera_position.z / m_viewport.window_aspect_ratio ),
		2.0f / ( (float) ( m_map->GetHeight() ) * m_camera_position.z * m_viewport.ratio.y * 0.707f ),
	});
}

void World::UpdateCameraScale() {
	m_camera->SetScale( { m_camera_position.z, m_camera_position.z, m_camera_position.z } );
	//UpdateUICamera();
}

void World::UpdateCameraAngle() {
	m_camera->SetAngle( m_camera_angle );
	//UpdateUICamera();
}

void World::UpdateCameraRange() {
	m_camera_range.min.z = 2.82f / ( m_map->GetHeight() + 1 ) / m_viewport.ratio.y; // TODO: why 2.82?
	m_camera_range.max.z = 0.22f; // TODO: fix camera z and allow to zoom in more
	if ( m_camera_position.z < m_camera_range.min.z ) {
		m_camera_position.z = m_camera_range.min.z;
	}
	if ( m_camera_position.z > m_camera_range.max.z ) {
		m_camera_position.z = m_camera_range.max.z;
	}
	m_camera_range.max.y = ( m_camera_position.z - m_camera_range.min.z ) * ( m_map->GetHeight() + 1 ) * m_viewport.ratio.y * 0.1768f; // TODO: why 0.1768?
	m_camera_range.min.y = -m_camera_range.max.y;
	
	//Log( "Camera range change: Z=[" + to_string( m_camera_range.min.z ) + "," + to_string( m_camera_range.max.z ) + "] Y=[" + to_string( m_camera_range.min.y ) + "," + to_string( m_camera_range.max.y ) + "], z=" + to_string( m_camera_position.z ) );
	
	m_camera_range.max.x = ( m_map->GetWidth() ) * m_camera_position.z / m_viewport.window_aspect_ratio * 0.25f;
	m_camera_range.min.x = -m_camera_range.max.x;
	
	UpdateCameraPosition();
	UpdateCameraScale();
}

void World::UpdateMapInstances() {
	// needed for horizontal scrolling
	std::vector< Vec3 > instances;
	
	const float mhw = Map::s_consts.tile.scale.x * m_map->GetWidth() / 2;
	
	uint8_t instances_before_after = floor( m_viewport.aspect_ratio / ( (float) m_map->GetWidth() / m_map->GetHeight() ) / 2 ) + 1;
	
	// don't spawn more instances than graphics system can support
	while ( instances_before_after * 2 + 1 > graphics::Graphics::MAX_WORLD_INSTANCES ) {
		instances_before_after--;
	}
	
	for ( uint8_t i = instances_before_after ; i > 0 ; i-- ) {
		instances.push_back( { -mhw * i, 0.0f, 0.0f } );
	}
	instances.push_back( { 0.0f, 0.0f, 0.0f} );
	for ( uint8_t i = 1 ; i <= instances_before_after ; i++ ) {
		instances.push_back( { +mhw * i, 0.0f, 0.0f } );
	}
	
	m_world_scene->SetInstances( instances );
}

void World::UpdateUICamera() {
	// TODO: finish it
	// snapshot camera matrix for world ui
	/*m_camera->GetMatrix()*/
	// tmp/hack
	/*for ( auto& a : m_map->GetActors() ) {
		for ( auto& m : ((scene::actor::Instanced*)a)->GetWorldMatrices() ) {
			g_engine->GetUI()->SetWorldUIMatrix( m );
			break;
		}
		break;
	}*/
}

void World::ReturnToMainMenu() {
	
#ifdef DEBUG
	const auto* config = g_engine->GetConfig();
	if ( config->HasDebugFlag( config::Config::DF_QUICKSTART ) ) {
		g_engine->ShutDown();
	}
	else
#endif
	{
		NEWV( task, game::mainmenu::MainMenu );
		g_engine->GetScheduler()->RemoveTask( this );
		g_engine->GetScheduler()->AddTask( task );
	}

}

void World::SelectTileAtPoint( const size_t x, const size_t y ) {
	Log( "Looking up tile at " + std::to_string( x ) + "x" + std::to_string( y ) );
	m_map->GetTileAtScreenCoords( x, m_viewport.window_height - y ); // async
}

void World::SelectTile( const Map::tile_info_t& tile_info ) {
	DeselectTile();
	auto tile = tile_info.tile;
	auto ts = tile_info.ts;
	
	Log( "Selecting tile at " + std::to_string( tile->coord.x ) + "x" + std::to_string( tile->coord.y ) );
	Map::tile_layer_type_t lt = ( tile->is_water_tile ? Map::LAYER_WATER : Map::LAYER_LAND );
	auto& layer = ts->layers[ lt ];
	auto coords = layer.coords;
	
	if ( !tile->is_water_tile && ts->is_coastline_corner ) {
		if ( tile->W->is_water_tile ) {
			coords.left = ts->layers[ Map::LAYER_WATER ].coords.left;
		}
		if ( tile->N->is_water_tile ) {
			coords.top = ts->layers[ Map::LAYER_WATER ].coords.top;
		}
		if ( tile->E->is_water_tile ) {
			coords.right = ts->layers[ Map::LAYER_WATER ].coords.right;
		}
		if ( tile->S->is_water_tile ) {
			coords.bottom = ts->layers[ Map::LAYER_WATER ].coords.bottom;
		}
	}
	
	NEW( m_actors.tile_selection, actor::TileSelection, coords );
	AddActor( m_actors.tile_selection );
	
	m_ui.bottom_bar->PreviewTile( m_map, tile_info );
	
	m_selected_tile_info = tile_info;
}

void World::DeselectTile() {
	if ( m_actors.tile_selection ) {
		RemoveActor( m_actors.tile_selection );
		m_actors.tile_selection = nullptr;
	}
	
	m_ui.bottom_bar->HideTilePreview();
}

void World::AddActor( actor::Actor* actor ) {
	ASSERT( m_actors_map.find( actor ) == m_actors_map.end(), "world actor already added" );
	NEWV( instanced, scene::actor::Instanced, actor );
	m_world_scene->AddActor( instanced );
	m_actors_map[ actor ] = instanced;
}

void World::RemoveActor( actor::Actor* actor ) {
	auto it = m_actors_map.find( actor );
	ASSERT( it != m_actors_map.end(), "world actor not found" );
	m_world_scene->RemoveActor( it->second );
	DELETE( it->second );
	m_actors_map.erase( it );
}

const Vec2< float > World::GetTileWindowCoordinates( const Map::tile_state_t* ts ) {
	return {
		ts->coord.x / m_viewport.window_aspect_ratio * m_camera_position.z,
		( ts->coord.y - std::max( 0.0f, Map::s_consts.clampers.elevation_to_vertex_z.Clamp( ts->elevations.center ) ) ) * m_viewport.ratio.y * m_camera_position.z / 1.414f
	};
}

void World::ScrollTo( const Vec3& target ) {
	m_scroller.Scroll( m_camera_position, target );
}

void World::ScrollToTile( const Map::tile_state_t* ts ) {
	
	auto tc = GetTileWindowCoordinates( ts );
	
	const float tile_x_shifted = m_camera_position.x > 0
		? tc.x - ( m_camera_range.max.x - m_camera_range.min.x )
		: tc.x + ( m_camera_range.max.x - m_camera_range.min.x )
	;
	if (
		fabs( tile_x_shifted - -m_camera_position.x )
			<
		fabs( tc.x - -m_camera_position.x )
	) {
		// smaller distance if going other side
		tc.x = tile_x_shifted;
	}
	
	ScrollTo({
		- tc.x,
		- tc.y,
		m_camera_position.z
	});
}

void World::CenterAtTile( const Map::tile_state_t* ts ) {
	ScrollToTile( ts );
}

void World::UpdateMinimap() {
	NEWV( camera, scene::Camera, scene::Camera::CT_ORTHOGRAPHIC );
	
	auto mm = m_ui.bottom_bar->GetMinimapDimensions();
	// 'black grid' artifact workaround
	// TODO: find reason and fix properly, maybe just keep larger internal viewport
	Vec2< float > scale = {
		(float) m_viewport.window_width / mm.x,
		(float) m_viewport.window_height / mm.y
	};
	
	mm.x *= scale.x;
	mm.y *= scale.y;
	
	const float sx = (float)mm.x / (float)m_map->GetWidth() / (float)Map::s_consts.tc.texture_pcx.dimensions.x;
	const float sy = (float)mm.y / (float)m_map->GetHeight() / (float)Map::s_consts.tc.texture_pcx.dimensions.y;
	const float sz = ( sx + sy ) / 2;
	const float ss = ( (float) mm.y / (float) m_viewport.window_height );
	const float sxy = (float) scale.x / scale.y;
	
	camera->SetAngle( m_camera->GetAngle() );
	camera->SetScale({
		sx * ss * sxy / scale.x,
		sy * ss * 1.38f / scale.y,
		0.01f
	});
	
	camera->SetPosition({
		ss * sxy,
		1.0f - ss * 0.48f,
		0.5f
	});
	
	m_map->GetMinimapTexture( camera, {
		mm.x,
		mm.y
	});
}

void World::SmoothScroll( const float scroll_value ) {
	SmoothScroll( { (float)m_viewport.width / 2, (float)m_viewport.height / 2 }, scroll_value );
}

void World::SmoothScroll( const Vec2< float > position, const float scroll_value ) {
	
	float speed = World::s_consts.map_scroll.smooth_scrolling.zoom_speed * m_camera_position.z;

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

	ScrollTo({
		( m_camera_position.x - m.x ) / diff + m.x,
		( m_camera_position.y - m.y ) / diff + m.y,
		new_z
	});
}

util::Random* World::GetRandom() const {
	return m_random;
}

void World::CloseMenus() {
	if ( m_ui.bottom_bar ) {
		m_ui.bottom_bar->CloseMenus();
	}
}

}
}
